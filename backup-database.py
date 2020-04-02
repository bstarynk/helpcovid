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

    print("NOTE: configuration file not found at " + path + "...")
   
    while True:
        path = input("Enter configuration file path: ")
        if os.access(path, os.F_OK):
            print("Reading configuration file at " + path + "...")
            return path

        print("NOTE: configuration file not found at " + path + "...")



def get_connection_string(path):
    keys = {}
    with open(path) as rcfile:
        for line in rcfile:
            name, var = line.partition("=")[::2]
            keys[name.strip()] = str(var)

    if "connection" in keys:
        print("Found connection string: " + keys["connection"])
        return keys["connection"]

    print("ERROR: connection string not found in " + path + "...")
    return None



def main():
    print("Starting HelpCovid database backup...")
    print("See https://github.com/bstarynk/helpcovid and its README.md\n")

    if check_postgres_exists == False:
        print("PostgreSql database server not available; skipping...")
        return

    path = find_config_file()
    connstr = get_connection_string(path)
    if connstr is None:
        return



if __name__ == '__main__':
    main()

