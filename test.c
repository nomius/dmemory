
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

#include "memory.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int *ptr = NULL;
    int i, *x;
    char *ss = (char *)&i;

    /* Initialice debug so full level is used */
    memory_init(5);

    /* ptr points to NULL, so xmalloc is sane */
    ptr = xmalloc(ptr, sizeof(int) * 10);

    /* Initialize the address space just created */
    for (i = 0; i < 10; i++)
        ptr[i] = i;

    /* since xmalloc didn't failed, xrealloc is a good choice */
    ptr = xrealloc(ptr, 15 * sizeof(int));

    /* Initialize the new address space */
    for (i = 10; i < 15; i++)
        ptr[i] = i;

    /* Show it... you know, for the kids, they like I/O */
    for (i = 0; i < 15; i++)
        printf("%d: %d\n", i, ptr[i]);
	/* We never call xfree on this one, to create a leak. */


    /* Since ss points to 'i', but it wasn't malloc'ed this will not issue a 
     * warning and will not be reported in the final report. */
    ss = xmalloc(ss, sizeof(char) * 14);
    /* In this case, xmalloc should be used as: xmalloc(NULL, sizeof(char)*14)
     * or as: ss = NULL; ss = xmalloc(ss, sizeof(char)*14) to be more 
     * consistent with the model, but don't worry, everything is handled */

    sprintf(ss, "Very small leak");
    puts(ss);

    /* Use xfree instead of free, xfree marks the pointer as "cleans" 
     * (NULL), so xfree(ss); xfree(ss) will never fail. */
    /* But on purpose of this demostration we comment it to create another 
     * memory leak */
    /* xfree(ss); */

    /* If xfree was uncommented up there, ss would now points to NULL, so the 
     * usage of xmalloc would be pretty straightforward and no warning would 
     * be issued. But this is not the case, ss was malloc'ed but never freed, 
     * so it remains registered in the library stack. So this should give us a 
     * warning and it actually does reporting that it will lead us to a memory
     * leak in the future */
    ss = xmalloc(ss, sizeof(char) * 14);
    sprintf(ss, "Dirty pointer and too big");
    puts(ss);

    /* This is on purpose to create another memory leak */
    /*xfree(ss);*/
    if (memory_end())
        return 1;

    return 0;
}
