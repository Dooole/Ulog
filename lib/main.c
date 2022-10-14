
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
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(ctx.db)); //
        sqlite3_close(ctx.db);
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
	sqlite3_stmt *res;
	char query[ULOG_MSG_MAX];
	int rc;

	if (level < ULOG_ERROR || level > ULOG_DEBUG) {
		return;
	}

	if (msg == NULL || strlen(msg) >= ULOG_MSG_MAX) {
		return;
	}

	snprintf(query, ULOG_MSG_MAX, "INSERT INTO Logs VALUES(?, ?, ?, ?);");

	rc = sqlite3_prepare_v2(ctx.db, query, -1, &res, 0);
	if (rc != SQLITE_OK)
		return;

	sqlite3_bind_int(res, 1, (int)time(NULL));
	sqlite3_bind_int(res, 2, level);
	sqlite3_bind_text(res, 3, ctx.name, -1, NULL);
	sqlite3_bind_text(res, 4, msg, -1, NULL);

	sqlite3_step(res);
	sqlite3_finalize(res);
}

static void print_time(int value)
{
	struct tm ts;
	char timestr[ULOG_NAME_MAX];

	time_t time = (time_t)value;
	ts = *localtime(&time);

	strftime(timestr, sizeof(timestr), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	printf("[%s]", timestr);
}

void ulog_print(const char *name, int level)
{
	sqlite3_stmt *res;
	char query[ULOG_MSG_MAX];
	int rc, step;

	// allow:
	// level 0 - all levels
	// name NULL - all programs
	if (level < ULOG_ALL || level > ULOG_DEBUG) {
		return;
	}

	if (name != NULL && strlen(name) >= ULOG_NAME_MAX) {
		return;
	}

	if (!name && !level) { // no filter - show all entries
		snprintf(query, ULOG_MSG_MAX, "SELECT * FROM Logs");
	} else if (name && level) { // filter by name and level
		snprintf(query, ULOG_MSG_MAX, "SELECT * FROM Logs WHERE Name = ? AND Level = ?");
	} else if (name) { // filter by name
		snprintf(query, ULOG_MSG_MAX, "SELECT * FROM Logs WHERE Name = ?");
	} else if (level) { // filter by level
		snprintf(query, ULOG_MSG_MAX, "SELECT * FROM Logs WHERE Level = ?");
	}

	rc = sqlite3_prepare_v2(ctx.db, query, -1, &res, 0);
	if (rc != SQLITE_OK)
		return;

	if (name && level) { // filter by name and level
		sqlite3_bind_text(res, 1, name, -1, NULL);
		sqlite3_bind_int(res, 2, level);
	} else if (name) { // filter by name
		sqlite3_bind_text(res, 1, name, -1, NULL);
	} else if (level) { // filter by level
		sqlite3_bind_int(res, 1, level);
	}

	while ((step = sqlite3_step(res)) == SQLITE_ROW)
	if (step == SQLITE_ROW) {
		print_time(sqlite3_column_int(res, 0));
		printf(" %s", ulog_level_name[sqlite3_column_int(res, 1)]);
		printf(" %s", sqlite3_column_text(res, 2));
		printf(" %s\n", sqlite3_column_text(res, 3));
	}

	sqlite3_finalize(res);
}

void ulog_destroy(void)
{

	if (ctx.db == NULL) {
		return;
	}

	sqlite3_close(ctx.db);

	memset(&ctx, 0, sizeof(struct ulog_context));
}