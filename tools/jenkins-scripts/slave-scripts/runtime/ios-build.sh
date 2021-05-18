#!/bin/bash
adxe_cmd=tools/console/bin/adxe
$adxe_cmd new -l $1 -t runtime

if [ $1 = "cpp" ];then
    schemename="MyCppGame-mobile"
    projectpath="MyCppGame/proj.ios_mac/MyCppGame.xcodeproj"
    adxe_project_path="MyCppGame"
elif [ $1 = "lua" ];then
    schemename="MyLuaGame-mobile"
    projectpath="MyLuaGame/frameworks/runtime-src/proj.ios_mac/MyLuaGame.xcodeproj"
    adxe_project_path="MyLuaGame"
elif [ $1 = "js" ];then
    schemename="MyJSGame-mobile"
    projectpath="MyJSGame/frameworks/runtime-src/proj.ios_mac/MyJSGame.xcodeproj"
    adxe_project_path="MyJSGame"
fi

echo "start building..."
adxe compile -p ios -s $adxe_project_path | xcpretty
adxe compile -p ios -s $adxe_project_path
# xcodebuild -project $projectpath -target "${schemename}"  -destination "platform=iOS Simulator,name=iPhone Retina (4-inch)" clean | xcpretty
# xcodebuild -project $projectpath -target "${schemename}"  -destination "platform=iOS Simulator,name=iPhone Retina (4-inch)" build | xcpretty
# #the following commands must not be removed
# xcodebuild -project $projectpath -target "${schemename}"  -destination "platform=iOS Simulator,name=iPhone Retina (4-inch)" build
