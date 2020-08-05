#
# cocos console installation script.
#
# Script idea based on Homebrew installation script.
# by Luis Parravicini.
#

import os
import sys
import urllib
from tarfile import TarFile
import shutil


COCOS2D_PREFIX = os.path.expanduser('~/.cocos2d')


# TODO copied from cocos.py, refactor.
class Logging:
    # TODO maybe the right way to do this is to use something like colorama?
    RED     = '\033[31m'
    GREEN   = '\033[32m'
    YELLOW  = '\033[33m'
    MAGENTA = '\033[35m'
    RESET   = '\033[0m'

    @staticmethod
    def _print(s, color=None):
        if color and sys.stdout.isatty() and sys.platform != 'win32':
            print color + s + Logging.RESET
        else:
            print s

    @staticmethod
    def debug(s):
        Logging._print(s, Logging.MAGENTA)

    @staticmethod
    def info(s):
        Logging._print(s, Logging.GREEN)

    @staticmethod
    def warning(s):
        Logging._print(s, Logging.YELLOW)

    @staticmethod
    def error(s):
        Logging._print(s, Logging.RED)


def die(msg):
    Logging.error(msg)
    sys.exit(1)

def touch(path):
    open(path, 'w').close()



Logging.info("Starting the installation for Cocos2D console")

console_path = os.path.join(COCOS2D_PREFIX, 'console')
install_mark_path = os.path.join(console_path, 'installed')
if os.path.exists(install_mark_path):
    die("""
It appears Cocos2d console is already installed. If you want to resintall you should
delete the directory "%s" before running the installer again.
""" % console_path)

Logging.info("Downloading...")
download_path, _ = urllib.urlretrieve('https://github.com/cocos2d/cocos2d-console/tarball/master')

Logging.info("Installing...")
with TarFile.open(download_path) as tar:
    tmp_path = os.path.join(COCOS2D_PREFIX, 'tmp')
    tar.extractall(tmp_path)
    # it's assumed the first entry is the directory containing all the other files
    dir = tar.getmembers()[0].name

extracted_path = os.path.join(tmp_path, dir)
if os.path.exists(console_path):
    shutil.rmtree(console_path)
shutil.move(extracted_path, console_path)

bin_path = os.path.join(console_path, 'console', 'bin')
path = os.environ.get('PATH', '')
if not bin_path in path.split(os.pathsep):
    Logging.warning("console path '%s' is not in PATH!" % bin_path)

touch(install_mark_path)

Logging.info("Cocos2d console installed successfully at %s" % console_path)
Logging.info("Now type: cocos")
