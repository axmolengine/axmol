
import os
import os.path
import json

import cocos
from MultiLanguage import MultiLanguage

class LocalPackagesDatabase(object):
    def __init__(self, path):
        self._path = path
        if os.path.isfile(self._path):
            f = open(self._path, "rb")
            self._data = json.load(f)
            f.close()
        else:
            self._data = {}

    def get_packages(self):
        return self._data.copy()

    def add_package(self, package_data):
        key = package_data["name"] + "-" + package_data["version"]
        self._data[key] = package_data
        self.update_database()
        print MultiLanguage.get_string('PACKAGE_PKG_ADD_OK_FMT', key)

    def remove_package(self, package_data):
        key = package_data["name"] + "-" + package_data["version"]
        if key in self._data:
            del self._data[key]
            self.update_database()
            print MultiLanguage.get_string('PACKAGE_PKG_REMOVE_OK_FMT', key)
        else:
            message = MultiLanguage.get_string('PACKAGE_PKG_NOT_FOUND_PKG_FMT', key)
            raise cocos.CCPluginError(message, cocos.CCPluginError.ERROR_WRONG_CONFIG)

    def update_database(self):
        f = open(self._path, "w+b")
        str = json.dump(self._data, f)
        f.close()
        print MultiLanguage.get_string('PACKAGE_PKG_UPDATE_OK_FMT', self._path)

