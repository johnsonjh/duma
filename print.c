
/*
 * DUMA - Red-Zone memory allocator.
 * Copyright (C) 2006 Michael Eddington <meddington@gmail.com>
 * Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
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


#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>

#ifndef WIN32
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/mman.h>
  #include <sys/param.h>
#else
  #define WIN32_LEAN_AND_MEAN 1
  #include <windows.h>
  #include <winbase.h>
  #include <io.h>

  typedef LPVOID caddr_t;
#endif

#ifdef _MSC_VER
  #include <crtdbg.h>
#endif


#include "duma.h"
#include "noduma.h"
#include "print.h"

/*
 * NBBY is the number of bits per byte. Some systems define it in <sys/param.h>
 */
#ifndef NBBY
#define NBBY	8
#endif

/*
 * These routines do their printing without using stdio. Stdio can't
 * be used because it calls malloc(). Internal routines of a malloc()
 * debugger should not re-enter malloc(), so stdio is out.
 */


/* define prototypes / forward declarations */

static int	sprintNumber(char* obuffer, DUMA_ADDR number, DUMA_ADDR base);
static int	sprintLong(char* obuffer, long number, long base);
static int	DUMA_sprintf(char* buffer, const char *pattern, va_list args);


/*
 * NUMBER_BUFFER_SIZE is the longest character string that could be needed
 * to represent an unsigned integer, assuming we might print in base 2.
 */
#define  NUMBER_BUFFER_SIZE  (sizeof(DUMA_ADDR) * NBBY)

#define STRING_BUFFER_SIZE	4096


/* Function: sprintNumber
 *
 * internal function to print a DUMA_ADDR into a buffer
 * int sprintNumber(char* obuffer, DUMA_ADDR number, DUMA_ADDR base)
 */
static int sprintNumber(char* obuffer, DUMA_ADDR number, DUMA_ADDR base)
{
  char	 buffer[NUMBER_BUFFER_SIZE+1];
  char * s = &buffer[NUMBER_BUFFER_SIZE];
  int	 size;
  DUMA_ADDR  digit;

  do
  {
	if ( --s == buffer )
	  DUMA_Abort("Internal error printing number.");

	digit = number % base;
	*s = (char)( (digit < 10) ? ('0' + digit) : ('a' + digit -10) );

  } while ( (number /= base) > 0 );

  size = &buffer[NUMBER_BUFFER_SIZE] - s;
  buffer[NUMBER_BUFFER_SIZE] = '\0';
  strcpy(obuffer, s);
  return size;
}


/* Function: sprintLong
 *
 * internal function to print a int into a buffer
 */
static int sprintLong(char* obuffer, long number, long base)
{
  char	 buffer[NUMBER_BUFFER_SIZE+1];
  char * s = &buffer[NUMBER_BUFFER_SIZE];
  long	 size;
  long	 digit;

  do
  {
	if ( --s == buffer )
	  DUMA_Abort("Internal error printing number.");

	digit = number % base;
	*s = (char)( (digit < 10) ? ('0' + digit) : ('a' + digit -10) );

  } while ( (number /= base) > 0 );

  size = &buffer[NUMBER_BUFFER_SIZE] - s;
  buffer[NUMBER_BUFFER_SIZE] = '\0';
  strcpy(obuffer, s);
  return size;
}


/* Function: DUMA_sprintf
 *
 * internal function to print a formatted string into a buffer
 * int sprintf(char* buffer, const char *pattern, va_list args)
 * allowed format specifier are:
 *
 *	 %a = adress of type DUMA_ADDR
 *	 %x = adress of type DUMA_ADDR
 *	 %d = unsigned of type DUMA_SIZE
 *	 %i = int
 *	 %l = long
 *	 %s = string teminated with '\0'
 *	 %c = char
 */
static int DUMA_sprintf(char* buffer, const char *pattern, va_list args)
{
  char	  c;
  static const char  bad_pattern[] = "\nDUMA: Bad pattern specifier %%%c in DUMA_Print().\n";
  const char *	s = pattern;
  int len = 0;
  DUMA_ADDR n;

  c = *s++;
  while ( c )
  {
	if ( c == '%' )
	{
	  c = *s++;
	  switch ( c )
	  {
	  case '%':
		buffer[len++] = c;
		break;
	  case 'a':   /* DUMA_ADDR */
	  case 'x':   /* DUMA_ADDR */
		/*
		 * Print an address passed as a void pointer.
		 * The type of DUMA_ADDR must be set so that
		 * it is large enough to contain all of the
		 * bits of a void pointer.
		 */
		n = va_arg(args, DUMA_ADDR);
		len += sprintNumber(&buffer[len], n, 0x10);
		break;
	  case 'd':   /* DUMA_SIZE */
		n = va_arg(args, DUMA_SIZE);
		len += sprintNumber(&buffer[len], n, 10);
		break;
	  case 'i':   /* int */
		{
		  long n = (long)va_arg(args, int);
		  if ( n < 0 )
		  {
			buffer[len++] = '-';
			n = -n;
		  }
		  len += sprintLong(&buffer[len], n, 10);
		}
		break;
	  case 'l':   /* long */
		{
		  long n = va_arg(args, long);
		  if ( n < 0 )
		  {
			buffer[len++] = '-';
			n = -n;
		  }
		  len += sprintLong(&buffer[len], n, 10);
		}
		break;
	  case 's':   /* string */
		{
		  const char *	string;
		  size_t	length;

		  string = va_arg(args, char *);
		  if (string)
		  {
			length = strlen(string);
			strcpy(&buffer[len], string);
		  }
		  else
		  {
			length = 4; /* = strlen("NULL") */
			strcpy(&buffer[len], "NULL");
		  }
		  len += length;
		}
		break;
	  case 'c':   /* char */
		/* characters are passed as int ! */
		buffer[len++] = (char)va_arg(args, int);
		break;
	  default:
		DUMA_Print(bad_pattern, c);
	  }
	}
	else
	  buffer[len++] = c;
	
	c = *s++;
  }

  buffer[len] = '\0';
  return len;
}



/* Function: DUMA_Abort
 *
 * external abort function
 * on Visual C++ it additionally prints to Debug Output of the IDE
 * void DUMA_Abort(const char * pattern, ...)
 */
void
DUMA_Abort(const char * pattern, ...)
{
  char buffer[STRING_BUFFER_SIZE];
  int lena, lenb;
  va_list  args;
  va_start(args, pattern);

  strcpy(buffer, "\nDUMA Aborting: ");
  lena = strlen(buffer);
  lenb = DUMA_sprintf(&buffer[lena], pattern, args);
  strcat(buffer, "\n");
#ifdef _MSC_VER
  _RPT0(_CRT_WARN, buffer);
#endif
  DUMA_Print("%s", buffer);
  va_end(args);

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

#include <FCNTL.H>

/* Function: DUMA_Print
 *
 * external print function
 * on Visual C++ it additionally prints to Debug Output of the IDE
 * void DUMA_Print(const char * pattern, ...)
 */
void
DUMA_Print(const char * pattern, ...)
{
  char buffer[STRING_BUFFER_SIZE];
  int len;
  int fd;
  va_list  args;
  va_start(args, pattern);

  len = DUMA_sprintf(buffer, pattern, args);
#ifdef _MSC_VER
  _RPT0(_CRT_WARN, buffer);
#endif

#ifdef WIN32
	if(DUMA_OUTPUT_DEBUG)
	{
		OutputDebugString(buffer);
	}
#endif

	if(DUMA_OUTPUT_STDOUT)
	{
		write(1, buffer, len);
	}

	if(DUMA_OUTPUT_STDERR)
	{
		write(2, buffer, len);
	}

	if(DUMA_OUTPUT_FILE != NULL)
	{
		fd = _open(DUMA_OUTPUT_FILE, _O_APPEND|_O_CREAT|_O_WRONLY);
		write(fd, buffer, len);
		_close(fd);
	}

	va_end(args);
}


/* Function: DUMA_Exit
 *
 * external exit function
 * on Visual C++ it additionally prints to Debug Output of the IDE
 * void DUMA_Exit(const char * pattern, ...)
 */
void
DUMA_Exit(const char * pattern, ...)
{
  char buffer[STRING_BUFFER_SIZE];
  int lena, lenb;
  va_list  args;
  va_start(args, pattern);

  strcpy(buffer, "\nDUMA Exiting: ");
  lena = strlen(buffer);
  lenb = DUMA_sprintf(&buffer[lena], pattern, args);
  strcat(buffer, "\n");
#ifdef _MSC_VER
  _RPT0(_CRT_WARN, buffer);
#endif
  DUMA_Print("%s", buffer);
  va_end(args);

  /*
   * I use _exit() because the regular exit() flushes stdio,
   * which may cause malloc() or free() to be called.
   */
  _exit(-1);
}

/* end */
