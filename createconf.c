
/*
 * DUMA - Red-Zone memory allocator.
 *
 * Copyright (C) 2006 Michael Eddington <meddington@gmail.com>
 * Copyright (C) 2002-2008 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
 * Copyright (C) 1987-1999 Bruce Perens <bruce@perens.com>
 *
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
 * --------------
 * This is a small tool to generate the "duma_config.h" configuration file.
 * Its purpose is to allow fixed size memory allocation on stack, which can
 * get protected calling page protection functions.
 * Also its nice for the user to be able to see the page size.
 */

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <stdarg.h>

#ifdef _MSC_VER
#include <direct.h>
#endif

#ifndef WIN32
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#else
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <winbase.h>

#ifndef __CYGWIN__
  /* already defined in cygwin headers */
  typedef LPVOID caddr_t;
  typedef unsigned u_int;
#endif

#endif


/*
* retrieve page size
* size_t  Page_Size(void)
*/
static size_t
Page_Size(void)
{
#if defined(WIN32)
  SYSTEM_INFO SystemInfo;
  GetSystemInfo( &SystemInfo );
  return (size_t)SystemInfo.dwPageSize;
#elif defined(_SC_PAGESIZE)
  return (size_t)sysconf(_SC_PAGESIZE);
#elif defined(_SC_PAGE_SIZE)
  return (size_t)sysconf(_SC_PAGE_SIZE);
#else
  /* extern int getpagesize(); */
  return getpagesize();
#endif
}

int initattr_ok = 0;

void
#ifdef __GNUC__
__attribute ((constructor))
#endif
init_function(void)
{
  initattr_ok = 1;
}



typedef struct
{
  int  id;
  int  size;
  char *type;
}
DataType_T;


DataType_T  sIntTypes[] =
{
    { 0, sizeof(char)     , "char"      }
  , { 1, sizeof(short int), "short int" }
  , { 2, sizeof(int)      , "int"       }
  , { 3, sizeof(long int) , "long int"  }
  /* add additional compiler specific typer here */
#ifdef _MSC_VER
  , { 4, sizeof(__int64), "__int64"     }
#endif
#ifdef __GNUC__
  , { 5, sizeof(long long int), "long long int" }
#endif
};


/* test access to each data type */
void testAlignment(int addrIdx, char * buffer, int alignment, int max_sizeof)
{
  int offset;

  switch ( sIntTypes[addrIdx].id )
  {
  case 0:
#define TYPE unsigned char
    for(offset=0; offset < max_sizeof; ++offset)
    {
      TYPE addr = (TYPE)(buffer) + offset;
      if ( addr % alignment == 0 )
      {
        *( (unsigned char *)     addr ) = 0;
        *( (unsigned short int*) addr ) = 0;
        *( (unsigned int *)      addr ) = 0;
        *( (unsigned long int *) addr ) = 0L;
        *( (float *)             addr ) = 0.0F;
        *( (double *)            addr ) = 0.0;
        *( (long double *)       addr ) = 0.0;
#ifdef _MSC_VER
        *( (unsigned __int64 *) addr ) = 0L;
#endif
#ifdef __GNUC__
        *( (unsigned long long int *) addr ) = 0L;
#endif
      }
    }
    break;
  case 1:
#undef TYPE
#define TYPE unsigned short int
    for(offset=0; offset < max_sizeof; ++offset)
    {
      TYPE addr = (TYPE)(buffer) + offset;
      if ( addr % alignment == 0 )
      {
        *( (unsigned char *)     addr ) = 0;
        *( (unsigned short int*) addr ) = 0;
        *( (unsigned int *)      addr ) = 0;
        *( (unsigned long int *) addr ) = 0L;
        *( (float *)             addr ) = 0.0F;
        *( (double *)            addr ) = 0.0;
        *( (long double *)       addr ) = 0.0;
#ifdef _MSC_VER
        *( (unsigned __int64 *) addr ) = 0L;
#endif
#ifdef __GNUC__
        *( (unsigned long long int *) addr ) = 0L;
#endif
      }
    }
    break;
  case 2:
#undef TYPE
#define TYPE unsigned int
    for(offset=0; offset < max_sizeof; ++offset)
    {
      TYPE addr = (TYPE)(buffer) + offset;
      if ( addr % alignment == 0 )
      {
        *( (unsigned char *)     addr ) = 0;
        *( (unsigned short int*) addr ) = 0;
        *( (unsigned int *)      addr ) = 0;
        *( (unsigned long int *) addr ) = 0L;
        *( (float *)             addr ) = 0.0F;
        *( (double *)            addr ) = 0.0;
        *( (long double *)       addr ) = 0.0;
#ifdef _MSC_VER
        *( (unsigned __int64 *) addr ) = 0L;
#endif
#ifdef __GNUC__
        *( (unsigned long long int *) addr ) = 0L;
#endif
      }
    }
    break;
  case 3:
#undef TYPE
#define TYPE unsigned long int
    for(offset=0; offset < max_sizeof; ++offset)
    {
      TYPE addr = (TYPE)(buffer) + offset;
      if ( addr % alignment == 0 )
      {
        *( (unsigned char *)     addr ) = 0;
        *( (unsigned short int*) addr ) = 0;
        *( (unsigned int *)      addr ) = 0;
        *( (unsigned long int *) addr ) = 0L;
        *( (float *)             addr ) = 0.0F;
        *( (double *)            addr ) = 0.0;
        *( (long double *)       addr ) = 0.0;
#ifdef _MSC_VER
        *( (unsigned __int64 *) addr ) = 0L;
#endif
#ifdef __GNUC__
        *( (unsigned long long int *) addr ) = 0L;
#endif
      }
    }
    break;
#ifdef _MSC_VER
  case 4:
#undef TYPE
#define TYPE unsigned __int64
    for(offset=0; offset < max_sizeof; ++offset)
    {
      TYPE addr = (TYPE)(buffer) + offset;
      if ( addr % alignment == 0 )
      {
        *( (unsigned char *)     addr ) = 0;
        *( (unsigned short int*) addr ) = 0;
        *( (unsigned int *)      addr ) = 0;
        *( (unsigned long int *) addr ) = 0L;
        *( (float *)             addr ) = 0.0F;
        *( (double *)            addr ) = 0.0;
        *( (long double *)       addr ) = 0.0;
#ifdef _MSC_VER
        *( (unsigned __int64 *) addr ) = 0L;
#endif
#ifdef __GNUC__
        *( (unsigned long long int *) addr ) = 0L;
#endif
      }
    }
    break;
#endif
#ifdef __GNUC__
  case 5:
#undef TYPE
#define TYPE unsigned long long int
    for(offset=0; offset < max_sizeof; ++offset)
    {
      TYPE addr = (TYPE)(buffer) + offset;
      if ( addr % alignment == 0 )
      {
        *( (unsigned char *)     addr ) = 0;
        *( (unsigned short int*) addr ) = 0;
        *( (unsigned int *)      addr ) = 0;
        *( (unsigned long int *) addr ) = 0L;
        *( (float *)             addr ) = 0.0F;
        *( (double *)            addr ) = 0.0;
        *( (long double *)       addr ) = 0.0;
#ifdef _MSC_VER
        *( (unsigned __int64 *) addr ) = 0L;
#endif
#ifdef __GNUC__
        *( (unsigned long long int *) addr ) = 0L;
#endif
      }
    }
    break;
#endif
    ;
  }
}



void writeFile(const char * filename, unsigned long pagesize, int addrIdx, int sizeIdx, int alignment, int malloc0strategy)
{
  FILE * f = fopen(filename, "w");
  if (!f)
  {
    fprintf(stderr, "createconf: Error: could not open file '%s'\n", filename);
    return;
  }

  fprintf(f, "/*\n");
  fprintf(f, " * WARNING: DO NOT CHANGE THIS FILE!\n");
  fprintf(f, " * This file is automatically generated from createconf\n");

#if defined(WIN32)
#if defined(_MSC_VER)
  fprintf(f, " * using Microsoft Visual C++ under MS Windows(TM) the %s\n", __DATE__ );
#else
  fprintf(f, " * under MS Windows(TM) the %s\n", __DATE__ );
#endif
#elif ( defined(sgi) )
  fprintf(f, " * under sgi the %s\n", __DATE__ );
#elif ( defined(_AIX) )
  fprintf(f, " * under AIX the %s\n", __DATE__ );
#elif ( defined(__linux__) )
  fprintf(f, " * under Linux the %s\n", __DATE__ );
#else
  fprintf(f, " * the %s \n", __DATE__ );
#endif

  fprintf(f, " */\n");
  fprintf(f, "\n");
  fprintf(f, "#ifndef _DUMA_CONFIG_H_\n");
  fprintf(f, "#define _DUMA_CONFIG_H_\n");

  fprintf(f, "\n");
  fprintf(f, "/*\n");
  fprintf(f, " * Configuration of DUMA:\n");
  fprintf(f, " */\n");

  // DUMA_SO_LIBRARY

  fprintf(f, "#ifdef DUMA_SO_LIBRARY\n");
  fprintf(f, "\n");

  fprintf(f, "#ifdef DUMA_NO_GLOBAL_MALLOC_FREE\n");
  fprintf(f, "#undef DUMA_NO_GLOBAL_MALLOC_FREE\n");
  fprintf(f, "#endif\n\n");

  fprintf(f, "#ifdef DUMA_EXPLICIT_INIT\n");
  fprintf(f, "#undef DUMA_EXPLICIT_INIT\n");
  fprintf(f, "#endif\n\n");

  fprintf(f, "#ifdef DUMA_NO_THREAD_SAFETY\n");
  fprintf(f, "#undef DUMA_NO_THREAD_SAFETY\n");
  fprintf(f, "#endif\n\n");

#ifndef DUMA_SEMAPHORES
  fprintf(f, "#ifdef DUMA_SEMAPHORES\n");
  fprintf(f, "#undef DUMA_SEMAPHORES\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_SEMAPHORES\n");
  fprintf(f, "#define DUMA_SEMAPHORES\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_NO_CPP_SUPPORT
  fprintf(f, "#ifdef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#undef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#define DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_NO_LEAKDETECTION
  fprintf(f, "#ifdef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#undef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#define DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_PREFER_ATEXIT
  fprintf(f, "#ifdef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#undef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#define DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_PREFER_GETENV
  fprintf(f, "#ifdef DUMA_PREFER_GETENV\n");
  fprintf(f, "#undef DUMA_PREFER_GETENV\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_PREFER_GETENV\n");
  fprintf(f, "#define DUMA_PREFER_GETENV\n");
  fprintf(f, "#endif\n\n");
#endif

  fprintf(f, "#ifdef DUMA_OLD_NEW_MACRO\n");
  fprintf(f, "#undef DUMA_OLD_NEW_MACRO\n");
  fprintf(f, "#endif\n\n");

  fprintf(f, "#ifdef DUMA_OLD_DEL_MACRO\n");
  fprintf(f, "#undef DUMA_OLD_DEL_MACRO\n");
  fprintf(f, "#endif\n\n");

#ifndef DUMA_NO_STRERROR
  fprintf(f, "#ifdef DUMA_NO_STRERROR\n");
  fprintf(f, "#undef DUMA_NO_STRERROR\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_STRERROR\n");
  fprintf(f, "#define DUMA_NO_STRERROR\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_NO_HANG_MSG
  fprintf(f, "#ifdef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#undef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#define DUMA_NO_HANG_MSG\n");
  fprintf(f, "#endif\n\n");
#endif

  // DUMA_DLL_LIBRARY

  fprintf(f, "#elif defined(DUMA_DLL_LIBRARY)\n");
  fprintf(f, "\n");

  fprintf(f, "#define DUMA_SKIP_SETUP 1\n");
  fprintf(f, "#define DUMA_NO_GLOBAL_MALLOC_FREE 1\n");
  fprintf(f, "#define DUMA_EXPLICIT_INIT 1\n");

  fprintf(f, "#ifdef DUMA_NO_THREAD_SAFETY\n");
  fprintf(f, "#undef DUMA_NO_THREAD_SAFETY\n");
  fprintf(f, "#endif\n\n");

#ifndef DUMA_SO_NO_CPP_SUPPORT
  fprintf(f, "#ifdef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#undef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#define DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_NO_LEAKDETECTION
  fprintf(f, "#ifdef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#undef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#define DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_PREFER_ATEXIT
  fprintf(f, "#ifdef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#undef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#define DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_PREFER_GETENV
  fprintf(f, "#ifdef DUMA_PREFER_GETENV\n");
  fprintf(f, "#undef DUMA_PREFER_GETENV\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_PREFER_GETENV\n");
  fprintf(f, "#define DUMA_PREFER_GETENV\n");
  fprintf(f, "#endif\n\n");
#endif

  fprintf(f, "#ifdef DUMA_OLD_NEW_MACRO\n");
  fprintf(f, "#undef DUMA_OLD_NEW_MACRO\n");
  fprintf(f, "#endif\n\n");

  fprintf(f, "#ifdef DUMA_OLD_DEL_MACRO\n");
  fprintf(f, "#undef DUMA_OLD_DEL_MACRO\n");
  fprintf(f, "#endif\n\n");

#ifndef DUMA_SO_NO_HANG_MSG
  fprintf(f, "#ifdef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#undef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#define DUMA_NO_HANG_MSG\n");
  fprintf(f, "#endif\n\n");
#endif

  // DUMA_DETOURS

  fprintf(f, "#elif defined(DUMA_DETOURS)\n");
  fprintf(f, "\n");

  fprintf(f, "#define DUMA_SKIP_SETUP 1\n");
  fprintf(f, "#define DUMA_NO_GLOBAL_MALLOC_FREE 1\n");
  fprintf(f, "#define DUMA_EXPLICIT_INIT 1\n");

  fprintf(f, "#ifdef DUMA_NO_THREAD_SAFETY\n");
  fprintf(f, "#undef DUMA_NO_THREAD_SAFETY\n");
  fprintf(f, "#endif\n\n");

#ifndef DUMA_SO_NO_CPP_SUPPORT
  fprintf(f, "#ifdef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#undef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#define DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_NO_LEAKDETECTION
  fprintf(f, "#ifdef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#undef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#define DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_PREFER_ATEXIT
  fprintf(f, "#ifdef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#undef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#define DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_SO_PREFER_GETENV
  fprintf(f, "#ifdef DUMA_PREFER_GETENV\n");
  fprintf(f, "#undef DUMA_PREFER_GETENV\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_PREFER_GETENV\n");
  fprintf(f, "#define DUMA_PREFER_GETENV\n");
  fprintf(f, "#endif\n\n");
#endif

  fprintf(f, "#ifdef DUMA_OLD_NEW_MACRO\n");
  fprintf(f, "#undef DUMA_OLD_NEW_MACRO\n");
  fprintf(f, "#endif\n\n");

  fprintf(f, "#ifdef DUMA_OLD_DEL_MACRO\n");
  fprintf(f, "#undef DUMA_OLD_DEL_MACRO\n");
  fprintf(f, "#endif\n\n");

#ifndef DUMA_SO_NO_HANG_MSG
  fprintf(f, "#ifdef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#undef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#define DUMA_NO_HANG_MSG\n");
  fprintf(f, "#endif\n\n");
#endif

  // REGULAR CONFIG

  fprintf(f, "#else\n");
  fprintf(f, "\n");

#ifndef DUMA_NO_GLOBAL_MALLOC_FREE
  fprintf(f, "#ifdef DUMA_NO_GLOBAL_MALLOC_FREE\n");
  fprintf(f, "#undef DUMA_NO_GLOBAL_MALLOC_FREE\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_GLOBAL_MALLOC_FREE\n");
  fprintf(f, "#define DUMA_NO_GLOBAL_MALLOC_FREE\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_EXPLICIT_INIT
  fprintf(f, "#ifdef DUMA_EXPLICIT_INIT\n");
  fprintf(f, "#undef DUMA_EXPLICIT_INIT\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_EXPLICIT_INIT\n");
  fprintf(f, "#define DUMA_EXPLICIT_INIT\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_NO_THREAD_SAFETY
  fprintf(f, "#ifdef DUMA_NO_THREAD_SAFETY\n");
  fprintf(f, "#undef DUMA_NO_THREAD_SAFETY\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_THREAD_SAFETY\n");
  fprintf(f, "#define DUMA_NO_THREAD_SAFETY\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_LIB_NO_CPP_SUPPORT
  fprintf(f, "#ifdef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#undef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#define DUMA_NO_CPP_SUPPORT\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_LIB_NO_LEAKDETECTION
  fprintf(f, "#ifdef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#undef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#define DUMA_NO_LEAKDETECTION\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_LIB_PREFER_ATEXIT
  fprintf(f, "#ifdef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#undef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#define DUMA_PREFER_ATEXIT\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_LIB_PREFER_GETENV
  fprintf(f, "#ifdef DUMA_PREFER_GETENV\n");
  fprintf(f, "#undef DUMA_PREFER_GETENV\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_PREFER_GETENV\n");
  fprintf(f, "#define DUMA_PREFER_GETENV\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_OLD_NEW_MACRO
  fprintf(f, "#ifdef DUMA_OLD_NEW_MACRO\n");
  fprintf(f, "#undef DUMA_OLD_NEW_MACRO\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_OLD_NEW_MACRO\n");
  fprintf(f, "#define DUMA_OLD_NEW_MACRO\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_OLD_DEL_MACRO
  fprintf(f, "#ifdef DUMA_OLD_DEL_MACRO\n");
  fprintf(f, "#undef DUMA_OLD_DEL_MACRO\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_OLD_DEL_MACRO\n");
  fprintf(f, "#define DUMA_OLD_DEL_MACRO\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_NO_STRERROR
  fprintf(f, "#ifdef DUMA_NO_STRERROR\n");
  fprintf(f, "#undef DUMA_NO_STRERROR\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_STRERROR\n");
  fprintf(f, "#define DUMA_NO_STRERROR\n");
  fprintf(f, "#endif\n\n");
#endif

#ifndef DUMA_LIB_NO_HANG_MSG
  fprintf(f, "#ifdef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#undef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#endif\n\n");
#else
  fprintf(f, "#ifndef DUMA_NO_HANG_MSG\n");
  fprintf(f, "#define DUMA_NO_HANG_MSG\n");
  fprintf(f, "#endif\n\n");
#endif


  fprintf(f, "#endif\n");
  fprintf(f, "\n");

  fprintf(f, "\n");
  fprintf(f, "/*\n");
  fprintf(f, " * Number of bytes per virtual-memory page, as returned by Page_Size().\n");
  fprintf(f, " */\n");
  fprintf(f, "#define DUMA_PAGE_SIZE %lu\n", pagesize);

  fprintf(f, "\n");
  fprintf(f, "/*\n");
  fprintf(f, " * Minimum required alignment by CPU.\n");
  fprintf(f, " */\n");
  fprintf(f, "#define DUMA_MIN_ALIGNMENT %d\n", alignment);

  fprintf(f, "\n");
  fprintf(f, "/*\n");
  fprintf(f, " * Build environment supports the '__attribute ((constructor))'?\n");
  fprintf(f, " */\n");
  if (initattr_ok)
    fprintf(f, "#define DUMA_GNU_INIT_ATTR 1\n");
  else
    fprintf(f, "/* #define DUMA_GNU_INIT_ATTR 0 */\n");

  fprintf(f, "\n");
  fprintf(f, "/*\n");
  fprintf(f, " * An integer type with same size as 'void *'\n");
  fprintf(f, " */\n");
  if (addrIdx >= 0)
    fprintf(f, "typedef unsigned %s DUMA_ADDR;\n", sIntTypes[addrIdx].type);
  else
    fprintf(f, "/* Error: No datatype for DUMA_ADDR found! */\n");

  fprintf(f, "\n");
  fprintf(f, "/*\n");
  fprintf(f, " * An integer type with same size as 'size_t'\n");
  fprintf(f, " */\n");
  if (sizeIdx >= 0)
    fprintf(f, "typedef unsigned %s DUMA_SIZE;\n", sIntTypes[sizeIdx].type);
  else
    fprintf(f, "/* No datatype for DUMA_SIZE found! */\n");


  fprintf(f, "\n");
  fprintf(f, "/*\n");
  fprintf(f, " * Default behaviour on malloc(0).\n");
  fprintf(f, " */\n");
  fprintf(f, "#define DUMA_DEFAULT_MALLOC_0_STRATEGY %d\n", malloc0strategy);

  fprintf(f, "\n");
  fprintf(f, "#endif /* _DUMA_CONFIG_H_ */\n");

  fclose(f);
}



#ifdef __cplusplus
  #define DUMA_EXTERN_C   extern "C"
#else
  #define DUMA_EXTERN_C   extern
#endif



int main()
{
  int iNumIntTypes, iIt;
  int addrIdx, sizeIdx;
  int alignment, max_sizeof;
  int malloc0strategy;
  char  buffer[1024];
  char filename[1024];

  unsigned long pagesize = Page_Size();

#if defined(__MINGW32__) || defined(__MINGW64__)
  fprintf(stderr, "createconf: Platform is MINGW\n");
#endif
#if defined(__CYGWIN__)
  fprintf(stderr, "createconf: Platform is CYGWIN\n");
#endif
#if defined(WIN32)
  fprintf(stderr, "createconf: WIN32 is set\n");
#endif
#if defined(_MSC_VER)
  fprintf(stderr, "createconf: Compiler is MS Visual C++ (_MSC_VER set)\n");
#endif

  iNumIntTypes = sizeof( sIntTypes ) / sizeof( sIntTypes[0] );

  // detect compatible type for ADDRESS
  addrIdx = -1;
  for (iIt = 0; iIt < iNumIntTypes; ++iIt)
  {
    if ( sizeof(void*) == sIntTypes[iIt].size )
    {
      addrIdx = iIt;
      break;
    }
  }

  // detect compatible type for SIZE_T
  sizeIdx = -1;
  for (iIt = 0; iIt < iNumIntTypes; ++iIt)
  {
    if ( sizeof(size_t) == sIntTypes[iIt].size )
    {
      sizeIdx = iIt;
      break;
    }
  }

  /* detect maximum data type, which should be maximum alignment */
  max_sizeof = 0;
  for (iIt = 0; iIt < iNumIntTypes; ++iIt)
  {
    if ( max_sizeof < sIntTypes[iIt].size )
      max_sizeof = sIntTypes[iIt].size;
  }
  if ( max_sizeof < (int)sizeof(float) )
    max_sizeof = sizeof(float);
  if ( max_sizeof < (int)sizeof(double) )
    max_sizeof = sizeof(double);
  if ( max_sizeof < (int)sizeof(long double) )
    max_sizeof = sizeof(long double);

  /* test for behaviour on malloc(0) */
  {
    char * pcNullPtrA = (char*)malloc(0);
    char * pcNullPtrB = (char*)malloc(0);
    if ( !pcNullPtrA )
      malloc0strategy = 1;
    else if ( pcNullPtrA == pcNullPtrB )
      malloc0strategy = 2;
    else
      malloc0strategy = 3;
  }

#ifdef _MSC_VER
  {
    /* fix output path when started form Visual C++ IDE */
    char  directory[1024];
    char  * start;

    _getcwd( directory, 1024 ); /* get current directory */
    if ( ( start = strstr( directory, "win32-msvc" ) ) )
    {
      *start = 0;
      strcpy(filename, directory);
      strcat(filename, "duma_config.h");
    }
    else
      strcpy( filename, "duma_config.h" );
  }
#else
  strcpy( filename, "duma_config.h" );
#endif

  /* detect minimum alignment */
  alignment = max_sizeof;
  do
  {
    /* do the alignment access tests */
    testAlignment(addrIdx, buffer, alignment, max_sizeof);
    /* write whole config file. next test may crash ! */
    writeFile(filename, pagesize, addrIdx, sizeIdx, alignment, malloc0strategy);
    /* try next lower alignment */
    alignment >>= 1;
  }
  while ( alignment > 0 );

  return 0;
}

