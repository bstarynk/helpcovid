# helpcovid

A quick and dirty C++17 application (GPLv3+) to help neighbours making
essential buying (food, medicine) during Covid epidemics. For Linux
only. [GCC](http://gcc.gnu.org/) compiled. On [https://github.com/bstarynk/helpcovid/](helpcovid)

Of course that application stores personal data in some
[PostGreSQL](http://postgresql.org/) database, so is concerned by
[GDPR](https://en.wikipedia.org/wiki/General_Data_Protection_Regulation)
... It is expected to be used in good faith.

If [RefPerSys](http://refpersys.org/) was ready, it should have been
used instead.


## license

Of course [GPLv3+](https://www.gnu.org/licenses/gpl-3.0.en.html)

You take the responsability about personal data issues.

## contributors

* [Basile Starynkevitch](http://starynkevitch.net/Basile/), near [Paris](https://en.wikipedia.org/wiki/Paris), France; email `basile-freelance@starynkevitch.net` 

* [Abhishek Chakravarti](http://taranjali.org/aboutme), near [Kolkota](https://en.wikipedia.org/wiki/Kolkata), India. email `abhishek@taranjali.org` 

* [Nimesh Neema](http://nimeshneema.com/) near [Indore](https://en.wikipedia.org/wiki/Indore), India.

## dependencies

* [libpq](https://www.postgresql.org/docs/11/libpq.html) from PostGreSQL 11 for the database.

* [libpqxx](http://pqxx.org/development/libpqxx) for C++ frontend to PostGreSQL.

* [cpp-httplib](https://github.com/yhirose/cpp-httplib) for C++ [HTTPS](https://en.wikipedia.org/wiki/HTTPS) service

* [Websocket++](https://github.com/zaphoyd/websocketpp) for [WebSocket](https://en.wikipedia.org/wiki/WebSocket)s

On Debian/Buster `sudo aptitude install postgresql-server-dev-11 postgresql-client-11 postgresql-11 libpqxx-dev`, but `libonion` needs to be built from source


## conventions

The web root is `webroot/` or the `$HELPCOVID_WEBROOT` environment variable.

The PostGreSQL connection URI is the `$HELPCOVID_POSTGRESQL` environment variable

## building

Edit the `Makefile` then run `make`
