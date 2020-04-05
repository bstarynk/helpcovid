#!/bin/python3

import argparse
import os
import sys



class Generator:
    def __init__(self):
        self.root = None

        try:
            with open(os.path.expanduser("~") + "/.helpcovidrc") as rcfile:
                for line in rcfile:
                    if "root" in line:
                        self.root = line.split("=")[1]
                        break

        except(FileNotFoundError):
            print("~/.helpcovidrc file not found, run generate-config.py first")
            sys.exit(1)


    def en(self):
        pass


    def fr(self):
        pass



class Cmdline:
    def __init__(self):
        prog = "admin.py"
        desc = "Generates i18n PO files for HelpCovid"
        usage = "%(prog)s lang"
        help = "language to generate, one of: en, fr"

        parser = argparse.ArgumentParser(prog = prog, description = desc, 
            usage = usage)
        parser.add_argument("lang", type = str, help = help)

        self.args = parser.parse_args()


    def parse(self):
        if self.args.lang == "en":
            Generator().en()
        elif self.args.lang == "fr":
            Generator().fr()
        else:
            print("unrecognised language: " + args.lang)



if __name__ == "__main__":
    Cmdline().parse()

