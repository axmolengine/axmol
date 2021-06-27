@echo off
rem Ensure vswhere installed for cmdline tool could find vs2017+
pip install vswhere
"%~dp0/adxe.py" %*
