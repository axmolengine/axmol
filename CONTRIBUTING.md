# Contributing to Axmol Engine

## General considerations

### Providing correct and relevant information is key

When asking a question, reporting a bug, or submitting a patch, it is **very important** to write down clearly all the relevant information the reviewer may need. For a bug report, provide the steps to reproduce it; for a pull request, outline the changes made and the reasons behind them. This will assist the reviewers and make the process of improving Axmol smoother for everybody.

## For questions

Please revise our [FAQ](https://github.com/axmolengine/axmol/wiki/FAQ) and check the [GitHub Discussions](https://github.com/axmolengine/axmol/discussions), your question may have been already answered.

If that is not the case, you can ask general questions by using:

-   Forum (preferred way): https://github.com/axmolengine/axmol/discussions

## Reporting bugs

To report bugs, please use the [Issue Tracker](https://github.com/axmolengine/axmol/issues).

Steps to report a bug:
* Open the [issues url](https://github.com/axmolengine/axmol/issues/new)
* Add all the needed information to reproduce the bug, the information include
    * engine version
    * steps to reproduce the bug
    * some pseudocode
    * resources link if needed


## Submitting patches

If you want to contribute code, please follow these steps:

(If you are new to git and/or GitHub, you should read [Pro Git](http://progit.org/book/) , especially the section on [Contributing to a project:Small/Large Public Project](http://progit.org/book/ch5-2.html#public_small_project) )


### Following steps must be done once in order to setup repository

-   Fork axmol on github page to your own github account
-   Clone your own fork:
```
$ git clone https://github.com/<your git accout>/axmol.git
```
-   Add remote official from axmolengine/axmol:
```
$ cd axmol
$ git remote add official https://github.com/axmolengine/axmol.git
```

### Following steps must be done for every pull request

-   Sync with official:
```
$ git fetch official
```
-   Create a new branch on top of the latest dev from official:
```
$ git checkout -b <new_branch_name> official/dev
```
-   Do local changes on your new branch. Do as much commits as you need and add short descriptive message to each commit:
```
$ git commit
```
-   When all changes are finished push your new branch to your forked repository:
```
$ git push origin <new_branch_name>
```
-   Send a “pull request” to user “axmol”
-   It must be _complete_. See the definition below
-   It must follow the _Releases_ rules. See the definition below

## Only _complete_ patches will be merged

The patch must be _complete_. And by that, we mean:

-   For C++ code follow the [axmol C++ Coding Style](docs/CODING_STYLE.md)
-   For Python code follow the [PEP8 guidelines](https://www.python.org/dev/peps/pep-0008)
-   Describe what the patch does
-   Include test cases if applicable
-   Include unit tests if applicable
-   Must be tested in all supported platforms [*]
-   Must NOT degrade the performance
-   Must NOT break existing tests cases
-   Must NOT break the Continuous Integration build
-   Must NOT break backward compatibility
-   Must compile WITHOUT warnings
-   New APIs MUST be **easy to use**, **familiar** to cocos2d-x users
-   Code MUST be **easy to extend** and **maintain**
-   Must have documentation: C++ APIs must use Doxygen strings, tools must have a README.md file that describe how to use the tool
-   Must be efficient (fast / low memory needs)
-   It must not duplicate existing code, unless the new code deprecates the old one
-   Patches that refactor key components will only be merged in the next major versions.

[*]: If you don't have access to test your code in all the supported platforms, let us know.

## About branch management

Please read [this announcement](https://github.com/axmolengine/axmol/discussions/1271).
