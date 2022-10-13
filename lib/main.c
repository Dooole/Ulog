
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>

#include "ulog.h"

struct ulog_context {
	sqlite3 *db;
	char name[ULOG_NAME_MAX];
};

static struct ulog_context ctx = {0};

//reference:
//https://zetcode.com/db/sqlitec/

int ulog_init(const char *name)
{
	char *err_msg = NULL;
	char query[2 * ULOG_MSG_MAX];
	int rc;

	if (ctx.db != NULL) {
		return 1;
	}

	rc = sqlite3_open("ulog.db", &ctx.db);
	if (rc != SQLITE_OK) {
		return 1;
	}

	sprintf(query, "CREATE TABLE Logs(Timestamp INT, Level INT, Name TEXT, Message TEXT);");

	rc = sqlite3_exec(ctx.db, query, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		sqlite3_free(err_msg);
		if (rc != SQLITE_ERROR) { // let SQL fail with SQLITE_ERROR - table may already exist. Exit on other codes.
			return 1;
		}
	}

	if (name && strlen(name) < ULOG_NAME_MAX)
		strncpy(ctx.name, name, ULOG_NAME_MAX-1);

	return 0;
}

void ulog(int level, const char *msg)
{
	char *err_msg = NULL;
	char query[2 * ULOG_MSG_MAX];
	int rc;

	if (level < ULOG_ERROR || level > ULOG_DEBUG) {
		return;
	}

	if (msg == NULL || strlen(msg) >= ULOG_MSG_MAX) {
		return;
	}

	sprintf(query, "INSERT INTO Logs VALUES(%d, %d, '%s', '%s');",
		(int)time(NULL), level, ctx.name, msg);

	rc = sqlite3_exec(ctx.db, query, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		sqlite3_free(err_msg);
		return;
	}
}

static void print_time(char *value)
{
	struct tm ts;
	char timestr[ULOG_NAME_MAX];

	time_t time = (time_t)atoi(value);
	ts = *localtime(&time);

	strftime(timestr, sizeof(timestr), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	printf("[%s]", timestr);
}

int print_callback(void *query, int ncols, char **nfields, 
	char **colnames) {

	int i;
	for (i = 0; i < ncols; i++) {
		switch (i)
		{
			case 0:
				print_time(nfields[i]);
				break
				;
			case 1:
				printf(" %s", ulog_level_name[atoi(nfields[i])]);
				break
				;
			default: // name and message
				printf(" %s", nfields[i]);
		}
	}
	printf("\n");
	
	return 0;
}

void ulog_print(const char *name, int level)
{
	char *err_msg = NULL;
	char query[2 * ULOG_MSG_MAX];
	int rc;

	// allow:
	// level 0 - all levels
	// name NULL - all programs
	if (level < ULOG_ALL || level > ULOG_DEBUG) {
		return;
	}

	if (name != NULL && strlen(name) >= ULOG_NAME_MAX) {
		return;
	}

	if (!name && !level) // no filter - show all entries
		sprintf(query, "SELECT * FROM Logs");
	else if (name && level) // filter by name and level
		sprintf(query, "SELECT * FROM Logs WHERE Name='%s' AND Level='%d'", name, level);
	else if (name) // filter by name
		sprintf(query, "SELECT * FROM Logs WHERE Name='%s'", name);
	else // filter by level
		sprintf(query, "SELECT * FROM Logs WHERE Level='%d'", level);

	rc = sqlite3_exec(ctx.db, query, print_callback, 0, &err_msg);
	if (rc != SQLITE_OK) {
		sqlite3_free(err_msg);
		return;
	}
}

void ulog_destroy(void)
{

	if (ctx.db == NULL) {
		return;
	}

	sqlite3_close(ctx.db);

	memset(&ctx, 0, sizeof(struct ulog_context));
}
