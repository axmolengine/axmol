#put xctool.sh into your PATH

#######
# Cmake build
####### 
# DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# ADXE_ROOT="$DIR"/../../..
# cd ${ADXE_ROOT}
# mkdir mac-build
# cd mac-build
# cmake ..
# make -j4

#######
# xcode build
####### 
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ADXE_ROOT="$DIR"/../../../..
xcodebuild -project "$ADXE_ROOT"/build/cocos2d_tests.xcodeproj -scheme "build all tests Mac" clean | xcpretty
xcodebuild -project "$ADXE_ROOT"/build/cocos2d_tests.xcodeproj -scheme "build all tests Mac" build | xcpretty
#xcpretty has a bug, some xcodebuid fails return value would be treated as 0.
xcodebuild -project "$ADXE_ROOT"/build/cocos2d_tests.xcodeproj -scheme "build all tests Mac" build
