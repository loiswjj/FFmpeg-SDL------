//Header 里面包含了相应的头文件以及全局变量的定义
#include "Header.h"

//相应函数的声明
void OpenVideoFile(HWND hWnd);
void PlayVideoFile(int iDevID);
void PauseVideoFile(int iDevID);
void StopVideoFile(int iDevID);
void ChangeVoice(int iDevID, int value);
void ChangeWard(int iDevID, int nMinute, int nSecond);
void ChangeBrightness(int iDevID, int bright);
unsigned long getmovietime(int iDevID);
unsigned long getnowposition(int iDevID);
void Paint(HWND hWnd, LPCWSTR lpBitmap);
void Display(HDC hdc,int Length,int Width);

//建立一个窗口需要定义相关的WNDPROC
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MainDialogProc(HWND, UINT, WPARAM, LPARAM);
HWND CreateListView(HWND hWnd);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("videoplayer"),szAppName2[10];
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, szAppName);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = CreateSolidBrush(RGB(54, 54, 54));
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	hInst = hInstance;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Need Windows NT"), szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName,
		TEXT("RgnWindow Demo"),
		WS_POPUP,
		cxScreen / 15,
		cyScreen / 8,
		cxScreen / 1.2,
		cyScreen * 3 / 4,
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//窗口消息处理
LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMessage, WPARAM wParam, LPARAM lParam)
{
	static HWND hbutton[15],hScroll[2],hstatic;
	static HBRUSH hBrush,hOldBrush;
	static UINT nHitTest;
	static TCHAR szBuffer[10] = TEXT("00:00:00"), szBuffer2[100] = TEXT("00:00:00");

	PAINTSTRUCT ps;
    HDC hdc;
	POINT pt;
	RECT rcClient;
	HWND hWndParent;

	static int npos;
	static int id,Count;
	int Flag = 0,num;
	static unsigned  long  Length, npos2 = 0, position;
	unsigned long  hour, minute, second;

	TCHAR Buffer[100], szBuffer3[100] = TEXT("00:00:00"),buffer[100] = TEXT("00:00:00");
	LPDRAWITEMSTRUCT pDI = (LPDRAWITEMSTRUCT)lParam;
	LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT)lParam;
	HFONT   hFont;
	LOGFONT   log;

	proc->Init();

	//Length = getmovietime(g_MciDevId);

	switch(uiMessage)
	{
	//button、滚动条、列表初始化
	case WM_CREATE:
		for (num = 0; num <= 13; num++)
		{
			hbutton[num] = CreateWindow(TEXT("button"), 0, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				100, 0, 50, 20, hWnd, (HMENU)num, hInst, NULL);
		}
		hstatic = CreateWindow(TEXT("static"), 0, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			100, 0, 50, 20, hWnd, (HMENU)15, hInst, NULL);

		proc->hVideo = CreateWindow(TEXT("static"), 0, WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			0, 30, 0, 0, hWnd, (HMENU)100, hInst, NULL);

		hScroll[0] = CreateWindow(TEXT("scrollbar"), 0, WS_CHILD | WS_VISIBLE | SBS_HORZ,
			0, 0, 150, 20, hWnd, (HMENU)11, hInst, NULL);
		SetScrollRange(hScroll[0], SB_CTL, 0, 1000, FALSE);    //设置滚动条的范围在0-100
		SetScrollPos(hScroll[0], SB_CTL,500, FALSE);          //设置初始值为0

		hScroll[1] = CreateWindow(TEXT("scrollbar"), 0, WS_CHILD | WS_VISIBLE | SBS_HORZ,
			0, 20, 150, 20, hWnd, (HMENU)13, hInst, NULL);     //进度条
		SetScrollRange(hScroll[1], SB_CTL, 0, 100, FALSE);    //设置滚动条的范围在0-100
		SetScrollPos(hScroll[1], SB_CTL, 0, FALSE);          //设置初始值为0

		hList = CreateListView(hWnd);
		break;

	//界面元素显示及button贴图
	case WM_PAINT:	
		hdc = BeginPaint(hWnd, &ps);
		//相应文字显示
		hBrush = CreateSolidBrush(RGB(0, 0, 0));
		SelectObject(hdc, hBrush);
		Rectangle(hdc, 0, Cy - 90, Cx, Cy);
		Rectangle(hdc, 0, 0, Cx, 30);
		DeleteObject(hBrush);
		Display(hdc, 20, 8);
		TextOut(hdc, 35, 5, TEXT("Bobo Video Player"), 17);
		TextOut(hdc, 90, Cy - 70, proc->TotalTime, lstrlen(proc->TotalTime));//显示总时长
		TextOut(hdc, 80, Cy - 70, TEXT("/"), 1);
		TextOut(hdc, 10, Cy - 70, proc->CurrentTime, lstrlen(proc->CurrentTime));//显示目前播放时长

		//窗口操作
		if (flag % 2 == 1)
		{
			Paint(hbutton[1], MAKEINTRESOURCE(IDB_BITMAP2));
		}
		if (flag % 2 == 0)
		{
			Paint(hbutton[1], MAKEINTRESOURCE(IDB_BITMAP1));
		}
		if (j % 2 == 1)
		{
			Paint(hbutton[7], MAKEINTRESOURCE(IDB_BITMAP8));	//最大化
		}
		else
		{
			Paint(hbutton[7], MAKEINTRESOURCE(IDB_BITMAP9));	//最大化
		}
		
		Paint(hstatic, MAKEINTRESOURCE(IDB_BITMAP18));		//播放器图标
		Paint(hbutton[0], MAKEINTRESOURCE(IDB_BITMAP13));	//设置
		Paint(hbutton[2], MAKEINTRESOURCE(IDB_BITMAP3));	//上一个
		Paint(hbutton[3], MAKEINTRESOURCE(IDB_BITMAP4));	//停止
		Paint(hbutton[4], MAKEINTRESOURCE(IDB_BITMAP5));	//下一个
		Paint(hbutton[5], MAKEINTRESOURCE(IDB_BITMAP6));	//打开文件
		Paint(hbutton[6], MAKEINTRESOURCE(IDB_BITMAP7));	//最小化
		Paint(hbutton[8], MAKEINTRESOURCE(IDB_BITMAP10));	//关闭
		Paint(hbutton[9], MAKEINTRESOURCE(IDB_BITMAP11));	//静音
		Paint(hbutton[10], MAKEINTRESOURCE(IDB_BITMAP12));	//最大音
		Paint(hbutton[11], MAKEINTRESOURCE(IDB_BITMAP14));	//循环播放
		Paint(hbutton[12], MAKEINTRESOURCE(IDB_BITMAP15));	//清空全部
		Paint(hbutton[13], MAKEINTRESOURCE(IDB_BITMAP16));	//删除单行
		EndPaint(hWnd, &ps);
		break;

	//设置控件位置大小
	case WM_SIZE:
		GetClientRect(hWnd, &ClientRect);
		Cx = ClientRect.right - ClientRect.left;
		Cy = ClientRect.bottom - ClientRect.top;
		MoveWindow(hstatic, 0, 0, 30, 30, TRUE);	//对话框
		MoveWindow(hbutton[0], 0.48*Cx+390, Cy - 52, 33, 33, TRUE);	//对话框
		MoveWindow(hbutton[1], 0.48*Cx, Cy-60, 45, 45, TRUE);	//播放
		MoveWindow(hbutton[2], 0.48*Cx - 45, Cy-52, 33, 33, TRUE);	//上一个
		MoveWindow(hbutton[3], 0.48*Cx - 88, Cy - 52, 33, 33, TRUE);	//停止
		MoveWindow(hbutton[4], 0.48*Cx + 57, Cy - 52, 33, 33, TRUE);	//下一个
		MoveWindow(hbutton[5], 0.48*Cx + 97, Cy - 52, 33, 33, TRUE);	//文件
		MoveWindow(hbutton[6], Cx - 100, 0, 30, 30, TRUE);					//最小化
		MoveWindow(hbutton[7], Cx - 65, 0, 30, 30, TRUE);					//最大化/恢复
		MoveWindow(hbutton[8], Cx - 30, 0, 30, 30, TRUE);					//关闭
		MoveWindow(hbutton[9], 0.48*Cx + 180, Cy - 47, 12.5, 20, TRUE);	//静音
		MoveWindow(hbutton[10], 0.48*Cx + 325, Cy - 47, 20, 20, TRUE);	//最大音
		MoveWindow(hScroll[0], 0.48*Cx + 200, Cy - 42, 120, 10, TRUE);  //音量调节
		MoveWindow(hScroll[1], 0, Cy - 90, Cx-270, 15, TRUE);  //进度调节
		MoveWindow(hList, Cx - 270, 30, 270, Cy-120, TRUE);	//列表框	
		MoveWindow(hbutton[11], Cx - 40, Cy - 90, 25, 25, TRUE);	//循环播放
		MoveWindow(hbutton[12], Cx - 80, Cy - 90, 25, 25, TRUE);	//清空全部
		MoveWindow(hbutton[13], Cx - 120, Cy - 90, 25, 25, TRUE);	//删除单行
		MoveWindow(proc->hVideo, 0, 30, Cx - 270, Cy - 120, TRUE);	//视频
		return 0;

	//窗口的缩放和移动
	case WM_NCHITTEST:
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ScreenToClient(hWnd, &pt);
		GetClientRect(hWnd, &rcClient);

		if (pt.x<rcClient.left + 20 && pt.y<rcClient.top + 20)//左上角,判断是不是在左上角，就是看当前坐标是不是即在左边拖动的范围内，又在上边拖动的范围内，其它角判断方法类似
		{
			return HTTOPLEFT;
		}
		else if (pt.x>rcClient.right - 20 && pt.y<rcClient.top + 20)//右上角
		{
			return HTTOPRIGHT;
		}
		else if (pt.x<rcClient.left + 20 && pt.y>rcClient.bottom - 20)//左下角
		{
			return HTBOTTOMLEFT;
		}
		else if (pt.x>rcClient.right - 20 && pt.y>rcClient.bottom - 20)//右下角
		{
			return HTBOTTOMRIGHT;
		}
		else if (pt.x<rcClient.left + 20)
		{
			return HTLEFT;
		}
		else if (pt.x>rcClient.right - 20)
		{
			return HTRIGHT;
		}
		else if (pt.y<rcClient.top + 20)
		{
			return HTTOP;
		}if (pt.y>rcClient.bottom - 20)
		{
			return HTBOTTOM;          //以上这四个是上、下、左、右四个边
		}
		else
		{
			return HTCAPTION;
		}
		break;

	//定时器
	/*case WM_TIMER:
		InvalidateRect(hWnd, 0, FALSE);
		//设置当前时间并以00:00:00的形式显示
		break;*/

	//控件消息处理
	case WM_COMMAND:
		switch (wParam)
		{
		case 0:	//播放器图标
			DialogBox(hInst, MAKEINTRESOURCE(IDD_MYDIALOG), hWnd, MainDialogProc);
			break;

		case 1:	//播放/暂停
			if (flag % 2 == 1)
			{
				proc->status = PAUSE;
				Paint(hbutton[1], MAKEINTRESOURCE(IDB_BITMAP1));
			}
			if (flag % 2 == 0)
			{
				proc->status = PLAY;
				Paint(hbutton[1], MAKEINTRESOURCE(IDB_BITMAP2));
			}
			flag++;
			break;

		case 2:	//上一个
			Index--;
			if (Index >= 0 && Index < total) {
				proc->szFileName = pBuffer_Directory[Index];
				proc->OpenVideoFile(proc->hVideo, m_streamstate, m_sdl);
				proc->EventProc(m_streamstate, m_sdl);
			}
			break;

		case 3:	//停止
			proc->status = QUIT;
			break;

		case 4:	//下一个
			Index++;
			if (Index >= 0 && Index < total) {
				proc->szFileName = pBuffer_Directory[Index];
				proc->OpenVideoFile(proc->hVideo, m_streamstate, m_sdl);
				proc->EventProc(m_streamstate, m_sdl);
			}
			break;

		case 5:	//打开文件
			OpenVideoFile(hWnd);
			break;

		case 6:	//最小化
			ShowWindow(hWnd, SW_MINIMIZE);
			break;

		case 7:	//最大化/恢复	
			j++;
			if (j % 2 ==0)
			{		
				ShowWindow(hWnd, SW_MAXIMIZE);
			}
			if (j % 2 == 1)
			{
				ShowWindow(hWnd, SW_RESTORE);
			}
			break;

		case 8:	//关闭
			PostQuitMessage(0);
			DestroyWindow(proc->hVideo);
			for (num = 0; num <= 13; num++)
			{
				DestroyWindow(hbutton[num]);
			}
			DestroyWindow(hScroll[0]);
			DestroyWindow(hScroll[1]);
			DestroyWindow(hstatic);
			DestroyWindow(hList);
			DestroyWindow(hWnd);
			break;

		case 9:	//静音
			SetScrollPos(hScroll[0], SB_CTL, 0, TRUE);
			break;

		case 10://最大音量
			SetScrollPos(hScroll[0], SB_CTL, 1000, TRUE);
			break;

		case 11://循环播放
			Count = 0;
			break;

		case 12://清空列表
			SendMessage(hList, LB_RESETCONTENT, 0, 0);
			break;

		case 13://删除单行
			SendMessage(hList, LB_DELETESTRING, id, 0);
			break;

		default:
			break;
		}
		break;

	//滚动条消息处理
	case WM_HSCROLL:
		if (GetWindowLong((HWND)lParam, GWL_ID) == 11)  //音量调节按钮
		{
			SetFocus(hScroll[0]);
			hWndParent = GetParent(hWnd);                   //父窗口的句柄
			switch (LOWORD(wParam))
			{
			case SB_PAGERIGHT:
				npos += 15;
				break;
			case SB_LINEDOWN:
				npos = min(1000, npos + 1);
				break;
			case SB_PAGELEFT:
				npos -= 15;
				break;
			case SB_LINELEFT:
				npos = max(0, npos - 1);
				break;
			case SB_LEFT:
				npos = 0;
				break;
			case SB_RIGHT:
				npos = 1000;
				break;
			case SB_THUMBTRACK:
				npos = HIWORD(wParam);
				//WM_VSCROLL的wParam参数的高字节为滚动条的位置，低字节为通知码
				break;
			}
			SetScrollPos(hScroll[0], SB_CTL, npos, TRUE);
			//ChangeVoice(g_MciDevId, npos);        //将音量调整至滚动条拖动的位置
		}
		else
		{	//进度调节
			Flag = 1;
			SetFocus(hScroll[1]);
			hWndParent = GetParent(hWnd);                   //父窗口的句柄
			switch (LOWORD(wParam))
			{
			case SB_PAGERIGHT:
				npos2 += 15;
				break;
			case SB_LINEDOWN:
				npos2 = min(100, npos2 + 1);
				break;
			case SB_PAGELEFT:
				npos2 -= 15;
				break;
			case SB_LINELEFT:
				npos2 = max(0, npos2 - 1);
				break;
			case SB_LEFT:
				npos2 = 0;
				break;
			case SB_RIGHT:
				npos2 = 100;
				break;
			case SB_THUMBTRACK:
				npos2 = HIWORD(wParam);
				//WM_VSCROLL的wParam参数的高字节为滚动条的位置，低字节为通知码
				break;
			}
			SetScrollPos(hScroll[1], SB_CTL, npos2, TRUE);
			//ChangeWard(g_MciDevId, npos2*Length / 1000 / 100 / 60 % 60, npos2*Length / 1000 / 100 % 60);
			//PlayVideoFile(g_MciDevId);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		DestroyWindow(0);
		return 0;

	//列表大小设置
	case WM_MEASUREITEM://ODT_LISTBOX
		if (wParam == 14)
		{
			lpmis->itemWidth = 400;
			lpmis->itemHeight = 25;
		}
		break;

	//列表外观设置
	case WM_DRAWITEM:
		if (wParam == 14)
		{
			SetTextColor(pDI->hDC, RGB(255, 255, 255));
			SetBkColor(pDI->hDC, RGB(0, 0, 0));
			SendMessage(hList, LB_GETTEXT, pDI->itemID, (LPARAM)filename[Index]);

			//如果选项处于选中状态,用下面的颜色
			if ((pDI->itemAction | ODA_SELECT) && (pDI->itemState & ODS_SELECTED))
			{
				SetTextColor(pDI->hDC, RGB(255, 255, 255));
				SetBkColor(pDI->hDC, RGB(54, 54, 54));
				FillRect(pDI->hDC, &pDI->rcItem, CreateSolidBrush(RGB(54, 54, 54)));
				id = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
			}

			//如果选项处于未选中状态,则用下面的颜色
			else
				FillRect(pDI->hDC, &pDI->rcItem, CreateSolidBrush(RGB(0, 0, 0)));

			const DWORD dwStyle = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS;
			DrawText(pDI->hDC, filename[Index], lstrlen(filename[Index]), &pDI->rcItem, dwStyle);

			SetTextColor(pDI->hDC, RGB(0, 0, 0));	//恢复
			SetBkColor(pDI->hDC, RGB(255, 255, 255));
		}
		break;

	//列表颜色设置
	case WM_CTLCOLORLISTBOX:
		hBrush = CreateSolidBrush(RGB(0, 0, 0));
		return (int)hBrush;
	}
	return DefWindowProc(hWnd, uiMessage, wParam, lParam);
}

//button贴图
void Paint(HWND hWnd, LPCWSTR lpBitmap)
{
	HDC hdc, hdcMem;
	HBITMAP hbm;
	BITMAP bminfo;
	PAINTSTRUCT ps;
	RECT rt;
	GetClientRect(hWnd, &rt);
	hbm = LoadBitmap(GetModuleHandle(NULL), lpBitmap);
	GetObject(hbm, sizeof(bminfo), &bminfo);
	hdc = BeginPaint(hWnd, &ps); 
	hdcMem = CreateCompatibleDC(hdc);
	SetBkMode(hdc, TRANSPARENT);
	SelectObject(hdcMem, hbm);
	StretchBlt(hdc, 0, 0, rt.right - rt.left, rt.bottom - rt.top, hdcMem, 0, 0, bminfo.bmWidth, bminfo.bmHeight, SRCCOPY);
	DeleteDC(hdcMem);
	EndPaint(hWnd, &ps);
}

//打开文件
BOOL OpenFileDlg(HWND hWnd, TCHAR* szFileSelect)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	//需要兼容UNICODE处理
	ZeroMemory(szFileSelect, sizeof(TCHAR)*MAX_PATH_LEN);
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFileSelect;
	ofn.lpstrFile[0] = '\0';
	ofn.lpstrFileTitle = szFileFilter;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = MAX_PATH_LEN;
	ofn.nMaxFileTitle = MAX_PATH_LEN;
	if(GetOpenFileName(&ofn))
	{
		//与网上给出的方法不同，实践中只要打开OK，文件路径就保存在ofn.lpstrFile参数中
		return TRUE;
	}
	return FALSE;
}

//打开选择目录，选择你所需打开的视频
void OpenVideoFile(HWND hWnd)
{
	//HWND hvideo;
	HFONT hFont;
	WIN32_FIND_DATA sfd;

	if (OpenFileDlg(hWnd, pBuffer_Directory[Index]))
	{
		HANDLE hfindfile = FindFirstFile(pBuffer_Directory[Index], &sfd);
		do
		{
			lstrcpy(filename[Index], pBuffer_Directory[Index]);
			PathStripPath(filename[Index]);
			hFont = CreateFont(22, 9, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
				DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
			SendMessage(hList, WM_SETFONT, (WPARAM)hFont, TRUE);
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)filename[Index]);

		} while (FindNextFile(hfindfile, &sfd));

		proc->szFileName = pBuffer_Directory[Index];
		proc->OpenVideoFile(proc->hVideo, m_streamstate, m_sdl);
		proc->EventProc(m_streamstate, m_sdl);
		total++;
	}
}

//调节音量大小
void ChangeVoice(int iDevID, int value)
{
}

//调整播放进度
void ChangeWard(int iDevID, int nMinute, int nSecond)
{
}

//调节屏幕亮度(未能成功实现该功能)
void ChangeBrightness(int iDevID, int bright)
{
	
}

//获得影片当前播放的位置
unsigned long getnowposition(int iDevID)
{
	unsigned long m_position;
	return m_position;
}

//显示在WM_PAINT里面的文字
void Display(HDC hdc, int Length, int Width)
{
	HFONT hFont;
	hFont = CreateFont(Length, Width, 0, 0, FW_MEDIUM, 0, 0, 0, DEFAULT_CHARSET, OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
			DRAFT_QUALITY, FIXED_PITCH | FF_MODERN, TEXT("Arial"));
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkColor(hdc, RGB(0, 0, 0));
	SelectObject(hdc, hFont);
	DeleteObject(hFont);
}

//有关播放器的设置的对话框设置
BOOL CALLBACK MainDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hScroll,hWndParent;
	LONG Value;
	HDC hdc;
	HBRUSH hbrush = CreateSolidBrush(RGB(0, 0, 0));
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_INITDIALOG:
		hWndParent = GetParent(hWnd);
		//增加窗口样式
		SetWindowLong(hWndParent, GWL_EXSTYLE, WS_EX_LAYERED);
		//设置窗口透明度
		SetLayeredWindowAttributes(hWndParent, RGB(255, 255, 255), 255, LWA_ALPHA);
		hScroll = GetDlgItem(hWnd, IDC_SLIDER3);
		SendMessage(hScroll, TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		break;

	case WM_HSCROLL:
		Value = (LONG)SendMessage(hScroll, TBM_GETPOS, 0, 0);
		//ChangeBrightness(g_MciDevId, Value);
		//SetLayeredWindowAttributes(hWndParent, RGB(255, 255, 255), Value, LWA_ALPHA);
		break;

	//控件消息处理
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
			EndDialog(hWnd, 1);
			break;

		case IDCANCEL:
			EndDialog(hWnd, 0);
			break;
		}

	//对话框外观
	case WM_CTLCOLORSTATIC:
	case WM_CTLCOLORDLG:
		hdc = (HDC)wParam;
		SetBkColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, RGB(255, 255, 255));
		return (int)hbrush;

	case WM_DESTROY:
		EndDialog(hWnd, 0);
		return 0;
	}
	return false;
}

//创建列表框
HWND CreateListView(HWND hWnd)
{
	HBRUSH hBrush;
	hList = CreateWindow(L"listbox", 0, WS_CHILD | WS_VISIBLE | LBS_HASSTRINGS | LBS_OWNERDRAWFIXED, 0, 0, 270, 270, hWnd, (HMENU)14, hInst, NULL);
	if (hList == NULL)
	{
		return NULL;
	}
	return hList;
}
