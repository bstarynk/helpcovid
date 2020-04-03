# database in HelpCovid

See https://github.com/bstarynk/helpcovid/ for more about HelpCovid

## requirements

We need a [PostGreSQL](http://postgresql.org/) [relational
database](https://en.wikipedia.org/wiki/Relational_database) server on
Linux. Version 11 at least. That server could run on a different Linux
system than `./helpcovid` in the same data center. Most deployements
would run it on `localhost` in practice.

Our `generate-config.py` [Python](https://python.org/) script is generating configuration files and asking about details.

Our `backup-database.py` [Python](https://python.org/) script is dumping the database.


## related program arguments and configuration

The `--clear-database` program argument is clearing the database
entirely. The `--postgresql-database=` or `-P` program argument sets
the PostgreSQL database URI, which uses by default the
`$HELPCOVID_POSTGRESQL` environment variable (see
[environ(7)](http://man7.org/linux/man-pages/man7/environ.7.html),
[getenv(3)](http://man7.org/linux/man-pages/man3/getenv.3.html)

In the configuration file provided by `--config=` or `-C` program
argument, or the `$HELPCOVID_CONFIG` environment variable, or by
default `/etc/helpcovid.conf` file,


## C++ code

Every C++ code interacting with the database should stay in file
`hcv_database.cc` and be declared as `extern "C"` in `hcv_header.cc`
(or be in some [plugin](PLUGINS.md) ...). The
`hcv_initialize_database` function there is initializing a database
without any contents.

We use [libpqxx](http://pqxx.org/development/libpqxx/) at least
version 6 (packages `libpqxx-6.2` and `libpqxx-dev` on
[Debian](https://debian.org/) Buster).

The `hcv_dbconn` C++ variable points to the PostGreSQL
connection. Access to it should be serialized and protected with the `hcv_dbmtx`
mutex.

## naming conventions 

The SQL tables and indexes are created in routine
`hcv_initialize_database`. Conventionally, table names start with
`tb_`, index names start with `ix_` (followed by something specific to
their table), prepared statements names end with `_pstm`.

In every SQL table, columns names share a common prefix. For example
columns of `tb_user` start with `user_`.


## SQL tables, indexes and prepared statements.

The `tb_user` table contains information about human users of
HelpCovid, with indexes `ix_user_familyname`, `ix_user_email`,
`ix_user_crtime`. See prepared statements `user_create_pstm`, etc...

The `tb_password` table should store passwords, encrypted using
[crypt(3)](http://man7.org/linux/man-pages/man3/crypt.3.html). See
prepared statements `user_get_password_by_email_pstm`, etc...

The `tb_web_cookie` table store web cookies, each having a randomly
generated key. Associated indexes are `ix_cookie_random` and
`ix_cookie_exptime`. See prepared statements `add_web_cookie_pstm`,
etc...

