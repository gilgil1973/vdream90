@echo off
rem ***************************************************************************
rem QT_PATH
rem ***************************************************************************
set QT_PATH=%1
if "%1" == "" goto usage

rem ***************************************************************************
rem header files
rem ***************************************************************************
mkdir %QT_PATH%\include\VDream

mkdir %QT_PATH%\include\VDream\Common
copy .\VDream\Common\*.* %QT_PATH%\include\VDream\Common\*.*

mkdir %QT_PATH%\include\VDream\Log
copy .\VDream\Log\*.* %QT_PATH%\include\VDream\Log\*.*

mkdir %QT_PATH%\include\VDream\Net
copy .\VDream\Net\*.* %QT_PATH%\include\VDream\Net\*.*

mkdir %QT_PATH%\include\VDream\Other
copy .\VDream\Other\*.* %QT_PATH%\include\VDream\Other\*.*

rem ***************************************************************************
rem lib files
rem ***************************************************************************
copy .\Bin\VDreamd.lib %QT_PATH%\lib
copy .\Bin\VDream.lib  %QT_PATH%\lib
goto eof

rem ***************************************************************************
rem usage
rem ***************************************************************************
:usage
echo "syntax :  _install <compiler folder>"
echo "example : _install C:\QtSDK\Desktop\Qt\4.8.1\msvc2008"
goto eof

rem ***************************************************************************
rem eof
rem ***************************************************************************
:eof
