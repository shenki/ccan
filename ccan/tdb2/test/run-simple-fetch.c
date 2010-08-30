#include <ccan/tdb2/tdb.c>
#include <ccan/tdb2/free.c>
#include <ccan/tdb2/lock.c>
#include <ccan/tdb2/io.c>
#include <ccan/tdb2/check.c>
#include <ccan/tap/tap.h>
#include "logging.h"

int main(int argc, char *argv[])
{
	unsigned int i;
	struct tdb_context *tdb;
	int flags[] = { TDB_INTERNAL, TDB_DEFAULT,
			TDB_INTERNAL|TDB_CONVERT, TDB_CONVERT };
	struct tdb_data key = { (unsigned char *)"key", 3 };
	struct tdb_data data = { (unsigned char *)"data", 4 };

	plan_tests(sizeof(flags) / sizeof(flags[0]) * 8 + 1);
	for (i = 0; i < sizeof(flags) / sizeof(flags[0]); i++) {
		tdb = tdb_open("run-simple-fetch.tdb", flags[i],
			       O_RDWR|O_CREAT|O_TRUNC, 0600, &tap_log_attr);
		ok1(tdb);
		if (tdb) {
			struct tdb_data d;

			/* fetch should fail. */
			d = tdb_fetch(tdb, key);
			ok1(d.dptr == NULL);
			ok1(tdb_error(tdb) == TDB_ERR_NOEXIST);
			ok1(tdb_check(tdb, NULL, NULL) == 0);
			/* Insert should succeed. */
			ok1(tdb_store(tdb, key, data, TDB_INSERT) == 0);
			ok1(tdb_check(tdb, NULL, NULL) == 0);
			/* Fetch should now work. */
			d = tdb_fetch(tdb, key);
			ok1(data_equal(d, data));
			ok1(tdb_check(tdb, NULL, NULL) == 0);
			tdb_close(tdb);
		}
	}
	ok1(tap_log_messages == 0);
	return exit_status();
}