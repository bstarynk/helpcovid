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

* [Twitter Bootstrap](https://github.com/twbs/bootstrap) for the responsive CSS
  framework

* [Start Bootstrap - Blog Home](https://github.com/BlackrockDigital/startbootstrap-blog-home)
  for the Bootstrap theme

* [Bootstrap Cookie Alert](https://github.com/Wruczek/Bootstrap-Cookie-Alert)
  for the GDPR notice popup


On  [Debian](https://debian.org/)/Buster `sudo aptitude install postgresql-server-dev-11
postgresql-client-11 postgresql-11 libpqxx-dev`, but both
`cpp-httplib` and `Websocket++` need to be built from source


## conventions

The web root is `webroot/` or the `$HELPCOVID_WEBROOT` environment variable. e. g. `export HELPCOVID_WEBROOT=/home/helpcovid/webroot`

The served URL is the `$HELPCOVID_URL` environment variable e.g. `export HELPCOVID_URL=https://b-star-y.tech/helpcovid`

The PostGreSQL connection URI is the `$HELPCOVID_POSTGRESQL` environment variable, e.g. `export HELPCOVID_POSTGRESQL=postgresql://www-data@localhost/helpcovid`

## building

Edit the `Makefile` then run `make`

## communication

We use the `HelpCovid software` group on [https://web.whatsapp.com/](WhatsApp)

The phone number of Basile Starynkevitch there is `+33 6 8501 2359`
