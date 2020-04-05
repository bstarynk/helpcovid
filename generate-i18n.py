#!/bin/python3

import argparse
import os
import sys



class Generator:
    def __init__(self):
        self.root = self.po_dir = None

        try:
            with open(os.path.expanduser("~") + "/.helpcovidrc") as rcfile:
                for line in rcfile:
                    if "root" in line:
                        self.root = line.split("=")[1].strip() + "/"
                        self.po_dir = self.root + "i18n" + "/"
                        if not os.path.exists(self.po_dir):
                            os.makedirs(self.po_dir)
                        break

        except(FileNotFoundError):
            if not self.root:
                print("~/.helpcovidrc file not found, run generate-config.py"
                        " first")
            else:
                print("Error in creating i18n directory!")

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
            print("unrecognised language: " + self.args.lang)



if __name__ == "__main__":
    Cmdline().parse()

