#put xctool.sh into your PATH
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ADXE_ROOT="$DIR"/../../../..
xcodebuild -project "$ADXE_ROOT"/build/cocos2d_tests.xcodeproj -scheme "build all tests iOS"  -destination "platform=iOS Simulator,name=iPhone Retina (4-inch)" clean | xcpretty
xcodebuild -project "$ADXE_ROOT"/build/cocos2d_tests.xcodeproj -scheme "build all tests iOS"  -destination "platform=iOS Simulator,name=iPhone Retina (4-inch)" build | xcpretty
#the following commands must not be removed
xcodebuild -project "$ADXE_ROOT"/build/cocos2d_tests.xcodeproj -scheme "build all tests iOS"  -destination "platform=iOS Simulator,name=iPhone Retina (4-inch)" build
