xcopy /s ".\tests\cpp-empty-test" ".\tests\new_cpp-empty-test\"
cmake -S .  -G "Visual Studio 16 2019" -A Win32
msbuild ".\tests\new_cpp-empty-test\cpp-empty-test.sln"