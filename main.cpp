#include <stdexcept>
#include <vector>
#include <iostream>

#include <windows.h>

#include <GL/glew.h>
#include <gl/gl.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.hpp"
#include "tools.hpp"
#include "globals.hpp"

#include "components/mouseState.hpp"
#include "components/screenInfo.hpp"
#include "components/mvp.hpp"

const bool fullScreen = false;
const bool console = true;
const glm::ivec2 windowRes = { 800, 800 };

const std::vector<glm::vec3> vertices = {
	{-1, -1, 0},
	{1, -1, 0},
	{-1, 1, 0},
	{1, 1, 0}
};

const std::vector<unsigned> indices = {
	0, 1, 2,
	1, 2, 3
};

shaders::ProgramId program;
GLuint vertexArray;

void OGLInitialize()
{
	const GLenum glewInitResult = glewInit();
	assert(GLEW_OK == glewInitResult);

	//glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);

	glCreateVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices.front()), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	GLuint elementBuffer;
	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices.front()), indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	program = shaders::LinkProgram(shaders::CompileShaders("shaders/basic.vs", "shaders/basic.fs"),
		{ {0, "bPos"} });
}

void Initialize()
{
	if (console)
	{
		tools::RedirectIOToConsole({ 800, 10 });
	}
	ShowCursor(false);
	OGLInitialize();
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE,
		glm::value_ptr(Globals::mvp.getVP()));

	glBindVertexArray(vertexArray);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

void PrepareFrame()
{
	const glm::ivec2 mouseDelta = Globals::mouseState.getMouseDelta();
	std::cout << mouseDelta.x << " " << mouseDelta.y << std::endl;
	tools::SetMousePos(Globals::screenInfo.windowCenterInScreenSpace);
	RenderScene();
}

void ChangeWindowSize(glm::ivec2 size)
{
	Globals::screenInfo.windowSize = { size.x, size.y };

	glViewport(0, 0, size.x, size.y);

	const float ratio = (float)size.x / size.y;
	Globals::mvp.projection = glm::ortho(-10.0f * ratio, 10.0f * ratio, -10.0f, 10.0f);
}

void ChangeWindowLocation(glm::ivec2 location)
{
	Globals::screenInfo.windowCenterInScreenSpace = { location + Globals::screenInfo.windowSize / 2 };
}

void WindowLocationInitialized()
{
	tools::SetMousePos(Globals::screenInfo.windowCenterInScreenSpace);
	Globals::mouseState.prevPosition = Globals::mouseState.position = Globals::screenInfo.windowCenterInScreenSpace;
}

void HandleKeyboard(bool const * const keys)
{
}

void HandleMouse()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	Globals::mouseState.prevPosition = Globals::mouseState.position;
	Globals::mouseState.position = { mousePos.x, mousePos.y };
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
static HDC hDC;

LRESULT CALLBACK WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
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
		case WM_KEYDOWN:
			keys[wParam] = true;
			break;
		case WM_KEYUP:
			keys[wParam] = false;
			break;
		case WM_RBUTTONDOWN:
			Globals::mouseState.rmb = true;
			break;
		case WM_RBUTTONUP:
			Globals::mouseState.rmb = false;
			break;
		case WM_LBUTTONDOWN:
			Globals::mouseState.lmb = true;
			break;
		case WM_LBUTTONUP:
			Globals::mouseState.lmb = false;
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
			PrepareFrame();
			SwapBuffers(hDC);
		}
	}
	
	return (int)msg.wParam;
}
