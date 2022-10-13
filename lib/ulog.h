#ifndef ULOG_H //include guard reikalingas isvengti multiple deklaravimo per klaida includinus header daugiau nei viena kart
#define ULOG_H // #if defined (ULOG_H) 

#include <stdio.h>
#include <stdarg.h>

#define ULOG_NAME_MAX 32
#define ULOG_MSG_MAX 128

#define ULOG_ALL 0
#define ULOG_ERROR 1
#define ULOG_WARNING 2
#define ULOG_INFO 3
#define ULOG_DEBUG 4

char *ulog_level_name[] =
{
	[ULOG_ALL] = "ALL",
    [ULOG_ERROR] = "ERROR",
    [ULOG_WARNING] = "WARNING",
    [ULOG_INFO]  = "INFO",
    [ULOG_DEBUG]  = "DEBUG",
};

int ulog_init(const char *name);
void ulog(int level, const char *msg);
void ulog_print(const char *name, int level);
void ulog_destroy(void);

#endif
