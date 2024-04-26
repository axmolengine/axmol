# unit-tests


## Description

`unit-tests` app is a console application that runs Axmol's automated unit tests. It can be used for
test driven development, checking for regressions during development, or running tests in a CI/CD
pipeline.

One of the `unit-tests` goals is for it to be fast, so it can be run frequently during development.


## Setup

The `unit-tests` app is a console app. For writting tests `unit-tests` uses
[doctest](https://github.com/doctest/doctest) library. It's pretty simple and easy to use.


## Usage

Supported platforms:

* Linux
* macOS
* Windows

To run tests simply build the `unit-tests` app and run it. It will run all tests automatically and
print the results to the console. The app also returns error code 0 if all tests pass and non 0
code if any test fails.

Use `axmol build -d tests/unit-tests` for building the `unit-tests` app. Then use command line to
run the app. You can also run it with `unit-tests --help` to see all available options.


## Writing tests


### Adding new tests

Let's say you want to write unit tests for `ax::Node` class. To add a new test follow these steps:

1. Find or create a source file to hold your tests. For easier navigation test source files follow
    the same layout and naming as the engine source files, and only add a `Tests` postfix. For
    example `ax::Node` in engine is in `core/2d/Node.h`, so the test file should be in
    `tests/unit-tests/Source/core/2d/NodeTests.cpp`.
2. If the file doesn't exist, then create a new file, in this case
    `tests/unit-tests/Source/core/2d/NodeTests.cpp`.
3. Register new file in `tests/unit-tests/CMakeLists.txt`.

4. For the test file use the following structure:
    ```cpp
    // <add copyright message here>
    #include <doctest.h>
    #include "2d/Node.h" // Include the thing you're testing

    USING_NS_AX;

    // For suite name use the part of the source file path, in this case `2d` followed by the
    // class name you will be testing, in this case `Node`. Separate the parts with a slash.
    TEST_SUITE("2d/Node") {
    }
    ```

5. If you will be testing functions, methods or their groups, then use their names in the test
   case like this:
   ```cpp
    TEST_SUITE("2d/Node") {
        TEST_CASE("addChild") {
            // addChild() tests go here
        }


        TEST_CASE("removeFromParent") {
            // removeFromParent() tests go here
        }
    }
    ```

6. If you will be testing some specific functionality, then name what you're testing like this:
   ```cpp
    TEST_SUITE("2d/Node") {
        TEST_CASE("adding_child_clears_dirty_flag") {
            // Test goes here
        }
    }
    ```

7. `TestUtils.h` header contains some helpers for testing.

8. For reference example see `FileUtils` tests in `tests\unit-tests\Source\core\platform\FileUtilsTests.cpp`.


### Writing tests

For more information on how to write tests using `doctest`, see the
[doctest documentation](https://github.com/doctest/doctest/blob/master/doc/markdown/readme.md)


### Things to keep in mind

* For suite, case or subcase names use only `a-zA-Z0-9_/[]` symbols for easier use in command line.
* Try to follow the established naming and structure for suites and cases. This is for easier
    filtering when running tests.
