#!/usr/bin/python3



import os
import pwd



def check_postgres_exists():
    try:
        pwd.getpwnam("postgres")
        return True

    except(KeyError):
        return False



def find_config_file():
    path = os.path.expanduser('~') + '/.helpcovidrc'
    if os.access(path, os.F_OK):
        print("Reading configuration file at " + path + "...")
        return path

    print("Configuration file not found at " + path + "...")
   
    while True:
        path = input("Enter configuration file path: ")
        if os.access(path, os.F_OK):
            print("Reading configuration file at " + path + "...")
            return path

        print("Configuration file not found at " + path + "...")



def main():
    print("Starting HelpCovid database backup...")
    print("See https://github.com/bstarynk/helpcovid and its README.md\n")

    if check_postgres_exists == False:
        print("PostgreSql database server not available; skipping...")
        return

    rc_path = find_config_file()


if __name__ == '__main__':
    main()

