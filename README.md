# helpcovid

A quick and dirty C++17 application (GPLv3+) to help neighbours making
essential buying (food, medicine) during the [2019–20 coronavirus pandemic](https://en.wikipedia.org/wiki/2019–20_coronavirus_pandemic).
For Linux only. [GCC](http://gcc.gnu.org/) compiled. On [https://github.com/bstarynk/helpcovid/](helpcovid)

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

* [Abhishek Chakravarti](http://taranjali.org/aboutme), near [Kolkota](https://en.wikipedia.org/wiki/Kolkata), India; email `abhishek@taranjali.org` 

* [Nimesh Neema](http://nimeshneema.com/) near [Indore](https://en.wikipedia.org/wiki/Indore), India; email `nimeshneema@gmail.com`

## dependencies

* [libpq](https://www.postgresql.org/docs/11/libpq.html) from PostGreSQL 11 for the database.

* [libpqxx](http://pqxx.org/development/libpqxx) for C++ frontend to PostGreSQL.

* [cpp-httplib](https://github.com/yhirose/cpp-httplib) for C++ [HTTPS](https://en.wikipedia.org/wiki/HTTPS) service.

* [Websocket++](https://github.com/zaphoyd/websocketpp) for [WebSockets](https://en.wikipedia.org/wiki/WebSocket).

* [Twitter Bootstrap](https://github.com/twbs/bootstrap) for the responsive CSS framework.

* [Start Bootstrap - Blog Home](https://github.com/BlackrockDigital/startbootstrap-blog-home) for the Bootstrap theme.

* [Bootstrap Cookie Alert](https://github.com/Wruczek/Bootstrap-Cookie-Alert) for the GDPR notice popup.

On  [Debian](https://debian.org/) (Buster) run:

`sudo aptitude install postgresql-server-dev-11 postgresql-client-11 postgresql-11 libpqxx-dev`

but both

`cpp-httplib` and `Websocket++` need to be built from the source

## conventions

The web root is `webroot/` or the `$HELPCOVID_WEBROOT` environment variable. e. g. `export HELPCOVID_WEBROOT=/home/helpcovid/webroot`

The served URL is the `$HELPCOVID_URL` environment variable e.g. `export HELPCOVID_URL=https://b-star-y.tech/helpcovid`

The PostGreSQL connection URI is the `$HELPCOVID_POSTGRESQL` environment variable, e.g. `export HELPCOVID_POSTGRESQL=postgresql://www-data@localhost/helpcovid`

## building

Edit the `Makefile` then run `make`

## communication

We use the `HelpCovid software` group on [https://web.whatsapp.com/](WhatsApp)

The phone number of Basile Starynkevitch there is `+33 6 8501 2359`


## email forum `helpcovid@framalistes.org`

See https://framalistes.org/sympa/info/helpcovid

Please follow the following conventions and stay civil and nice. The forum is overloaded, so don't work quickly.

* all emails are in English and public

* an email related to medical questions (e.g. should we ask our users about their body temperature) should have a subject starting with `MEDICAL:`

* an email about privacy issues (e.g. GDPR concerns) should start with `PRIVACY:`

* an email about database  (e.g. what SQL request is best) should have a subject starting with `DATABASE:`

* an email about software development (e.g.  how to compile libpqxx) should start with `DEVEL:`

* an email about software deployment (e.g.  how to start the executable) should start with `DEPLOY:`

* any other topic should start with a lowercase letter


## relevant hyperlinks

#### For France:

Some of the hyperlinks are in French, sorry.

* https://help.openstreetmap.org/questions/73656/html5-javascript-code-to-show-a-map-and-get-a-gps-position/73666

* https://nouvelle-techno.fr/actualites/2018/05/11/pas-a-pas-inserer-une-carte-openstreetmap-sur-votre-site

* https://www.data.gouv.fr/fr/datasets/base-officielle-des-codes-postaux/


For a few requests, https://geo.api.gouv.fr/adresse for example https://api-adresse.data.gouv.fr/search/?q=92340 

which actually gives a JSON with all streets in Bourg La Reine

#### For India:

????

http://download.geonames.org/export/zip/


## intuition

We suppose honest users. Given Covid 19 urgency (in France, [*Décret
n° 2020-260 du 16 mars
2020*)(https://www.legifrance.gouv.fr/affichTexte.do?cidTexte=JORFTEXT000041728476&categorieLien=id),
we are supposing
[GDPR](https://en.wikipedia.org/wiki/General_Data_Protection_Regulation)
compliance.... (which is relevant for *deployment* not for coding).

First, one would **volunteerly** register
himself/herself, with *first name*, *last name*, *birth year*,
*email*, *phone*, *home-address* and optionally possibly more health
related information (e.g. French [National Identification
Number](https://en.wikipedia.org/wiki/National_identification_number)
also known as [*numéro de Sécurité
Sociale*](https://fr.wikipedia.org/wiki/Num%C3%A9ro_de_s%C3%A9curit%C3%A9_sociale_en_France)...).

Then a user would add more information about himself/herself. We dream of providing an infrastructure where someone could on his free will declare:

* I am health professional

* I am a fragile person (pregnant, sick, aged above 65 years, etc...)

* I am willing to do some shopping for close neighbour

* I need some neighbour help to get me food

* etc...

And we optimistically hope to provide a list of close neighbours
(closer than 2km, so at a walking distance) willing to help.
