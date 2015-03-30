#include <Windows.h>

bool run = true;
LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CLOSE:
		run = false;
		break;
	}

	return DefWindowProc(window, message, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
	WNDCLASS wnd_class = { 0 };
	wnd_class.style = CS_OWNDC;
	wnd_class.lpfnWndProc = WinProc;
	wnd_class.hInstance = instance;
	wnd_class.lpszClassName = "main_class";
	
	ATOM main_class = RegisterClassA(&wnd_class);
	if (!main_class) return 1;

	HWND window = CreateWindowExA(
		0, MAKEINTATOM(main_class),
		"Test Window",
		WS_TILEDWINDOW|WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1280, 720,
		NULL, NULL, instance, NULL);

	while (run)
	{
		MSG message;
		while (PeekMessageA(&message, window, 0, 0, 1))
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}
		Sleep(10);
	}

	return 0;
}

