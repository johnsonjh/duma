@echo off

rem Run a quick test

set DUMA_OUTPUT_STACKTRACE=1
set DUMA_OUTPUT_STACKTRACE_MAPFILE=C:\Projects\duma-dd-unstable\win32-msvc.2005\detoursexample1\detoursexample1\Debug\detoursexample1.map
set DUMA_DETOURS_DLL=detours\dumadetoursdll.dll

..\dumadetours\detours\dumadetours.exe ..\detoursexample1\detoursexample1\debug\detoursexample1.exe

rem end
