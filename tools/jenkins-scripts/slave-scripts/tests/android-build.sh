#put xctool.sh into your PATH
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ADXE_ROOT="$DIR"/../../../..
cd ${ADXE_ROOT}
python build/android-build.py -p 13 cpp-tests lua-tests js-tests
