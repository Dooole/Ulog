#ifndef ULOG_H
#define ULOG_H /* same as #if defined(ULOG_H) */

#define ULOG_DB_NAME "ulog.db"

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

#define ULOG_ERROR_NONE 0
#define ULOG_ERROR_INITIALIZED 1
#define ULOG_ERROR_DB 2
#define ULOG_ERROR_SQL 3
#define ULOG_ERROR_PARAMS 4

int ulog_init(const char *name);
int ulog(int level, const char *msg);
int ulog_print(const char *name, int level);
void ulog_destroy(void);

#endif /* ULOG_H */
