@echo off
rem ***************************************************************************
rem QT_PATH
rem ***************************************************************************
set QT_PATH=%1
if "%1" == "" goto usage

rem ***************************************************************************
rem header files
rem ***************************************************************************
del /q %QT_PATH%\include\VDream\Common\*.*
rmdir  %QT_PATH%\include\VDream\Common

del /q %QT_PATH%\include\VDream\Log\*.*
rmdir  %QT_PATH%\include\VDream\Log

del /q %QT_PATH%\include\VDream\Net\*.*
rmdir  %QT_PATH%\include\VDream\Net

del /q %QT_PATH%\include\VDream\Other\*.*
rmdir  %QT_PATH%\include\VDream\Other

rmdir  %QT_PATH%\include\VDream

rem ***************************************************************************
rem lib files
rem ***************************************************************************
del %QT_PATH%\lib\VDreamd.lib 
del %QT_PATH%\lib\VDream.lib 
goto eof

rem ***************************************************************************
rem usage
rem ***************************************************************************
:usage
echo "syntax :  _uninstall <compiler folder>"
echo "example : _uninstall C:\QtSDK\Desktop\Qt\4.8.1\msvc2008"
goto eof

rem ***************************************************************************
rem eof
rem ***************************************************************************
:eof
