#!/bin/python3

import argparse
import glob
import os
import sys



class Generator:
    def __init__(self):
        self.root = self.html_files = self.po_dir = None

        try:
            with open(os.path.expanduser("~") + "/.helpcovidrc") as rcfile:
                for line in rcfile:
                    if "root" in line:
                        self.root = line.split("=")[1].strip()
                        self.html_files = glob.glob(self.root + "html/*.html")
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


    def run(self, lang):
        tag = "<?hcv msg"
        fmt = "msgid \"{0}\"\nmsgstr \"\"\n\n"
        path = self.po_dir + "helpcovid." + lang + ".po"
        with open(path, "w") as dest_file:
            for html_file in self.html_files:
                with open(html_file, "r") as src_file:
                    for line in src_file:
                        if tag in line:
                            msgid = line.split(tag)[1].split()[0]
                            dest_file.write(fmt.format(msgid))

        print("Generated " + path);



class Cmdline:
    def __init__(self):
        prog = "admin.py"
        desc = "Generates i18n PO files for HelpCovid"
        usage = "%(prog)s lang"
        help = "language to generate, such as en_US.UTF-8, fr_FR.UTF-8"

        parser = argparse.ArgumentParser(prog = prog, description = desc, 
            usage = usage)
        parser.add_argument("lang", type = str, help = help)

        self.args = parser.parse_args()


    def parse(self):
        if self.args.lang == "en_US.UTF-8" or self.args.lang == "fr_FR.UTF-8":
            Generator().run(self.args.lang)
        else:
            print("unrecognised language: " + self.args.lang)



if __name__ == "__main__":
    Cmdline().parse()

