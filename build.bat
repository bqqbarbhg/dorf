@echo off

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
cd bin

set IgnoreWarn= -wd4100
set CLFlags= -MT -nologo -Od -W4 -WX -Zi %IgnoreWarn%
set LDFlags= -opt:ref user32.lib gdi32.lib

cl %CLFlags% ../src/frontend_gl.cpp /link opengl32.lib /DLL /OUT:frontend_gl.dll
cl %CLFlags% ../src/main_gl_win32.cpp /link %LDFlags% opengl32.lib -subsystem:windows 
cd ..

