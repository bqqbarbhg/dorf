@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
cd bin

cl -Zi -SUBSYSTEM:WINDOWS ../src/main_gl_win32.cpp user32.lib

