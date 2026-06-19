How to Use bindings-generator
==================

In the current release, the `tools/cmdline/genbindings.ps1` serves as a compromise solution: the script invokes the existing genbindings.py (tested with Python 3.12.x on Windows) and performs a cleanup step to remove duplicated empty lines in generated C++ Lua binding sources.
For v3, we plan to fully re‑implement the Lua binding generator in PowerShell, eliminating the Python dependency and providing a native, streamlined workflow.
