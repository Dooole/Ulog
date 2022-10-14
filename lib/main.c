
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>

#include "ulog.h"

struct ulog_context {
	sqlite3 *db;
	char name[ULOG_NAME_MAX];
};

static struct ulog_context ctx = {0};

/* 
 reference:
 https://zetcode.com/db/sqlitec/
 */

int ulog_init(const char *name)
{
	char *err_msg = NULL;
	char query[ULOG_MSG_MAX];
	int rc;

	if (ctx.db != NULL) {
		fprintf(stderr, "%s:%s():%d: already initialized\n",
			__FILE__, __func__, __LINE__);
		return ULOG_ERROR_INITIALIZED;
	}

	rc = sqlite3_open(ULOG_DB_NAME, &ctx.db);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "%s:%s():%d: failed to open db: %s: %s\n",
			__FILE__, __func__, __LINE__,
			ULOG_DB_NAME, sqlite3_errmsg(ctx.db));
		return ULOG_ERROR_DB;
	}

	snprintf(query, ULOG_MSG_MAX,
		"CREATE TABLE IF NOT EXISTS Logs(Timestamp INT, Level INT, Name TEXT, Message TEXT)");

	rc = sqlite3_exec(ctx.db, query, 0, 0, &err_msg);
	if (rc != SQLITE_OK) {
		sqlite3_free(err_msg);
		fprintf(stderr, "%s:%s():%d: sql error: %s\n",
			__FILE__, __func__, __LINE__, err_msg);
		return ULOG_ERROR_SQL;
	}

	if (name && strlen(name) < ULOG_NAME_MAX)
		strncpy(ctx.name, name, ULOG_NAME_MAX-1);

	return ULOG_ERROR_NONE;
}

int ulog(int level, const char *msg)
{
	sqlite3_stmt *res;
	char query[ULOG_MSG_MAX];
	int rc;

	if (level < ULOG_ERROR || level > ULOG_DEBUG) {
		fprintf(stderr, "%s:%s():%d: invalid argument: level\n",
				__FILE__, __func__, __LINE__);
		return ULOG_ERROR_PARAMS;
	}

	if (msg == NULL || strlen(msg) >= ULOG_MSG_MAX) {
		fprintf(stderr, "%s:%s():%d: invalid argument: name\n",
				__FILE__, __func__, __LINE__);
		return ULOG_ERROR_PARAMS;
	}

	snprintf(query, ULOG_MSG_MAX, "INSERT INTO Logs VALUES(?, ?, ?, ?);");

	rc = sqlite3_prepare_v2(ctx.db, query, -1, &res, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "%s:%s():%d: sql error: %s\n",
				__FILE__, __func__, __LINE__, sqlite3_errmsg(ctx.db));
		return ULOG_ERROR_SQL;
	}

	sqlite3_bind_int(res, 1, (int)time(NULL));
	sqlite3_bind_int(res, 2, level);
	sqlite3_bind_text(res, 3, ctx.name, -1, NULL);
	sqlite3_bind_text(res, 4, msg, -1, NULL);

	if (sqlite3_step(res) != SQLITE_DONE) {
		sqlite3_finalize(res);
		fprintf(stderr, "%s:%s():%d: sql error: %s\n",
				__FILE__, __func__, __LINE__, sqlite3_errmsg(ctx.db));
		return ULOG_ERROR_SQL;
	}

	sqlite3_finalize(res);

	return ULOG_ERROR_NONE;
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

int ulog_print(const char *name, int level)
{
	sqlite3_stmt *res;
	char query[ULOG_MSG_MAX];
	int rc, step;

	// allow:
	// level 0 - all levels
	// name NULL - all programs
	if (level < ULOG_ALL || level > ULOG_DEBUG) {
		fprintf(stderr, "%s:%s():%d: invalid argument: level\n",
				__FILE__, __func__, __LINE__);
		return ULOG_ERROR_PARAMS;
	}

	if (name != NULL && strlen(name) >= ULOG_NAME_MAX) {
		fprintf(stderr, "%s:%s():%d: invalid argument: name\n",
				__FILE__, __func__, __LINE__);
		return ULOG_ERROR_PARAMS;
	}

	const char *select = "SELECT * FROM Logs";

	if (!name && !level) { // no filter - show all entries
		snprintf(query, ULOG_MSG_MAX, "%s", select);
	} else if (name && level) { // filter by name and level
		snprintf(query, ULOG_MSG_MAX, "%s WHERE Name = ? AND Level = ?", select);
	} else if (name) { // filter by name
		snprintf(query, ULOG_MSG_MAX, "%s WHERE Name = ?", select);
	} else if (level) { // filter by level
		snprintf(query, ULOG_MSG_MAX, "%s WHERE Level = ?", select);
	}

	rc = sqlite3_prepare_v2(ctx.db, query, -1, &res, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "%s:%s():%d: sql error: %s\n",
				__FILE__, __func__, __LINE__, sqlite3_errmsg(ctx.db));
		return ULOG_ERROR_SQL;
	}

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

	if (step != SQLITE_DONE) {
		sqlite3_finalize(res);
		fprintf(stderr, "%s:%s():%d: sql error: %s\n",
				__FILE__, __func__, __LINE__, sqlite3_errmsg(ctx.db));
		return ULOG_ERROR_SQL;
	}

	sqlite3_finalize(res);

	return ULOG_ERROR_NONE;
}

void ulog_destroy(void)
{
	if (ctx.db == NULL) {
		fprintf(stderr, "%s:%s():%d: not initialized\n",
			__FILE__, __func__, __LINE__);
		return;
	}

	sqlite3_close(ctx.db);

	memset(&ctx, 0, sizeof(struct ulog_context));
}
