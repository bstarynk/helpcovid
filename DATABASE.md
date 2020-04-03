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

The `--clear-database` program argument is clearing the database entirely.


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
`tb_`, index names start with `ix_`, prepared statements names end with `_pstm`.

In every SQL table, columns names share a common prefix. For example
columns of `tb_user` start with `user_`.
