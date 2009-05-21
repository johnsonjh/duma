// dumadll.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
BOOL APIENTRY DllMain( HANDLE hModule
                      , DWORD  ul_reason_for_call
                      , LPVOID lpReserved
                      )
{
  return TRUE;
}

#include "duma.h"
#define DUMA_PARAMS_UK          , "unknown file", 0

void * duma_malloc(size_t size)
{
  return _duma_malloc(size  DUMA_PARAMS_UK);
}

void * duma_calloc(size_t nelem, size_t elsize)
{
  return _duma_calloc(nelem, elsize  DUMA_PARAMS_UK);
}

void   duma_free(void * address)
{
  _duma_free(address  DUMA_PARAMS_UK);
}

void * duma_memalign(size_t alignment, size_t size)
{
  return _duma_memalign(alignment, size  DUMA_PARAMS_UK);
}


void * duma_realloc(void * oldBuffer, size_t newSize)
{
  return _duma_realloc(oldBuffer, newSize  DUMA_PARAMS_UK);
}


void * duma_valloc(size_t size)
{
  return _duma_valloc(size  DUMA_PARAMS_UK);
}


char * duma_strdup(const char * str)
{
  return _duma_strdup(str  DUMA_PARAMS_UK);
}


void * duma_memcpy(void *dest, const void *src, size_t size)
{
  return _duma_memcpy(dest, src, size  DUMA_PARAMS_UK);
}


void * duma_memmove(void *dest, const void *src, size_t size)
{
  return _duma_memmove(dest, src, size);
}


char * duma_strcpy(char *dest, const char *src)
{
  return _duma_strcpy(dest, src  DUMA_PARAMS_UK);
}


char * duma_strncpy(char *dest, const char *src, size_t size)
{
  return _duma_strncpy(dest, src, size  DUMA_PARAMS_UK);
}


char * duma_strcat(char *dest, const char *src)
{
  return _duma_strcat(dest, src  DUMA_PARAMS_UK);
}


char * duma_strncat(char *dest, const char *src, size_t size)
{
  return _duma_strncat(dest, src, size  DUMA_PARAMS_UK);
}

// end
