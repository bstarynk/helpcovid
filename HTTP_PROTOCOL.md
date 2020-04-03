# List of HTTP Requests in HelpCovid

See https://github.com/bstarynk/helpcovid/ for more about HelpCovid


The HelpCovid server interacts with the web browser through HTTP requests made
by two categories of URLs. The standard URLs are GET requests returning HTML
pages, and the API URLS are used to make AJAX requests. The API URLs are
prefixed with `/api/`.

| URL             | Method | MIME  | Purpose                           |
| :-------------: |:------:|:-----:|-----------------------------------|
| /login          | GET    | HTML  | Display login page                |
| /api/login      | POST   | JSON  | Perform login request             |
| /index          | GET    | HTML  | Display index page                |
| /profile        | GET    | HTML  | Display user's profile page       |
| /api/profile    | PUT    | JSON  | Update user's profile             |
| /api/profile    | DELETE | JSON  | Delete user's profile             |
| /help           | GET    | HTML  | Display neighbours requiring help |
| /api/help       | POST   | JSON  | Respond to neighbour needing help |
| /helper         | GET    | HTML  | Display neighbours will to help   |
| /api/helper     | POST   | JSON  | Accept help from a neighbour      |
