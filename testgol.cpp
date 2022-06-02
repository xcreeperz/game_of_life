#include <Windows.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "resource.h"
using namespace std;

//��������
HMENU MakeMenu(HWND hwnd);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

bool cell[44][82] = {0};//�����ʼϸ������

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR szCmdLine, _In_ int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("������Ϸ");
	HWND	hwnd;
	MSG		msg;
	//���ڳ�ʼ��
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(99));//����ͼ��
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = szAppName;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))//����ע��
	{
		MessageBox(NULL, TEXT("�������"), szAppName, MB_ICONERROR);
		return 0;
	}
	//��������
	hwnd = CreateWindow(szAppName, TEXT("������Ϸ"), WS_OVERLAPPEDWINDOW | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_SYSMENU, 100, 100, 1280, 720, NULL, NULL, hInstance, NULL);

	//��ʾ����
	ShowWindow(hwnd, iCmdShow);
	// ���´���
	UpdateWindow(hwnd);

	// ������Ϣ�������
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	// �˳����ڣ�������Ϣ��Windows
	return msg.wParam;

}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HMENU hMenu;

	//���ݾ��
	HDC hdc, hdcMem;
	hdc = GetDC(hwnd);

	static HBITMAP dead, live, blank;
	static int wxc, wyc;

	//���ڵ�ʵ�����
	HINSTANCE hInstance;
	int mouseX, mouseY;

	PAINTSTRUCT ps;
	
	const int BL = 20;	//��λ������ƶ�����(���ӱ߳�)

	int numx, numy;		//������±�

	bool temp[44][82];	//��ʱ�����ж�����

	int xx, yy, cnt;	//ϸ���бꡢ�бꡢ��Χϸ������

	static int dymax, dxmax;	//��ǰ���ڳߴ�����߳�

	static int generation, population;	//�������˿�
	RECT rect;

	switch (message)
	{
	case WM_CREATE:
		//MessageBeep(1);//������
		
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;//ȡ��ʵ�����

		dead = LoadBitmap(hInstance, MAKEINTRESOURCE(101));
		live = LoadBitmap(hInstance, MAKEINTRESOURCE(103));
		blank = LoadBitmap(hInstance, MAKEINTRESOURCE(113));
		
		//�˵���ʼ��
		hMenu = MakeMenu(hwnd);
		SetMenu(hwnd, hMenu);
		MessageBox(hwnd, TEXT("����һ���������Ϸ���ڶ�ά���������У�ÿ���������һ��ϸ������Ϊ������Ϊ����һ��ϸ������һ��������ȡ�������ڰ˸������л�ϸ����������������ڷ����ϸ���������࣬���ϸ����������Դ�ѷ����෴�����ϸ�������ڹµ���\n��Ϸ����ҿ���ͨ���������ı�ϸ����״̬��\n�������һ������ʹ����仯һ�֡�\n�������ʼ�����潫����1��/����ٶȸı䣬�������ͣ��������ͣ���棬������������ջ��档"), TEXT("������Ϸ-Game of Life | V1.1.4 | By �ŷ��"), MB_OK); break;
		break;
	
	case WM_SIZE:
		wxc = LOWORD(lParam);
		wyc = HIWORD(lParam);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
	
		//�����ڴ���豸����������ֵ��һ���ڴ��豸�����ľ��
		hdcMem = CreateCompatibleDC(hdc);
		//��һ������ѡ���豸�����У�����Ϊ���豸���������������
		GetClientRect(hwnd, &rect);
		dymax = (rect.bottom - rect.top) / BL + 3;
		dxmax = (rect.right - rect.left) / BL + 3;

		for (numy = 1; BL * numy < wyc; numy++)
		{
			for (numx = 1; BL * numx < wxc; numx++)
			{
				if (cell[numy + 1][numx + 1] == 0)		//�������ϸ������ǰλ����Ҹ�
				{
						SelectObject(hdcMem, dead);
						BitBlt(hdc, BL * (numx - 1), BL * (numy - 1), BL, BL, hdcMem, 0, 0, SRCCOPY);
				}
				else {					//����ǻ�ϸ������ǰλ�����̸�
						SelectObject(hdcMem, live);
						BitBlt(hdc, BL * (numx - 1), BL * (numy - 1), BL, BL, hdcMem, 0, 0, SRCCOPY);
				}
				if (numx == wxc / BL || numy == wyc / BL)	//��������һ��±߽磬�ⲿ��Ϊ�հ�
				{
					SelectObject(hdcMem, blank);
					BitBlt(hdc, BL * numx, BL * (numy - 1), BL, BL, hdcMem, 0, 0, SRCCOPY);
					BitBlt(hdc, BL * (numx - 1), BL * numy, BL, BL, hdcMem, 0, 0, SRCCOPY);
					BitBlt(hdc, BL * numx, BL * (numy), BL, BL, hdcMem, 0, 0, SRCCOPY);
				}
			}
		}
		DeleteDC(hdcMem);
		//���ƴ��ڽ���
		EndPaint(hwnd, &ps);
		break;

	case WM_LBUTTONDOWN:

		mouseX = (LOWORD(lParam));
		mouseY = (HIWORD(lParam));

			mouseX = ((int)(mouseX) / BL) + 1;
			mouseY = ((int)(mouseY) / BL) + 1;
			cell[mouseY + 1][mouseX + 1] = !cell[mouseY + 1][mouseX + 1];

			GetClientRect(hwnd, &rect);//����Ҫ���µ�����
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
		//����������ȡ��������ȡ��

	case WM_ERASEBKGND:		//��ֹ����
		return TRUE;

	case WM_GETMINMAXINFO:
		MINMAXINFO* sizeinfo;
		sizeinfo = (PMINMAXINFO)lParam;
		//������С�ߴ�
		sizeinfo->ptMinTrackSize.x = 640;
		sizeinfo->ptMinTrackSize.y = 360;

		//�������ߴ�
		sizeinfo->ptMaxTrackSize.x = 1600;
		sizeinfo->ptMaxTrackSize.y = 900;

		break;
	case WM_COMMAND:
		switch (wParam)
		{
			//���ڴ�С��
		case 114://С����
			SetWindowPos(hwnd, NULL, NULL, NULL, 640, 360, SWP_NOZORDER | SWP_NOMOVE); break;
		case 115://�д���
			SetWindowPos(hwnd, NULL, NULL, NULL, 1280, 720, SWP_NOZORDER | SWP_NOMOVE); break;
		case 116://�󴰿�
			SetWindowPos(hwnd, NULL, NULL, NULL, 1600, 900, SWP_NOZORDER | SWP_NOMOVE); break;
		case 103://Ĭ�ϣ��д���
			SetWindowPos(hwnd, NULL, NULL, NULL, 1280, 720, SWP_NOZORDER | SWP_NOMOVE); break;

			//��Ϸ������
		case 104://�������
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

		case 105://��ʼ
			SetTimer(hwnd, 105, 1000, NULL);
			generation = 0;
			break;
		case 106://����
			generation = 0;		//��������
			population = 0;		//�˿�����
			KillTimer(hwnd, 105);
			for (yy = 1; yy < 43; yy++)
			{
				for (xx = 1; xx < 81; xx++)
				{
					cell[yy][xx] = 0;
				}
			}
			GetClientRect(hwnd, &rect);		//ȫѡ����
			InvalidateRect(hwnd, &rect, true);	//ˢ�»���
			break;
		case 107://��ͣ
			KillTimer(hwnd, 105);
			break;

			//������Ϣ��
		case 109://
			MessageBox(hwnd, TEXT("����һ���������Ϸ���ڶ�ά���������У�ÿ���������һ��ϸ������Ϊ������Ϊ����һ��ϸ������һ��������ȡ�������ڰ˸������л�ϸ����������������ڷ����ϸ���������࣬���ϸ����������Դ�ѷ����෴�����ϸ�������ڹµ���\n��Ϸ����ҿ���ͨ���������ı�ϸ����״̬��\n�������һ������ʹ����仯һ�֡�\n�������ʼ�����潫����1��/����ٶȸı䣬�������ͣ��������ͣ���棬������������ջ��档"), TEXT("����"), MB_OK); break;
		case 108:
			MessageBox(hwnd, TEXT("���⣺������Ϸ-Game of Life\n���ߣ��ŷ��\n�汾��1.1.4"), TEXT("���ڳ���"), MB_OK); break;
		case 110:
			MessageBox(hwnd, TEXT("�ð������"), TEXT("����"), MB_OK);
			break;
		
		//�ڲ�����
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
					//������Ƶ�ǰ�˿�
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

		case 666://��ʾ����
			WCHAR buf[10];
			wsprintf(buf, TEXT("%d"), generation);
			MessageBox(hwnd, buf, TEXT("����"), MB_OK);
			break;
			
		case 777://��ʾ�˿�
			WCHAR buff[10];
			wsprintf(buff, TEXT("%d"), population);
			MessageBox(hwnd, buff, TEXT("ϸ����"), MB_OK);
			break;
		}
		break;
	
	case WM_TIMER://��������
		SendMessage(hwnd, WM_COMMAND, 233, NULL);
		break;
	
	case WM_DESTROY:
		//�رմ��ڣ��˳���Ϣѭ��
		PostQuitMessage(0);
		return 0;
	}
	
	//������Ϣ����Ĭ�ϴ�����
	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//�˵�����ʼ��
HMENU MakeMenu(HWND hwnd)
{
	HMENU hMenu = CreateMenu();
	HMENU hPopMenu = CreateMenu();

	HINSTANCE hInstance = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);

	AppendMenu(hPopMenu, MF_STRING, 114, TEXT("С"));
	AppendMenu(hPopMenu, MF_STRING, 115, TEXT("��"));
	AppendMenu(hPopMenu, MF_STRING, 116, TEXT("��"));
	AppendMenu(hPopMenu, MF_STRING, 103, TEXT("Ĭ��"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopMenu, TEXT("���ڴ�С"));

	hPopMenu = CreateMenu();
	//AppendMenu(hPopMenu, MF_STRING, 104, TEXT("�������"));		//��ʱ��ûʵ��
	AppendMenu(hPopMenu, MF_STRING, 105, TEXT("��ʼ"));
	AppendMenu(hPopMenu, MF_STRING, 107, TEXT("��ͣ"));
	AppendMenu(hPopMenu, MF_STRING, 106, TEXT("����"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopMenu, TEXT("��Ϸ����"));

	hPopMenu = CreateMenu();
	AppendMenu(hPopMenu, MF_STRING, 666, TEXT("����"));
	AppendMenu(hPopMenu, MF_STRING, 777, TEXT("ϸ����"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopMenu, TEXT("ͳ��"));
	
	hPopMenu = CreateMenu();
	AppendMenu(hMenu, MF_STRING, 233, TEXT("��һ��(&N)"));

	hPopMenu = CreateMenu();
	AppendMenu(hPopMenu, MF_STRING, 108, TEXT("����(&A)"));
	AppendMenu(hPopMenu, MF_STRING, 109, TEXT("����(&I)"));
	AppendMenu(hPopMenu, MF_STRING, 110, TEXT("����(&S)"));
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hPopMenu, TEXT("���ڳ���"));

	return hMenu;
}