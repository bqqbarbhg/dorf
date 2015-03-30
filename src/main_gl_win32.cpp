#include <Windows.h>
#include <GL/gl.h>

void create_gl_context(HWND window)
{
	HDC dc = GetDC(window);
	PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1 };
	pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, pixelFormat, &pfd);

	HGLRC glrc = wglCreateContext(dc);

	wglMakeCurrent(dc, glrc);
}

bool run = true;
LRESULT CALLBACK WinProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		create_gl_context(window);
		break;
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

		glClearColor(0x64/255.0f, 0x95/255.0f, 0xED/255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		SwapBuffers(GetDC(window));
	}

	return 0;
}

