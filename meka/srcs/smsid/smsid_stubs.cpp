// Minimum stub for db.c to link properly
#include "shared.h"
#include "db.h"
#include "vdp.h"

// from meka.c
t_meka_config g_config;

// from misc.c
void* Memory_Alloc(size_t size)
{
	return malloc(size);
}

// from console.c
void ConsolePrint(char const* msg)
{
	printf("%s", msg);
}

void ConsolePrintf(char const* format, ...)
{
	char buf[512];
	va_list args;
	va_start(args, format);
	vsnprintf(buf, countof(buf), format, args);
	va_end(args);
}

void Quit_Msg(char const* format, ...)
{
	char buf[512];
	va_list args;
	va_start(args, format);
	vsnprintf(buf, countof(buf), format, args);
	va_end(args);
	exit(0);
}

const char*	Msg_Get(int n)
{
	switch (n)
	{
	case MSG_DB_Loading: return "";//Silent//"Loading MEKA.NAM...";
	case MSG_DB_SyntaxError: return "On line %d: Syntax error!";
	}
	assert(0);
	return "*Msg_Get()ERROR*";
}

// from errors.c
int meka_errno = MEKA_ERR_OK;
const char* meka_strerror()
{
	return "*ERROR*";
}

// from vdp.c
int     VDP_Model_FindByName(const char* name)
{
    if (!strcmp(name, "315-5124"))
        return VDP_MODEL_315_5124;
    else if (!strcmp(name, "315-5246")) //|| !strcmp(name, "315-5226")) // Prior to 2024 we had this listed (incorrectly) as 315-5226, however it was never used by meka.nam
        return VDP_MODEL_315_5246;
    else if (!strcmp(name, "315-5378"))
        return VDP_MODEL_315_5378;
    else if (!strcmp(name, "315-5313"))
        return VDP_MODEL_315_5313;
    return -1;
}
