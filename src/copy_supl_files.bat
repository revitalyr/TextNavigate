set SRC=%1
set DST=%2

if NOT exist %SRC% exit 3
if exist %DST% exit 0

FOR /f %%i IN ("%DST%") DO SET DST_PATH=%%~dpi
if NOT exist %DST_PATH%nul md %DST_PATH%

copy %SRC% %DST%
