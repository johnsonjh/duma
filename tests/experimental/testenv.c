
#include <stdio.h>

extern char **environ;

static
const char * duma_getenv( const char * varname )
{
  const char * ret = NULL;
  int varno = 0;

  if ( !varname )
    return ret;

  if ( varname[0] == '\0' )
    return ret;

  while ( environ[varno] )
  {
    const char * v = environ[varno++];
    int idx = 0;

    while ( varname[idx] != '\0' && v[idx] == varname[idx] )
      ++idx;

    if ( idx > 0 && varname[idx] == '\0' && v[idx] == '=' )
      return v + (idx +1);
  }
  return ret;
}


int main(int argc, const char * argv[] )
{
  int i = 0;
  const char * v;
  const char * varname;

#if 0
  printf("environment variables:\n");
  while ( environ[i] )
  {
    const char * v = environ[i];
    printf("%d: %s\n", i, v);
    ++i;
  }
#endif

  if ( argc < 2 )
  {
    fprintf(stderr, "Usage: testenv <varname\n");
    return 5;
  }

  varname = argv[1];

  v = duma_getenv(varname);
  if ( v )
    printf("%s is '%s'\n", varname, v);
  else
    printf("%s undefined\n", varname);

  return 0;
}
