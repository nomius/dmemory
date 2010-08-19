
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

#ifndef MEMORY_C
#define MEMORY_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include "debug.h"
#include "list.h"
#include "exceptions.h"

#define SIGNATURE "(D)(M).\0"
#define SIZE_SIGNATURE ((int)(strlen(SIGNATURE)+1))
#define VAR_REPORT_FILENAME "DMEMORY_REPORT"

int __DMEMORY_DEBUG_LEVEL = -1;

static void add_signature_to_variable(void *ptr, size_t size);
static int CheckSignatures(void *ptr, size_t size);

static stack_variable *stack = NULL;;

static stack_variable *ptr_last_var;
static void *last_checked;
static char *last_file_ok;
static int last_line_ok;

static FILE *report = NULL;

void __dmemory_sigsegv_handler(int sig)
{
	char *filename;

	/* This will happen in case he screw up the stack badley */
	if (!report) {

		/* Load the variable in the filename */
		if ((filename = getenv(VAR_REPORT_FILENAME)) == NULL) {
			debug(INFO, "%s not defined, no report will be generated\n", "LIBRARY", 0, VAR_REPORT_FILENAME);
			exit(1);
		}

		/* Check if it was defined and not empty */
		if (*filename == '\0') {
			debug(INFO, " %s defined but empty, no report will be generated\n", "LIBRARY", 0, VAR_REPORT_FILENAME);
			exit(1);
		}

		/* Open the exceptions file */
		if ((report = fopen(filename, "w")) == NULL) {
			debug(INFO, " opening %s", strerror(errno), errno, filename);
			exit(1);
		}
	}

	/* Write the info. Sorry, this is all you get! */
	fprintf(report, "There's a major corruption at 0x%0.12x right after %s at %d.\n", last_checked, last_file_ok, last_line_ok);
	fprintf(report, "This normally means that you really screwed things up.\n");

	fclose(report);
	exit(1);
}

static void add_signature_to_variable(void *ptr, size_t size)
{
	char *tmp = (char *)ptr;

	/* Add the signature to the beginning */
	memcpy(tmp, SIGNATURE, SIZE_SIGNATURE);

	/* Add the signature to the end */
	memcpy(tmp + SIZE_SIGNATURE + size, SIGNATURE, SIZE_SIGNATURE);
}

static int CheckSignatures(void *ptr, size_t size)
{
	char *tmp = (char *)ptr;

	/* Check the signature in the beginning and in the end */
	if (!memcmp(tmp, SIGNATURE, SIZE_SIGNATURE) && !memcmp(tmp + SIZE_SIGNATURE + size, SIGNATURE, SIZE_SIGNATURE))
		return 1;

	return 0;
}

void *__xmalloc(size_t size, char *file, int line)
{
#ifdef MEM_DEBUG
	void *ptr = NULL;

	if (__DMEMORY_DEBUG_LEVEL != -1) {

		/* Get that memory! */
		if ((ptr = malloc(size + SIZE_SIGNATURE * 2)) != NULL) {

			if (stack->next == NULL)
				/* Register the variable in the stack */
				ptr_last_var = add_pointer_to_stack(stack, ptr, size, file, line);
			else
				/* Save the first pointer */
				add_pointer_to_stack(stack, ptr, size, file, line);

			add_signature_to_variable(ptr, size);
			return ((char *)ptr) + SIZE_SIGNATURE;
		}

		/* Ok, something went wrong */
		debug(ERROR, "Memory exausted\n", file, line);
		abort();
		return NULL;
	}
	else {
		;
#else
		return malloc(size);
#endif
	}
}

void *__xrealloc(void *ptr, size_t size, char *file, int line)
{
#ifdef MEM_DEBUG
	void *tptr = NULL;
	stack_variable *myvar = NULL;

	if (__DMEMORY_DEBUG_LEVEL != -1) {

		/* If ptr is NULL and size is not 0, then is just plain malloc */
		if (ptr == NULL) {
			if (size != 0)
				return __xmalloc(size, file, line);
			return NULL;
		}

		/* If size is 0, then, plain free */
		else if (size == 0) {
			__xfree(ptr, file, line);
			return NULL;
		}

		/* ptr != NULL and size != 0. Classic realloc */

		/* If the pointer doesn't exists but he stills want that memory let's just give it to him */
		if ((myvar = search_pointer(stack, ((char *)ptr) - SIZE_SIGNATURE)) == NULL)
			return __xmalloc(size, file, line);

		/* It does exists in our stack, let's just increase it and leave */
		if ((tptr = realloc(((char *)ptr) - SIZE_SIGNATURE, size + SIZE_SIGNATURE * 2)) != NULL) {

			/* Add the signature and update the stack information */
			add_signature_to_variable(tptr, size);
			myvar->size = size;
			myvar->variable = tptr;
			return ((char *)tptr) + SIZE_SIGNATURE;
		}

		/* Ok, something went wrong */
		debug(ERROR, "Memory exausted\n", file, line);
		abort();
		return NULL;
	}
	else {
		;
#else
		return realloc(ptr, size);
#endif
	}
}

void *__xcalloc(size_t nmemb, size_t size, char *file, int line)
{
#ifdef MEM_DEBUG
	stack_variable *myvar = NULL;
	void *ptr = NULL;
	size_t tot = nmemb*size;

	if (__DMEMORY_DEBUG_LEVEL != -1) {

		/* Get that memory and blank it! */
		if (nmemb == 0)
			return NULL;

		/* We get the memory with malloc and then clean it, sorry, can't figure how to do it with real calloc */
		if ((ptr = malloc(tot + SIZE_SIGNATURE * 2)) != NULL) {
			memset(ptr, '\0', tot + SIZE_SIGNATURE * 2);

			/* Register the variable in the stack and put signature to it */
			if (stack->next == NULL)
				/* Register the variable in the stack */
				ptr_last_var = add_pointer_to_stack(stack, ptr, tot, file, line);
			else
				/* Save the first pointer */
				add_pointer_to_stack(stack, ptr, tot, file, line);

			add_signature_to_variable(ptr, tot);
			return ((char *)ptr) + SIZE_SIGNATURE;
		}

		/* Ok, something went wrong */
		debug(ERROR, "Memory exausted\n", file, line);
		abort();
		return NULL;
	}
	else {
		;
#else
		return calloc(nmemb, size);
#endif
	}
}

int __xfree(void *ptr, char *file, int line)
{
#ifdef MEM_DEBUG
	stack_variable *myvar = NULL;

	if (__DMEMORY_DEBUG_LEVEL != -1) {

		/* If the pointer doesn't exists just leave, there's nothing to free */
		if ((myvar = search_pointer(stack, ((char *)ptr) - SIZE_SIGNATURE)) != NULL) {

			/* Check if there was a memory corruption before deleting it */
			if (!CheckSignatures((void *)myvar->variable, (int)myvar->size))
				/* Hey oh! What happened here? Let's save that information for latter */
				myvar->df = 2;
			else 
				/* Everything went fine. Remove the variable from the stack */
				remove_pointer_from_stack(stack, myvar);

			/* Clean up the mess */
			free(((char *)ptr) - SIZE_SIGNATURE);
			return 0;
		}
		if (stack->next == NULL)
			/* Register the variable in the stack */
			myvar = ptr_last_var = add_pointer_to_stack(stack, ptr, 0, file, line);
		else
			/* Save the first pointer */
			myvar = add_pointer_to_stack(stack, ptr, 0, file, line);

		/* Mark it as a double free */
		myvar->df = 1;
		debug(WARNING, "You can not free this variable, as it was never reserved\n", file, line);
		return 1;
				
	}
	else {
		;
#else
		free(ptr);
#endif
	}
	return 0;
}

void __dmemory_init(int level, char *file, int line)
{
#ifdef MEM_DEBUG
	/* Initialize the stack */
	if (!stack) {
		/* Initialize the debug level */
		__DMEMORY_DEBUG_LEVEL = level;

		/* We capture SIGSEV because there could be a memory corruption by the user somewhere */
		/* TODO: This should be changed in order of sigaction, since it is more polite */
		signal(SIGSEGV, __dmemory_sigsegv_handler);

		/* Initialize the stack */
		stack = malloc(sizeof(stack_variable));
		stack->next = stack->prev = stack->variable = NULL;
		stack->size = -1;
		ptr_last_var = NULL;
	}
	else
		debug(WARNING, "You can not re-initialize dmemory twice\n", file, line);
#endif
}

int dmemory_end(void)
{
#ifdef MEM_DEBUG
	char *filename;
	stack_variable *ptr;

	if (__DMEMORY_DEBUG_LEVEL != -1) {
		/* Load the variable in the filename */
		if ((filename = getenv(VAR_REPORT_FILENAME)) == NULL) {
			debug(INFO, "%s not defined, no report will be generated\n", "LIBRARY", 0, VAR_REPORT_FILENAME);
			return 1;
		}

		/* Check if it was defined and not empty */
		if (*filename == '\0') {
			debug(INFO, " %s defined but empty, no report will be generated\n", "LIBRARY", 0, VAR_REPORT_FILENAME);
			return 1;
		}

		/* Open the exceptions file */
		if ((report = fopen(filename, "w")) == NULL) {
			debug(INFO, " opening %s", strerror(errno), errno, filename);
			return 1;
		}

		/* Load the exceptions array */
		__load_exceptions_file();

		/* For every variable in the stack let's see what the user left */
		/* We go from back to top in case he broke things up badley */
		for (ptr = ptr_last_var;; ptr = ptr->prev) {

			/* This will help us in case he screw up the stack badley */
			last_file_ok = ptr->filename;
			last_line_ok = ptr->line;
			last_checked = ptr->variable;

			/* Go cleaning the stack while moving through it */
			free(ptr->next);
			ptr->next = NULL;

			if (ptr->variable != NULL || ptr->df) {

				/* If it was a double free, show it in the report */
				if (ptr->df == 1) {
					fprintf(report, "(F) [%s] [%d] (address: 0x%0.12x)\n", ptr->filename, ptr->line, ((char *)ptr->variable) + SIZE_SIGNATURE);
					free(ptr->filename);
					continue;
				}

				/* If it was a memory corruption from a freed memory space, show it in the report */
				if (ptr->df == 2) {
					fprintf(report, "(C) [%s] [%d] (address: 0x%0.12x)\n", ptr->filename, ptr->line, ((char *)ptr->variable) + SIZE_SIGNATURE);
					free(ptr->filename);
					continue;
				}

				/* If it is not in the exception list, show it in the report */
				if (!__ExceptLeak(ptr->filename, ptr->line))
					fprintf(report, "(L) [%s] [%d] (address: 0x%0.12x)\n", ptr->filename, ptr->line, ((char *)ptr->variable) + SIZE_SIGNATURE);

				/* If it was corrupted and never freed either, show it in the report */
				if (!CheckSignatures((void *)ptr->variable, (int)ptr->size))
					fprintf(report, "(C) [%s] [%d] (address: 0x%0.12x)\n", ptr->filename, ptr->line, ((char *)ptr->variable) + SIZE_SIGNATURE);

				/* Release the filename saved in the stack */
				free(ptr->filename);
			}

			/* Enough, it's over */
			if (ptr->prev == NULL)
				break;
		}
		/* Free the last one */
		free(ptr->next);

		/* Free the exceptions list */
		__free_exceptions();

		fclose(report);
	}
#endif
	return 0;
}

#endif
