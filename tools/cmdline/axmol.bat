@echo off

REM looking powershell core 7+
where pwsh 1>nul 2>nul
if %ERRORLEVEL% EQU 0 pwsh "%~dp0/axmol.ps1" %* && goto :eof

REM Looking system installed powershell 5.x
where powershell 1>nul 2>nul
if %ERRORLEVEL% EQU 0 (
    powershell "%~dp0/axmol.ps1" %* && goto :eof
) else (
    echo "powershell 5+ required" 1>&2
)
exit /b 1
