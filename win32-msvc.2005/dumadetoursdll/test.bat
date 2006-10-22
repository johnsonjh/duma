@echo off

rem Run a quick test

set DUMA_OUTPUT_STACKTRACE=1
set DUMA_OUTPUT_STACKTRACE_MAPFILE=C:\Projects\duma\duma-head\win32-msvc.2005\detoursexample1\detoursexample1\Debug\detoursexample1.map
set DUMA_DETOURS_DLL=detours2.1\dumadetoursdll.dll

..\dumadetours\detours2.1\dumadetours.exe C:\Projects\duma\duma-head\win32-msvc.2005\detoursexample1\detoursexample1\Debug\detoursexample1.exe

rem end
