@echo off

setlocal enabledelayedexpansion

set "AXMOL_CONSOLE_BIN_DIRECTORY=%~dp0"

where python3 >nul 2>nul
if %errorlevel% equ 0 (
    set "PYTHON=python3"
) else (
    where python2 >nul 2>nul
    if %errorlevel% equ 0 (
        set "PYTHON=python2"
    ) else (
        where python >nul 2>nul
        if %errorlevel% equ 0 (
            set "PYTHON=python"
        ) else (
            echo Python 2+ required.
            exit /b 1
        )
    )
)

"%PYTHON%" "%AXMOL_CONSOLE_BIN_DIRECTORY%\axmol.py" %*