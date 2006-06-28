#include "../printStackTrace.h"
#include <stdio.h>

//-----------------------------------------------------------------------------

#define testAssert( EXPR ) if (EXPR) {} else {testAssertFailed(__FILE__,__LINE__,#EXPR);}

//-----------------------------------------------------------------------------

void testAssertFailed( const char* fname, int line, const char* expr )
{
	char buf[1000];
	printStackTrace( buf, sizeof(buf) );

	printf( "Test assertion: %s(%i)\n", fname, line );
	printf( "Expression: %s\n", expr );
	printf( "Stack trace:\n" );
	puts( buf );
}

//-----------------------------------------------------------------------------

void testfunc3( int x )
{
	testAssert( x == 3 );
}

void testfunc2( int arg1 )
{
	testfunc3( arg1 );
}

void testfunc1( int arg1 )
{
	testfunc2( arg1 );
}

int main()
{
	testfunc1( 4 );
	puts( "Press enter to exit" );
	getchar();
	return 0;
}
