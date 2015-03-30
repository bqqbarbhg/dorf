#include <Windows.h>
#include <GL/gl.h>

extern "C" __declspec(dllexport) void render()
{
	glClearColor(0x64/255.0f, 0x95/255.0f, 0xED/255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

