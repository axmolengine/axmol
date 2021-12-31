#!/bin/bash 
adxe_cmd=tools/console/bin/adxe
$adxe_cmd new -l $1

if [ $1 = "cpp" ]; then
    projectname="MyCppGame"
elif [ $1 = "lua" ]; then
    projectname="MyLuaGame"
elif [ $1 = "js" ]; then
    projectname="MyJSGame"
fi

$adxe_cmd compile -p linux -s $projectname -m release -j4 --compile-script 0
