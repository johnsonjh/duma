/* Test program for duma. Compile it like this:
g++ foo.cpp -o foo -pthread -ggdb -I /home/andrel/Desktop/duma_2_5_14_x86/ -L /home/andrel/Desktop/duma_2_5_14_x86 -l duma -lpthread
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <new>
#include <dumapp.h>

static void *threadfunc( void *null )
{
    for( int i=0; true; i++ )
    {
        unsigned char *foo = new unsigned char[60];
        if( foo )
        {
            void *copy = foo;
            fprintf(stdout, "%d: before delete of %p\n", pthread_self(), copy);
            fflush(stdout);
            delete[] foo;
            fprintf(stdout, "%d: after  delete of %p\n", pthread_self(), copy);
            fflush(stdout);
        }
    }
}

int main( int argc, char *argv[] )
{
    pthread_t t1, t2, t3;
    pthread_create( &t1, NULL, threadfunc, NULL );
    pthread_create( &t2, NULL, threadfunc, NULL );
    pthread_create( &t3, NULL, threadfunc, NULL );
    pthread_join( t1, NULL );
    pthread_join( t2, NULL );
    pthread_join( t3, NULL );
    return 0;
}
