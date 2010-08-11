
/* vim: set sw=4 sts=4 : */

/* 
 * Copyright (c) 2009, David B. Cortarello
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the David B. Cortarello.
 * 4. Neither the name of the David B. Cortarello nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DAVID B. CORTARELLO ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL DAVID B. CORTARELLO BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include "dmemory.h"

int main(int argc, char *argv[])
{
	int *ptr = NULL;
	int i, *x;
	char *ss = (char *)&i;

	/* Initialice the memory library and set to full debug. */
	dmemory_init(2);
	dmemory_init(-1);

	/* Let's ask for some memory with calloc. */
	ptr = calloc(10, sizeof(int));

	/* Initialize the address space just created. */
	for (i = 0; i < 10; i++)
		ptr[i] = i;

	/* Since calloc didn't failed, realloc is sane. */
	ptr = realloc(ptr, 15 * sizeof(int));

	/* Initialize the new address space with a small a memory corruption. */
	for (i = 10; i <= 15; i++)
		ptr[i] = i;

	/* Show it... you know, for the kids, they like I/O. */
	for (i = 0; i < 15; i++)
		printf("%d: %d\n", i, ptr[i]);
	/* We never call free on this one, to create a leak. */

	/* Since ss points to 'i', but it wasn't malloc'ed this will not issue a 
	 * warning and will not be reported in the final report. So the usage of 
	 * malloc is pretty straightforward. */
	ss = malloc(sizeof(char) * 14);
	sprintf(ss, "Hello world!");
	puts(ss);
	/* Let's not create a memory leak out of this so it's not shown in the 
	 * final report. */
	free(ss);
	/* Let's create a "double free" known error on this one */
	free(ss);

	ss = malloc(sizeof(char) * 14);
	sprintf(ss, "Dirty pointer and too big");
	puts(ss);
	/* free(ss); */
	/* If free was uncommented up there everything would be fine.  But this 
	 * is not the case, ss was malloc'ed but never freed, so it remains 
	 * registered in the library stack. */

	free((void *)0x123);
	/* On purpose of this demostration we free a memory address that was 
	 * never reserved, so this normally should lead us to a BUS ERROR 
	 * (SIGBUS). But now this is handled */

	if (dmemory_end())
		return 1;

	return 0;
}
