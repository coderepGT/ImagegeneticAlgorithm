#include <windows.h>  
#include <stdlib.h>
#include <tchar.h> 
#include <objidl.h>
#include <gdiplus.h>
#include <time.h>      
#include <cmath> 
#include <iostream>  
#include <string>

#pragma comment (lib, "Gdiplus.lib")

using namespace Gdiplus;

int Width, Height;
int Radius = 0;
int DullWorkCount = 0;

int ColorDiff(Gdiplus::Color color1, Gdiplus::Color color2)
{
	int r1 = color1.GetR();
	int g1 = color1.GetG();
	int b1 = color1.GetB();
	int r2 = color2.GetR();
	int g2 = color2.GetG();
	int b2 = color2.GetB();
	return abs(r1 - r2) + abs(g1 - g2) + abs(b1 - b2);
}

void GenPicture(Bitmap* SourceBitmap, Bitmap* ResultBitmap)
{
	int randx = rand() % Width;
	int randy = rand() % Height;
	int r = rand()% Radius;
	ARGB NewColor = Gdiplus::Color::MakeARGB(255, rand() % 256, rand() % 256, rand() % 256);
	long int SourceDiff = 0;
	long int ResultDiff = 0;
	for (int i =-r; i < r; i++)
		for (int j =-r; j < r; j++)
		{
			int x = randx + i;
			int y = randy + j;
			if (x > 0 && x < Width && y > 0 && y < Height)
			{
				double dist = i*i + j*j;
				if(dist <= r*r)
				{
					Gdiplus::Color SourceColor, ResultColor;
					SourceBitmap->GetPixel(x, y, &SourceColor);
					ResultBitmap->GetPixel(x, y, &ResultColor);
					SourceDiff += ColorDiff(SourceColor, NewColor);
					ResultDiff += ColorDiff(ResultColor, SourceColor);
				}
			}
		}
	if (SourceDiff < ResultDiff)
	{
		for (int i = -r; i <= r; i++)
			for (int j = -r; j <= r; j++)
			{
				int x = randx + i;
				int y = randy + j;
				if (x > 0 && x < Width && y > 0 && y < Height)
				{
					double dist = i*i + j*j;
					if (dist <= r*r)
						ResultBitmap->SetPixel(x, y, NewColor);
				}
			}
		DullWorkCount = 0;
	}
	else
	{
		DullWorkCount++;
	}
};

Gdiplus::Bitmap* bitmap;
Gdiplus::Bitmap* genbitmap;

LRESULT CALLBACK WndProc(
	_In_ HWND hWnd,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (uMsg)
	{
	case WM_PAINT:
		{
			if (bitmap != NULL)
			{
				SolidBrush  brush(Color(255, 255, 0, 0));
				FontFamily  fontFamily(L"Times New Roman");
				Font        font(&fontFamily, 24, FontStyleRegular, UnitPixel);
				PointF      pointF(0.0f, 0.0f);
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				Gdiplus::Graphics graphics(hdc);
				graphics.DrawImage(bitmap, 0, 0, Width, Height);
				for (;;)
				{
					for( int i = 0; i <20; i++)
						graphics.DrawImage(genbitmap, Width, 0, Width, Height);
					GenPicture(bitmap, genbitmap);
					if (DullWorkCount > 200)
					{
						if (Radius > 3)
						{
							Radius--;
							DullWorkCount = 0;
						}
						else
							break;
					}
				}
				EndPaint(hWnd, &ps);
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}
	return 0;
};

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow)
{
	Gdiplus::GdiplusStartupInput gdiplusInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusInput, NULL);

	static TCHAR szWindowClass[] = _T("GAApp");
	static TCHAR szTitle[] = _T("Genetic Algorithm");

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	HWND hWnd = CreateWindow(
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Win32 Guided Tour"),
			NULL);

		return 1;
	}

	//bitmap = new Bitmap(L"F:\\Images\\1.jpg");
	bitmap = new Bitmap(L"1.jpg");
	Width = bitmap->GetWidth();
	Height = bitmap->GetHeight();
	Radius = Width > Height ? Height / 8 : Width / 8;
	if (Width == 0 || Height == 0)
		return 0;
	SetWindowPos(hWnd, 0, 0, 0, 2 * Width, Height + 39, 0);
	genbitmap = new Bitmap(Width, Height);
	Gdiplus::Color color(0, 0, 0);
	for (int i = 0; i < genbitmap->GetWidth(); i++)
		for (int j = 0; j < genbitmap->GetHeight(); j++)
		{
			genbitmap->SetPixel(i, j, color);
		}
	srand(time(NULL));

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
	return (int)msg.wParam;
}