@echo off

REM this script is useful to fix any git repo modified status,
REM sometimes, even through the file binary not changed,
REM but will be in modified list when execute git status

setlocal ENABLEDELAYEDEXPANSION

for /f "usebackq tokens=1,* delims= " %%i in (`git status`) do (
    set status=%%i
    CALL :TRIM !status! status
    if "!status!"=="modified:" (
        set /p tmp=Execute git diff %%j ... <nul
        git diff %%j | findstr . >nul
        if !errorlevel! EQU 1 (
             echo no, reverting...
             git checkout -- %%j
        ) else (
             echo yes.
        )
    )
)

goto :EOF

:TRIM
set %2=%1
goto :EOF
