#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "resource.h"
using namespace std;

//函数声明
HMENU MakeMenu(HWND hwnd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

bool cell[44][82] = {0};//定义初始细胞数组

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR szCmdLine, _In_ int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("生命游戏");
	HWND	hwnd;
	MSG		msg;
	//窗口初始化
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(99));//窗口图标
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = szAppName;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))//窗口注册
	{
		MessageBox(NULL, TEXT("这合理吗？"), szAppName, MB_ICONERROR);
		return 0;
	}
	//创建窗口
	hwnd = CreateWindow(szAppName, TEXT("生命游戏"), WS_OVERLAPPEDWINDOW | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_SYSMENU, 100, 100, 1280, 720, NULL, NULL, hInstance, NULL);

	//显示窗口
	ShowWindow(hwnd, iCmdShow);
	// 更新窗口
	UpdateWindow(hwnd);

	// 启动消息处理机制
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	// 退出窗口，返回消息到Windows
	return msg.wParam;

}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HMENU hMenu;

	//内容句柄
	HDC hdc, hdcMem;
	hdc = GetDC(hwnd);

	static HBITMAP dead, live, blank;
	static int wxc, wyc;

	//窗口的实例句柄
	HINSTANCE hInstance;
	int mouseX, mouseY;

	PAINTSTRUCT ps;
	
	const int BL = 20;	//单位方格的移动距离(格子边长)

	int numx, numy;		//数组的下标

	bool temp[44][82];	//临时拷贝判定数组

	int xx, yy, cnt;	//细胞列标、行标、周围细胞计数

	static int dymax, dxmax;	//当前窗口尺寸的最大边长

	static int generation, population;	//代数、人口
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		//MessageBeep(1);//听个响
		
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;//取得实例句柄

		dead = LoadBitmap(hInstance, MAKEINTRESOURCE(101));
		live = LoadBitmap(hInstance, MAKEINTRESOURCE(103));
		blank = LoadBitmap(hInstance, MAKEINTRESOURCE(113));
		
		//菜单初始化
		hMenu = MakeMenu(hwnd);
		SetMenu(hwnd, hMenu);
		MessageBox(hwnd, TEXT("这是一个零玩家游戏。在二维矩形世界中，每个方格代表一个细胞，绿为生，灰为死。一个细胞在下一个刻生死取决于相邻八个方格中活细胞的数量。如果相邻方格活细胞数量过多，这个细胞会死于资源匮乏；相反，这个细胞会死于孤单。\n游戏中玩家可以通过鼠标左击改变细胞的状态。\n点击“下一步”会使画面变化一轮。\n点击“开始”画面将会以1轮/秒的速度改变，点击“暂停”可以暂停画面，“结束”会清空画面。"), TEXT("生命游戏-Game of Life | V1.1.4 | By 张峰睿"), MB_OK); break;
		break;
	
	case WM_SIZE:
		wxc = LOWORD(lParam);
		wyc = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
	
		//创建内存的设备环境，返回值是一个内存设备环境的句柄
		hdcMem = CreateCompatibleDC(hdc);
		//将一个对象选择到设备环境中，参数为：设备环境句柄，对象句柄
		GetClientRect(hwnd, &rect);
		dymax = (rect.bottom - rect.top) / BL + 3;
		dxmax = (rect.right - rect.left) / BL + 3;

		for (numy = 1; BL * numy < wyc; numy++)
		{
			for (numx = 1; BL * numx < wxc; numx++)
			{
				if (cell[numy + 1][numx + 1] == 0)		//如果是死细胞，当前位置填灰格
				{
						SelectObject(hdcMem, dead);
						BitBlt(hdc, BL * (numx - 1), BL * (numy - 1), BL, BL, hdcMem, 0, 0, SRCCOPY);
				}
				else {					//如果是活细胞，当前位置填绿格
						SelectObject(hdcMem, live);
						BitBlt(hdc, BL * (numx - 1), BL * (numy - 1), BL, BL, hdcMem, 0, 0, SRCCOPY);
				}
				if (numx == wxc / BL || numy == wyc / BL)	//如果到了右或下边界，外部填为空白
				{
					SelectObject(hdcMem, blank);
					BitBlt(hdc, BL * numx, BL * (numy - 1), BL, BL, hdcMem, 0, 0, SRCCOPY);
					BitBlt(hdc, BL * (numx - 1), BL * numy, BL, BL, hdcMem, 0, 0, SRCCOPY);
					BitBlt(hdc, BL * numx, BL * (numy), BL, BL, hdcMem, 0, 0, SRCCOPY);
				}
			}
		}
		DeleteDC(hdcMem);
		//绘制窗口结束
		EndPaint(hwnd, &ps);
		break;

	case WM_LBUTTONDOWN:

		mouseX = (LOWORD(lParam));
		mouseY = (HIWORD(lParam));

			mouseX = ((int)(mouseX) / BL) + 1;
			mouseY = ((int)(mouseY) / BL) + 1;
			cell[mouseY + 1][mouseX + 1] = !cell[mouseY + 1][mouseX + 1];

			GetClientRect(hwnd, &rect);//设置要更新的区域
			InvalidateRect(hwnd, &rect, true);
			
			population = 0;
			for (yy = 1; yy < 43; yy++)
			{
				for (xx = 1; xx < 81; xx++)
				{
					if (cell[yy][xx] == 1)
					{
						population++;
					}
				}
			}
		return 0;
		//鼠标左键，获取坐标向下取整

	case WM_ERASEBKGND:		//防止闪屏
		return TRUE;

	case WM_GETMINMAXINFO:
		MINMAXINFO* sizeinfo;
		sizeinfo = (PMINMAXINFO)lParam;
		//窗口最小尺寸
		sizeinfo->ptMinTrackSize.x = 640;
		sizeinfo->ptMinTrackSize.y = 360;

		//窗口最大尺寸
		sizeinfo->ptMaxTrackSize.x = 1600;
		sizeinfo->ptMaxTrackSize.y = 900;

		break;
	case WM_COMMAND:
		switch (wParam)
		{
			//窗口大小栏
		case 114://小窗口
			SetWindowPos(hwnd, NULL, NULL, NULL, 640, 360, SWP_NOZORDER | SWP_NOMOVE); break;
		case 115://中窗口
			SetWindowPos(hwnd, NULL, NULL, NULL, 1280, 720, SWP_NOZORDER | SWP_NOMOVE); break;
		case 116://大窗口
			SetWindowPos(hwnd, NULL, NULL, NULL, 1600, 900, SWP_NOZORDER | SWP_NOMOVE); break;
		case 103://默认（中窗口
			SetWindowPos(hwnd, NULL, NULL, NULL, 1280, 720, SWP_NOZORDER | SWP_NOMOVE); break;

			//游戏运行栏
		case 104://随机生成
			//SendMessage(hwnd, WM_LBUTTONDOWN, NULL, lParam);
			//int mnt;
			//srand(time(0));
			//mnt = (int)(100 * rand()) % 100;
			//int i, rnx, rny;
			///*mouseX = (LOWORD(lParam));
			//mouseY = (HIWORD(lParam));
			//mouseX = ((int)(mouseX) / BL);
			//mouseY = ((int)(mouseY) / BL);*/
			//for (i = 1; i < mnt; i++)
			//{
			//	srand(time(0));
			//	rnx = (int)(100 * rand()) % dxmax;
			//	srand(time(0));
			//	rny = (int)(100 * rand()) % dymax;
			//	cell[rny + 1][rnx + 1] = 1;
			//}
			//GetClientRect(hwnd, &rect);
			//InvalidateRect(hwnd, &rect, true);
			//break;

		case 105://开始
			SetTimer(hwnd, 105, 1000, NULL);
			generation = 0;
			break;
		case 106://结束
			generation = 0;		//轮数清零
			population = 0;		//人口清零
			KillTimer(hwnd, 105);
			for (yy = 1; yy < 43; yy++)
			{
				for (xx = 1; xx < 81; xx++)
				{
					cell[yy][xx] = 0;
				}
			}
			GetClientRect(hwnd, &rect);		//全选窗口
			InvalidateRect(hwnd, &rect, true);	//刷新画面
			break;
		case 107://暂停
			KillTimer(hwnd, 105);
			break;

			//程序信息栏
		case 109://
			MessageBox(hwnd, TEXT("这是一个零玩家游戏。在二维矩形世界中，每个方格代表一个细胞，绿为生，灰为死。一个细胞在下一个刻生死取决于相邻八个方格中活细胞的数量。如果相邻方格活细胞数量过多，这个细胞会死于资源匮乏；相反，这个细胞会死于孤单。\n游戏中玩家可以通过鼠标左击改变细胞的状态。\n点击“下一步”会使画面变化一轮。\n点击“开始”画面将会以1轮/秒的速度改变，点击“暂停”可以暂停画面，“结束”会清空画面。"), TEXT("介绍"), MB_OK); break;
		case 108:
			MessageBox(hwnd, TEXT("标题：生命游戏-Game of Life\n作者：张峰睿\n版本：1.1.4"), TEXT("关于程序"), MB_OK); break;
		case 110:
			MessageBox(hwnd, TEXT("用爱发电√"), TEXT("赞助"), MB_OK);
			break;
		
		//内部命令
		case 233:
			population = 0;
			for (yy = 1; yy < dymax; yy++)
			{
				for (xx = 1; xx < dxmax; xx++)
				{
					temp[yy][xx] = cell[yy][xx];
				}
			}
			for (yy = 1; yy < dymax; yy++)
			{
				for (xx = 1; xx < dxmax; xx++)
				{
					cnt = temp[yy - 1][xx - 1] + temp[yy - 1][xx] + temp[yy - 1][xx + 1] + temp[yy][xx - 1] + temp[yy][xx + 1] + temp[yy + 1][xx - 1] + temp[yy + 1][xx] + temp[yy + 1][xx + 1];
					if (cnt < 2)
					{
						cell[yy][xx] = 0;
					}
					else if (cnt == 3)
					{
						cell[yy][xx] = 1;
					}
					else if (cnt > 3)
					{
						cell[yy][xx] = 0;
					}
					//结束后计当前人口
					if (cell[yy][xx] == 1)
					{
						++population;
					}
				}
			}
			generation++;
			GetClientRect(hwnd, &rect);
			InvalidateRect(hwnd, &rect, true);
			break;

		case 666://显示轮数
			WCHAR buf[10];
			wsprintf(buf, TEXT("%d"), generation);
			MessageBox(hwnd, buf, TEXT("轮数"), MB_OK);
			break;
			
		case 777://显示人口
			WCHAR buff[10];
			wsprintf(buff, TEXT("%d"), population);
			MessageBox(hwnd, buff, TEXT("细胞数"), MB_OK);
			break;
		}
		break;
	
	case WM_TIMER://持续运行
		SendMessage(hwnd, WM_COMMAND, 233, NULL);
		break;
	
	case WM_DESTROY:
		//关闭窗口，退出消息循环
		PostQuitMessage(0);
		return 0;
	}
	
	//其他消息交给默认处理函数
	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//菜单栏初始化
HMENU MakeMenu(HWND hwnd)
{
	HMENU hMenu = CreateMenu();
	HMENU hPopMenu = CreateMenu();

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);

	AppendMenu(hPopMenu, MF_STRING, 114, TEXT("小"));
	AppendMenu(hPopMenu, MF_STRING, 115, TEXT("中"));
	AppendMenu(hPopMenu, MF_STRING, 116, TEXT("大"));
	AppendMenu(hPopMenu, MF_STRING, 103, TEXT("默认"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopMenu, TEXT("窗口大小"));

	hPopMenu = CreateMenu();
	//AppendMenu(hPopMenu, MF_STRING, 104, TEXT("随机生成"));		//暂时还没实现
	AppendMenu(hPopMenu, MF_STRING, 105, TEXT("开始"));
	AppendMenu(hPopMenu, MF_STRING, 107, TEXT("暂停"));
	AppendMenu(hPopMenu, MF_STRING, 106, TEXT("结束"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopMenu, TEXT("游戏运行"));

	hPopMenu = CreateMenu();
	AppendMenu(hPopMenu, MF_STRING, 666, TEXT("轮数"));
	AppendMenu(hPopMenu, MF_STRING, 777, TEXT("细胞数"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopMenu, TEXT("统计"));
	
	hPopMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, 233, TEXT("下一步(&N)"));

	hPopMenu = CreateMenu();
	AppendMenu(hPopMenu, MF_STRING, 108, TEXT("关于(&A)"));
	AppendMenu(hPopMenu, MF_STRING, 109, TEXT("介绍(&I)"));
	AppendMenu(hPopMenu, MF_STRING, 110, TEXT("赞助(&S)"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopMenu, TEXT("关于程序"));

	return hMenu;
}