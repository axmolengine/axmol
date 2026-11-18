How to Use bindings-generator
==================

Lua feature switches
--------------------

Generate Lua bindings with the parser's superset of features enabled. The generated
`axlua_base_auto.cpp` remains one file: optional declarations, functions, and
registration calls carry the feature guards found in their C++ headers. The Lua
templates also guard registration of optional classes. The extension parser enables
GUI, AssetsManager, and Particle3D so their bindings remain available when those
features are enabled later.

Changing `AX_ENABLE_*` options in an application requires CMake configuration and
C++ compilation, but does not require rerunning `genbindings.py`. If bindings are
regenerated after a header or API change, use this generator and its Lua templates;
editing only `extensions/scripting/lua-bindings/auto/*.cpp` would lose these guards.
Keep optional features available to the parser during regeneration, then build the
desired application configuration separately.

Prerequisites
------------
1. Download libclang from https://github.com/simdsoft/1kiss/releases/download/llvmorg-15.0.7/llvmorg-15.0.7.zip
2. Copy `libclang.dll/libclang.dylib/libclang.so` (match with your OS) to `tools/bindings-generator/libclang/`, refer to `tools/win-ci/genbindings.ps1`

On Windows:
------------

* Make sure that you have installed `android-ndk-r23c` or later.
* Install python3.7+(64bit) from https://www.python.org with `Add to 'PATH'` install option.
* Install python dependices by pip.
<pre>
    pip install PyYAML Cheetah3
</pre>
* Go to "tools/tolua" folder, and run "genbindings.py". The generated codes will be under "cocos\scripting\auto-generated\lua-bindings".

On MAC:
----------

* Use [Homebrew](http://brew.sh/) to install the python and use pip install the python dependencies.
<pre>
    brew install python3
</pre>

* Install python dependices by pip3.
<pre>
    pip3 install PyYAML Cheetah3
</pre>
    
* Download NDK 64bit r23c or later from [Android NDK](https://developer.android.com/ndk/downloads/index.html)
* Run
<pre>
    export ANDROID_NDK=/path/to/android-ndk-r23c
    ./genbindings.py
</pre>


On Ubuntu Linux 22.04 64bit
------------

* Install python
<pre>
    sudo apt-get install python3
</pre>
* Install python dependices by pip.
<pre>
    sudo apt-get install python3-pip
    sudo pip3 install PyYAML Cheetah3
</pre>
* Download NDK 64bit r23c from [Android NDK](https://developer.android.com/ndk/downloads/index.html)
* Go to "tools/tolua", Run
<pre>
    export ANDROID_NDK=/path/to/android-ndk-r23c
    python3 ./genbindings.py
</pre>
