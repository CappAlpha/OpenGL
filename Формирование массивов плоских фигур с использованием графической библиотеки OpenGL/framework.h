#pragma once

#define STRICT

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <gl\gl.h>
#include <gl\\glu.h>
// #include <gl\\glaux.h>
#pragma comment (linker, "/defaultlib:opengl32.lib")
#pragma comment (linker, "/defaultlib:glu32.lib")


#define MIN(a,b) ((a)>=(b) ? (b) : (a))
#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MsgBox(str) MessageBox(hwndMain, str, aczAppName, 0L)
#define COLUMNS 5
#define ROWS    5


char aczAppName[512] = "GlBaseSamp";
LPCSTR pczMainWndClass = "WC__FrameWnd";

HINSTANCE hiApp;
HWND hwndMain;
HDC hdcMain;
HGLRC hGlRc;

// область вывода в оконных координатах
RECT rcViewport;

//  Размер области вывода в модельных (габаритный куб)
# define DIM_SCENE 1000.0 
int scale = COLUMNS > ROWS ? DIM_SCENE * 0.95 / COLUMNS : DIM_SCENE * 0.95 / ROWS;
int columns = COLUMNS;
int rows = ROWS;
double beta = 3.14 / 2;

void InitOpenGL();
void UninitOpenGL();
void InitViewport(const int cx, const int cy);
void Draw();
void DrawModel(void);
LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);
void MainOnPaint(HWND);
int RegisterWndClasses(void);

//Все, что связано с фигурой и ее отрисовкой
struct MYPOINT
{
	double x;
	double y;
};
class Element
{
	int vertexCount;
	double figureRadius1;
	double figureRadius2;
	double alpha = 3.14 / 2;
	COLORREF figureColor;

public:
	Element();
	Element(int count, double radius1, double radius2, COLORREF color);


	double GetFigureRadius(int n);
	// метод для изменения внешнего радиуса
	// метод для изменения внутреннего радиуса
	// метод для изменения количества ребер
	// метод для изменения цвета фигуры

	// метод для изменения поворота
	void RotateFigure(double coef);
	// метод для вывода объекта на экран
	void PrintElement(HDC hdc, int x, int y);
	void PrintElement(double scene, double x, double y);
	//метод для вывода объекта в файл
};
Element::Element(int count, double radius1, double radius2, COLORREF color)
{
	vertexCount = count;
	figureRadius1 = radius1;
	figureRadius2 = radius2;
	figureColor = color;
}
double Element::GetFigureRadius(int n)
{
	return n * this->figureRadius2;
}
void Element::PrintElement(HDC hdc, int x, int y)
{
	POINT* m = new POINT[vertexCount];
	double n = (360.0 / vertexCount) * (3.14 / 180.0);
	for (int i = 0; i < vertexCount; i++)
	{
		m[i].x = figureRadius1 * cos(n * i - alpha) + x;
		m[i].y = figureRadius1 * sin(n * i - alpha) + y;
	}
	HPEN pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	HBRUSH brush = CreateSolidBrush(figureColor);
	SelectObject(hdc, pen);
	SelectObject(hdc, brush);
	Polygon(hdc, m, vertexCount);


	for (int i = 0; i < vertexCount; i++)
	{
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, m[i].x, m[i].y);
		m[i].x = figureRadius2 * cos(n * i - alpha) + x;
		m[i].y = figureRadius2 * sin(n * i - alpha) + y;
		Ellipse(hdc, m[i].x - figureRadius1 / 5,
			m[i].y - figureRadius1 / 5,
			m[i].x + figureRadius1 / 5,
			m[i].y + figureRadius1 / 5);
		MoveToEx(hdc, m[i].x, m[i].y, NULL);
		if (i != 0)
			LineTo(hdc, m[i - 1].x, m[i - 1].y);
		if (i == vertexCount - 1)
		{
			MoveToEx(hdc, m[i].x, m[i].y, NULL);
			LineTo(hdc, m[0].x, m[0].y);
		}
	}

	DeleteObject(brush);
	DeleteObject(pen);
	delete []m;
}
void Element::PrintElement(double scene, double x, double y)
{
	//габаритный коэфф.
	double k_gab = 0.95;

	
	// нарисовать габарит области  вывода
	glColor3d(1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex2d(-DIM_SCENE * k_gab, -DIM_SCENE * k_gab);
		glVertex2d(DIM_SCENE * k_gab, -DIM_SCENE * k_gab);
		glVertex2d(DIM_SCENE * k_gab, DIM_SCENE * k_gab);
		glVertex2d(-DIM_SCENE * k_gab, DIM_SCENE * k_gab);
		glVertex2d(-DIM_SCENE * k_gab, -DIM_SCENE * k_gab);
	glEnd();

	// нарисовать внутренний многоугольник
	glBegin(GL_POLYGON);
	MYPOINT* m = new MYPOINT[vertexCount];
	double n = (360.0 / vertexCount) * (3.14 / 180.0);
	for (int i = 0; i < vertexCount; i++)
	{
		m[i].x = figureRadius1 * cos(n * i - alpha) + x;
		m[i].y = figureRadius1 * sin(n * i - alpha) + y;
		glColor3d((i + 1) / vertexCount, 1, 0);
		glVertex2d(m[i].x, m[i].y);
	}
	glEnd();

	// нарисовать внешний многоугольник с линиями
	glColor3d(1, 1, 1);

	for (int i = 0; i < vertexCount; i++)
	{
		glBegin(GL_LINES);
			glVertex2d(x, y);
			glVertex2d(m[i].x, m[i].y);
		glEnd();
		glColor3d(1, 1, 1);
		glBegin(GL_POINTS);
			m[i].x = figureRadius1 * cos(n * i - alpha) + x;
			m[i].y = figureRadius1 * sin(n * i - alpha) + y;
			glVertex2d(m[i].x, m[i].y);
		glEnd();

		glColor3d(rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1), rand() / ((float)RAND_MAX + 1));
		
		glBegin(GL_LINES);
			glVertex2d(m[i].x, m[i].y);
		if (i != 0)
			glVertex2d(m[i - 1].x, m[i - 1].y);
		glEnd();

		if (i == vertexCount - 1)
		{
			glColor3d(1, 1, 1);
			glBegin(GL_LINES);
			glVertex2d(m[i].x, m[i].y);
			glVertex2d(m[0].x, m[0].y);
			glEnd();
		}
	}
	delete []m;
}
void Element::RotateFigure(double coef)
{
	alpha += coef / 240;
}

//Объявление и инициализация объекта
Element element = Element(10, scale*0.7, scale, RGB(180, 100, 80));
Element ellipse = Element(10, 10, 10, RGB(180, 100, 80));

// инициализация контекста воспроизведения OpenGL
void InitOpenGL()
{
	hdcMain = GetDC(hwndMain);
	int iPixelFormat;
	// инициализация структуры формат пикселей
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  // размер структуры
		1,                              // номер версии OpenGL
		PFD_DRAW_TO_WINDOW |          // поддержка вывода в окно
		PFD_SUPPORT_OPENGL |          // поддержка OpenGL
		PFD_DOUBLEBUFFER,		// двойная буферизация
		PFD_TYPE_RGBA,          // цвета в режиме RGBA
		24,                     // 24 разряда на цвет
		0, 0, 0, 0, 0, 0,       // биты цвета игнорируются
		0,                      // не используется альфа-параметр
		0,                      // смещение цветов игнорируются
		0,                      // буфер аккумулятора не используется
		0, 0, 0, 0,             // биты аккумулятора игнорируются
		32,                     // 32-разрядный буфер глубины
		0,                      // буфер трафарета не используется
		0,                      // вспомогательный буфер не используется
		PFD_MAIN_PLANE,         // основной слой
		0,                      // зарезервирован
		0, 0, 0                 // маски слоя игнорируются
	};
	// подбор pix-формата
	iPixelFormat = ChoosePixelFormat(hdcMain, &pfd);
	if (!iPixelFormat)
		MsgBox("Неверный формат пикселей");
	// установка pix-формата
	if (!SetPixelFormat(hdcMain, iPixelFormat, &pfd))
		MsgBox("Формат пикселей не установлен");
	// создание OpenGL-контекста
	hGlRc = wglCreateContext(hdcMain);
	if (!hGlRc)
		MsgBox("Контекст OpenGL не создан");
	// установка текущего OpenGL-контекста
	if (!wglMakeCurrent(hdcMain, hGlRc))
		MsgBox("Контекст OpenGL не установлен");
}  // func InitOpenGL

 // освобождение контекста воспроизведения
void UninitOpenGL()
{
	// сбросить текущий контекст воспроизведения OpenGL
	wglMakeCurrent(NULL, NULL);
	// удаленить контекста воспроизведения OpenGL
	if (hGlRc)
		wglDeleteContext(hGlRc);
	// освобождение контекста рабочей области
	ReleaseDC(hwndMain, hdcMain);
}  //func UninitOpenGL

 // инициализация области вывода
void InitViewport(const int cx,
	const int cy)
{
	// задать область вывода OpenGL по наименьшему размеру окна
	
	int viewSize = MIN(cx, cy) - 20;
	rcViewport.left = (cx - viewSize) / 2;
	rcViewport.top = (cy - viewSize) / 2;
	rcViewport.right = rcViewport.left + viewSize;
	rcViewport.bottom = rcViewport.top + viewSize;
	glMatrixMode(GL_PROJECTION);
	glViewport(rcViewport.left,
		cy - rcViewport.bottom,
		viewSize, viewSize);
	glLoadIdentity();
	glOrtho(-DIM_SCENE, DIM_SCENE, -DIM_SCENE, DIM_SCENE, -DIM_SCENE, DIM_SCENE);
	glMatrixMode(GL_MODELVIEW);
}  //func InitViewport

 // вывод модели
void DrawModel()
{
	// установить чёрный цвет фона
	glClearColor(0, 0, 0, 1.0);

	// габаритный коэфф.
	double k_gab = 0.95;

	// очистить буфер цвета
	glClear(GL_COLOR_BUFFER_BIT);

	// установить текущий цвет (красный)
	glColor3d(1, 0, 0);
	// нарисовать орезок по диагонали через всю область вывода
	glBegin(GL_LINES);
	glVertex2d(-DIM_SCENE * 0.575, -DIM_SCENE * 0.575);
	glVertex2d(DIM_SCENE * 0.775, DIM_SCENE * 0.775);
	glEnd();

	// нарисовать габарит области  вывода
	glColor3d(1, 1, 1);
	glBegin(GL_LINE_LOOP);
		glVertex2d(-DIM_SCENE * k_gab, -DIM_SCENE * k_gab);
		glVertex2d(DIM_SCENE * k_gab, -DIM_SCENE * k_gab);
		glVertex2d(DIM_SCENE * k_gab, DIM_SCENE * k_gab);
		glVertex2d(-DIM_SCENE * k_gab, DIM_SCENE * k_gab);
		glVertex2d(-DIM_SCENE * k_gab, -DIM_SCENE * k_gab);
	glEnd();

	/*
	// нарисовать треугольник с интерполяцией цветов
	glBegin(GL_TRIANGLES);
		glColor3d(1, 0, 0);
		glVertex2d(DIM_SCENE * 0.8, DIM_SCENE * 0.05);
		glColor3d(0, 1, 0);
		glVertex2d(-DIM_SCENE * 0.82, 2*DIM_SCENE * 0.89);
		glColor3d(0, 0, 1);
		glVertex2d(DIM_SCENE * 0.72, DIM_SCENE * 0.71);
	glEnd();
	// нарисовать точки
	glColor3d(1, 1, 0);
	glPointSize(4);
	glBegin(GL_POINTS);
		glVertex2d(DIM_SCENE * 0.025, -DIM_SCENE * 0.025);
	glEnd();
	glColor3d(1, 0, 1);
	glPointSize(6);
	glBegin(GL_POINTS);
		glVertex2d(DIM_SCENE * 0.05, -DIM_SCENE * 0.05);
	glEnd();
	glColor3d(0, 1, 1);
	glPointSize(12);
	glBegin(GL_POINTS);
		glVertex2d(DIM_SCENE * 0.075, -DIM_SCENE * 0.075);
	glEnd();
	*/
	// дождаться завершения растеризации заданных примитивов
	// во вторичном буфере
	glFinish();
	// скопировать растр из вторичного буфера в основной
	SwapBuffers(hdcMain);
}  // func DrawModel

 // процедура главного окна
LRESULT WINAPI MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_SIZE:
		InitViewport(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_PAINT:
		MainOnPaint(hwnd);
		break;
	case WM_MOUSEWHEEL:
		if (LOWORD(wParam)==MK_CONTROL)
		{
			columns += 1 * GET_WHEEL_DELTA_WPARAM(wParam)/120;
			rows += 1 * GET_WHEEL_DELTA_WPARAM(wParam)/120;
			scale = columns > rows ? DIM_SCENE * 0.95 / columns : DIM_SCENE * 0.95 / rows;
			element = Element(10, scale * 0.7, scale, RGB(180, 100, 80));
			InvalidateRect(hwnd, &rcViewport, FALSE);
		}
		else
		{
			element.RotateFigure((int)wParam / 1189);
			InvalidateRect(hwnd, &rcViewport, FALSE);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0L;
}  // func MainWndProc

 // обработчик WM_PAINT
void MainOnPaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	BeginPaint(hwnd, &ps);
	glClearColor(0, 0, 0, 1.0);
	// очистить буфер цвета
	glClear(GL_COLOR_BUFFER_BIT);

	//рисование массива

	glBegin(GL_LINES);
	glVertex2d(360.0 * beta,0);
	glVertex2d(240.0 * beta, 240.0 * beta);
	glBegin(GL_LINES);
	glVertex2d(240.0 * beta, 240.0 * beta);
	glVertex2d(0, 360.0 * beta);
	glBegin(GL_LINES);
	glVertex2d(0, 360.0 * beta);
	glVertex2d(-1.0 * 240.0 * beta, 240.0 * beta);
	glBegin(GL_LINES);
	glVertex2d(-1.0 * 360.0 * beta, 0);
	glVertex2d(-1.0 * 240.0 * beta, 240.0 * beta);
	glBegin(GL_LINES);
	glVertex2d(-1.0 * 360.0 * beta, 0);
	glVertex2d(-1.0 * 240.0 * beta, -1.0 * 240.0 * beta);
	glBegin(GL_LINES);
	glVertex2d(-1.0 * 240.0 * beta, -1.0 * 240.0 * beta);
	glVertex2d(0, -1.0 * 360.0 * beta);
	glBegin(GL_LINES);
	glVertex2d(240.0 * beta, -1.0 * 240.0 * beta);
	glVertex2d(0, -1.0 * 360.0 * beta);
	glBegin(GL_LINES);
	glVertex2d(360.0 * beta, 0);
	glVertex2d(240.0 * beta, -1.0 * 240.0 * beta);
	glEnd();

		element.PrintElement(DIM_SCENE,
			240.0 * beta, //x
			240.0 * beta); //y

		element.PrintElement(DIM_SCENE,
			360.0 * beta, //x
			0); //y

		element.PrintElement(DIM_SCENE,
			0, //x
			360.0 * beta); //y

		element.PrintElement(DIM_SCENE,
			-1.0 * 240.0 * beta, //x
			240.0 * beta); //y

		element.PrintElement(DIM_SCENE,
			-1.0 * 360.0 * beta, //x
			0); //y

		element.PrintElement(DIM_SCENE,
			-1.0 * 240.0 * beta, //x
			-1.0 * 240.0 * beta); //y

		element.PrintElement(DIM_SCENE,
			0, //x
			-1.0 * 360.0 * beta); //y

		element.PrintElement(DIM_SCENE,
			240.0 * beta, //x
			-1.0 * 240.0 * beta); //y

	glFinish();
	SwapBuffers(hdcMain);
	//DrawModel();
	EndPaint(hwnd, &ps);
}  //func MainOnPaint

 // регистрация классов окон
int RegisterWndClasses()
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.hInstance = hiApp;
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	// wc.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	//wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU);
	wc.lpszClassName = pczMainWndClass;
	if (!RegisterClassEx(&wc))
		return 0;
	return 1;
}  // func RegisterWndClasses

 // инициализация приложения
void InitializeApp()
{
	// зарегистрировать класс и создать главное окно
	RegisterWndClasses();
	hwndMain = CreateWindow(pczMainWndClass, NULL,
		WS_CAPTION | WS_VISIBLE | WS_SYSMENU | WS_THICKFRAME |
		WS_OVERLAPPED | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		0, 0, 800, 600, NULL,
		NULL, hiApp, NULL);
	if (!hwndMain)
		return;
	// инициализировать OpenGL
	InitOpenGL();
	// выбрать кисть и перо для обводки области вывода
	SelectObject(hdcMain, GetStockObject(NULL_BRUSH));
	SelectObject(hdcMain, GetStockObject(WHITE_PEN));
	//
	RECT r;
	GetClientRect(hwndMain, &r);
	InitViewport(r.right, r.bottom);

	
}  //func InitializeApp

 // освобождение ресурсов приложения
void UninitializeApp()
{
	UnregisterClass(pczMainWndClass, hiApp);
}  //func UninitializeApp

 // точка входа приложения
int APIENTRY WinMain(HINSTANCE hi, HINSTANCE hi_void, LPSTR pc_cl, int wsm)
{
	hiApp = hi;
	InitializeApp();
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
		DispatchMessage(&msg);
	UninitializeApp();
	return 0;
}  //func WinMain
