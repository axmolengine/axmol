
from helper import PackageHelper
import cocos
from MultiLanguage import MultiLanguage

class FrameworkAdd(object):
    @staticmethod
    def plugin_name():
        return "search"

    @staticmethod
    def brief_description():
        return MultiLanguage.get_string('PACKAGE_SEARCH_BRIEF')

    def parse_args(self, argv):
        from argparse import ArgumentParser
        parser = ArgumentParser(
            prog="cocos package %s" % self.__class__.plugin_name(),
            description=self.__class__.brief_description())
        parser.add_argument("keyword", metavar="PACKAGE_NAME",
                            help=MultiLanguage.get_string('PACKAGE_SEARCH_ARG_KEY'))
        return parser.parse_args(argv)

    def run(self, argv):
        args = self.parse_args(argv)
        keyword = args.keyword
        packages = PackageHelper.search_keyword(keyword)
        if packages is None:
            print MultiLanguage.get_string('PACKAGE_SEARCH_ERROR_NO_KEY_FMT', keyword)
            return

        keys = packages.keys()
        print MultiLanguage.get_string('PACKAGE_SEARCH_TIP_FMT', keyword)
        keys.sort()
        for k in keys:
            package_data = packages[k]
            print MultiLanguage.get_string('PACKAGE_ITEM_FMT',
                                           (package_data["name"], package_data["version"], package_data["author"]))

        print ""
