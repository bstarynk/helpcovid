#  HTTP and HTTPS protocol in HelpCovid

See https://github.com/bstarynk/helpcovid/ for more about HelpCovid

It uses [HTTP
1.1](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol). HTTP2
is not supported yet in April 2020. The user browser should be a
recent one (e.g. [Firefox](https://www.mozilla.org/en-US/firefox/) 68
or newer) for HTML5 and JavaScript6. Old browsers are not supported.

HTTP [AJAX](https://en.wikipedia.org/wiki/Ajax_(programming)) requests (either `GET` or `PUT` have an URL starting with `/ajax/`

The user browser should support HTML5, AJAX, and
[WebSocket](https://en.wikipedia.org/wiki/WebSocket)s. The websocket
URL start with `/websocket/` and should be secure.


## HTTP cookies

HelpCovid manage one single [web
cookie](https://en.wikipedia.org/wiki/HTTP_cookie) named
`HelpCovid_COOKIE` (in C++, the `HCV_COOKIE_NAME` macro), containing
some randomly generated data. See C++ functions
`hcv_web_register_fresh_cookie` and `hcv_web_forget_cookie` and SQL
table `tb_web_cookie`, PostGreSQL prepared statement
`add_web_cookie_pstm` used by function
`hcv_database_get_id_of_added_web_cookie`. A typical cookie value
could be `HCV0002a4-hD1c22YSXqrf2i8bhnPGR6Rr-Aee0a9546` where
`hD1c22YSXqrf2i8bhnPGR6Rr` is a random string kept in the database.
That session cookie should expire in less than 12 hours (both in the
browser and in the database).

## HTTP requests and responses

The HelpCovid server interacts with the web browser through HTTP (or
HTTPS) requests made by two categories of URLs. The standard URLs are
`GET` requests returning HTML pages, and the API URLS are used to make
AJAX requests. The API URLs are prefixed with `/api/`.

| URL             | Method | MIME  | Purpose                           |
| :-------------: |:------:|:-----:|-----------------------------------|
| /login          | GET    | HTML  | Display login page                |
| /api/login      | POST   | JSON  | Perform login request             |
| /register       | GET    | HTML  | Display registration page         |
| /api/register   | POST   | JSON  | Perform registration request      |
| /index          | GET    | HTML  | Display index page                |
| /profile        | GET    | HTML  | Display user's profile page       |
| /api/profile    | PUT    | JSON  | Update user's profile             |
| /api/profile    | DELETE | JSON  | Delete user's profile             |
| /help           | GET    | HTML  | Display neighbours requiring help |
| /api/help       | POST   | JSON  | Respond to neighbour needing help |
| /helper         | GET    | HTML  | Display neighbours will to help   |
| /api/helper     | POST   | JSON  | Accept help from a neighbour      |

