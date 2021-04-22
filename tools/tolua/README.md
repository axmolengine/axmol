How to Use bindings-generator
==================

On Windows:
------------

* Make sure that you have installed `android-ndk-r16` or later.
* Install python2.7.18 (32bit) from http://www.python.org/ftp/python/2.7.18/python-2.7.18.msi with `Add to 'PATH'` install option.
* Install python dependices by pip.
<pre>
    pip install PyYAML
    pip install Cheetah
</pre>
* Go to "tools/tolua" folder, and run "genbindings.py". The generated codes will be under "cocos\scripting\auto-generated\lua-bindings".


On MAC:
----------

* The OSX 10.10 has a built-in python2.7 and if your os don't have python2.7 then use [Homebrew](http://brew.sh/) to install the python and use pip install the python dependencies.
<pre>
	brew install python
</pre>

* Install python dependices by pip.
<pre>
    sudo easy_install pip
    sudo pip install PyYAML
    sudo pip install Cheetah
</pre>
    
* Download NDK 64bit r10c or later from [Android Ndk](https://developer.android.com/ndk/downloads/index.html)
* Run
<pre>
	export ANDROID_NDK=/path/to/android-ndk-10c
    ./genbindings.py
</pre>


On Ubuntu Linux 12.04 64bit
------------

* Install python
<pre>
	sudo apt-get install python2.7
</pre>
* Install python dependices by pip.
<pre>
	sudo apt-get install python-pip
	sudo pip install PyYAML
	sudo pip install Cheetah
</pre>
* Download NDK 64bit r10c or later from [Android Ndk](https://developer.android.com/ndk/downloads/index.html)
* Go to "tools/tolua", Run
<pre>
	export ANDROID_NDK=/path/to/android-ndk-r10c
    ./genbindings.py
</pre>
