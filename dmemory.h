
/* vim: set sw=4 sts=4 tw=80 */

/* 
 * Copyright (c) 2010 , David B. Cortarello <dcortarello@gmail.com>
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
 *    This product includes software developed by David B. Cortarello.
 * 4. Neither the name of David B. Cortarello nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY DAVID B. CORTARELLO 'AS IS' AND ANY
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

#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdio.h>

void *__xmalloc(size_t size, char *file, int line);
void *__xcalloc(size_t nmemb, size_t size, char *file, int line);
void *__xrealloc(void *ptr, size_t size, char *file, int line);
int __xfree(void *ptr, char *file, int line);

void __dmemory_init(int level, char *file, int line);
int __dmemory_stack_inspect(int dofree, char *file, int line);

#define malloc(size) __xmalloc((size), (char *)__FILE__, __LINE__)
#define calloc(nmemb, size) __xcalloc((nmemb), (size), (char *)__FILE__, __LINE__)
#define realloc(ptr, size) __xrealloc((ptr), (size), (char *)__FILE__, __LINE__)
#define free(ptr) __xfree((ptr), (char *)__FILE__, __LINE__)
#define dmemory_init(level) __dmemory_init(level, (char *)__FILE__, __LINE__)
#define dmemory_inspect_stack() __dmemory_inspect_stack(0, (char *)__FILE__, __LINE__)
#define dmemory_end() __dmemory_inspect_stack(1, NULL, 0)

#endif
