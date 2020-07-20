
# options

usage(){
cat << EOF
usage: $0 [options]

Test the C++ <-> JS bindings generator

OPTIONS:
-h	this help

Dependencies :
PYTHON_BIN
CLANG_ROOT
NDK_ROOT

Define this to run from a different directory
CXX_GENERATOR_ROOT

EOF
}

while getopts "dvh" OPTION; do
case "$OPTION" in
d)
debug=1
;;
v)
verbose=1
;;
h)
usage
exit 0
;;
esac
done

# exit this script if any commmand fails
set -e

# find current dir
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# read user.cfg if it exists and is readable

_CFG_FILE=$(dirname "$0")"/user.cfg"
if [ -e "$_CFG_FILE" ]
then
    [ -r "$_CFG_FILE" ] || die "Fatal Error: $_CFG_FILE exists but is unreadable"
    . "$_CFG_FILE"
fi

# paths

if [ -z "${NDK_ROOT+aaa}" ]; then
# ... if NDK_ROOT is not set, use "$HOME/bin/android-ndk"
    NDK_ROOT="$HOME/bin/android-ndk"
fi

if [ -z "${CLANG_ROOT+aaa}" ]; then
# ... if CLANG_ROOT is not set, use "$HOME/bin/clang+llvm-3.1"
    CLANG_ROOT="$HOME/bin/clang+llvm-3.1"
fi

if [ -z "${PYTHON_BIN+aaa}" ]; then
# ... if PYTHON_BIN is not set, use "/usr/bin/python2.7"
    PYTHON_BIN="/usr/bin/python2.7"
fi

# paths with defaults hardcoded to relative paths

if [ -z "${CXX_GENERATOR_ROOT+aaa}" ]; then
    CXX_GENERATOR_ROOT="$DIR/.."
fi

echo "CLANG_ROOT: $CLANG_ROOT"
echo "NDK_ROOT: $NDK_ROOT"
echo "CXX_GENERATOR_ROOT: $CXX_GENERATOR_ROOT"
echo "PYTHON_BIN: $PYTHON_BIN"

# Generate bindings for simpletest using Android's system headers
echo "Generating bindings for simpletest with Android headers..."
set -x
LD_LIBRARY_PATH=${CLANG_ROOT}/lib $PYTHON_BIN ${CXX_GENERATOR_ROOT}/generator.py ${CXX_GENERATOR_ROOT}/test/test.ini -s testandroid -o ./simple_test_bindings
