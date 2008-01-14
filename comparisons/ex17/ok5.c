struct Test {
  int a;
  char st[10];
};

int main() {
  printf("Hello world!\n");

  struct Test* ar = (struct Test*)malloc(10*sizeof(struct Test));
  ar[9].a=10;
  printf("Let's correctly delete an array of 10 objects\n");
  free(ar);
  printf("Done\n");
  
  printf("There should be 0 error in this run\n");
  return 0;
}
