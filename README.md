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

The PostGreSQL connection URI is the `$HELPCOVID_POSTGRESQL` environment variable, e.g. `export HELPCOVID_POSTGRESQL=postgresql://www-data@localhost/helpcovid_db`

The [OpenSSL](https://www.openssl.org/) encryption for
[HTTPS](https://en.wikipedia.org/wiki/HTTPS) may use the OpenSSL
certificate provided by `$HELPCOVID_SSLCERT` and the OpenSSL key
provided by `$HELPCOVID_SSLKEY`. See also the `--websslcert` and
`--websslkey` program options. These files should not be
world-readable.

## building

Edit the `Makefile` then run `make`

## PostGreSQL database

We use [PostGreSQL](https://www.postgresql.org/) and we require 
a [PostGreSQL 12](https://www.postgresql.org/docs/12/index.html) server.


To create the database on Debian, first get Linux root permission (e.g. 
with `sudo -s`). Then, according to
[this PostGreSQL tutorial](https://www.tutorialspoint.com/postgresql/postgresql_environment.htm) you need to run (as Linux root) the `su - postgres` command (which gives you access to the "PostGreSQL superuser") 
and run under that PostGreSQL user the `createdb helpcovid_db` command.

Concretely, the steps needed to setup the helpcovid database from psql are as
follows:

```
$ sudo -u postgres psql
postgres =# CREATE DATABASE helpcovid_db;
postgres =# CREATE USER helpcovid_usr WITH PASSWORD 'password';
postgres =# ALTER ROLE helpcovid_usr SET client_ encoding TO 'utf8';
postgres =# ALTER ROLE helpcovid_usr SET default_transaction_isolation TO  'read committed';
postgres =# ALTER ROLE helpcovid_usr SET timezone TO 'UTC ';
postgres =# GRANT ALL PRIVILEGES ON DATABASE helpcovid_db TO helpcovid_usr;
postgres =# \q 
```

When running `helpcovid`, the database password may sit in [your
`$HOME/.pgpass`
file](https://www.postgresql.org/docs/current/libpq-pgpass.html).

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

* https://adresse.data.gouv.fr/donnees-nationales

For a few requests, https://geo.api.gouv.fr/adresse for example https://api-adresse.data.gouv.fr/search/?q=92340 

which actually gives a JSON with all streets in Bourg La Reine

#### For India:

????

http://download.geonames.org/export/zip/


## intuition (in English)

We suppose honest adult and responsible users. Given Covid 19 urgency (in France, [see the *Décret
n° 2020-260 du 16 mars
	2020* by French prime minister](https://www.legifrance.gouv.fr/affichTexte.do?cidTexte=JORFTEXT000041728476&categorieLien=id) for example)
we are supposing
[GDPR](https://en.wikipedia.org/wiki/General_Data_Protection_Regulation)
compliance, even if this software collects personal data.... (which is relevant for *deployment* not for coding). We don't have time to check for that compliance.

First, one would **volunteerly** register
himself/herself, with *first name*, *last name*, *birth year*,
*email*, *phone*, *home-address* and optionally possibly more health
related information (e.g. French [National Identification
Number](https://en.wikipedia.org/wiki/National_identification_number)
also known as [*numéro de Sécurité
Sociale*](https://fr.wikipedia.org/wiki/Num%C3%A9ro_de_s%C3%A9curit%C3%A9_sociale_en_France)...).

Then a user would -on his/her free will- add more information about himself/herself. We dream of providing an infrastructure where someone could on his free will declare:

* I am health professional

* I am a fragile person (pregnant, sick, aged above 65 years, etc...)

* I am willing to do some shopping for close neighbour

* I need some neighbour help to get me food

* etc...

And we optimistically hope to provide a list of close neighbours
(closer than 2km, so at a walking distance) willing to help.


## intuition (en Français)

*in French*

Nous supposons des utilisateurs adultes, honnêtes et
responsables. Compte tenu de l'urgence (en France, [voir le *Décret n°
2020-260 du 16 mars 2020* du Premier
ministre](https://www.legifrance.gouv.fr/affichTexte.do?cidTexte=JORFTEXT000041728476&categorieLien=id)
	etc...) nous supposons -sans en être sûr- être conforme à la
[RGPD](https://fr.wikipedia.org/wiki/R%C3%A8glement_g%C3%A9n%C3%A9ral_sur_la_protection_des_donn%C3%A9es)
même si bien sûr ce logiciel collecte des données personnelles (c'est
pertinent au *déploiement*, pas au codage). Nous n'avons pas le temps de
vérifier cette conformité.

En premier lieu, un utilisateur s'enregistrerait **de son plein gré** avec ses *prénoms*, *nom de famille*, *année de naissance*, *lieu de résidence*, et peut-être même des informations liées à sa santé (notamment son  [*numéro de Sécurité
Sociale*](https://fr.wikipedia.org/wiki/Num%C3%A9ro_de_s%C3%A9curit%C3%A9_sociale_en_France)...).

Puis cet utilisateur ajouterait -de sa propre volonté- des informations supplémentaires. Nous rêvons de fournir une infrastructure permettant d'indiquer:

* je suis un professionnel de santé

* je suis fragile (car enceinte, malade, agé de plus de 65 ans, etc...)

* je suis volontaire pour faire des courses pour de proches voisins

* je cherche un voisin pour faire des courses (alimentaires) pour moi.

* etc...

Et nous espérons pouvoir fournir une liste de proches voisins (à moins de 2 km) désireux d'aider.

	
	
## important notice

We provide the software, not the data.
