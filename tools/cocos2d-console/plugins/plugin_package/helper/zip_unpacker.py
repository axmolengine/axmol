
import os
import os.path
import zipfile

import cocos
from MultiLanguage import MultiLanguage

from functions import *

class ZipUnpacker(object):
    def __init__(self, filename):
        self._filename = filename

    def unpack(self, extract_dir):
        """Unpack zip `filename` to `extract_dir`

        Raises ``UnrecognizedFormat`` if `filename` is not a zipfile (as determined
        by ``zipfile.is_zipfile()``).
        """

        if not zipfile.is_zipfile(self._filename):
            raise UnrecognizedFormat(MultiLanguage.get_string('PACKAGE_ERROR_NOT_ZIP_FMT', self._filename))

        print(MultiLanguage.get_string('PACKAGE_EXTRACT_TIP'))
        z = zipfile.ZipFile(self._filename)
        try:
            for info in z.infolist():
                name = info.filename

                # don't extract absolute paths or ones with .. in them
                if name.startswith('/') or '..' in name:
                    continue

                target = os.path.join(extract_dir, *name.split('/'))
                if not target:
                    continue
                if name.endswith('/'):
                    # directory
                    ensure_directory(target)
                else:
                    # file
                    data = z.read(info.filename)
                    f = open(target, 'wb')
                    try:
                        f.write(data)
                    finally:
                        f.close()
                        del data
                unix_attributes = info.external_attr >> 16
                if unix_attributes:
                    os.chmod(target, unix_attributes)
        finally:
            z.close()
            print(MultiLanguage.get_string('PACKAGE_EXTRACT_END'))

