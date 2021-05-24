# helpcovid

A quick and dirty C++17 web application ([free
software](https://www.gnu.org/philosophy/free-sw.en.html),
[GPLv3+](https://www.gnu.org/licenses/gpl-3.0.en.html)) to help
neighbours making essential buying (food, medicine) during the
[2019–20 coronavirus
pandemic](https://en.wikipedia.org/wiki/2019–20_coronavirus_pandemic)
for older neighbours (avoiding elder persons having to go out,
improving the necessary
[confinement](https://www.who.int/emergencies/diseases/novel-coronavirus-2019/advice-for-public),
assuming users are well behaved and wantinng to help or needing
help).

**For Linux/x86-64 only**. [GCC](http://gcc.gnu.org/) compiled.

The web server application runs on Linux (like most web servers). The
users are using *recent* web browsers (Firefox, Chrome, mobile phones)
on various computers (perhaps running non-Linux based operating
systems, such as Windows, MacOSX, etc...), tablets, mobile phones
connected to the [World Wide
Web](https://en.wikipedia.org/wiki/World_Wide_Web).

The `helpcovid` web server application is extensible thru
[plugins](https://en.wikipedia.org/wiki/Plug-in_(computing)). See our
file [PLUGINS.md](PLUGINS.md). So site-specific extensions are
possible. In particular because different countries have different
regulations.

### Related projects

[covid19md-voluntari-server](https://github.com/code4moldova/covid19md-voluntari-server)
in Python, from Moldavia. Also work in progress.

## development help is wanted

Please contact one of the authors by email. See email addresses
elsewhere.

If you read French, see also
[this](https://lists.debian.org/debian-user-french/2020/04/msg00063.html)
and the [CNIL](https://www.cnil.fr/) agency.

-----

Of course that application stores personal data in some
[PostGreSQL](http://postgresql.org/) database, so is concerned by
[GDPR](https://en.wikipedia.org/wiki/General_Data_Protection_Regulation)
... It is expected to be used in good faith. *HelpCovid* accepts
[plug-ins](https://en.wikipedia.org/wiki/Plug-in_(computing)) having a
[GPLv3+ compatible
license](https://www.gnu.org/licenses/license-list.html#GPLCompatibleLicenses),
and these are
[dlopen(3)](http://man7.org/linux/man-pages/man3/dlopen.3.html) at
startup time.

*HelpCovid* may be configured to use
[setuid](https://en.wikipedia.org/wiki/Setuid) techniques. Beware that
using them incorrectly can ruin the cybersecurity of your entire Linux
servers.

If [RefPerSys](http://refpersys.org/) was ready, it should have been
used instead.

In France, see also [enpremiereligne.fr](https://enpremiereligne.fr/)
and [covid19-que-lire.fr](https://www.covid19-que-lire.fr); look also
into
[covid19-floss-initiatives](https://github.com/bzg/covid19-floss-initatives)


## why C++17 with HTML5

Because the main developers know it, and because it should be more
efficient than a PHP or Python application (which one of the
developers [Basile
Starynkevitch](http://starynkevitch.net/Basile/index_en.html) does not
know well). Hopefully, a C++17 code could be more
efficient. Scalability is achieved also by using a PostGreSQL database
which could run on a different server.

## expected end-user audience

**The end-user is expected to have a computer or a mobile phone with a
recent Web browser** (e.g. Firefox 68) understanding HTML5 and
JavaScript
(i.e. [AJAX](https://en.wikipedia.org/wiki/Ajax_(programming)) and
[EcmaScript6](https://ecma-international.org/ecma-262/6.0/) ...) and
with [WebSocket](https://en.wikipedia.org/wiki/WebSocket)s and
connected to the Internet. The end-user is supposed to be honest and a
good enough person, and will volunteerly but in good faith add
honestly some personal information about him/her-self (real name,
email, phone number, perhaps even if he/she is in good health). We do
try to code in a defensive way, against some malicious hackers.  We
have no time for [W3C accessibility
issues](https://www.w3.org/standards/webdesign/accessibility)
(e.g. usage of `helpcovid` by blind persons) but accept patches to
improve accessibility.

We hope to add something to help health professionals and food shops
(e.g. registering their actual open hours and availability).

We want to use [OpenStreetMap](https://www.openstreetmap.org/), but
are not familiar with it.


We want to code a web application which could be deployed in several
countries (hopefully one Linux VPS server per city).

## license

Of course [GPLv3+](https://www.gnu.org/licenses/gpl-3.0.en.html)

Notice that you take the responsability about personal data and
privacy issues when *deploying* this free software
(e.g. [GDPR](https://en.wikipedia.org/wiki/General_Data_Protection_Regulation)
in Europe).


## plugins

See [PLUGINS.md](PLUGINS.md) for more. Several plugins are possible,
and might provide country or city specific features.

## relational database

We use a [PostGreSQL](https://postgresql.org) relational database. See
[DATABASE.md](DATABASE.md) for more.

## contributors

* [Basile Starynkevitch](http://starynkevitch.net/Basile/), near
  [Paris](https://en.wikipedia.org/wiki/Paris), France; email
  `basile-freelance@starynkevitch.net` - see [my web
  page](http://starynkevitch.net/Basile/) and [my
  resume](http://starynkevitch.net/Basile/cv-Basile-Starynkevitch.pdf)
  for contact details.

* [Abhishek Chakravarti](http://taranjali.org/aboutme), near [Kolkota](https://en.wikipedia.org/wiki/Kolkata), India; email `abhishek@taranjali.org` 


* [Niklas Rosencrantz](https://github.com/montao/), from
  [Stockholm](https://en.wikipedia.org/wiki/Stockholm), Sweden; email `niklasro@gmail.com`

Captchas photos made by Basile Starynkevitch and resized by [Matthieu
Starynkevitch](http://matthieu-starynkevitch.com/).

## dependencies

* [libpq](https://www.postgresql.org/docs/11/libpq.html) from PostGreSQL 11 for the database.

* [libpqxx](http://pqxx.org/development/libpqxx) for C++ frontend to PostGreSQL.

* [cpp-httplib](https://github.com/yhirose/cpp-httplib) for C++ [HTTPS](https://en.wikipedia.org/wiki/HTTPS) service.

* [curlpp](https://www.curlpp.org/), a C++ wrapper around the famous
  [libcurl](https://curl.haxx.se/libcurl/), which is an HTTP and HTTPS
  client library. Some resources useful for *HelpCovid* may need to be
  downloaded at startup time by `./helpcovid` or its plugins.

* [Websocket++](https://github.com/zaphoyd/websocketpp) for [WebSockets](https://en.wikipedia.org/wiki/WebSocket).

* [Twitter Bootstrap](https://github.com/twbs/bootstrap) for the responsive CSS framework.

* [Start Bootstrap - Blog Home](https://github.com/BlackrockDigital/startbootstrap-blog-home) for the Bootstrap theme.

* [Bootstrap Cookie Alert](https://github.com/Wruczek/Bootstrap-Cookie-Alert) for the GDPR notice popup.

On  [Debian](https://debian.org/) (Buster) run:

`sudo aptitude install postgresql-server-dev-11 postgresql-client-11 postgresql-11 libpqxx-dev libconfig++-dev libglibmm-2.4-dev libcurlpp-dev`

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
processing instructions on a *single* line such as, for example
something as `<?hcv msg MAINPAGE_HEADING Page Heading?>` handled by
C++ code in `hcv_template.cc` function `hcv_initialize_templates`
which should use
[locale(7)](http://man7.org/linux/man-pages/man7/locale.7.html)
facilities such as
[gettext(3)](http://man7.org/linux/man-pages/man3/gettext.3.html), and
[chunk customization](CUSTOMIZATION.md) facilities.


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

The user browser should support HTML5, AJAX, and
[WebSocket](https://en.wikipedia.org/wiki/WebSocket)s. The websocket
URL start with `/websocket/` and should be secure.

See mostly [HTTP_PROTOCOL.md](HTTP_PROTOCOL.md) for more.


-----------


## building

Edit the `Makefile` then run `make`. Run the `./generate-config.py` script to
generate the configuration details for the web server and the PostgreSQL server. That `./generate-config.py` script knows about the `--help` option.

Subsequently run `make localtest0` to run the test web server, and point your
browser to `http://localhost:8089/login` (replacing `http://localhost:8089` with
the URL you had specified in the `./generate-config.py` script).

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

It provides "global" settings. See our C++ function
`hcv_config_handle_helpcovid_config_group`.

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

* `html_email_popen_command`, a command
  [`popen(3)`](http://man7.org/linux/man-pages/man3/popen.3.html)-ed
  to send HTML5 emails. Invoked as *command* *subject*
  *destination-email* ... and getting the HTML5 body as standard
  output.

* `pid_file`, a file path where the process id of the running `helpcovid`
  process is written. Defaults to
  `/var/run/helpcovid.pid`. Overridable by `$HELPCOVID_PIDFILE` or
  `--write-pid` option.

* `threads`, the number of working threads. Overridable by `$HELPCOVID_NBWORKERTHREADS` or
  `--threads` option.

* `locale`, for localization, see
  [locale(7)](http://man7.org/linux/man-pages/man7/locale.7.html),
  [setlocale(3)](http://man7.org/linux/man-pages/man3/setlocale.3.html),
  [dggettext(3)](http://man7.org/linux/man-pages/man3/dgettext.3.html). Overridable
  by `$HELPCOVID_LOCALE` or `--locale` option. See C++ functions
  `hcv_view_expand_msg` in file `hcv_views.cc` and `hcv_get_locale` in
  file `hcv_main.cc` ...

* `custom_messages_file` for customized messages organized in one or
  several chunkfiles. See also
  [CUSTOMIZATION.md](CUSTOMIZATION.md). That file path is tilde- and
  dollar- expanded (like shells do,
  e.g. [globbing](https://en.wikipedia.org/wiki/Glob_(programming)))
  to *several* files using
  [wordexp(3)](http://man7.org/linux/man-pages/man3/wordexp.3.html)
  (see also
  [glob(7)](http://man7.org/linux/man-pages/man7/glob.7.html)...).
  These chunk files should have a name ending with a letter or digit.


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

## email sent by HelpCovid application

See C++ class `Hcv_email_template_data` implemented in C++ file `hcv_template.cc` and configuration  `html_email_popen_command` in group `[helpcovid]`.
See C++ functions `hcv_database_with_known_email`, `hcv_user_model_find_by_email`, etc...

The emails are sent in HTML5 format and customized by files under `emailtempl/`  directory.

## communication

We use the `HelpCovid software` group on [https://web.whatsapp.com/](WhatsApp)

The phone number of Basile Starynkevitch there is `+33 6 8501 2359`


## email forum `helpcovid@framalistes.org`

See https://framalistes.org/sympa/info/helpcovid

Please follow the following conventions and stay civil and nice. The
forum website is overloaded, so don't work quickly. Expect a few hours
of delay between sending a message there and seeing it thru the web
interface.

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

First, one would **voluntarily** register himself/herself, with *first
name*, *last name*, *birth year* or approximate age (e.g. I am between
50 and 55 years old), *email*, *phone*, *home-address* and optionally
possibly more health related information.

Then a user would -on his/her free will- add more information about
himself/herself. We dream of providing an infrastructure where someone
could on his free will declare:

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

En premier lieu, un utilisateur s'enregistrerait **de son plein gré** avec ses *prénoms*, *nom de famille*, *année de naissance* (ou *tranche d'âge*), *lieu de résidence*, et peut-être même des informations liées à sa santé.

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

## Deploying HelpCovid

See [DEPLOYMENT.md](https://github.com/bstarynk/helpcovid/blob/master/DEPLOYMENT.md)

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

If your system's `/etc/hosts` file contains a line `0.0.0.0 anyhost`
you could try to run `./helpcovid -W http://anyhost:8089/ -R
$PWD/webroot/ -T 2 -D` but then beware if your computer is connected
to the Internet with some open ports: your `helpcovid` process would
then be accessible from outside.

To generate self-signed HTTPS certificates for our `--websslcert=` and
`--websslkey` program options, see
[this](https://stackoverflow.com/a/10176685/841108).


[hcv_main.cc]:https://github.com/bstarynk/helpcovid/blob/master/hcv_main.cc
