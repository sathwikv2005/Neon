@echo off
profiler.exe %* || exit /b 1
gprof profiler.exe gmon.out > report.txt