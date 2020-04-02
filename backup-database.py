#!/usr/bin/python3



import os
import pwd
import time



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



def parse_connection_string(connstr):
    keys = {}

    split = connstr.split("=", 1)[1]
    split = split.split()

    keys["dbname"] = split[0]
    keys["user"] = split[1].split("=")[1]
    keys["host"] = split[3].split("=")[1]
    keys["port"] = split[4].split("=")[1]

    return keys



def backup(keys):
    fname = time.strftime("helpcovid_db-%Y-%m-%d-%H-%M-%S", time.gmtime())
    cmd = "sudo pg_dump -U {0} -h {1} -p {2} {3} | gzip > {4}.gz".format(
        keys["user"], keys["host"], keys["port"], keys["dbname"], fname)

    print("Starting database backup...")
    print(cmd)
    os.system(cmd)



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

    keys = parse_connection_string(connstr)
    backup(keys)


if __name__ == '__main__':
    main()

