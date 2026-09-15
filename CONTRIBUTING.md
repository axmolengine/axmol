# Contributing to Axmol

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

If you are new to Git or GitHub, read [Pro Git](https://git-scm.com/book/en/v2/Git-on-the-Server-Contributing-to-a-Project), especially the section on contributing to a public project.


### One-time repository setup

- Fork Axmol on GitHub to your own account.
- Clone your fork:
```
git clone https://github.com/<your-github-account>/axmol.git
```
- Add the canonical Axmol repository as `upstream`:
```
cd axmol
git remote add upstream https://github.com/axmolengine/axmol.git
```

### For each pull request

- Update your local view of the upstream repository:
```
git fetch upstream
```
- Create a focused branch from the current v3 development branch:
```
git switch -c <branch-name> --track upstream/dev
```
- Make focused changes, run the relevant checks, and use short descriptive commit messages.
```
git commit
```
- Push the branch to your fork:
```
git push -u origin <branch-name>
```
- Open a pull request against `axmolengine/axmol`, normally targeting `dev`.
- Target `release/2.x` only for an applicable critical or security fix; new v3 features belong on `dev`.
- Complete the pull-request description and checklist before requesting review.

## Only _complete_ patches will be merged

The patch must be _complete_. And by that, we mean:

-   For C++ code follow the [axmol C++ Coding Style](docs/CODING_STYLE.md)
-   Describe what the patch does and why.
-   Include regression or unit tests when applicable.
-   Run the relevant C++23 build, test, and formatting checks; use the Axmol CLI or the documented CMake workflow.
-   Test every platform available to you and report the actual platform/compiler matrix in the pull request. If a platform cannot be tested, say so explicitly [*].
-   Must not degrade performance, break existing tests, break CI, or introduce avoidable backward incompatibility.
-   Must compile without warnings in the configurations tested.
-   New APIs must be easy to use, familiar to Axmol users, documented with Doxygen, and easy to extend and maintain.
-   Tools must include a README.md describing how to use them.
-   Keep implementations efficient and avoid duplicating existing code unless the new code deliberately deprecates or replaces it.
-   Large refactors of key components should be coordinated with maintainers and are generally reserved for the next major version.

[*]: If you don't have access to test your code in all the supported platforms, let us know.

## Copyright and Intellectual Property

By submitting contributions you agree to the Axmol Contributor Agreement.

Source‑file headers state the copyright holder of assigned economic rights (Simdsoft Limited).
Original author attribution is preserved in Git commit history, CLA signing records, AUTHORS.md and the root `LICENSE` file.

- Newly created project-owned source files should use the current MIT/SPDX header style. They do not need historical attribution that does not apply to the new file:

  ```cpp
  /****************************************************************************
   Copyright (c) 2019-present Simdsoft Limited.

   https://axmol.dev/

   SPDX-License-Identifier: MIT
   ****************************************************************************/
  #pragma once
  ```
- When modifying an existing file, preserve its applicable historical copyright and author notices, and add the current project notice when appropriate.
- New pull requests must complete CLA signing before being merged. The CLA assigns economic copyright rights for incoming contributions to Simdsoft Limited.
- Some historical code segments are from former external contributors who are inactive and cannot sign the CLA. Those contributions are licensed under the original MIT license; Simdsoft does not hold assigned copyright ownership over those code segments.
- The root `LICENSE` lists `Simdsoft Limited and other Axmol contributors` as the overall project copyright holders.

## About branch management

Please read [this announcement](https://github.com/axmolengine/axmol/discussions/1271).
