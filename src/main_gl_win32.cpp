#include <Windows.h>
#include <GL/gl.h>
#include <stdio.h>

#define snprintf _snprintf

bool run = true;

void create_gl_context(HWND window)
{
	// Request some sort of pixel format
	// NOTE: This needs more thought later
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

	// Create and set the context
	HGLRC glrc = wglCreateContext(dc);
	wglMakeCurrent(dc, glrc);
}

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

typedef void (*render_t)();
struct Frontend_Module
{
	HINSTANCE instance;
	render_t render;
	FILETIME write_time;
};

bool load_frontend(Frontend_Module *module, const char *path)
{
	// Unload the old instance if we have one
	if (module->instance)
		FreeLibrary(module->instance);

	// Try to load the new one
	module->instance = LoadLibrary(path);
	if (!module->instance)
		return false;

	// Resolve functions
	module->render = (render_t)GetProcAddress(module->instance, "render");

	return true;
}

Frontend_Module frontend = { 0 };

struct File_Find_Info
{
	char filename[MAX_PATH];
	FILETIME write_time;
	bool exists;
};
File_Find_Info find_most_recently_changed(const char *filename)
{
	File_Find_Info newest = { 0 };

	WIN32_FIND_DATA find_data;
	HANDLE find_h = FindFirstFile(filename, &find_data);

	bool found = (find_h != INVALID_HANDLE_VALUE);
	while (found)
	{
		// Compare the time of the found file and our current newest file
		// The newest starts out cleared to zero so all files will be newer than it
		FILETIME write_time = find_data.ftLastWriteTime;
		if (CompareFileTime(&write_time, &newest.write_time) > 0)
		{
			// Copy the newest match into the returned value
			newest.write_time = write_time;
			strcpy(newest.filename, find_data.cFileName);
			newest.exists = true;
		}

		found = FindNextFile(find_h, &find_data) != 0;
	}

	if (find_h)
		FindClose(find_h);

	return newest;
}

const char *temp_dll_path = "dlltemp";
void create_temp_dll_filename(const char *in, char *out, unsigned out_size)
{
	CreateDirectory(temp_dll_path, NULL);

	// Create sequential filenames for now, this might fail when there is old files
	// in an directory which hasn't been cleaned
	static int dll_load_count;
	++dll_load_count;
	snprintf(out, out_size, "%s/%s_%d.dll", temp_dll_path, in, dll_load_count);
}

void check_dynamic_dlls()
{
	File_Find_Info info = find_most_recently_changed("frontend_gl.dll");

	if (!info.exists)
		return;
	if (CompareFileTime(&info.write_time, &frontend.write_time) <= 0)
		return;

	// Windows doesn't release DLLs while they are in use so in order to be able to write
	// a new one while the game is running let's copy the DLL into a temp directory
	// and load if from there
	char frontend_temp_dll_name[MAX_PATH];
	create_temp_dll_filename("frontend", frontend_temp_dll_name, MAX_PATH);
	CopyFile(info.filename, frontend_temp_dll_name, FALSE);

	load_frontend(&frontend, frontend_temp_dll_name);
	frontend.write_time = info.write_time;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
	// -- Create the window for the application
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

	// -- Main Loop
	while (run)
	{
		// Check if there are new DLLs
		// This is done now every frame but it could be done more rarely or in another thread
		check_dynamic_dlls();

		MSG message;
		while (PeekMessageA(&message, window, 0, 0, 1))
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}

		// TODO: Better timing
		Sleep(10);

		if (frontend.render)
			frontend.render();

		SwapBuffers(GetDC(window));
	}

	return 0;
}

