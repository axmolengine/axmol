# Requirements

* python2.7
* py-yaml
* cheetah (for target language templates)
* libclang, from clang 3.1

# Usage

    Usage: generator.py [options] {configfile}
    
    Options:
      -h, --help   show this help message and exit
      -s SECTION   sets a specific section to be converted
      -t TARGET    specifies the target vm. Will search for TARGET.yaml

Basically, you specify a target vm (spidermonkey is the only current target vm) and the section from
the `.ini` file you want to generate code for.

## Test

Included in this repository is a simple test. Use this to confirm the generator is working and that your environment is set up correctly.

#### NOTE

* The test uses &lt;string&gt; and &lt;stdint.h&gt; so you need a C++ implementation that provides these
* Currently, the test script is setup to use the Android NDK's GNU libstdc++

### Mac OS X

* Use MacPorts to install the python dependencies

<pre>
    sudo port install python27 py27-yaml py27-cheetah
</pre>

* Download and extract [clang+llvm-3.1](http://llvm.org/releases/3.1/clang+llvm-3.1-x86_64-apple-darwin11.tar.gz) from http://llvm.org/releases/download.html#3.1
* Download and extract the [Android NDK](http://dl.google.com/android/ndk/android-ndk-r8c-darwin-x86.tar.bz2) from http://d.android.com
* Customize `test/userconf.ini` and `test/user.cfg` for your environment
  * Copy the userconf.ini.sample and user.cfg.sample
  * Since we are using MacPorts, set the absolute path to MacPorts' python `PYTHON_BIN` in `user.cfg`
  * Set the absolute path to where you extracted the Android NDK `androidndkdir`  in `userconf.ini`
  * Set the absolute path to where you extracted Clang+LLVM `clangllvmdir`  in `userconf.ini`
  * Set the absolute path to your clone of this repository `cxxgeneratordir` in `userconf.ini`
* Run the test using MacPorts' python and the downloaded clang

<pre>
    ./test.sh
</pre>

### Expected output

Upon running the test you might see some warnings but should not see any errors.

The test will create a directory named simple_test_bindings that contains 3 files

* A .hpp header file for the bindings class
* A .cpp file implementing the bindings class
* A .js file that documents how to call (from JavaScript) the methods the C++ class exposes

# The `.ini` file

The `.ini` file is a simple text file specifying the settings for the code generator. Here's the
default one, used for cocos2d-x

    [cocos2d-x]
    prefix = cocos2dx
    events  = CCNode#onEnter CCNode#onExit
    extra_arguments = -I../../cocos2dx/include -I../../cocos2dx/platform -I../../cocos2dx/platform/ios -I../../cocos2dx -I../../cocos2dx/kazmath/include -arch i386 -DTARGET_OS_IPHONE -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator5.1.sdk -x c++
    headers = ../../cocos2dx/include/cocos2d.h
    classes = CCSprite
    functions = my_free_function

## Required sections

* prefix: the prefix for the project. Must be a valid identifier for the language of the target vm.
  Most of the time, the name will be intermixed between the class name and the function name, since
  all generated (probably) will be free functions, we do that in order to avoid name collition. The
  script will generate ${prefix}.cpp and ${prefix}.hpp as a result.
* events: a list of identifiers in the form of ClassName#functionName that are events to be called
  from the native world to the target vm.
* extra_arguments: extra arguments to pass to the clang interface. Basically you can think of this
  as the arguments to pass to the "compiler", so add as many as you need here. If you're targetting
  C++, make sure you add "-x c++" as the last argument to force C++ mode on a ".h" file. Otherwise,
  name your header files as ".hpp".
* headers: list of headers to parse. Usually you add a single header that in turn `#include`s the
  rest of the files.
* classes: the classes that will be parsed. Right not is just a string, but it will accept regular
  expressions
* functions: space-separated list of free functions to be binded. Same as with classes, it will
  support regular expressions.
* skip: a space-separated list of `Classes::functions` or just `functions` to not generate any code.

# The templates

The generator is using [Cheetah templates](http://www.cheetahtemplate.org/) to create a more
flexible generator. The way it was thought, is that for every target environment, you should provide
with a way to generate the same C/C++ functionality. Every template has access to the proper meta
information for the code or generator (function, classes, etc.)

Right now it's separated in the following set of templates:

* prelude.c/.h: The header of the generated files.
* ifunction.c/.h: The template for an instance function
* ifunction_overloaded.c: The template for the implementation of an overloaded function. An
  overloaded function is exactly the same as a function, but it has an array of functions sharing
  the same name. The current implementation for spidermonkey only works if the overloading is with
  different number of arguments.
* sfunction.c/.h: The template for a static function
* sfunction_overloaded.c: The template for an overloaded static function
* register.c: Here you should add the constructor/finalizer, the registration function (if needed)
  and the footer of the header file. This is the last chunk being generated

Templates are stored in the `templates/${target}` directory and follow the naming specified above.

One final part of the puzzle is the `${target}.yaml` file, that contains specific type conversion
snippets to be used by the templates. For instance, for spidermonkey, this is the place where we
specify the conversion routines for the native types (to and from int, float, string, etc.)

# Limitations

Currently the generator is leveraging clang in order to get information about the C/C++ code, so we
can only get as much information as clang give us. Known list of things that won't work:

* variable number of arguments. Solution: write a manual wrapper
* default arguments. Solution: write another function that actually calls the real function with the
  default argument.

