#define STRICT
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include <assert.h>
#include <tchar.h>

#include <gl\\gl.h>
#include <gl\\glu.h>

#pragma comment (linker, "/defaultlib:opengl32.lib")
#pragma comment (linker, "/defaultlib:glu32.lib")

#ifndef _countof
 #define _countof (a) (sizeof (a) / sizeof (a [0]))
#endif

HINSTANCE g_hApp = nullptr;
LPCTSTR g_szAppName = _T("GL Tests");

HWND g_hWindow = nullptr;
HDC g_hDC = nullptr;
HGLRC g_hGLRC = nullptr;
LPCTSTR g_szWndClass = _T("WcOglTests"),
g_szTitle = g_szAppName;

int g_wndWidth = -1, g_wndHeight = -1;

double g_angle = 0.0,
g_angle0z = 0.0,
g_angle0x = 0.0,
g_angle0y = 0.0,
g_Angle = 0.0,
g_PosX = 0.0,
g_PosY = 0.0,
g_PosZ = 0.0;

int g_iPyrEdge = 0;

double g_sceneWidth = 6.0;

double g_dUnitLen = 1.0,
g_dAngOfUnit1 = 0.0,
g_dAngOfUnit2 = 0.0,
g_dAngOfUnit3 = 0.0,
g_dAngOfUnit4 = 0.0,
g_dAngOfUnit5 = 0.0,
fAngViewX = 0.0,
fAngViewY = 0.0,
fAngViewZ = 0.0;

GLUquadricObj* g_pGluQuadObj = nullptr;

BOOL SetPixelFormat(HDC dc)
{
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cAlphaBits = 24;
	pfd.cStencilBits = 0;
	pfd.cDepthBits = 24;
	pfd.cAccumBits = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pf = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat(dc, pf, &pfd);

	return !(pfd.dwFlags & PFD_NEED_PALETTE);
}

void DrawAxes(double dAxisSize_)
{
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);
	glDisable(GL_DEPTH_TEST);

	gluQuadricDrawStyle(g_pGluQuadObj, GLU_FILL);

	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3d(-dAxisSize_ / 2, 0, 0);
	glVertex3d(dAxisSize_, 0, 0);
	glColor3d(0, 1, 0);
	glVertex3d(0, -dAxisSize_ / 2, 0);
	glVertex3d(0, dAxisSize_, 0);
	glColor3d(0, 0, 1);
	glVertex3d(0, 0, -dAxisSize_ / 2);
	glVertex3d(0, 0, dAxisSize_);
	glEnd();

	glPushMatrix();
	glColor3d(1, 0, 0);
	glTranslated(dAxisSize_, 0, 0);
	glRotated(90, 0, 1, 0);
	gluCylinder(g_pGluQuadObj, dAxisSize_ / 10, 0, dAxisSize_ / 5, 32, 1);
	glPopMatrix();

	glPushMatrix();
	glColor3d(0, 1, 0);
	glTranslated(0, dAxisSize_, 0);
	glRotated(-90, 1, 0, 0);
	gluCylinder(g_pGluQuadObj, dAxisSize_ / 10, 0, dAxisSize_ / 5, 32, 1);
	glPopMatrix();

	glPushMatrix();
	glColor3d(0, 0, 1);
	glTranslated(0, 0, dAxisSize_);
	gluCylinder(g_pGluQuadObj, dAxisSize_ / 10, 0, dAxisSize_ / 5, 32, 1);
	glPopMatrix();

}  //DrawAxes

void Draw_cabine()
{
	glBegin(GL_POLYGON);
	glColor3d(0, 1, 0);
	glVertex3f(-4.0 * g_dUnitLen, 0.0 * g_dUnitLen, 0.5);
	glVertex3f(-0.5 * g_dUnitLen, 0.0 * g_dUnitLen, 0.5);
	glVertex3f(0.5 * g_dUnitLen, 0.0 * g_dUnitLen, -0.5);
	glVertex3f(0.5 * g_dUnitLen, 0.0 * g_dUnitLen, -3.0);
	glVertex3f(-4.0 * g_dUnitLen, 0.0 * g_dUnitLen, -3.0);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3d(0, 1, 0);
	glVertex3f(-4.0 * g_dUnitLen, 2.0 * g_dUnitLen, 0.5);
	glVertex3f(-0.5 * g_dUnitLen, 2.0 * g_dUnitLen, 0.5);
	glVertex3f(0.5 * g_dUnitLen, 2.0 * g_dUnitLen, -0.5);
	glVertex3f(0.5 * g_dUnitLen, 2.0 * g_dUnitLen, -3.0);
	glVertex3f(-4.0 * g_dUnitLen, 2.0 * g_dUnitLen, -3.0);
	glEnd();


	glBegin(GL_QUADS); //pered kabini
	glColor3d(0, 0.9, 0.5);
	glVertex3f(0.5 * g_dUnitLen, 0.0 * g_dUnitLen, -3.0);
	glVertex3f(0.5 * g_dUnitLen, 0.0 * g_dUnitLen, -0.5);
	glVertex3f(0.5 * g_dUnitLen, 2.0 * g_dUnitLen, -0.5);
	glVertex3f(0.5 * g_dUnitLen, 2.0 * g_dUnitLen, -3.0);
	glEnd();

	glBegin(GL_QUADS); //pered verx kabini
	glColor3d(0, 0.9, 0.5);
	glVertex3f(-0.5 * g_dUnitLen, 0.0 * g_dUnitLen, 0.5);
	glVertex3f(0.5 * g_dUnitLen, 0.0 * g_dUnitLen, -0.5);
	glVertex3f(0.5 * g_dUnitLen, 2.0 * g_dUnitLen, -0.5);
	glVertex3f(-0.5 * g_dUnitLen, 2.0 * g_dUnitLen, 0.5);
	glEnd();

	glBegin(GL_QUADS); //verx kabini
	glColor3d(0, 0, 0.5);
	glVertex3f(-0.5 * g_dUnitLen, 0.0 * g_dUnitLen, 0.5);
	glVertex3f(-4.0 * g_dUnitLen, 0.0 * g_dUnitLen, 0.5);
	glVertex3f(-4.0 * g_dUnitLen, 2.0 * g_dUnitLen, 0.5);
	glVertex3f(-0.5 * g_dUnitLen, 2.0 * g_dUnitLen, 0.5);
	glEnd();

	glBegin(GL_QUADS); //niz kabini
	glColor3d(0, 0, 0.5);
	glVertex3f(0.5 * g_dUnitLen, 0.0 * g_dUnitLen, -3.0);
	glVertex3f(-4.0 * g_dUnitLen, 0.0 * g_dUnitLen, -3.0);
	glVertex3f(-4.0 * g_dUnitLen, 2.0 * g_dUnitLen * g_dUnitLen, -3.0);
	glVertex3f(0.5 * g_dUnitLen, 2.0 * g_dUnitLen, -3.0);
	glEnd();

	glBegin(GL_QUADS); //zad kabini
	glColor3d(0, 0.9, 0.5);
	glVertex3f(-4.0 * g_dUnitLen, 0.0 * g_dUnitLen, -3);
	glVertex3f(-4.0 * g_dUnitLen, 0.0 * g_dUnitLen, 0.5);
	glVertex3f(-4.0 * g_dUnitLen, 2 * g_dUnitLen, 0.5);
	glVertex3f(-4.0 * g_dUnitLen, 2 * g_dUnitLen, -3);
	glEnd();
}  //func Draw

void Draw_strela()
{
	glBegin(GL_POLYGON); //первая стрела
	glColor3d(0, 0.6, 0); //bok
	glVertex3f(1.5 * g_dUnitLen, 0.5 * g_dUnitLen, 1.5);
	glVertex3f(4.0 * g_dUnitLen, 0.5 * g_dUnitLen, 3.0);
	glVertex3f(0.0 * g_dUnitLen, 0.5 * g_dUnitLen, -1.0 );
	glVertex3f(2.5 * g_dUnitLen, 0.5 * g_dUnitLen, 0.5);
	glVertex3f(4.0 * g_dUnitLen, 0.5 * g_dUnitLen, 3.0);
	glEnd(); 

	glBegin(GL_POLYGON);
	glColor3d(0, 0.6, 0); //bok
	glVertex3f(1.5 * g_dUnitLen, 2.0 * g_dUnitLen, 1.5);
	glVertex3f(4.0 * g_dUnitLen, 2.0 * g_dUnitLen, 3.0);
	glVertex3f(0.0 * g_dUnitLen, 2.0 * g_dUnitLen, -1.0);
	glVertex3f(2.5 * g_dUnitLen, 2.0 * g_dUnitLen, 0.5);
	glVertex3f(4.0 * g_dUnitLen, 2.0 * g_dUnitLen, 3.0);
	glEnd(); 

	glBegin(GL_QUADS);
	glColor3d(0, 0, 0.5);
	glVertex3f(1.5 * g_dUnitLen, 0.5 * g_dUnitLen, 1.5);
	glVertex3f(4.0 * g_dUnitLen, 0.5 * g_dUnitLen, 3.0);
	glVertex3f(4.0 * g_dUnitLen, 2.0 * g_dUnitLen, 3.0);
	glVertex3f(1.5 * g_dUnitLen, 2.0 * g_dUnitLen, 1.5);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0, 0.5);
	glVertex3f(2.5 * g_dUnitLen, 0.5 * g_dUnitLen, 0.5);
	glVertex3f(4.0 * g_dUnitLen, 0.5 * g_dUnitLen, 3.0);
	glVertex3f(4.0 * g_dUnitLen, 2.0 * g_dUnitLen, 3.0);
	glVertex3f(2.5 * g_dUnitLen, 2.0 * g_dUnitLen, 0.5);
	glEnd();

	
	glBegin(GL_QUADS);
	glColor3d(0, 0, 0.5);
	glVertex3f(1.5 * g_dUnitLen, 0.5 * g_dUnitLen, 1.5);
	glVertex3f(0.0 * g_dUnitLen, 0.5 * g_dUnitLen, -1.0);
	glVertex3f(0.0 * g_dUnitLen, 2.0 * g_dUnitLen, -1.0);
	glVertex3f(1.5 * g_dUnitLen, 2.0 * g_dUnitLen, 1.5);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0, 0.5);
	glVertex3f(2.5 * g_dUnitLen, 0.5 * g_dUnitLen, 0.5);
	glVertex3f(0.0 * g_dUnitLen, 0.5 * g_dUnitLen, -1.0);
	glVertex3f(0.0 * g_dUnitLen, 2.0 * g_dUnitLen, -1.0);
	glVertex3f(2.5 * g_dUnitLen, 2.0 * g_dUnitLen, 0.5);
	glEnd();
}

void Draw_kovsh()
{
	glBegin(GL_POLYGON); 
	glColor3d(0, 1, 0); //bok
	glVertex3f(4.0 * g_dUnitLen, 0.5, 0.0 * g_dUnitLen);
	glVertex3f(0.0 * g_dUnitLen, 0.5, 0.0 * g_dUnitLen);
	glVertex3f(g_dUnitLen, 0.5, -2.5 * g_dUnitLen);
	glVertex3f(3.0 * g_dUnitLen, 0.5, -2.5 * g_dUnitLen);
	glEnd();

	glBegin(GL_POLYGON); 
	glColor3d(0, 1, 0); //bok
	glVertex3f(4.0 * g_dUnitLen, 2.5, 0 * g_dUnitLen);
	glVertex3f(0 * g_dUnitLen, 2.5, 0 * g_dUnitLen);
	glVertex3f(g_dUnitLen, 2.5, -2.5 * g_dUnitLen);
	glVertex3f(3.0 * g_dUnitLen, 2.5, -2.5 * g_dUnitLen);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0, 0.5); //niz kovsha
	glVertex3f(3.0 * g_dUnitLen, 0.5, -2.5 * g_dUnitLen);
	glVertex3f(g_dUnitLen, 0.5, -2.5 * g_dUnitLen);
	glVertex3f(g_dUnitLen, 2.5, -2.5 * g_dUnitLen);
	glVertex3f(3.0 * g_dUnitLen, 2.5, -2.5 * g_dUnitLen);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0.9, 0.5); //boka naklon
	glVertex3f(3.0 * g_dUnitLen, 0.5, -2.5 * g_dUnitLen);
	glVertex3f(4.0 * g_dUnitLen, 0.5, 0 * g_dUnitLen);
	glVertex3f(4.0 * g_dUnitLen, 2.5, 0 * g_dUnitLen);
	glVertex3f(3.0 * g_dUnitLen, 2.5, -2.5 * g_dUnitLen);
	glEnd();

	glBegin(GL_QUADS);
	glColor3d(0, 0.9, 0.5); //boka naklon
	glVertex3f(0.0 * g_dUnitLen, 0.5, 0 * g_dUnitLen);
	glVertex3f(g_dUnitLen, 0.5, -2.5 * g_dUnitLen);
	glVertex3f(g_dUnitLen, 2.5, -2.5 * g_dUnitLen);
	glVertex3f(0.0 * g_dUnitLen, 2.5, 0 * g_dUnitLen);
	glEnd();
}

void Draw()
{
	GLsizei viewportSize = g_wndHeight;
	glViewport(0, 0, viewportSize, viewportSize);

	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-g_sceneWidth * 0.5, g_sceneWidth * 0.5, -g_sceneWidth * 0.5, g_sceneWidth * 0.5,
		-g_sceneWidth * 1000.0, g_sceneWidth * 1000.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	glTranslated(g_PosX, 0.0, g_PosZ); //движения экскаватора

	glPushMatrix();

	//glRotated(g_angle, 0.0, 0.0, 1.0);
	glRotated(-30.0, 1.0, 0.0, 0.0);
	glRotated(-35.0, 0.0, 0.0, 1.0);

	glRotated(g_angle0x, 1.0, 0.0, 0.0);
	glRotated(g_angle0y, 0.0, 1.0, 0.0);
	glRotated(g_angle0z, 0.0, 0.0, 1.0);

	glPushMatrix();

	glScalef(0.1, 0.1, 0.1);
	Draw_cabine();

	glRotated(g_dAngOfUnit1, 0.0, 1.0, 0.0);
	glScalef(0.8, 0.8, 0.8);

	Draw_strela();
	
	glTranslated(4.5, 0.5, 2.95);
	glRotated(g_dAngOfUnit2, 0.0, 1.0, 0.0);
	glScalef(0.6, 0.6, 0.6);
	glRotated(110.0, 0.0, 1.0, 0.0);

	Draw_strela();
	
	glTranslated(4.0, 0.0, 3.0);
	glRotated(g_dAngOfUnit3, 0.0, 1.0, 0.0);
	glRotated(-120.0, 0.0, 1.0, 0.0);
	//glScaled(0.15, 0.15, 0.15);

	Draw_kovsh();

	glPopMatrix();

	// нарисовать оси
	//glDisable(GL_BLEND);
	//DrawAxes(0.5);

	glPopMatrix();

	glFinish();
	SwapBuffers(g_hDC);
}  //Draw


BOOL MainOnCreate(HWND hwnd, LPCREATESTRUCT p_cs)
{
	g_hDC = GetDC(hwnd);

	SetPixelFormat(g_hDC);

	g_hGLRC = wglCreateContext(g_hDC);
	wglMakeCurrent(g_hDC, g_hGLRC);

	g_pGluQuadObj = gluNewQuadric();
	assert(g_pGluQuadObj);

	return TRUE;
}


BOOL MainOnSize(int width_, int height_)
{
	g_wndWidth = width_;
	g_wndHeight = height_;
	return TRUE;
}


BOOL MainOnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(g_hWindow, &ps);
	Draw();
	EndPaint(g_hWindow, &ps);
	return TRUE;
}


BOOL MainOnKeyDown(char k_)
{
	switch (k_)
	{
	case VK_LEFT:
		g_PosX -= 0.05;
		break;

	case VK_RIGHT:
		g_PosX += 0.05;
		break;

	case VK_DOWN:
		g_PosZ -= 0.05;
		break;

	case VK_UP:
		g_PosZ += 0.05;
		break;
	}

	InvalidateRect(g_hWindow, nullptr, FALSE);

	return TRUE;
}


BOOL MainOnChar(char c_)
{
	switch (c_)
	{
	case '1':
		if (g_dAngOfUnit1 > -30.0)
		{
			g_dAngOfUnit1 -= 5.0;
			break;
		}
		else { break; }

	case '2':
		if (g_dAngOfUnit1 < 30.0)
		{
			g_dAngOfUnit1 += 5.0;
			break;
		}
		else { break; }

	case '3':
		if (g_dAngOfUnit2 > -30.0)
		{
			g_dAngOfUnit2 -= 5.0;
			break;
		}
		else { break; }

	case '4':
		if (g_dAngOfUnit2 < 15.0)
		{
			g_dAngOfUnit2 += 5.0;
			break;
		}
		else { break; }

	case '5':
		if (g_dAngOfUnit3 > -30.0)
		{
			g_dAngOfUnit3 -= 5.0;
			break;
		}
		else { break; }

	case '6':	
		if (g_dAngOfUnit3 < 60.0)
		{
			g_dAngOfUnit3 += 5.0;
			break;
		}
		else { break; }

	case 'q':
		g_angle0x += 30.0;
		break;
	case 'w':
		g_angle0y += 30.0;
		break;
	case 'e':
		g_angle0z += 30.0;
		break;
	}
	g_iPyrEdge = c_ - '0';

	InvalidateRect(g_hWindow, nullptr, FALSE);

	return TRUE;
}


BOOL MainOnDestroy()
{
	gluDeleteQuadric(g_pGluQuadObj);
	g_pGluQuadObj = nullptr;

	wglMakeCurrent(nullptr, nullptr);
	if (g_hGLRC)
		wglDeleteContext(g_hGLRC);

	PostQuitMessage(0);

	return TRUE;
}


LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		return MainOnCreate(hwnd, (LPCREATESTRUCT)lParam);

	case WM_SIZE:
		return MainOnSize(LOWORD(lParam), HIWORD(lParam));

	case WM_PAINT:
		return MainOnPaint();

	//case WM_LBUTTONDOWN:
		//return MainOnLButtonDown();

	//case WM_MOUSEMOVE:
		//return MainOnMouseMove(LOWORD(lParam), HIWORD(lParam), msg);

	case WM_KEYDOWN:
		return MainOnKeyDown(wParam);

	case WM_CHAR:
		return MainOnChar(wParam);

	case WM_DESTROY:
		return MainOnDestroy();

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0L;
}  //MainWindowProc

/*BOOL MainOnMouseMove(int x, int y, UINT flags)
{

	if (!((MK_LBUTTON | MK_RBUTTON) & flags))
		return FALSE;


	if (MK_LBUTTON == flags)
	{

		float fDAng = 90.f * (x - g_PosX) / g_wndWidth;
		fAngViewY += fDAng;

		if (fAngViewY > 360.f)
			fAngViewY -= 360.f;
		if (fAngViewY < -360.f)
			fAngViewY += 360.f;
		fDAng = 180.f * (y - g_PosY) / g_wndHeight;
		fAngViewX += fDAng;
		if (fAngViewX > 360.f)
			fAngViewX -= 360.f;
		if (fAngViewX < -360.f)
			fAngViewX += 360.f;


		g_PosX = x;
		g_PosY = y;
	}
	InvalidateRect(g_hWindow, nullptr, FALSE);
	return TRUE;
}  //func MainOnMouseMove

BOOL MainOnMButtonDown(int x, int y)
{
	g_PosX = x;
	g_PosY = y;
	return TRUE;
}  //func MainOnLButtonDown
*/

BOOL InitApp()
{
	WNDCLASSEX wce;
	ZeroMemory(&wce, sizeof(WNDCLASSEX));
	wce.cbSize = sizeof(WNDCLASSEX);
	wce.hInstance = g_hApp;
	wce.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wce.lpfnWndProc = MainWindowProc;
	wce.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wce.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wce.lpszClassName = g_szWndClass;
	if (!RegisterClassEx(&wce))
		return FALSE;

	SetLastError(0);
	g_hWindow = CreateWindow(g_szWndClass, g_szTitle, WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0, 0, 800, 800, nullptr, nullptr, g_hApp, nullptr);
	if (!g_hWindow)
	{
		DWORD err = GetLastError();
		return FALSE;
	}
	ShowWindow(g_hWindow, SW_SHOW);
	UpdateWindow(g_hWindow);

	return TRUE;
}


void UninitApp()
{
	UnregisterClass(g_szWndClass, g_hApp);
}


int APIENTRY WinMain(HINSTANCE hApp_, HINSTANCE, LPSTR, int)
{
	g_hApp = hApp_;

	if (InitApp())
	{
		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	UninitApp();

	return 0;
}

// --- main.cpp ---