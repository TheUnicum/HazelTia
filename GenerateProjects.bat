@echo off
setlocal
:PROMPT
SET /P AREYOUSURE=Do you want regenerate VS2019 Solution Project? Are you sure(Y/[N])?
IF /I "%AREYOUSURE%" NEQ "Y" GOTO END

call vendor\bin\premake\premake5.exe vs2019
PAUSE
:END
endlocal
