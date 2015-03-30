#include <Windows.h>

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
	typedef const char*(*get_message_t)();
	HMODULE module = LoadLibrary("frontend_gl.dll");
	get_message_t get_message = (get_message_t)GetProcAddress(module, "get_message");
	MessageBox(NULL, get_message(), "Winapi box", 0);
	return 0;
}

