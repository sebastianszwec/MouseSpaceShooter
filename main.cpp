#include <stdexcept>
#include <vector>
#include <iostream>

#include <windows.h>

#include <GL/glew.h>
#include <gl/gl.h>

#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "tools.hpp"
#include "globals.hpp"

#include "components/mouseState.hpp"
#include "components/screenInfo.hpp"
#include "components/mvp.hpp"
#include "components/physics.hpp"

#include "systems/player.hpp"

const bool fullScreen = true;
const bool console = true;
const glm::ivec2 windowRes = { 800, 800 };

void OGLInitialize()
{
	const GLenum glewInitResult = glewInit();
	assert(GLEW_OK == glewInitResult);

	//glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);
}

void Initialize()
{
	if (console) tools::RedirectIOToConsole({ 2000, 10 });
	ShowCursor(false);
	OGLInitialize();

	Globals::Systems::Initialize();
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Globals::Systems::AccessPlayer().render();
}

void PrepareFrame(bool focus)
{
	if (!focus) return;

	Globals::Systems::AccessPlayer().step();

	Globals::Components::physics.world.Step(1.0f / 60, 3, 8);

	RenderScene();
}

void HandleKeyboard(bool const* const keys)
{
}

void HandleMouse()
{
	using namespace Globals::Components;

	POINT mousePos;
	GetCursorPos(&mousePos);
	const auto prevPosition = mouseState.position;
	mouseState.position = { mousePos.x, mousePos.y };
	mouseState.delta = mouseState.position - prevPosition;
	
	tools::SetMousePos(screenInfo.windowCenterInScreenSpace);
	mouseState.position = screenInfo.windowCenterInScreenSpace;
}

void ChangeWindowSize(glm::ivec2 size)
{
	using namespace Globals::Components;
	using namespace Globals::Defaults;

	const float ratio = (float)size.x / size.y;

	screenInfo.windowSize = { size.x, size.y };

	glViewport(0, 0, size.x, size.y);

	mvp.projection = glm::ortho(-hProjectionSize * ratio, hProjectionSize * ratio, -hProjectionSize, hProjectionSize);
}

void ChangeWindowLocation(glm::ivec2 location)
{
	using namespace Globals::Components;

	screenInfo.windowCenterInScreenSpace = { location + screenInfo.windowSize / 2 };
}

void WindowLocationInitialized()
{
	using namespace Globals::Components;

	tools::SetMousePos(screenInfo.windowCenterInScreenSpace);
	mouseState.position = screenInfo.windowCenterInScreenSpace;
}

void SetDCPixelFormat(HDC hDC)
{
	const PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	const int pixelFormt = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormt, &pfd);
}

static bool keys[256];
static bool quit;
static bool focus;
static HDC hDC;

LRESULT CALLBACK WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	using namespace Globals::Components;

	static HGLRC hRC;
	static bool locationInitialized = false;
	static bool firstSize = true;
	static bool firstMove = true;

	switch(message)
	{
		case WM_CREATE:
		{
			hDC = GetDC(hWnd);
			SetDCPixelFormat(hDC);
			hRC = wglCreateContext(hDC);
			wglMakeCurrent(hDC, hRC);
			try
			{
				Initialize();
			}
			catch (const std::runtime_error & error)
			{
				MessageBox(nullptr, error.what(), "Runtime error",
					MB_OK | MB_ICONEXCLAMATION);
				ExitProcess(0);
			}
			break;
		}
		case WM_DESTROY:
			wglMakeCurrent(hDC, nullptr);
			wglDeleteContext(hRC);
			PostQuitMessage(0);
			quit = true;
			break;
		case WM_SIZE:
		{
			const glm::ivec2 size{ LOWORD(lParam), HIWORD(lParam) };
			ChangeWindowSize(size);
			if (firstSize)
			{
				if (!firstMove && !locationInitialized)
				{
					WindowLocationInitialized();
					locationInitialized = true;
				}
				firstSize = false;
			}
			break;
		}
		case WM_MOVE:
		{
			const glm::ivec2 location{ LOWORD(lParam), HIWORD(lParam) };
			ChangeWindowLocation(location);
			if (firstMove)
			{
				if (!firstSize && !locationInitialized)
				{
					WindowLocationInitialized();
					locationInitialized = true;
				}
				firstMove = false;
			}
			break;
		}
		case WM_SETFOCUS:
			WindowLocationInitialized();
			focus = true;
			break;
		case WM_KILLFOCUS:
			focus = false;
			break;
		case WM_KEYDOWN:
			keys[wParam] = true;
			break;
		case WM_KEYUP:
			keys[wParam] = false;
			break;
		case WM_RBUTTONDOWN:
			mouseState.rmb = true;
			break;
		case WM_RBUTTONUP:
			mouseState.rmb = false;
			break;
		case WM_LBUTTONDOWN:
			mouseState.lmb = true;
			break;
		case WM_LBUTTONUP:
			mouseState.lmb = false;
			break;
		case WM_MBUTTONDOWN:
			break;
		case WM_MBUTTONUP:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0l;
}

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	const LPCTSTR lpszAppName = "OpenGL window";
	const int winPosX = 10, winPosY = 10;

	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = lpszAppName;
	
	if(!RegisterClass(&wc))
	{
		MessageBox(nullptr, "Window class registration failed.", "Error",
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	HWND hWnd = CreateWindow(
		lpszAppName,
		lpszAppName,
		fullScreen ? WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_MAXIMIZE : 
		WS_OVERLAPPEDWINDOW,
		winPosX, winPosY,
		windowRes.x, windowRes.y,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if(!hWnd)
	{
		MessageBox(nullptr, "Window creation failed.", "Error",
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	ShowWindow(hWnd, SW_SHOW);
	
	MSG msg{};

	while (!keys[VK_ESCAPE] && !quit)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			HandleKeyboard(keys);
			HandleMouse();
			PrepareFrame(focus);
			SwapBuffers(hDC);
		}
	}
	
	return (int)msg.wParam;
}
