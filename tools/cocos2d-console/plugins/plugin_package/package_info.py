
import time

from helper import PackageHelper

import cocos
from MultiLanguage import MultiLanguage

class PackageInfo(object):
    @staticmethod
    def plugin_name():
        return "info"

    @staticmethod
    def brief_description():
        return MultiLanguage.get_string('PACKAGE_INFO_BRIEF')

    def parse_args(self, argv):
        from argparse import ArgumentParser

        parser = ArgumentParser(prog="cocos package %s" % self.__class__.plugin_name(),
                                description=self.__class__.brief_description())
        parser.add_argument("name", metavar="NAME", help=MultiLanguage.get_string('PACKAGE_INFO_ARG_NAME'))
        parser.add_argument('-v', '--version', default='all', help=MultiLanguage.get_string('PACKAGE_INFO_ARG_VERSION'))
        return parser.parse_args(argv)

    def run(self, argv):
        args = self.parse_args(argv)
        name = args.name
        version = args.version
        package_data = PackageHelper.query_package_data(name, version)
        if package_data is None:
            print MultiLanguage.get_string('PACKAGE_INFO_ERROR_NO_PKG_FMT', (name, version))
            return

        if isinstance(package_data, list):
            for data in package_data:
                self.show_info(name, data)
                return

        if package_data.has_key('err'):
            print MultiLanguage.get_string('PACKAGE_INFO_ERROR_NO_PKG_FMT', (name, version))
            return
            
        self.show_info(name, package_data)

    def show_info(self, name, package_data):
        print MultiLanguage.get_string('PACKAGE_INFO_PKG_FMT',
              (name, package_data["name"], package_data["version"],
               time.strftime("%Y-%m-%d %H:%I:%S", time.gmtime(int(package_data["filetime"]))),
               package_data["author"], (int(package_data["filesize"]) / 1024),
               package_data["description"]))
