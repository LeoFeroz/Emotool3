#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include "debug.h"

#define DEBUG_BUFFER_SIZE	512

static int debug_level = 0;

int set_debug_level(int level)
{
	int ret = debug_level;
	debug_level = level;
	
	return ret;
}

int mod_debug_level(int mod_by)
{
	return set_debug_level(debug_level+mod_by);
}

int FORMAT_PRINTF DebugPrintf(const char* fmt, ...)
{
	char *dbg;
	va_list ap;

	dbg = (char *)malloc(DEBUG_BUFFER_SIZE);
	if (!dbg)
		return 0;
		
	va_start(ap, fmt);
	size_t len = vsnprintf(dbg, DEBUG_BUFFER_SIZE, fmt, ap);
	va_end(ap);

	fputs(dbg, stderr);
	free(dbg);
	return len;
}

int FORMAT_PRINTF UserPrintf(const char* fmt, ...)
{
	char *dbg;
	va_list ap;

	dbg = (char *)malloc(DEBUG_BUFFER_SIZE);
	if (!dbg)
		return 0;
		
	va_start(ap, fmt);
	size_t len = vsnprintf(dbg, DEBUG_BUFFER_SIZE, fmt, ap);
	va_end(ap);

	fputs(dbg, stdout);
	free(dbg);
	return len;
}

