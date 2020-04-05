#!/bin/python3

import argparse



class Generator:
    def __init__(self):
        pass


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

