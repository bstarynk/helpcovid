#!/usr/bin/python3

##
## Description:
##      Internationalization and customization script for https://github.com/bstarynk/helpcovid
##
## File generate-i18n.py of github.com/bstarynk/helpcovid
##
## Author(s):
##      © Copyright 2020
##      Basile Starynkevitch <basile@starynkevitch.net>
##      Abhishek Chakravarti <abhishek@taranjali.org>
##
##
## License:
##    This HELPCOVID program is free software: you can redistribute it and/or modify
##    it under the terms of the GNU General Public License as published by
##    the Free Software Foundation, either version 3 of the License, or
##    (at your option) any later version.
##
##    This program is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.
##
##    You should have received a copy of the GNU General Public License
##    along with this program.  If not, see <http://www.gnu.org/licenses/>.
##****************************************************************************


import argparse
import glob
import os
import sys


# https://www.metamodpro.com/browser-language-codes
ACCEPT_LANGUAGES = [
    "af",       # Afrikaans
    "sq",       # Albanian
    "ar",       # Arabic (Standard)
    "ar-dz",    # Arabic (Algeria)
    "ar-bh",    # Arabic (Bahrain)
    "ar-eg",    # Arabic (Egypt)
    "ar-iq",    # Arabic (Iraq)
    "ar-jo",    # Arabic (Jordan)
    "ar-kw",    # Arabic (Kuwait)
    "ar-lb",    # Arabic (Lebanon)
    "ar-ly",    # Arabic (Libya)
    "ar-ma",    # Arabic (Morocco)
    "ar-om",    # Arabic (Oman)
    "ar-qa",    # Arabic (Qatar)
    "ar-sa",    # Arabic (Saudi Arabia)
    "ar-sy",    # Arabic (Syria)
    "ar-tn",    # Arabic (Tunisia)
    "ar-ae",    # Arabic (U.A.E.)
    "ar-ye",    # Arabic (Yemen)
    "ar",       # Aragonese
    "hy",       # Armenian
    "as",       # Assamese
    "ast",      # Asturian
    "az",       # Azerbaijani
    "eu",       # Basque
    "bg",       # Bulgarian
    "be",       # Belarusian
    "bn",       # Bengali
    "bs",       # Bosnian
    "br",       # Breton
    "bg",       # Bulgarian
    "my",       # Burmese
    "ca",       # Catalan
    "ch",       # Chamorro
    "ce",       # Chechen
    "zh",       # Chinese
    "zh-hk",    # Chinese (Hong Kong)
    "zh-cn",    # Chinese (PRC)
    "zh-sg",    # Chinese (Singapore)
    "zh-tw",    # Chinese (Taiwan)
    "cv",       # Chuvash
    "co",       # Corsican
    "cr",       # Cree
    "hr",       # Croatian
    "cs",       # Czech
    "da",       # Danish
    "nl",       # Dutch (Standard)
    "nl-be",    # Dutch (Belgian)
    "en",       # English
    "en-au",    # English (Australia)
    "en-bz",    # English (Belize)
    "en-ca",    # English (Canada)
    "en-ie",    # English (Ireland)
    "en-jm",    # English (Jamaica)
    "en-nz",    # English (New Zealand)
    "en-ph",    # English (Philippines)
    "en-za",    # English (South Africa)
    "en-tt",    # English (Trinidad & Tobago)
    "en-gb",    # English (United Kingdom)
    "en-us",    # English (United States)
    "en-zw",    # English (Zimbabwe)
    "eo",       # Esperanto
    "et",       # Estonian
    "fo",       # Faeroese
    "fa",       # Farsi
    "fj",       # Fijian
    "fi",       # Finnish
    "fr",       # French (Standard)
    "fr-be",    # French (Belgium)
    "fr-ca",    # French (Canada)
    "fr-fr",    # French (France)
    "fr-lu",    # French (Luxembourg)
    "fr-mc",    # French (Monaco)
    "fr-ch",    # French (Switzerland)
    "fy",       # Frisian
    "fur",      # Friulian
    "gd",       # Gaelic (Scots)
    "gd-ie",    # Gaelic (Irish)
    "gl",       # Galacian
    "ka",       # Georgian
    "de",       # German (Standard)
    "de-at",    # German (Austria)
    "de-de",    # German (Germany)
    "de-li",    # German (Liechtenstein)
    "de-lu",    # German (Luxembourg)
    "de-ch",    # German (Switzerland)
    "el",       # Greek
    "gu",       # Gujurati
    "ht",       # Haitian
    "he",       # Hebrew
    "hi",       # Hindi
    "hu",       # Hungarian
    "is",       # Icelandic
    "id",       # Indonesian
    "iu",       # Inuktitut
    "ga",       # Irish
    "it",       # Italian (Standard)
    "it-ch",    # Italian (Switzerland)
    "ja",       # Japanese
    "kn",       # Kannada
    "ks",       # Kashmiri
    "kk",       # Kazakh
    "km",       # Khmer
    "ky",       # Kirghiz
    "tlh",      # Klingon
    "ko",       # Korean
    "ko-kp",    # Korean (North Korea)
    "ko-kr",    # Korean (South Korea)
    "la",       # Latin
    "lv",       # Latvian
    "lt",       # Lithuanian
    "lb",       # Luxembourgish
    "mk",       # FYRO Macedonian
    "ms",       # Malay
    "ml",       # Malayalam
    "mt",       # Maltese
    "mi",       # Maori
    "mr",       # Marathi
    "mo",       # Moldavian
    "nv",       # Navajo
    "ng",       # Ndonga
    "ne",       # Nepali
    "no",       # Norwegian
    "nb",       # Norwegian (Bokmal)
    "nn",       # Norwegian (Nynorsk)
    "oc",       # Occitan
    "or",       # Oriya
    "om",       # Oromo
    "fa",       # Persian
    "fa-ir",    # Persian/Iran
    "pl",       # Polish
    "pt",       # Portuguese
    "pt-br",    # Portuguese (Brazil)
    "pa",       # Punjabi
    "pa-in",    # Punjabi (India)
    "pa-pk",    # Punjabi (Pakistan)
    "qu",       # Quechua
    "rm",       # Rhaeto-Romanic
    "ro",       # Romanian
    "ro-mo",    # Romanian (Moldavia)
    "ru",       # Russian
    "ru-mo",    # Russian (Moldavia)
    "sz",       # Sami (Lappish)
    "sg",       # Sango
    "sa",       # Sanskrit
    "sc",       # Sardinian
    "gd",       # Scots Gaelic
    "sd",       # Sindhi
    "si",       # Sinhalese
    "sr",       # Serbian
    "sk",       # Slovak
    "sl",       # Slovenian
    "so",       # Somali
    "sb",       # Sorbian
    "es",       # Spanish
    "es-ar",    # Spanish (Argentina)
    "es-bo",    # Spanish (Bolivia)
    "es-cl",    # Spanish (Chile)
    "es-co",    # Spanish (Colombia)
    "es-cr",    # Spanish (Costa Rica)
    "es-do",    # Spanish (Dominican Republic)
    "es-ec",    # Spanish (Ecuador)
    "es-sv",    # Spanish (El Salvador)
    "es-gt",    # Spanish (Guatemala)
    "es-hn",    # Spanish (Honduras)
    "es-mx",    # Spanish (Mexico)
    "es-ni",    # Spanish (Nicaragua)
    "es-pa",    # Spanish (Panama)
    "es-py",    # Spanish (Paraguay)
    "es-pe",    # Spanish (Peru)
    "es-pr",    # Spanish (Puerto Rico)
    "es-es",    # Spanish (Spain)
    "es-uy",    # Spanish (Uruguay)
    "es-ve",    # Spanish (Venezuela)
    "sx",       # Sutu
    "sw",       # Swahili
    "sv",       # Swedish
    "sv-fi",    # Swedish (Finland)
    "sv-sv",    # Swedish (Sweden)
    "ta",       # Tamil
    "tt",       # Tatar
    "te",       # Teluga
    "th",       # Thai
    "tig",      # Tigre
    "ts",       # Tsonga
    "tn",       # Tswana
    "tr",       # Turkish
    "tk",       # Turkmen
    "uk",       # Ukrainian
    "hsb",      # Upper Sorbian
    "ur",       # Urdu
    "ve",       # Venda
    "vi",       # Vietnamese
    "vo",       # Volapuk
    "wa",       # Walloon
    "cy",       # Welsh
    "xh",       # Xhosa
    "ji",       # Yiddish
    "zu",       # Zulu
]
    


class Generator:
    def __init__(self, lang):

        self.lang = lang

        self.root = None
        self.__load_root()

        self.html_files = glob.glob(self.root + "html/*.html")
        self.po_dir = self.root + "i18n" + "/"

        self.messages = {}
        if os.path.exists(self.po_dir):
            self.__load_messages()
        else:
            self.__make_po_dir()


    def __load_root(self):
        try:
            with open(os.path.expanduser("~") + "/.helpcovidrc") as rcfile:
                for line in rcfile:
                    if "root" in line:
                        self.root = line.split("=")[1].strip()
                        return

            print("root key not found in ~/.helpcovidrc file, please run"
                    "generate-config.py again")

        except(FileNotFoundError):
            print("~/.helpcovidrc file not found, run generate-configy.py")
            sys.exit(1)


    def __make_po_dir(self):
        try:
            print("i18n directory not found, creating...")
            os.makedirs(self.po_dir)

        except(FileNotFoundError):
            print("Error in creating i18n directory!")


    def __load_messages(self):
        path = self.po_dir + "helpcovid." + self.lang + ".po"

        try:
            with open(path, "r") as po_file:
                print(path + " found, loading existing messages...")
                
                msgid = None

                for line in po_file:
                    if "msgid" in line:
                        msgid = line.split("msgid ")[1].strip().strip('"')
                        continue

                    if "msgstr" in line and msgid:
                        msgstr = line.split("msgstr ")[1].strip().strip('"')
                        self.messages[msgid] = msgstr
                        msgid = None
                        continue

        except(FileNotFoundError):
            return


    def __scan_msgids_from_html(self):
        tag = "<?hcv msg"
        msgids = []
        for html_file in self.html_files:
            with open(html_file, "r") as src_file:
                for line in src_file:
                    if tag in line:
                        msgid = line.split(tag)[1].split()[0]
                        msgids.append(msgid)

        msgids = list(set(msgids))
        msgids.sort()

        return msgids


    def __generate_serial_number(self, num, total):
        padding = len(list(str(total))) - len(list(str(num)))
        if padding:
            padding = "0" * padding
        else:
            padding = ""

        slno = padding + str(num)
        slno = slno.replace("0", "\u2080")
        slno = slno.replace("1", "\u2081")
        slno = slno.replace("2", "\u2082")
        slno = slno.replace("3", "\u2083")
        slno = slno.replace("4", "\u2084")
        slno = slno.replace("5", "\u2085")
        slno = slno.replace("6", "\u2086")
        slno = slno.replace("7", "\u2087")
        slno = slno.replace("8", "\u2088")
        slno = slno.replace("9", "\u2089")

        return slno


    def run(self):
        path = self.po_dir + "helpcovid." + self.lang + ".po"
        msgids = self.__scan_msgids_from_html()

        if len(self.messages):
            print("Updating " + path + "...")
            delta = list(set(msgids) - set(self.messages.keys()))

            for msgid in delta:
                self.messages[msgid] = ""

            self.messages = dict(sorted(self.messages.items()))

            fmt = "msgid \"{0}{1}\"\nmsgstr \"{2}\"\n\n"
            with open(path, "w") as dest_file:
                i = 0
                total = len(self.messages.items())
                for msgid, msgstr in self.messages.items():
                    i = i + 1
                    slno = self.__generate_serial_number(i, total)
                    dest_file.write(fmt.format(slno, msgid, msgstr))

        else:
            fmt = "msgid \"{0}{1}\"\nmsgstr \"\"\n\n"
            with open(path, "w") as dest_file:
                i = 0
                total = len(msgids)
                for msgid in msgids:
                    i = i + 1
                    slno = self.__generate_serial_number(i, total)
                    dest_file.write(fmt.format(slno, msgid))

            print("Generated skeletal " + path);



class Cmdline:
    def __init__(self):
        prog = "generate-i18n.py"
        desc = "Generates i18n and customizing PO files for HelpCovid; see https://github.com/bstarynk/helpcovid/"
        usage = "%(prog)s lang"
        help = ("language recognised by the Accept-Language header,"
                "such as en-us, fr-fr, sv")

        parser = argparse.ArgumentParser(prog = prog, description = desc, 
            usage = usage)
        parser.add_argument("lang", type = str, help = help)

        self.args = parser.parse_args()


    def parse(self):
        if self.args.lang in ACCEPT_LANGUAGES:
            Generator(self.args.lang).run()
        else:
            print("unrecognised language: " + self.args.lang)



if __name__ == "__main__":
    Cmdline().parse()

