
import os
import os.path
import json
import urllib2
import re

import cocos
from MultiLanguage import MultiLanguage

from functions import *
from local_package_database import LocalPackagesDatabase
from zip_downloader import ZipDownloader

def convert_version_part(version_part):
    tag = '(\d+)(\D*.*)'
    match = re.search(tag, version_part)
    if match is None:
        return 0

    return int(match.group(1)), match.group(2)

def compare_extra_version_string(str1, str2):
    if str1 == str2:
        return 0

    if len(str1) == 0:
        return 1
    elif len(str2) == 0:
        return -1

    if str1 > str2:
        return 1
    else:
        return -1


def compare_version(version1, version2):
    v1 = re.split('\.', version1)
    v2 = re.split('\.', version2)
    n1 = len(v1)
    n2 = len(v2)

    if n1 > n2:
        n = n1
        for x in xrange(n2,n):
            v2.append("0")
    else:
        n = n2
        for x in xrange(n1,n):
            v1.append("0")

    for x in xrange(0,n):
        ver_num1, ver_str1 = convert_version_part(v1[x])
        ver_num2, ver_str2 = convert_version_part(v2[x])
        if ver_num1 > ver_num2:
            return 1
        elif ver_num2 > ver_num1:
            return -1
        
        c = compare_extra_version_string(ver_str1, ver_str2)
        if c != 0:
            return c

    return 0        

def get_newer_package(package1, package2):
    if compare_version(package1["version"], package2["version"]) > 0:
        return package1
    else:
        return package2

def get_packages_adapt_engine(packages, engine):
    packages_out = []
    for package in packages:
        package_engine = package["engine"]
        if package_engine[-1] == '+':
            flag = True
            package_engine = package_engine[:-1]
        else:
            flag = False
        c = compare_version(engine, package_engine)
        if flag and c >= 0:
            packages_out.append(package)
        elif c == 0:
            packages_out.append(package)

    if len(packages_out) > 0:
        return packages_out

class PackageHelper:
    REPO_URL = "http://pmr.cocos.com/"
    REPO_PACKAGES_DIR = "packages"
    WORKDIR = ".cocos" + os.sep + "packages"
    LOCALDB_FILENAME = "local_packages.json"
    QUERY_PACKAGE_URL = REPO_URL + "?name=%s"
    QUERY_KEYWORD_URL = REPO_URL + "?keyword=%s"

    @classmethod
    def get_workdir(cls):
        home = os.path.expanduser("~").rstrip("/\\")
        return home + os.sep + cls.WORKDIR

    @classmethod
    def get_local_database_path(cls):
        return cls.get_workdir() + os.sep + cls.LOCALDB_FILENAME

    @classmethod
    def get_package_path(cls, package_data):
        return cls.get_workdir() + os.sep + package_data["name"] + "-" + package_data["version"]

    @classmethod
    def search_keyword(cls, keyword):
        url = cls.QUERY_KEYWORD_URL % keyword
        # print "[PACKAGE] query url: %s" % url
        response = urllib2.urlopen(url)
        html = response.read()
        packages_data = json.loads(html)
        if packages_data is None or len(packages_data) == 0:
            return None

        if "err" in packages_data:
            message = MultiLanguage.get_string('PACKAGE_ERROR_WITH_CODE_FMT',
                      (packages_data["err"], packages_data["code"]))
            raise cocos.CCPluginError(message, cocos.CCPluginError.ERROR_WRONG_CONFIG)

        return packages_data

    @classmethod
    def query_package_data(cls, name, version = 'all'):
        url = cls.QUERY_PACKAGE_URL % name + '&version=' + version
        # print "[PACKAGE] query url: %s" % url
        response = urllib2.urlopen(url)
        html = response.read()
        package_data = json.loads(html)
        # d1 = json.dumps(package_data,indent=4)
        # print d1
        if package_data is None or len(package_data) == 0 or ("err" in package_data and "code" in package_data and package_data["code"] == "1002"):
            return None

        if "err" in package_data:
            message = MultiLanguage.get_string('PACKAGE_ERROR_WITH_CODE_FMT',
                      (package_data["err"], package_data["code"]))
            raise cocos.CCPluginError(message, cocos.CCPluginError.ERROR_WRONG_CONFIG)

        return package_data

    @classmethod
    def download_package_zip(cls, package_data, force):
        download_url = cls.REPO_URL + cls.REPO_PACKAGES_DIR + "/" + package_data["filename"]
        workdir = cls.get_package_path(package_data)
        print MultiLanguage.get_string('PACKAGE_WORKDIR_FMT', workdir)
        downloader = ZipDownloader(download_url, workdir, package_data, force)
        downloader.run()

    @classmethod
    def add_package(cls, package_data):
        localdb = LocalPackagesDatabase(cls.get_local_database_path())
        localdb.add_package(package_data)

    @classmethod
    def get_installed_packages(cls):
        localdb = LocalPackagesDatabase(cls.get_local_database_path())
        return localdb.get_packages()

    @classmethod
    def get_installed_package_data(cls, package_name, version = None):
        localdb = LocalPackagesDatabase(cls.get_local_database_path())
        packages = localdb.get_packages()
        keys = packages.keys()
        keys.sort()
        keys.reverse()
        for key in keys:
            package_data = packages[key]
            if package_data["name"] == package_name:
                if version == None:
                    return package_data
                elif package_data["version"] == version:
                    return package_data

    @classmethod
    def get_installed_package_newest_version(cls, package_name, engine = None):
        localdb = LocalPackagesDatabase(cls.get_local_database_path())
        packages = localdb.get_packages()
        keys = packages.keys()
        keys.sort()
        keys.reverse()
        package_list = []
        for key in keys:
            package_data = packages[key]
            if package_data["name"] == package_name:
                package_list.append(package_data)

        n = len(package_list)
        if n < 1:
            return

        if not engine is None:
            package_list = get_packages_adapt_engine(package_list, engine)
            if package_list is None:
                return

        package_newest = package_list[0]
        for x in xrange(1,n-1):
            package_newest = get_newer_package(package_list[x], package_newest)

        return package_newest

    @classmethod
    def get_installed_package_zip_path(cls, package_data):
        workdir = cls.get_package_path(package_data)
        return workdir + os.sep + package_data["filename"]
