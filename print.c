/*
 * Electric Fence - Red-Zone memory allocator.
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
 * Copyright (C) 2002 Hayati Ayguen <hayati.ayguen@epost.de>, Procitec GmbH
 * License: GNU GPL (GNU General Public License, see COPYING-GPL)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * FILE CONTENTS:
 * internal implementation file
 * contains aborting, printing functions with minor system/platform dependencies
 */

#ifndef NDEBUG

#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <stdarg.h>
#include <io.h>
typedef unsigned u_int;
#endif
#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#include <string.h>
#include <signal.h>

#include "efenceint.h"


/*
 * These routines do their printing without using stdio. Stdio can't
 * be used because it calls malloc(). Internal routines of a malloc()
 * debugger should not re-enter malloc(), so stdio is out.
 */

/*
 * NUMBER_BUFFER_SIZE is the longest character string that could be needed
 * to represent an unsigned integer, assuming we might print in base 2.
 */
#define	NUMBER_BUFFER_SIZE	(sizeof(ef_number) * NBBY)

#define STRING_BUFFER_SIZE  1024


/*
 * internal abort function
 * void do_abort(void)
 */
static void
do_abort(void)
{
#ifndef WIN32
	/*
	 * I use kill(getpid(), SIGILL) instead of abort() because some
	 * mis-guided implementations of abort() flush stdio, which can
	 * cause malloc() or free() to be called.
	 */
	kill(getpid(), SIGILL);
#else
  /* Windows doesn't have a kill() */
  abort();
#endif
	/* Just in case something handles SIGILL and returns, exit here. */
	_exit(-1);
}


/*
 * internal function to print a number into a buffer
 * int sprintNumber(char* obuffer, ef_number number, ef_number base)
 */
static int
sprintNumber(char* obuffer, ef_number number, ef_number base)
{
	char		buffer[NUMBER_BUFFER_SIZE+1];
	char *		s = &buffer[NUMBER_BUFFER_SIZE];
	int		size;
	ef_number	digit;

	do
  {
		if ( --s == buffer )
			EF_Abort("\nElectric Fence: Internal error printing number.");

		digit = number % base;
    *s = (char)( (digit < 10) ? ('0' + digit) : ('a' + digit -10) );

	} while ( (number /= base) > 0 );

	size = &buffer[NUMBER_BUFFER_SIZE] - s;
  buffer[NUMBER_BUFFER_SIZE] = '\0';
  strcpy(obuffer, s);
  return size;
}




/*
 * internal function to print a formatted string into a buffer
 * int sprintf(char* buffer, const char *pattern, va_list args)
 */
static int
EF_sprintf(char* buffer, const char *pattern, va_list args)
{
	char		c;
	static const char	bad_pattern[] = "\nElectric Fence: Bad pattern specifier %%%c in EF_Print().\n";
	const char *	s = pattern;
  int len = 0;
  ef_number n;

  c = *s++;
	while ( c )
  {
		if ( c == '%' )
    {
			c = *s++;
			switch ( c ) {
			case '%':
        buffer[len++] = c;
				break;
			case 'a':
				/*
				 * Print an address passed as a void pointer.
				 * The type of ef_number must be set so that
				 * it is large enough to contain all of the
				 * bits of a void pointer.
				 */
        n = (ef_number) va_arg(args, void *);
        len += sprintNumber(&buffer[len], n, 0x10);
				break;
			case 's':
				{
					const char *	string;
					size_t		length;

					string = va_arg(args, char *);
					length = strlen(string);

          strcpy(&buffer[len], string);
          len += length;
				}
				break;
			case 'd':
				{
					int	n = va_arg(args, int);
					if ( n < 0 )
          {
            buffer[len++] = '-';
						n = -n;
					}
          len += sprintNumber(&buffer[len], n, 10);
				}
				break;
			case 'x':
        n = (ef_number) va_arg(args, u_int);
        len += sprintNumber(&buffer[len], n, 0x10);
				break;
			case 'c':
				{
					char	c = va_arg(args, char);
          buffer[len++] = c;
				}
				break;
			default:
				{
					EF_Print(bad_pattern, c);
				}
		
			}
		}
		else
      buffer[len++] = c;
    
    c = *s++;
	}

  buffer[len] = '\0';
  return len;
}



/*
 * external print function
 * on Visual C++ it additionally prints to Debug Output of the IDE
 * void EF_Print(const char * pattern, ...)
 */
void
EF_Print(const char * pattern, ...)
{
  char buffer[STRING_BUFFER_SIZE];
  int len;
	va_list	args;
	va_start(args, pattern);

  len = EF_sprintf(buffer, pattern, args);
#ifdef _MSC_VER
  _RPT0(_CRT_WARN, buffer);
#endif
  write(2, buffer, len);
	va_end(args);
}



/*
 * external abort function
 * on Visual C++ it additionally prints to Debug Output of the IDE
 * void EF_Abort(const char * pattern, ...)
 */
void
EF_Abort(const char * pattern, ...)
{
  char buffer[STRING_BUFFER_SIZE];
  int len;
	va_list	args;
	va_start(args, pattern);

  strcpy(buffer, "\nElectricFence Aborting: ");
  len = EF_sprintf(&buffer[strlen(buffer)], pattern, args);
  strcat(buffer, "\n");
#ifdef _MSC_VER
  _RPT0(_CRT_WARN, buffer);
#endif
  write(2, buffer, len);
	va_end(args);
	do_abort();
}



/*
 * external exit function
 * on Visual C++ it additionally prints to Debug Output of the IDE
 * void EF_Exit(const char * pattern, ...)
 */
void
EF_Exit(const char * pattern, ...)
{
  char buffer[STRING_BUFFER_SIZE];
  int len;
	va_list	args;
	va_start(args, pattern);

  strcpy(buffer, "\nElectricFence Exiting: ");
  len = EF_sprintf(&buffer[strlen(buffer)], pattern, args);
  strcat(buffer, "\n");
#ifdef _MSC_VER
  _RPT0(_CRT_WARN, buffer);
#endif
  write(2, buffer, len);
	va_end(args);

	/*
	 * I use _exit() because the regular exit() flushes stdio,
	 * which may cause malloc() or free() to be called.
	 */
	_exit(-1);
}


#endif /* NDEBUG */
