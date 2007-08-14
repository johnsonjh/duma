
/*
 * DUMA - Red-Zone memory allocator.
 * Copyright (C) 2002-2005 Hayati Ayguen <h_ayguen@web.de>, Procitec GmbH
 * License: GNU LGPL (GNU Lesser General Public License, see COPYING-GPL)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * FILE CONTENTS:
 * example showing how to implement member new/delete operators
 * the implementation might not be the best, but you get a clou which
 *   operators need to be written.
 *
 */

#include <stdio.h>

#include <new>

#include "dumapp.h"
#include "noduma.h"


class optest
{
public:
  /* 1x : SINGLE OBJECT FORM - NO DEBUG INFORMATION */
  void * operator new( DUMA_SIZE_T )                            throw(std::bad_alloc);
  void * operator new( DUMA_SIZE_T , const std::nothrow_t & )   throw();
  void   operator delete( void * )                              throw();
  void   operator delete( void * , const std::nothrow_t & )     throw();

  /* 2x : ARRAY OBJECT FORM - NO DEBUG INFORMATION */
  void * operator new[]( DUMA_SIZE_T )                          throw(std::bad_alloc);
  void * operator new[]( DUMA_SIZE_T , const std::nothrow_t & ) throw();
  void   operator delete[]( void * )                            throw();
  void   operator delete[]( void *, const std::nothrow_t & )    throw();

#ifndef DUMA_NO_LEAKDETECTION
  /* 3x : SINGLE OBJECT FORM - WITH DEBUG INFORMATION */
  void * operator new( DUMA_SIZE_T, const char *, int )                         throw( std::bad_alloc );
  void * operator new( DUMA_SIZE_T, const std::nothrow_t &, const char *, int ) throw();
  void   operator delete( void *, const char *, int )                           throw();
  void   operator delete( void *, const std::nothrow_t &, const char *, int )   throw();

  /* 4x : ARRAY OBJECT FORM - WITH DEBUG INFORMATION */
  void * operator new[]( DUMA_SIZE_T, const char *, int )                         throw( std::bad_alloc );
  void * operator new[]( DUMA_SIZE_T, const std::nothrow_t &, const char *, int ) throw();
  void   operator delete[]( void *, const char *, int )                           throw();
  void   operator delete[]( void *, const std::nothrow_t &, const char *, int )   throw();
#endif

private:
  int dummy;
};


/* 1x : SINGLE OBJECT FORM - NO DEBUG INFORMATION */

void * optest::operator new( DUMA_SIZE_T s )
throw(std::bad_alloc)
{
  return ::new optest;
}


void * optest::operator new( DUMA_SIZE_T s, const std::nothrow_t & n )
throw()
{
  return ::new(n) optest;
}


void   optest::operator delete( void * p )
throw()
{
  ::operator delete ((optest*)p);
}


void   optest::operator delete( void * p, const std::nothrow_t & n )
throw()
{
  ::operator delete((optest*)p, n);
}


/* 2x : ARRAY OBJECT FORM - NO DEBUG INFORMATION */
void * optest::operator new[]( DUMA_SIZE_T s )
throw(std::bad_alloc)
{
  return ::new optest[ s / sizeof(optest) ];    // "s / sizeof()" not correct but works for this test
}

void * optest::operator new[]( DUMA_SIZE_T s, const std::nothrow_t & n )
throw()
{
  return ::new(n) optest[ s / sizeof(optest) ]; // "s / sizeof()" not correct but works for this test
}

void   optest::operator delete[]( void * p )
throw()
{
  ::operator delete []((optest*)p);
}

void   optest::operator delete[]( void * p, const std::nothrow_t & n )
throw()
{
  ::operator delete[]((optest*)p, n);
}


#ifndef DUMA_NO_LEAKDETECTION

/* 3x : SINGLE OBJECT FORM - WITH DEBUG INFORMATION */
void * optest::operator new( DUMA_SIZE_T s, const char * f, int l )
throw( std::bad_alloc )
{
  return ::new(f,l) optest;
}

void * optest::operator new( DUMA_SIZE_T s, const std::nothrow_t & n, const char * f, int l )
throw()
{
  return ::new(n,f,l) optest;
}

void   optest::operator delete( void * p, const char * f, int l )
throw()
{
  ::operator delete((optest*)p, f,l);
}

void   optest::operator delete( void * p, const std::nothrow_t & n, const char * f, int l )
throw()
{
  ::operator delete((optest*)p, n, f,l);
}


/* 4x : ARRAY OBJECT FORM - WITH DEBUG INFORMATION */
void * optest::operator new[]( DUMA_SIZE_T s, const char * f, int l )
throw( std::bad_alloc )
{
  return ::new(f,l) optest[s / sizeof(optest)];     // "s / sizeof()" not correct but works for this test
}

void * optest::operator new[]( DUMA_SIZE_T s, const std::nothrow_t & n, const char * f, int l )
throw()
{
  return ::new(n, f,l) optest[s / sizeof(optest)];  // "s / sizeof()" not correct but works for this test
}

void   optest::operator delete[]( void * p, const char * f, int l )
throw()
{
  ::operator delete[]((optest*)p, f,l);
}

void   optest::operator delete[]( void * p, const std::nothrow_t & n, const char * f, int l )
throw()
{
  ::operator delete[]((optest*)p, n, f,l);
}

#endif /* DUMA_NO_LEAKDETECTION */



void pure_test()
{
/* how to call the operators without any DUMA macros defined */
#include "noduma.h"

  optest * s, * v;

  s = new optest;
  delete s;

  s = new(std::nothrow) optest;
  delete s;

  v = new optest[0];
  delete []v;

  v = new optest[10];
  delete []v;

  v = new(std::nothrow) optest[10];
  delete []v;
}


void rich_test()
{
/* how to call the operators with having DUMA macros defined */
#include "dumapp.h"

  optest * s, * v;

  s = new optest;
  delete s;

#include "noduma.h"
  #ifndef DUMA_NO_LEAKDETECTION
    s = new(std::nothrow,__FILE__,__LINE__) optest;
  #else
    s = new(std::nothrow) optest;
  #endif
#include "dumapp.h"
  delete s;


  v = new optest[0];
  delete []v;

  v = new optest[10];
  delete []v;

#include "noduma.h"
  #ifndef DUMA_NO_LEAKDETECTION
    v = new(std::nothrow,__FILE__,__LINE__) optest[10];
  #else
    v = new(std::nothrow) optest[10];
  #endif
#include "dumapp.h"
  delete []v;
}



int main( int argc, char *argv[] )
{
  pure_test();
  rich_test();
  return 0;
}

