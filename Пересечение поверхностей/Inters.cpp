#define STRICT
#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#include <gl\gl.h>
#include <gl\\glu.h>
#include "resource.h"

#include <tchar.h>

using namespace std;

#pragma comment (linker, "/defaultlib:opengl32.lib")
#pragma comment (linker, "/defaultlib:glu32.lib")

HINSTANCE hApp = nullptr;
HWND hwnd = nullptr;
HDC hdc = nullptr;
HGLRC hglrc = nullptr;
GLUquadricObj* g_pGluQuadObj = nullptr;

LPCTSTR pc_WindowClassName = _T("WindowClass"),
        pc_wndTitle = _T("Пересечение поверхностей тора и конуса");

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

void Init(int, int);
RECT rcClip;

#define PI 3.14159265

double fAngViewX = 30.0,
fAngViewY = 45.0;

int stPosX, stPosY;
int g_wndW, g_wndH;

BOOL InitializeApp(void);
void UninitializeApp(void);
void UnregisterWndClasses(void);

BOOL CreateWindows(void);
BOOL RegisterClassaOkna(void);

LRESULT CALLBACK MainWindowProc(HWND, UINT, WPARAM, LPARAM);
BOOL SetPixelFormat(HDC dc);
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void Draw(HDC hdc);
void SetLight();

BOOL MainOnCreate(HWND);
BOOL MainOnDestroy(HWND);
BOOL MainOnPaint(HWND);
BOOL MainOnSize(HWND, int, int);
BOOL MainOnLButtonDown(HWND , int , int );
BOOL MainOnMouseMove(HWND , int , int , UINT );

struct Point
{
	double x;
	double y;
	double z;
};

class Cone
{
private:
	double x0, y0, z0;
	double R;
public:
	void SetX(double X)
	{
		x0 = X;
	}
	void SetY(double Y)
	{
		y0 = Y;
	}
	void SetZ(double Z)
	{
		z0 = Z;
	}

	vector<Point> vector_points;
	vector<Point> Initializing()
	{
		int chastota = 10;
		Point intermediate_point;
		double x, y, z;
		int level_of_detail = 10;
		for (int i = -chastota * level_of_detail; i < chastota * level_of_detail; i++)
		{
			for (int j = -chastota * level_of_detail; j < chastota * level_of_detail; j++)
			{
				x = i;
				y = j;
				x /= (level_of_detail * 2);
				y /= (level_of_detail * 2);
				z = sqrt(pow(x, 2) + pow(y, 2)) * R;
				if (abs(z) - R <= 0)
				{
					intermediate_point.x = x + x0;
					intermediate_point.y = y + y0;
					intermediate_point.z = -z + z0;
					vector_points.insert(vector_points.end(), intermediate_point);

				}
			}
		}
		return vector_points;
	}
	void Draw()
	{
		glPushMatrix();
		glTranslated(x0, y0, z0 - 5);		
		gluCylinder(g_pGluQuadObj, 5 / 5, 0, 5, 15, 15);
		glPopMatrix();
	}
	Cone(double x00, double y00, double z00, double R0)
	{
		x0 = x00;
		y0 = y00;
		z0 = z00;
		R = R0;
	}
};

class IntersactionObjects
{
private:
	struct C3dVector
	{
		double x, y, z;
	};
public:
	vector<Point> vector_points;
	vector<Point>  Intersection(vector<Point> vector1, vector<Point> vector2)
	{
		Point intermediate_point;
		double tochnost = 0.035;
		for (int i = 0; i < vector1.size(); i++)
			for (int j = 0; j < vector2.size(); j++)
			{
				if ((abs(vector1[i].x - vector2[j].x) < tochnost) && (abs(vector1[i].y - vector2[j].y) < tochnost) && (abs(vector1[i].z - vector2[j].z) < tochnost))
				{
					intermediate_point.x = vector2[j].x;
					intermediate_point.y = vector2[j].y;
					intermediate_point.z = vector2[j].z;

					vector_points.insert(vector_points.end(), intermediate_point);

					intermediate_point.x = vector1[i].x;
					intermediate_point.y = vector1[i].y;
					intermediate_point.z = vector1[i].z;
					vector_points.insert(vector_points.end(), intermediate_point);

				}

			}
		return vector_points;
	}
	void Draw(vector<Point> vector_points)
	{
		for (int i = 0; i < vector_points.size(); i++)
		{
			glPushMatrix();
			glTranslated(vector_points[i].x, vector_points[i].y, vector_points[i].z);
			gluSphere(g_pGluQuadObj, 0.2, 16, 16);
			glPopMatrix();
		}
	}
	IntersactionObjects(){}
};

class Torus
{
private:
	double x0, y0, z0, r_max, r_min, n;
	struct C3dVector
	{
		double x, y, z;
	};
public:
	void SetX(double X)
	{
		x0 = X;
	}
	void SetY(double Y)
	{
		y0 = Y;
	}
	void SetZ(double Z)
	{
		z0 = Z;
	}
	vector<Point> vector_points;
	vector<Point> Initializing()
	{
		int chastota = 15;
		Point intermediate_point;
		double x, y, z;
		int level_of_detail = 15;
		for (int i = -chastota * level_of_detail; i < chastota * level_of_detail; i++)
		{
			for (int j = -chastota * level_of_detail; j < chastota * level_of_detail; j++)
			{
				x = i;
				y = j;
				x /= (level_of_detail * 2);
				y /= (level_of_detail * 2);
				z = sqrt(sqrt(4 * pow(r_max, 2) * ((pow(x, 2)) + (pow(y, 2)))) - pow(x, 2) - pow(y, 2) - pow(r_max, 2) + pow(r_min, 2));

				intermediate_point.x = x + x0;
				intermediate_point.y = y + y0;
				intermediate_point.z = z + z0;

				vector_points.insert(vector_points.end(), intermediate_point);
				intermediate_point.z = -z + z0;

				vector_points.insert(vector_points.end(), intermediate_point);
			}
		}
		return vector_points;
	}
	void Draw()
	{
		GLfloat         x1, x2, x3, x4,
			y1, y2, y3, y4,
			z1, z2, z3, z4, xn, yn, zn,
			fi, fi_max, dr, dr1, DFI, ln;

		C3dVector  v1, v2, v3, v12, v13, vn[10];

		int  i;

		glPushMatrix();
		glTranslated(x0, y0, z0);
		glFrontFace(GL_CCW);

		DFI = 2 * PI / n;

		glBegin(GL_QUADS);

		for (fi_max = -PI; fi_max < PI; fi_max += DFI)
		{
			for (fi = -PI; fi < PI; fi += DFI)
			{
				dr = r_min * cos(fi);
				dr1 = r_min * cos(fi + DFI);

				x1 = (r_max + dr) * cos(fi_max);      x2 = (r_max + dr1) * cos(fi_max);
				x3 = (r_max + dr1) * cos(fi_max + DFI);  x4 = (r_max + dr) * cos(fi_max + DFI);

				y1 = (r_max + dr) * sin(fi_max);      y2 = (r_max + dr1) * sin(fi_max);
				y3 = (r_max + dr1) * sin(fi_max + DFI);  y4 = (r_max + dr) * sin(fi_max + DFI);

				z1 = r_min * sin(fi);    z2 = r_min * sin(fi + DFI);
				z3 = z2;               z4 = z1;

				vn[0].x = x1; vn[0].y = y1; vn[0].z = z1;
				vn[1].x = x2; vn[1].y = y2; vn[1].z = z2;
				vn[2].x = x3; vn[2].y = y3; vn[2].z = z3;
				vn[3].x = x4; vn[3].y = y4; vn[3].z = z4;

				for (i = 0; i < 4; i++)
				{
					ln = sqrt(vn[i].x * vn[i].x + vn[i].y * vn[i].y);

					xn = vn[i].x - r_max * (vn[i].x / ln);
					yn = vn[i].y - r_max * (vn[i].y / ln);
					zn = vn[i].z;

					glNormal3f(xn, yn, zn);

					glVertex3f(vn[i].x, vn[i].y, vn[i].z);
				}
			}
		}
		glEnd();

		glFrontFace(GL_CW);
		glPopMatrix();
	}
	Torus(double x00, double y00, double z00, double Rmax, double Rmin, int n0)
	{
		x0 = x00;
		y0 = y00;
		z0 = z00;
		r_max = Rmax;
		r_min = Rmin;
		n = n0;
	}
};

Cone cone = Cone(0, 0, 0, 5);
Torus torus = Torus(0, 4, 0, 6, 1, 32);
//IntersactionObjects intersactionObjects;
double x_cone = 6, y_cone = 3, z_cone = 4;
double x_torus = 0, y_torus = 0, z_torus = 0;
IntersactionObjects intersactionObjects = IntersactionObjects();

vector<Point> vector_points;

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

BOOL MainOnCreate(HWND hwnd)
{
	hdc = GetDC(hwnd);
	SetPixelFormat(hdc);
	hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);
	g_pGluQuadObj = gluNewQuadric();
	assert(g_pGluQuadObj);

	CreateDialog(hApp, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DlgProc);

	return TRUE;
}

BOOL MainOnSize(HWND hwnd, int g_wndW_, int g_wndH_)
{
	g_wndW = g_wndW_;
	g_wndH = g_wndH_;
	Init(g_wndW, g_wndH);
	return TRUE;
}

BOOL MainOnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	Draw(hdc);
	EndPaint(hwnd, &ps);
	return TRUE;
}

void Draw(HDC hdc)
{
	glClearColor(0.5, 0.5, 0.5, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glEnable(GL_LIGHTING);

	SetLight();

	glRotatef(fAngViewX, 1.0f, 0.0f, 0.0f);
	glRotatef(fAngViewY, 0.0f, 1.0f, 0.0f);
	glRotatef(1, 0.0f, 0.0f, 1.0f);

	glLineWidth(1.0f);

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);

	float sphere_mat[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, sphere_mat);
	cone.Draw();

	float torus_mat[] = { 0.0f, 1.0f, 1.0f, 0.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, torus_mat);
	torus.Draw();

	float intersaction_mat[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, intersaction_mat);
	intersactionObjects.Draw(intersactionObjects.vector_points);

	glDisable(GL_LIGHTING);
	glFinish();
	SwapBuffers(hdc);
}

void SetLight()
{
	GLfloat lightPos[] = { -1.0, 1.0, 1.0, 0.0 };
	GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	glEnable(GL_LIGHT0);
}

void Init(int ww, int wh)
{
	glFrontFace(GL_CW);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int viewportSize = ww / 2;
	rcClip.left = ww / 2 - ww / 4;
	rcClip.bottom = wh / 2 + viewportSize / 2;

	rcClip.right = rcClip.left + viewportSize;
	rcClip.top = rcClip.bottom - viewportSize;

	glViewport(rcClip.left, rcClip.top,
		viewportSize, viewportSize);
	double Ortho = 10;
	glOrtho(-Ortho, Ortho, -Ortho, Ortho, -Ortho, Ortho);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool GetDlgItemReal(HWND hDlg, int id, double& val)
{
	TCHAR buff[512];
	GetDlgItemText(hDlg, id, buff, _countof(buff));

	LPTSTR err = nullptr;
	val = _tcstod(buff, &err);
	return nullptr == err;
}

void SetDlgItemReal(HWND hDlg, int id, double val)
{
	TCHAR buff[256];
	_stprintf_s(buff, _T("%g"), val);
	SetDlgItemText(hDlg, id, buff);
}

INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool s_fInit;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		s_fInit = 1;

		SetDlgItemReal(hDlg, IDC_CONE_X, x_cone);
		SetDlgItemReal(hDlg, IDC_CONE_Y, y_cone);
		SetDlgItemReal(hDlg, IDC_CONE_Z, z_cone);
		SetDlgItemReal(hDlg, IDC_TORUS_X, x_torus);
		SetDlgItemReal(hDlg, IDC_TORUS_Y, y_torus);
		SetDlgItemReal(hDlg, IDC_TORUS_Z, z_torus);

		s_fInit = 0;
		break;
	}
	case WM_COMMAND:
	{

	    if (s_fInit == 1)
		GetDlgItemReal(hDlg, IDC_CONE_X, x_cone);
		GetDlgItemReal(hDlg, IDC_CONE_Y, y_cone);
		GetDlgItemReal(hDlg, IDC_CONE_Z, z_cone);
		GetDlgItemReal(hDlg, IDC_TORUS_X, x_torus);
		GetDlgItemReal(hDlg, IDC_TORUS_Y, y_torus);
		GetDlgItemReal(hDlg, IDC_TORUS_Z, z_torus);
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_UPDATE:
		{
			cone.SetX(x_cone);
			cone.SetY(y_cone);
			cone.SetZ(z_cone);

			torus.SetX(x_torus);
			torus.SetY(y_torus);
			torus.SetZ(z_torus);

			intersactionObjects.vector_points.clear();
			cone.vector_points.clear();
			torus.vector_points.clear();
			intersactionObjects.vector_points = intersactionObjects.Intersection(cone.Initializing(), torus.Initializing());
			InvalidateRect(hwnd, nullptr, TRUE);
			break;
		}
		}
		InvalidateRect(hwnd, nullptr, FALSE);
		break;
	}
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL InitializeApp()
{
	if (!RegisterClassaOkna())
	{
		MessageBox(nullptr, "Ошибка регистрации классов окон", pc_wndTitle, MB_OK);
	}
	CreateWindows();
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	return TRUE;
}

void UninitializeApp()
{
	UnregisterWndClasses();
}

void UnregisterWndClasses()
{
	UnregisterClass(pc_WindowClassName, hApp);
}

BOOL CreateWindows()
{
	hwnd = CreateWindow(pc_WindowClassName,
		pc_wndTitle,
		WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX |
		WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN,
		0,
		0,
		800,
		600,
		nullptr,
		nullptr,
		hApp,
		0
	);
	hdc = GetDC(hwnd);

	return TRUE;
}

BOOL RegisterClassaOkna(void)
{
	WNDCLASSEX wce_main;
	memset(&wce_main, 0, sizeof(WNDCLASSEX));
	wce_main.cbSize = sizeof(WNDCLASSEX);
	wce_main.hInstance = hApp;
	wce_main.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC | CS_DBLCLKS;
	wce_main.lpfnWndProc = (WNDPROC)MainWindowProc;
	wce_main.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wce_main.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wce_main.lpszClassName = pc_WindowClassName;

	return RegisterClassEx(&wce_main);
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		return MainOnCreate(hwnd);
	case WM_SIZE:
		return MainOnSize(hwnd, LOWORD(lParam), HIWORD(lParam));
	case WM_LBUTTONDOWN:
		return MainOnLButtonDown(hwnd, LOWORD(lParam), HIWORD(lParam));
	case WM_MOUSEMOVE:
		return MainOnMouseMove(hwnd, LOWORD(lParam), HIWORD(lParam), wParam);
	case WM_PAINT:
		return MainOnPaint(hwnd);
	case WM_DESTROY:
		return MainOnDestroy(hwnd);
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0L;
}

BOOL MainOnMouseMove(HWND hwnd_, int x, int y, UINT flags)
{
	if (!((MK_LBUTTON | MK_RBUTTON) & flags))
		return FALSE;

	if (MK_LBUTTON == flags)
	{
		float fDAng = 180.0f * (x - stPosX) / g_wndW;
		fAngViewY += fDAng;
		if (fAngViewY > 360.0f)
			fAngViewY -= 360.0f;
		if (fAngViewY < -360.0f)
			fAngViewY += 360.0f;
		fDAng = 180.0f * (y - stPosY) / g_wndH;
		fAngViewX += fDAng;
		if (fAngViewX > 360.0f)
			fAngViewX -= 360.0f;
		if (fAngViewX < -360.0f)
			fAngViewX += 360.0f;
		stPosX = x;
		stPosY = y;
	}
	else
	{
		stPosX = x;
		stPosY = y;
	}

	InvalidateRect(hwnd_, nullptr, FALSE);
	return TRUE;
}

BOOL MainOnLButtonDown(HWND hwnd, int x, int y)
{
	stPosX = x;
	stPosY = y;
	return TRUE;
}

BOOL MainOnDestroy(HWND hwnd)
{
	if (g_pGluQuadObj)
	{
		gluDeleteQuadric(g_pGluQuadObj);
		g_pGluQuadObj = nullptr;
	}

	wglMakeCurrent(nullptr, nullptr);

	if (hglrc)	wglDeleteContext(hglrc);

	ReleaseDC(hwnd, hdc);

	PostQuitMessage(0);
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hi_crr, HINSTANCE, LPSTR, int)
{
	MSG msg;

	InitializeApp();
	BOOL return_;
	while (return_ = GetMessage(&msg, nullptr, 0, 0) != 0)
	{
		if (return_ == -1)
		{
			MessageBox(hwnd, "return_ = -1", "Ошибка", MB_ICONSTOP);
			return 0;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	UninitializeApp();

	return msg.wParam;
}
