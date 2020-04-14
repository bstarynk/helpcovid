# database in HelpCovid

See https://github.com/bstarynk/helpcovid/ for more about HelpCovid

Our [README.md](README.md) contains instructions to setup the database
on a pristine Debian system.

## requirements

We need a [PostGreSQL](http://postgresql.org/) [relational
database](https://en.wikipedia.org/wiki/Relational_database) server on
Linux. Version 11 at least. That server could run on a different Linux
system than `./helpcovid` in the same data center. Most deployements
would run it on `localhost` in practice.

Currently, we are using the following PostgreSQL extensions:

| Extension | Synopsis |
| --- | --- |
| pgtap | Testing for correct database schema creation |
| pgcrypto | Encryption of sensitive data such as identity and password |

Please see [DEPLOYMENT.md]() for details on how to install these extensions
along with PostgreSQL.

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


## SQL tables, indices and prepared statements.

In this section, we discuss in some detail the tables, indices, and prepared
statements in the HelpCovid database.

### Table `tb_user`

The `tb_user` table contains information about human users of
HelpCovid, with indexes `ix_user_familyname`, `ix_user_email`,
`ix_user_crtime`. See prepared statements `user_create_pstm`, etc...

| Column | Type | Constraints | Synopsis |
| --- |:---:| --- | --- |
| user_id | serial | primary key | unique user ID |
| user_firstname | varchar (31) | not null | first name, in capitals, UTF8 |
| user_familyname | varchar (62) | not null, index | family name, in capitals, UTF8 |
| user_email | varchar (71) | not null, index | email in lowercase, UTF8 |
| user_telephone | varchar (23) | not null | telephone number (digits, +, - or space) |
| user_gender | char (1) | not null | 'F' or 'M' or '?' |
| user_crtime | timestamp | not null, default, index | user entry creation time |

| Index | Column | 
| --- | --- |
| ix_user_familyname | user_familyname | 
| ix_user_email | user_email |
| ix_user_crtime | user_crtime |


### Table `tb_password`

The `tb_password` table should store passwords, encrypted using
[crypt(3)](http://man7.org/linux/man-pages/man3/crypt.3.html). See
prepared statements `user_get_password_by_email_pstm`, etc...

| Column | Type | Constraints | Synopsis |
| --- |:---:| --- | --- |
| passw_id | serial | primary key | unique key in this table |
| passw_userid | int | not null | the user ID whose password we store |
| passw_encr | varchar (62) | not null | the encrypted password |
| passw_mtime | timestamp | default | the last time that password was modified |


### Table `tb_webcookie`

The `tb_web_cookie` table store web cookies, each having a randomly
generated key. Associated indexes are `ix_cookie_random` and
`ix_cookie_exptime`. See prepared statements `add_web_cookie_pstm`,
etc...

| Column | Type | Constraints | Synopsis |
| --- |:---:| --- | --- |
| wcookie_id | serial | primary key | unique key in this table |
| wcookie_random | char (24) | not null, index | a random key, hopefully usually unique |
| wcookie_exptime | timestamp | not null, index | the cookie expiration time |
| wcookie_webagenthash | int | not null | a quick hashcode of the browser's User-Agent |

| Index | Column | 
| --- | --- |
| ix_cookie_random | user_familyname | 
| ix_cookie_exptime | user_email |

## Testing the database

Testing is achieved through the [`pgtap`](https://pgtap.org/) utility. This 
utility is used to run the unit tests related to the schema of the HelpCovid 
database.

