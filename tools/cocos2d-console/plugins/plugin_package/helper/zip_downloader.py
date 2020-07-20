
import os
import os.path
import zipfile
import sys
import hashlib

import cocos
from MultiLanguage import MultiLanguage

from time import time
from functions import *

class ZipDownloader(object):
    def __init__(self, url, destdir, package_data, force):
        self._url = url
        self._destdir = destdir
        self._package_data = package_data
        self._force = force
        self._zip_file_size = int(package_data["filesize"])
        self._filename = destdir + os.sep + package_data["filename"]

    def download_file(self):
        print(MultiLanguage.get_string('PACKAGE_READY_DOWNLOAD_FMT', (self._filename, self._url)))
        import urllib2

        try:
            u = urllib2.urlopen(self._url)
        except urllib2.HTTPError as e:
            if e.code == 404:
                print(MultiLanguage.get_string('PACKAGE_ERROR_URL_FMT', self._url))
            print(MultiLanguage.get_string('PACKAGE_ERROR_DOWNLOAD_FAILED_FMT',
                  (str(e.code), e.read())))
            sys.exit(1)

        f = open(self._filename, 'wb')
        file_size = self._zip_file_size
        print(MultiLanguage.get_string('PACKAGE_START_DOWNLOAD'))

        file_size_dl = 0
        block_sz = 8192
        block_size_per_second = 0
        old_time = time()

        while True:
            buf = u.read(block_sz)
            if not buf:
                break

            file_size_dl += len(buf)
            block_size_per_second += len(buf)
            f.write(buf)
            new_time = time()
            if (new_time - old_time) > 1:
                speed = block_size_per_second / (new_time - old_time) / 1000.0
                status = ""
                if file_size != 0:
                    percent = file_size_dl * 100. / file_size
                    status = MultiLanguage.get_string('PACKAGE_DOWNLOAD_PERCENT_FMT_1',
                                                      (file_size_dl / 1000, file_size / 1000, percent, speed))
                else:
                    status = MultiLanguage.get_string('PACKAGE_DOWNLOAD_PERCENT_FMT_2',
                                                      (file_size_dl / 1000, speed))

                status += chr(8) * (len(status) + 1)
                print(status),
                sys.stdout.flush()
                block_size_per_second = 0
                old_time = new_time

        print(MultiLanguage.get_string('PACKAGE_DOWNLOAD_END'))
        f.close()

    def check_file_md5(self):
        if not os.path.isfile(self._filename):
            return False

        block_size = 65536  # 64KB
        md5 = hashlib.md5()
        f = open(self._filename)
        while True:
            data = f.read(block_size)
            if not data:
                break
            md5.update(data)
        hashcode = md5.hexdigest()
        return hashcode == self._package_data["md5"]

    def download_zip_file(self):
        if os.path.isfile(self._filename):
            if self._force or not self.check_file_md5():
                os.remove(self._filename)
            else:
                print MultiLanguage.get_string('PACKAGE_EXISTS_FMT', self._filename)

        if not os.path.isfile(self._filename):
            self.download_file()

        try:
            if not zipfile.is_zipfile(self._filename):
                raise UnrecognizedFormat(MultiLanguage.get_string('PACKAGE_ERROR_NOT_ZIP_FMT', (self._filename)))
        except UnrecognizedFormat as e:
            print(MultiLanguage.get_string('PACKAGE_ERROR_UNKNOWN_FORMAT_FMT', self._filename))
            if os.path.isfile(self._filename):
                os.remove(self._filename)
                # print("==> Download it from internet again, please wait...")
                # self.download_zip_file()


    def run(self):
        ensure_directory(self._destdir)
        self.download_zip_file()

