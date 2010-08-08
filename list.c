
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

#ifndef LIST_C
#define LIST_C

#include <stdlib.h>
#include <string.h>
#include "list.h"

/* This function is a classic ;-) */
int empty_stack(stack_variable * stack)
{
	if (stack->next == NULL)
		return 1;
	return 0;
}

/* Check the whole stack looking up for a certain pointer */
stack_variable *search_pointer(stack_variable * stack, void *addr)
{
	stack_variable *ptr;

	for (ptr = stack;; ptr = ptr->next) {
		if (ptr->variable == addr)
			return ptr;
		if (ptr->next == NULL)
			break;
	}
	return NULL;
}

/* Save a pointer in the stack */
stack_variable *add_pointer_to_stack(stack_variable * stack, void *addr, size_t size, char *file, int line)
{
	stack_variable *myvar = NULL;

	/* Save space for one more and place it first */
	myvar = malloc(sizeof(stack_variable));
	myvar->next = stack->next;

	if (!empty_stack(stack))
		stack->next->prev = myvar;
	stack->next = myvar;
	myvar->prev = stack;
	myvar->variable = addr;
	myvar->size = size;
	myvar->filename = strdup(file);
	myvar->line = line;
	return myvar;
}

/* To be consistent with our memory, if he free up memory, remove it from the stack */
void remove_pointer_from_stack(stack_variable * stack, stack_variable * myvar)
{
	myvar->prev->next = myvar->next;
	myvar->next->prev = myvar->prev;
	free(myvar->filename);
	free(myvar);
}

#endif
