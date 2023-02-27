How to Use bindings-generator
==================

Prerequisites
------------
1. Download libclang from https://github.com/axmolengine/buildware/releases/download/llvmorg-15.0.7/llvmorg-15.0.7.zip
2. Copy `libclang.dll/libclang.dylib/libclang.so` (match with your OS) to `tools/bindings-generator/libclang/`, refer to `tools/win-ci/genbindings.ps1`

On Windows:
------------

* Make sure that you have installed `android-ndk-r19c` or later.
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
    
* Download NDK 64bit r19c or later from [Android NDK](https://developer.android.com/ndk/downloads/index.html)
* Run
<pre>
    export ANDROID_NDK=/path/to/android-ndk-19c
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
* Download NDK 64bit r19c or later from [Android NDK](https://developer.android.com/ndk/downloads/index.html)
* Go to "tools/tolua", Run
<pre>
    export ANDROID_NDK=/path/to/android-ndk-r19c
    python3 ./genbindings.py
</pre>
