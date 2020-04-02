#!/usr/bin/python3


import pwd


def check_postgres_exists():
    try:
        pwd.getpwnam("postgres")
        return True

    except(KeyError):
        return False



def main():
    print("Starting HelpCovid database backup...")
    print("See https://github.com/bstarynk/helpcovid and its README.md\n")

    if check_postgres_exists == False:
        print("PostgreSql database server not available; skipping...")
        return


if __name__ == '__main__':
    main()

