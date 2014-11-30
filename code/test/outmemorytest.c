




#include "syscall.h"

int A[100000];	/* size of physical memory; with code, we'll run out of space!*/
			
int
main()
{
    int i, j, tmp;

    /* first initialize the array, in reverse sorted order */
    for (i = 0; i < 100000; i++) {
        A[i] = 1023 - i;
    }


    Exit(A[0]);		/* and then we're done -- should be 0! */
}
