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
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.hpp"
#include "tools.hpp"
#include "globals.hpp"

#include "components/mouseState.hpp"
#include "components/screenInfo.hpp"
#include "components/mvp.hpp"
#include "components/physics.hpp"
#include "components/player.hpp"

const bool fullScreen = false;
const bool console = true;
const glm::ivec2 windowRes = { 800, 800 };

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

	Globals::player.updateVerticesCache();
	glBufferData(GL_ARRAY_BUFFER, Globals::player.verticesCache.size() * sizeof(Globals::player.verticesCache.front()), Globals::player.verticesCache.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	program = shaders::LinkProgram(shaders::CompileShaders("shaders/basic.vs", "shaders/basic.fs"),
		{ {0, "bPos"} });
}

void PhysicsInitialize()
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 0.0f);
	bodyDef.angle = 0.0f;

	const float playerSize = 1.0f;
	const b2Vec2 playerTriangle[3] = {
		{ playerSize, 0 },
		{ -playerSize / 2.0f, playerSize / 2.0f },
		{ -playerSize / 2.0f, -playerSize / 2.0f } 
	};
	b2PolygonShape polygonShape;
	polygonShape.Set(playerTriangle, 3);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &polygonShape;
	fixtureDef.density = 1.0f;

	Globals::player.body.reset(Globals::physics.world.CreateBody(&bodyDef));
	Globals::player.body->CreateFixture(&fixtureDef);
	Globals::player.body->SetFixedRotation(true);
}

void Initialize()
{
	if (console)
	{
		tools::RedirectIOToConsole({ 2000, 10 });
	}
	ShowCursor(false);
	PhysicsInitialize();
	OGLInitialize();
}

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE,
		glm::value_ptr(Globals::mvp.getMVP(Globals::player.getModelMatrix())));

	glBindVertexArray(vertexArray);
	glDrawArrays(GL_TRIANGLES, 0, Globals::player.verticesCache.size());
}

b2Vec2 operator *(const b2Vec2 v, const float s)
{
	return {v.x * s, v.y * s};
}

void PreparePlayerInFrame(const glm::vec2& mouseDelta)
{
	if (glm::length(mouseDelta) > 20)
	{
		const auto normalizedMouseDelta = glm::normalize(mouseDelta);
		const float targetAngle = glm::orientedAngle(glm::vec2(1.0f, 0.0f), normalizedMouseDelta);

		Globals::player.body->SetTransform(Globals::player.body->GetPosition(), targetAngle);
		std::cout << targetAngle << std::endl;
	}

	if (Globals::mouseState.rmb)
	{
		const float force = 10.0f;
		const float currentAngle = Globals::player.body->GetAngle();

		Globals::player.body->ApplyForce(b2Vec2(glm::cos(currentAngle),
			glm::sin(currentAngle)) * force, Globals::player.body->GetWorldCenter(), true);
	}
}

void PrepareFrame(bool focus)
{
	if (!focus) return;

	const glm::ivec2 mouseDelta = Globals::mouseState.getMouseDelta();
	const glm::vec2 gameSpaceMouseDelta = { mouseDelta.x, -mouseDelta.y };

	PreparePlayerInFrame(gameSpaceMouseDelta);

	Globals::physics.world.Step(1.0f / 60, 3, 8);

	RenderScene();
}

void HandleKeyboard(bool const* const keys)
{
}

void HandleMouse()
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	const auto prevPosition = Globals::mouseState.position;
	Globals::mouseState.position = { mousePos.x, mousePos.y };
	Globals::mouseState.delta = Globals::mouseState.position - prevPosition;
	
	tools::SetMousePos(Globals::screenInfo.windowCenterInScreenSpace);
	Globals::mouseState.position = Globals::screenInfo.windowCenterInScreenSpace;
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
	Globals::mouseState.position = Globals::screenInfo.windowCenterInScreenSpace;
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
			PrepareFrame(focus);
			SwapBuffers(hDC);
		}
	}
	
	return (int)msg.wParam;
}
