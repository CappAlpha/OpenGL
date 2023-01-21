#define STRICT
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <tchar.h>

#include <gl\\gl.h>
#include <gl\\glu.h>


#pragma comment (linker, "/defaultlib:opengl32.lib")
#pragma comment (linker, "/defaultlib:glu32.lib")

#include "resource.h"

#ifndef _countof
	#define _countof (a) (sizeof (a) / sizeof (a [0]))
#endif

HINSTANCE g_hApp = nullptr;
LPCTSTR g_szAppName = _T("Прямоугольная труба с диалогом свойств");

HWND g_hWindow = nullptr;
HDC g_hDC = nullptr;
HGLRC g_hGLRC = nullptr;
LPCTSTR g_szWndClass = _T("WcOglBoxpipe"),
	    g_szTitle = g_szAppName;

int g_wndWidth = -1, g_wndHeight = -1;

double g_angle = 0;
const double g_angIncr = 1;

GLUquadricObj* g_pGluQuadObj = nullptr;

#define M_PI 3.1415926


 //
 // свойства
 //

struct Color
{
	LPCTSTR szName;
	COLORREF value;

	Color() : szName (_T("")), value (0) {}
	Color (LPCTSTR szName_, COLORREF value_) : szName (szName_), value (value_) {}
};

Color g_colors [] =
{
	Color (_T("- Красный"), RGB (255, 0, 0)),  // 0x0000FF
	Color (_T("- Зелёный"), RGB (0, 255, 0)),  // 0x00FF00
	Color (_T("- Жёлтый"),  RGB (255, 255, 0)),  // 0x00FFFF
};

struct BoxpipeProps
{
	double W, H, T, L, LB;
	int iColor;

	BoxpipeProps() : W (5.), H (10.), T (0.), L (30.), LB (5.), iColor (0) {}

	void CorrectValues()
	{
		if (W < 1e-5)
			W = 1e-5;
		if (H < 1e-5)
			H = 1e-5;
		if (L < 1e-5)
			L = 1e-5;
		if (T < 1e-5)
			T = 1e-5;
		if (LB < 1e-5)
			LB = 1e-5;
		if (T * 2 > W)
			T = W / 2;
		if (T * 2 > H)
			T = H / 2;

		//if (LB * 2 > W)
		//	LB = W / 2;

		if (iColor < 0 || iColor >= _countof (g_colors))
			iColor = 0;
	}
};

BoxpipeProps g_boxpipeProps;


 // размер сцены
double g_sceneWidth = 60.;


LRESULT CALLBACK MainWindowProc (HWND, UINT, WPARAM, LPARAM);
BOOL MainOnCreate (HWND, LPCREATESTRUCT);
BOOL MainOnCommand (int, HWND, UINT);
BOOL MainOnSize (int width, int height);
BOOL MainOnPaint();
BOOL MainOnSize (UINT, int, int);
BOOL MainOnDestroy();

INT_PTR CALLBACK DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

BOOL InitApp (void);
void UninitApp (void);

int APIENTRY WinMain (HINSTANCE, HINSTANCE, LPSTR, int);


 // установка формата пикселей
BOOL SetPixelFormat (HDC dc)
{
	PIXELFORMATDESCRIPTOR pfd;

	ZeroMemory (&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize        = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion     = 1;
	pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType   = PFD_TYPE_RGBA;
	pfd.cColorBits   = 24;
	pfd.cAlphaBits   = 24;
	pfd.cStencilBits = 0;
	pfd.cDepthBits   = 24;
    pfd.cAccumBits   = 0;
	pfd.iLayerType   = PFD_MAIN_PLANE;

	int pf = ChoosePixelFormat(dc, &pfd);
	SetPixelFormat (dc, pf, &pfd);

	return !(pfd.dwFlags & PFD_NEED_PALETTE);
}  //SetPixelFormat

 //
 // рисование трубы
 //
void DrawBoxpipe (bool fFill = true)
{
	float r = GetRValue (g_colors [g_boxpipeProps.iColor].value) / 255.f / 2,
		  g = GetGValue (g_colors [g_boxpipeProps.iColor].value) / 255.f / 2,
		  b = GetBValue (g_colors [g_boxpipeProps.iColor].value) / 255.f / 2;

	double w = g_boxpipeProps.W / 2,
		   h = g_boxpipeProps.H / 2,
		   l = g_boxpipeProps.L / 2,
		   lb = g_boxpipeProps.LB / 2;

	glPolygonMode (GL_FRONT, fFill ? GL_FILL : GL_LINE);
	glPolygonMode (GL_BACK, GL_LINE);

	glEnable (GL_DEPTH_TEST);

	glBegin (GL_QUADS);

		glColor3d (r * 1.5, g * 1.5, b * 1.5);
		glVertex3d (  w ,   h,   l);
		glVertex3d (  w ,   h, - l);
		glVertex3d (- w ,   h, - l);
		glVertex3d (- w ,   h,   l);

		glVertex3d(-w - lb, h, l);
		glVertex3d(-w - lb, h, -l);
		glVertex3d(-w * 3.0 - lb, h, -l);
		glVertex3d(-w * 3.0 - lb, h, l);

		//glVertex3d(-w * 2.0 - lb, h, l);
		//glVertex3d(-w * 2.0 - lb, h, -l);
	    //glVertex3d(-w * 4.0 - lb, h, -l);
		//glVertex3d(-w * 4.0 - lb, h, l);

		
		glColor3d (r, g, b);
		glVertex3d (- w,   h,   l);
		glVertex3d (- w,   h, - l);
		glVertex3d (- w, - h, - l);
		glVertex3d (- w, - h,   l);

		glColor3d (r * 1.5, g * 1.5, b * 1.5);
		glVertex3d (- w, - h,   l);
		glVertex3d (- w, - h, - l);
		glVertex3d (  w, - h, - l);
		glVertex3d (  w, - h,   l);


		glColor3d (r, g, b);
		glVertex3d (  w, - h,   l);
		glVertex3d (  w, - h, - l);
		glVertex3d (  w,   h, - l);
		glVertex3d (  w,   h,   l);

	

	glEnd();

	//double w1 = w - g_boxpipeProps.T,
	//	   h1 = h - g_boxpipeProps.T;

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	glBegin(GL_QUADS);

	glVertex3d(w + lb, h, l);
	glVertex3d(w + lb, h, -l);
	glVertex3d(w + lb, h - 3, -l);
	glVertex3d(w + lb, h - 3, l);

	glVertex3d(-w * 3.0 - lb, h, l);
	glVertex3d(-w * 3.0 - lb, h, -l);
	glVertex3d(-w * 3.0 - lb, h - 3, -l);
	glVertex3d(-w * 3.0 - lb, h - 3, l);

	glColor3f(r, g, b);
	glVertex3d(w * 3.0 + lb, h, l);
	glVertex3d(w * 3.0 + lb, h, -l);
	glVertex3d(w * 3.0 + lb, h - 3, -l);
	glVertex3d(w * 3.0 + lb, h - 3, l);

	glColor3f(r / 2, g / 2, b / 2);
	glVertex3d(-w * 3.0 - lb, h - 3, l);
	glVertex3d(-w * 3.0 - lb, h - 3, -l);
	glVertex3d(-w - lb, h - 3, -l);
	glVertex3d(-w - lb, h - 3, l);

	glEnd();

	glPolygonMode (GL_FRONT, GL_LINE);
	glPolygonMode (GL_BACK, fFill ? GL_FILL : GL_LINE);

	glBegin (GL_QUADS);

		glColor3f (r / 2, g / 2, b / 2);
		glVertex3d (- w, - h,   l);
		glVertex3d (- w, - h, - l);
		glVertex3d (  w, - h, - l);
		glVertex3d (  w, - h,   l);

		glColor3f (r, g, b);
		glVertex3d (  w, - h,   l);
		glVertex3d (  w, - h, - l);
		glVertex3d (  w,   h, - l);
		glVertex3d (  w,   h,   l);




		glColor3f(r/2, g/2, b/2);
		glVertex3d(-w * 3.0 - lb, h - 3, l);
		glVertex3d(-w * 3.0 - lb, h - 3, -l);
		glVertex3d(-w - lb, h - 3, -l);
		glVertex3d(-w - lb, h - 3, l);

		glVertex3d(-w - lb, h, l);
		glVertex3d(-w - lb, h, -l);
		glVertex3d(-w - lb, h - 3, -l);
		glVertex3d(-w - lb, h - 3, l);

		glVertex3d(w + lb, h - 3, l);
		glVertex3d(w + lb, h - 3, -l);
		glVertex3d(w * 3.0 + lb, h - 3, -l);
		glVertex3d(w * 3.0 + lb, h - 3, l);

	glEnd();

	//glPolygonMode (GL_FRONT, fFill ? GL_FILL : GL_LINE);
	//glPolygonMode (GL_BACK, GL_LINE);

	/*glBegin (GL_QUAD_STRIP);

		glColor3f (.5f, .5f, .5f);
		glVertex3d (  w,  h,   l);
		glVertex3d (  w,   h,    l);
		glVertex3d (- w,  h,   l);
		glVertex3d (- w,   h,    l);
		glVertex3d (- w, - h,  l);
		glVertex3d (- w,  - h,   l);
		glVertex3d (  w, - h,  l);
		glVertex3d (  w,  - h,   l);
		glVertex3d (  w,  h,   l);
		glVertex3d (  w,   h,    l);

	glEnd();
*/
/*	glPolygonMode (GL_FRONT, GL_LINE);
	glPolygonMode (GL_BACK, fFill ? GL_FILL : GL_LINE);

	glBegin (GL_QUAD_STRIP);

		glColor3f (.5f, .5f, .5f);
		glVertex3d (  w,  h,  - l);
		glVertex3d (  w,   h,   - l);
		glVertex3d (- w,  h,  - l);
		glVertex3d (- w,   h,   - l);
		glVertex3d (- w, - h, - l);
		glVertex3d (- w,  - h,  - l);
		glVertex3d (  w, - h, - l);
		glVertex3d (  w,  - h,  - l);
		glVertex3d (  w,  h,  - l);
		glVertex3d (  w,   h,   - l);


	glEnd();
*/

	glBegin(GL_QUADS);
	glColor3d(r * 1.5, g * 1.5, b * 1.5);
	glVertex3d(w + lb, h, l);
	glVertex3d(w + lb, h, -l);
	glVertex3d(w * 3.0 + lb, h, -l);
	glVertex3d(w * 3.0 + lb, h, l);

	//glVertex3d(w * 2.0 + lb, h, l);
	//glVertex3d(w * 2.0 + lb, h, -l);
	//glVertex3d(w * 4.0 + lb, h, -l);
	//glVertex3d(w * 4.0 + lb, h, l);

	glEnd();
}  //DrawBoxpipe

 //
 // рисование осей
 //
void DrawAxes (double dAxisSize)
{
	glPolygonMode (GL_FRONT, GL_FILL);
	glPolygonMode (GL_BACK, GL_LINE);
	gluQuadricDrawStyle (g_pGluQuadObj, GLU_FILL);
	glDisable (GL_DEPTH_TEST);

	glBegin (GL_LINES);
		glColor3d (1, 0, 0);
		glVertex3d (- dAxisSize / 2, 0, 0);
		glVertex3d (dAxisSize, 0, 0);
		glColor3d (0, 1, 0);
		glVertex3d (0, - dAxisSize / 2, 0);
		glVertex3d (0, dAxisSize, 0);
		glColor3d (0, 0, 1);
		glVertex3d (0, 0, - dAxisSize / 2);
		glVertex3d (0, 0, dAxisSize);
	glEnd();

	glPushMatrix();
		glColor3d (1, 0, 0);
		glTranslated (dAxisSize, 0, 0);
		glRotated (90, 0, 1, 0);
		gluCylinder (g_pGluQuadObj, dAxisSize / 10, 0, dAxisSize / 5, 32, 1);
	glPopMatrix();

	glPushMatrix();
		glColor3d (0, 1, 0);
		glTranslated (0, dAxisSize, 0);
		glRotated (-90, 1, 0, 0);
		gluCylinder (g_pGluQuadObj, dAxisSize / 10, 0, dAxisSize / 5, 32, 1);
	glPopMatrix();

	glPushMatrix();
		glColor3d (0, 0, 1);
		glTranslated (0, 0, dAxisSize);
		gluCylinder (g_pGluQuadObj, dAxisSize / 10, 0, dAxisSize / 5, 32, 1);
	glPopMatrix();

}  //DrawAxes

 //
 // рисование
 //
void Draw()
{
	double maxWH = g_boxpipeProps.W;
	if (maxWH < g_boxpipeProps.H)
		maxWH = g_boxpipeProps.H;

	double minWHL = g_boxpipeProps.W;
	if (minWHL > g_boxpipeProps.H)
		minWHL = g_boxpipeProps.H;
	if (minWHL > g_boxpipeProps.L)
		minWHL = g_boxpipeProps.L;

	double maxWHL = g_boxpipeProps.L;
	if (maxWHL < g_boxpipeProps.W)
		maxWHL = g_boxpipeProps.W;
	if (maxWHL < g_boxpipeProps.H)
		maxWHL = g_boxpipeProps.H;

	GLsizei viewportSize = g_wndHeight; // g_wndWidth > wndHeight ? wndHeight : g_wndWidth;

	glViewport (0, 0, viewportSize, viewportSize);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable (GL_DEPTH_TEST);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	glOrtho (- g_sceneWidth / 2, g_sceneWidth / 2, - g_sceneWidth / 2, g_sceneWidth / 2, - g_sceneWidth / 2, g_sceneWidth / 2);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();

		glRotated (30., 1., 0., 0.);
		glRotated (g_angle, 0., 1., 0.);

		 // нарисовать трубу
		DrawBoxpipe();

		 // нарисовать оси
		DrawAxes (minWHL / 2);

	glPopMatrix();

	glViewport (viewportSize, 0, viewportSize, viewportSize);

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();

	double gap = maxWH / 2,
		   size = maxWH + maxWHL + gap;

	double scw2 = size / 2 * 1.5;
	glOrtho (- scw2, scw2, - scw2, scw2, - scw2, scw2);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	 // нарисовать проекции трубы

	double _05size = size / 2,
		   _05maxWH = maxWH / 2,
		   _05L = maxWHL / 2;
	
	glPushMatrix();
		glTranslated (- _05size + _05maxWH, _05size - _05maxWH, 0.);
		DrawBoxpipe(false);
		DrawAxes (minWHL / 2);
	glPopMatrix();

	glPushMatrix();
		glTranslated (- _05size + _05maxWH, _05size - maxWH - _05L - gap, 0.);
		glRotated (90., 1., 0., 0.);
		DrawBoxpipe();
		DrawAxes (minWHL / 2);
	glPopMatrix();

	glPushMatrix();
		glTranslated (- _05size + maxWH + _05L + 4 * gap, _05size - _05maxWH, 0.);
		glRotated (90., 0., 1., 0.);
		DrawBoxpipe();
		DrawAxes (minWHL / 2);
	glPopMatrix();

	glPushMatrix();
		glTranslated (- _05size + maxWH + _05L + 4 * gap, _05size - maxWH - _05L - gap, 0.);
		glRotated (30., 1., 0., 0.);
		glRotated (int (g_angle / 22.5) * 22.5, 0, -1., 0.);
		glScaled (0.6, 0.6, 0.6);
		DrawBoxpipe (false);
		DrawAxes (minWHL * 1 / 2);
	glPopMatrix();

	glFinish();
	SwapBuffers (g_hDC);
}  //Draw


LRESULT CALLBACK MainWindowProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_CREATE:
			return MainOnCreate (hwnd, (LPCREATESTRUCT)lParam);

		case WM_SIZE:
			return MainOnSize (LOWORD (lParam), HIWORD (lParam));

		case WM_PAINT:
			return MainOnPaint();

		case WM_DESTROY:
			return MainOnDestroy();

		case WM_TIMER:
			if ((g_angle = g_angle + g_angIncr) >= 360)
				g_angle -= 360;
			InvalidateRect (g_hWindow, nullptr, FALSE);
		break;

		default:
			return DefWindowProc (hwnd, msg, wParam, lParam);
	}
	return 0L;
}  //MainWindowProc

 //
 // WM_CREATE
 //
BOOL MainOnCreate (HWND hwnd, LPCREATESTRUCT p_cs)
{
	 // получить контекст окна
	 // (значение дескриптора не меняется, т.к. класс окна имеет стиль CS_OWNDC)
	g_hDC = GetDC (hwnd);

	 // установить формат пикселей
	SetPixelFormat (g_hDC);

	 // создать контекст воспроизведения OpenGL, сделать его текущим
	g_hGLRC = wglCreateContext (g_hDC);
	wglMakeCurrent (g_hDC, g_hGLRC);

	 // установить таймер
	SetTimer (hwnd, 0, 20, 0);

	 // создать объект OpenGL для рисования осей координат
	g_pGluQuadObj = gluNewQuadric();
	assert (g_pGluQuadObj);

	CreateDialog (g_hApp, MAKEINTRESOURCE (IDD_PROPS), hwnd, DlgProc);

	return TRUE;
}  //MainOnCreate

 //
 // WM_SIZE
 //
BOOL MainOnSize (int width, int height)
{
	g_wndWidth = width;
	g_wndHeight = height;
	return TRUE;
}

 //
 // WM_PAINT
 //
BOOL MainOnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint (g_hWindow, & ps);
	Draw();
	EndPaint (g_hWindow, & ps);
	return TRUE;
}


BOOL MainOnDestroy()
{
	gluDeleteQuadric (g_pGluQuadObj);
	g_pGluQuadObj = nullptr;

	 // удалить контекст воспроизведения OpenGL
	wglMakeCurrent (nullptr, nullptr);
	if (g_hGLRC)
		wglDeleteContext (g_hGLRC);

	PostQuitMessage (0);

	return TRUE;
}


void SetDlgItemReal (HWND hDlg, int id, double val)
{
	TCHAR buff [256];
	_stprintf (buff, _T("%g"), val);
	SetDlgItemText (hDlg, id, buff);
}

bool GetDlgItemReal (HWND hDlg, int id, double& val)
{
	TCHAR buff [512];
	GetDlgItemText (hDlg, id, buff, _countof (buff));

	LPTSTR err = nullptr;
	val = _tcstod (buff, & err);
	return nullptr == err;
}

INT_PTR CALLBACK DlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool s_fInit;

	switch (message)
	{
		case WM_INITDIALOG:
		{
			s_fInit = true;

			SetDlgItemReal (hDlg, IDC_ED_W, g_boxpipeProps.W);
			SetDlgItemReal (hDlg, IDC_ED_H, g_boxpipeProps.H);
			SetDlgItemReal (hDlg, IDC_ED_T, g_boxpipeProps.T);
			SetDlgItemReal (hDlg, IDC_ED_L, g_boxpipeProps.L);
			SetDlgItemReal (hDlg, IDC_ED_LB, g_boxpipeProps.LB);

			for (int i = 0; i < _countof (g_colors); i++)
				SendDlgItemMessage (hDlg, IDC_LB_COLORS, LB_ADDSTRING, 0, (LPARAM) g_colors [i].szName);

			SendDlgItemMessage (hDlg, IDC_LB_COLORS, LB_SETCURSEL, g_boxpipeProps.iColor, 0);

			s_fInit = false;
		}
		return (INT_PTR)TRUE;

		case WM_COMMAND:
		{
			if (s_fInit)
				return (INT_PTR)FALSE;

			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}

			if (   HIWORD (wParam) == EN_CHANGE
				|| HIWORD (wParam) == LBN_SELCHANGE)
			{
				GetDlgItemReal (hDlg, IDC_ED_W, g_boxpipeProps.W);
				GetDlgItemReal (hDlg, IDC_ED_H, g_boxpipeProps.H);
				GetDlgItemReal (hDlg, IDC_ED_T, g_boxpipeProps.T);
				GetDlgItemReal (hDlg, IDC_ED_L, g_boxpipeProps.L);
				GetDlgItemReal (hDlg, IDC_ED_LB, g_boxpipeProps.LB);

				g_boxpipeProps.iColor = SendDlgItemMessage (hDlg, IDC_LB_COLORS, LB_GETCURSEL, 0, 0);

				g_boxpipeProps.CorrectValues();

				InvalidateRect (g_hWindow, nullptr, FALSE);
			}
		}
		break;

		case WM_CLOSE:
			DestroyWindow (g_hWindow);
		break;
	}
	return (INT_PTR)FALSE;
}


BOOL InitApp()
{
	WNDCLASSEX wce;
	ZeroMemory (& wce, sizeof(WNDCLASSEX));
	wce.cbSize        = sizeof(WNDCLASSEX);
	wce.hInstance     = g_hApp;
	wce.style         = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wce.lpfnWndProc   = MainWindowProc;
	wce.hCursor       = LoadCursor (nullptr, IDC_ARROW);
	wce.hbrBackground = (HBRUSH) GetStockObject (NULL_BRUSH);
	wce.lpszClassName = g_szWndClass;
	if (! RegisterClassEx (& wce))
		return FALSE;

	SetLastError(0);
	g_hWindow = CreateWindow(g_szWndClass, g_szTitle,
							WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
							| WS_THICKFRAME | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
							50, 50, 900, 400, nullptr, nullptr, g_hApp, nullptr);
	if (! g_hWindow)
	{
		DWORD err = GetLastError();
		return FALSE;
	}

	ShowWindow (g_hWindow, SW_SHOW);
	UpdateWindow (g_hWindow);

	return TRUE;
}


void UninitApp()
{
	UnregisterClass (g_szWndClass, g_hApp);
}


int APIENTRY WinMain (HINSTANCE hApp_, HINSTANCE, LPSTR, int)
{
	g_hApp = hApp_;

	if (InitApp())
	{
		MSG msg;
		while (GetMessage (& msg, nullptr, 0, 0))
		{
			TranslateMessage (& msg);
			DispatchMessage (& msg);
		}
	}

	UninitApp();

	return 0;
}

  // --- main.cpp ---