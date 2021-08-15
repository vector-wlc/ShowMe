@echo off

set QT_DIR=D:\Software\Qt\5.15.0\msvc2019
set SRC_DIR=%cd%
set BUILD_DIR=%cd%\build

if not exist %QT_DIR% exit
if not exist %SRC_DIR% exit
if not exist %BUILD_DIR% md %BUILD_DIR%

cd /d %BUILD_DIR%

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

%QT_DIR%\bin\qmake.exe %SRC_DIR%\ShowMe.pro -spec win32-msvc  "CONFIG+=release"
if exist %BUILD_DIR%\release\ShowMe.exe del %BUILD_DIR%\release\ShowMe.exe
nmake Release
if not exist %BUILD_DIR%\release\Qt5Cored.dll (
  %QT_DIR%\bin\windeployqt.exe %BUILD_DIR%\release\ShowMe.exe
)
