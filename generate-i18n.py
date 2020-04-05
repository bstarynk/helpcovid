#!/bin/python3

import argparse



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
        if self.args.cmd == "en":
            pass
        elif self.args.cmd == "fr":
            pass
        else:
            print("unrecognised language: " + args.lang)



if __name__ == "__main__":
    Cmdline().parse()

