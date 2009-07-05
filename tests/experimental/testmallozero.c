#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <regex.h>

char *res = "^[[:space:]]*([[:alpha:]]+)[[:space:]]*=[[:space:]]*((\"([^\"]*)\")|([^[:space:]]+))([[:space:]]|$)";

int main()
{
  regex_t rec;
  int result;
  result = regcomp(&rec, res, REG_EXTENDED);
  if (result != 0)
    printf("Error %d\n",result);
  return 0;
}

