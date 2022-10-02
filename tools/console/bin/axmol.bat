@echo off
rem Ensure vswhere installed for cmdline tool could find vs2017+
pip install vswhere
python "%~dp0/axmol.py" %*
