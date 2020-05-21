@echo off
setlocal
:PROMPT
SET /P AREYOUSURE=Do you want regenerate VS2019 Solution Project? Are you sure(Y/[N])?
IF /I "%AREYOUSURE%" NEQ "Y" GOTO END

pushd ..\
call vendor\bin\premake\premake5.exe vs2019
popd

PAUSE

REM Smoother project generation (#83)
REM IF %ERRORLEVEL% NEQ 0 (
REM   PAUSE
REM )

:END
endlocal
