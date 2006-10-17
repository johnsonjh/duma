
// Print a stacktrace on i386 with
// signal support (on Linux at least)
// Does not do symbol lookup.
// need to write binutils map file
// parser, or steal code from
// addr2line
void printStackTrace(void)
{
	volatile void *firstvar;
	char *frame, *pf, *eip;

	frame = ((char *)(&firstvar)) + 4;
	while(1)
	{
		eip = (char *)(*(int *)(frame + 4));
		if(!memcmp(eip, "\x58\xb8\x77\x00\x00\x00\xcd\x80", 8))
		{
			printf("Signal hander for signal %i\n", *(int *)(frame + 8));
			pf = (char *)(*(int *)(frame + 36));
			eip = (char *)(*(int *)(frame + 68));
		}
		else
		{
			pf = (char *)(*(int *)frame);
		}
		
		printf("Prev frame: %p, Prev EIP: %p\n", pf, eip);
		
		frame = pf;
		if(frame == NULL)
			break;
	}
}

// end
