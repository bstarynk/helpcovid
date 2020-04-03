# helpcovid

A quick and dirty C++17 application (GPLv3+) to help neighbours making
essential buying (food, medicine) during the [2019–20 coronavirus pandemic](https://en.wikipedia.org/wiki/2019–20_coronavirus_pandemic).
**For Linux/x86-64 only**. [GCC](http://gcc.gnu.org/) compiled. On [https://github.com/bstarynk/helpcovid/](helpcovid)

Of course that application stores personal data in some
[PostGreSQL](http://postgresql.org/) database, so is concerned by
[GDPR](https://en.wikipedia.org/wiki/General_Data_Protection_Regulation)
... It is expected to be used in good faith.

If [RefPerSys](http://refpersys.org/) was ready, it should have been
used instead.

In France, see also [enpremiereligne.fr](https://enpremiereligne.fr/)
and [covid19-que-lire.fr](https://www.covid19-que-lire.fr); look also
into
[covid19-floss-initiatives](https://github.com/bzg/covid19-floss-initatives)

## license

Of course [GPLv3+](https://www.gnu.org/licenses/gpl-3.0.en.html)

Notice that you take the responsability about personal data and
privacy issues when *deploying* this free software
(e.g. [GDPR](https://en.wikipedia.org/wiki/General_Data_Protection_Regulation)
in Europe).


## plugins

See [PLUGINS.md](PLUGINS.md) for more.

## relational database

We use a [PostGreSQL](https://postgresql.org) relational database. See
[DATABASE.md](DATABASE.md) for more.

## contributors

* [Basile Starynkevitch](http://starynkevitch.net/Basile/), near [Paris](https://en.wikipedia.org/wiki/Paris), France; email `basile-freelance@starynkevitch.net` 

* [Abhishek Chakravarti](http://taranjali.org/aboutme), near [Kolkota](https://en.wikipedia.org/wiki/Kolkata), India; email `abhishek@taranjali.org` 


## dependencies

* [libpq](https://www.postgresql.org/docs/11/libpq.html) from PostGreSQL 11 for the database.

* [libpqxx](http://pqxx.org/development/libpqxx) for C++ frontend to PostGreSQL.

* [cpp-httplib](https://github.com/yhirose/cpp-httplib) for C++ [HTTPS](https://en.wikipedia.org/wiki/HTTPS) service.

* [Websocket++](https://github.com/zaphoyd/websocketpp) for [WebSockets](https://en.wikipedia.org/wiki/WebSocket).

* [Twitter Bootstrap](https://github.com/twbs/bootstrap) for the responsive CSS framework.

* [Start Bootstrap - Blog Home](https://github.com/BlackrockDigital/startbootstrap-blog-home) for the Bootstrap theme.

* [Bootstrap Cookie Alert](https://github.com/Wruczek/Bootstrap-Cookie-Alert) for the GDPR notice popup.

On  [Debian](https://debian.org/) (Buster) run:

`sudo aptitude install postgresql-server-dev-11 postgresql-client-11 postgresql-11 libpqxx-dev libconfig++-dev libglibmm-2.4-dev`

but both

`cpp-httplib` and `Websocket++` need to be built from the source

## conventions

The `./helpcovid` executable accepts both `--help` and `--version`
program arguments. It uses environment variables (see
[environ(7)](http://man7.org/linux/man-pages/man7/environ.7.html),
[getenv(3)](http://man7.org/linux/man-pages/man3/getenv.3.html) ...)
prefixed with `HELPCOVID_`. The default configuration file (it is some
[Glib key
file](https://developer.gnome.org/glib/stable/glib-Key-value-file-parser.html)...)
is `/etc/helpcovid.conf` (or given with `--config=` program argument
or `$HELPCOVID_CONFIG` environment variable) parsed by
[GlibMM](https://developer.gnome.org/glibmm/stable/) with `.ini` like
syntax in our file [hcv_main.cc][] at startup.

The web root is `webroot/` or the `$HELPCOVID_WEBROOT` environment variable. e. g. `export HELPCOVID_WEBROOT=/home/helpcovid/webroot`

The served URL is the `$HELPCOVID_URL` environment variable e.g. `export HELPCOVID_URL=https://b-star-y.tech/helpcovid`

The PostGreSQL connection URI is the `$HELPCOVID_POSTGRESQL` environment variable, e.g. `export HELPCOVID_POSTGRESQL=postgresql://www-data@localhost/helpcovid_db`

The [OpenSSL](https://www.openssl.org/) encryption for
[HTTPS](https://en.wikipedia.org/wiki/HTTPS) may use the OpenSSL
certificate provided by `$HELPCOVID_SSLCERT` and the OpenSSL key
provided by `$HELPCOVID_SSLKEY`. See also the `--websslcert` and
`--websslkey` program options. These files should not be
world-readable.

Customization of HTML contents visible to the end-user happens with
[processing
instructions](https://en.wikipedia.org/wiki/Processing_Instruction)
starting with `<?hcv` (that triggers dynamic HTML generation) and
ending with `?>` on the same line. In particular,
[internationalization and
localization](https://en.wikipedia.org/wiki/Internationalization_and_localization)
of messages in our HTML files (under `webroot/html/`) is done by
processing instructions on a *single* line such as, for example something as
`<?hcv msg MAINPAGE_HEADING Page Heading?>` handled by C++ code in
`hcv_template.cc` function `hcv_initialize_templates` which should use
[locale(7)](http://man7.org/linux/man-pages/man7/locale.7.html)
facilities such as
[gettext(3)](http://man7.org/linux/man-pages/man3/gettext.3.html).

### template conventions

In some HTML-like files, template expansion occurs. These are
SGML-like [processing
instructions](https://en.wikipedia.org/wiki/Processing_Instruction)
starting with `<?hcv ` in lower-case. See our source file
`hcv_template.cc`. For example `<?hcv now?>` should be expanded into the
current date and time.

HTML files subject to template expansion should have an HTML comment
containing `!HelpCoVidDynamic!` in the first 8 lines.

### web conventions

HTTP [AJAX](https://en.wikipedia.org/wiki/Ajax_(programming)) requests (either `GET` or `PUT` have an URL starting with `/ajax/`

The user browser should support HTML5, AJAX, and
[WebSocket](https://en.wikipedia.org/wiki/WebSocket)s. The websocket
URL start with `/websocket/` and should be secure.



-----------


## building

Edit the `Makefile` then run `make`. Run the `./generate-config.py` script to
generate the configuration details for the web server and the PostgreSQL server.
Subsequently run `make localtest0` to run the test web server, and point your
browser to `http://localhost:8089/login` (replacing `http://localhost:8089` with
the URL you had specified in the ./generate-config.py script).

## PostGreSQL database

We use [PostGreSQL](https://www.postgresql.org/) and we require 
a [PostGreSQL 12](https://www.postgresql.org/docs/12/index.html) server.


To create the database on Debian, first get Linux root permission (e.g. 
with `sudo -s`). Then, according to
[this PostGreSQL tutorial](https://www.tutorialspoint.com/postgresql/postgresql_environment.htm) you need to run (as Linux root) the `su - postgres` command (which gives you access to the "PostGreSQL superuser") 
and run under that PostGreSQL user the `createdb helpcovid_db` command.

Concretely, the steps needed to setup the helpcovid database from psql are as
follows with PostGreSQL password `1234helpcovid` is:

```
$ sudo -u postgres psql
postgres =# CREATE DATABASE helpcovid_db;
postgres =# CREATE USER helpcovid_usr WITH PASSWORD 'passwd1234helpcovid';
postgres =# ALTER ROLE helpcovid_usr SET client_encoding TO 'utf8';
postgres =# ALTER ROLE helpcovid_usr SET default_transaction_isolation TO  'read committed';
postgres =# ALTER ROLE helpcovid_usr SET timezone TO 'UTC';
postgres =# GRANT ALL PRIVILEGES ON DATABASE helpcovid_db TO helpcovid_usr;
postgres =# \q 
```

Notice that to destroy entirely the database, we would use `DROP DATABASE helpcovid_db;`.

When running `helpcovid`, the database password may sit in [your
`$HOME/.pgpass`
file](https://www.postgresql.org/docs/current/libpq-pgpass.html). This file may
need to be set to have `0600` permissions.

The `$HOME/.pgpass` file would contain a single line similar to the following:
```
localhost:5432:helpcovid_db:helpcovid_usr:
```

However, there is a possible caveat to consider -- that the `$HOME/.pgpass` file
might be applicable to the `psql` program, and not to the `libpqxx` library. In
such a case, we could still benefit from having a password file (other than
`$HOME/.pgpass`) that contains the following connection string:
```
dbname=helpcovid_db user=helpcovid_usr password=passwd1234helpcovid  \
hostaddr=localhost port=5432
```

Read more about [PostGreSQL connection
string](https://www.postgresql.org/docs/12/libpq-connect.html#LIBPQ-CONNSTRING). Notice
that the password could be kept in a file given with `passfile`.


This connection string (excluding the backslash) would be read by the 
`hcv_initialize_database()` function and used in the constructor for 
`pqxx::Connection`.

To change passwords in PostGreSQL see [this](https://www.postgresqltutorial.com/postgresql-change-password/).



## configuration file

A mandatory configuration file should be provided, by the `--config`
program argument, or by the `$HELPCOVID_CONFIG` environment variable,
or in `$HOME/helpcovid.conf` or in the `/etc/helpcovid.conf` system
configuration file.

That configuration file is a [Glib key-value
file](https://developer.gnome.org/glib/stable/glib-Key-value-file-parser.html)
and cannot be world-readable. It is read by the `hcv_load_config_file`
C++ function and can be accessed by C++ functions
`hcv_config_has_group`, `hcv_config_has_key`, `hcv_config_do`.

### configuration groups

#### `helpcovid` group

It provides "global" settings

* `log_message`, a string giving some log message.

* `seteuid`, to use dangerous
  [seteuid](https://en.wikipedia.org/wiki/Setuid) facilities. **Never
  use them without first reviewing our [hcv_main.cc][] source file,
  because there could be a huge cybersecurity risk**.

* `startup_popen_command`, a dangerous string passed to
  [`popen(3)`](http://man7.org/linux/man-pages/man3/popen.3.html)
  which is run once at startup. Use with great caution, and review our
  code in [hcv_main.cc][] before using it.  The output of that pipe goes
  to the system log. It is run after `seteuid` facilities.

* `pid_file`, a file path where the process id of the running `helpcovid`
  process is written. Defaults to
  `/var/run/helpcovid.pid`. Overridable by `$HELPCOVID_PIDFILE` or
  `--write-pid` option.

* `threads`, the number of working threads. Overridable by `$HELPCOVID_NBWORKERTHREADS` or
  `--threads` option.

#### `web` group

It can provide the following keys (for [cpp-httplib](https://github.com/yhirose/cpp-httplib) ...):

* `url` for the served URL basename, e.g. `http://localhost:8089`; same role as `$HELPCOVID_WEBURL` environment variable or `--web-url` program option.

* `root` for the served HTTP root document directory, e.g. `/var/www/helpcovid/`; same role as `$HELPCOVID_WEBROOT` environment variable or `--webroot` program option.

* `sslcert` for the OpenSSL certificate (used for HTTPS) e.g. `/etc/helpcovid/sslcert.pem`; same role as `$HELPCOVID_SSLCERT` or `--websslcert`

* `sslkey` for the OpenSSL private key (used for HTTPS) e.g. `/etc/helpcovid/sslkey.pem`; same role as `$HELPCOVID_SSLKEY` or  `--websslkey`


### `postgresql` group

It can provide the following keys (for [libpqxx](http://pqxx.org/development/libpqxx) that is to [PostGreSQL](https://postgresql.org/):

* `connection`, the connection string for
  [`pcxx::connection`](http://pqxx.org/development/libpqxx); same role
  as `$HELPCOVID_POSTGRESQL` or `--postgresl-database`


------------------------------------------------

## Customising the logo

The logo for the application can be changed by replacing the default 
`helpcovid-logo.svg` icon found in the `webroot/images` directory.


## simple tests

First, run once and successfully the `generate-config.py`.

If something goes wrong, you may need to restore PostGreSQL to a pristine state with:

```
sudo -u postgres psql
DROP DATABASE helpcovid_db;
DROP USER helpcovid_usr;
\q
```

and one of the `DROP` above could be not needed.

The run `make localtest0`

After that, use your browser, e.g. on `http://localhost:8089/` if that was the URL you configured for `helpcovid`


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

First, one would **voluntarily** register
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


## List of HTTP Requests

See [HTTP_PROTOCOL.md](https://github.com/bstarynk/helpcovid/blob/master/HTTP_PROTOCOL.md)
	
## important notice

We provide the software, not the data.

## testing

Once you run `make` you might try something like `./helpcovid -W
http://192.168.0.1:8083/ -R $PWD/webroot/ -T 3 -D` for debugging
purposes (where `192.168.0.1` is an IPv4 address of your Linux computer, as
reported by `ip addr` -see
[ip(8)](http://man7.org/linux/man-pages/man8/ip.8.html)- or
[`ifconfig(8)`](http://man7.org/linux/man-pages/man8/ifconfig.8.html)
Linux commands). Then from a browser (maybe your mobile phone) access
http://192.168.0.1:8083/ or http://192.168.0.1:8083/status.json

We use the [address
sanitizer](https://en.wikipedia.org/wiki/AddressSanitizer). See the
`Makefile` and build with `make sanitized-helpcovid`.

You can also do `make localtest0`; see the `Makefile`

In March 2020 `./helpcovid` is often crashing.

[hcv_main.cc]:https://github.com/bstarynk/helpcovid/blob/master/hcv_main.cc
