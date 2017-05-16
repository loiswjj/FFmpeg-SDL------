//Header �����������Ӧ��ͷ�ļ��Լ�ȫ�ֱ����Ķ���
#include "Header.h"

//��Ӧ����������
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

//����һ��������Ҫ������ص�WNDPROC
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

//������Ϣ����
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
	//button�����������б��ʼ��
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
		SetScrollRange(hScroll[0], SB_CTL, 0, 1000, FALSE);    //���ù������ķ�Χ��0-100
		SetScrollPos(hScroll[0], SB_CTL,500, FALSE);          //���ó�ʼֵΪ0

		hScroll[1] = CreateWindow(TEXT("scrollbar"), 0, WS_CHILD | WS_VISIBLE | SBS_HORZ,
			0, 20, 150, 20, hWnd, (HMENU)13, hInst, NULL);     //������
		SetScrollRange(hScroll[1], SB_CTL, 0, 100, FALSE);    //���ù������ķ�Χ��0-100
		SetScrollPos(hScroll[1], SB_CTL, 0, FALSE);          //���ó�ʼֵΪ0

		hList = CreateListView(hWnd);
		break;

	//����Ԫ����ʾ��button��ͼ
	case WM_PAINT:	
		hdc = BeginPaint(hWnd, &ps);
		//��Ӧ������ʾ
		hBrush = CreateSolidBrush(RGB(0, 0, 0));
		SelectObject(hdc, hBrush);
		Rectangle(hdc, 0, Cy - 90, Cx, Cy);
		Rectangle(hdc, 0, 0, Cx, 30);
		DeleteObject(hBrush);
		Display(hdc, 20, 8);
		TextOut(hdc, 35, 5, TEXT("Bobo Video Player"), 17);
		TextOut(hdc, 90, Cy - 70, proc->TotalTime, lstrlen(proc->TotalTime));//��ʾ��ʱ��
		TextOut(hdc, 80, Cy - 70, TEXT("/"), 1);
		TextOut(hdc, 10, Cy - 70, proc->CurrentTime, lstrlen(proc->CurrentTime));//��ʾĿǰ����ʱ��

		//���ڲ���
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
			Paint(hbutton[7], MAKEINTRESOURCE(IDB_BITMAP8));	//���
		}
		else
		{
			Paint(hbutton[7], MAKEINTRESOURCE(IDB_BITMAP9));	//���
		}
		
		Paint(hstatic, MAKEINTRESOURCE(IDB_BITMAP18));		//������ͼ��
		Paint(hbutton[0], MAKEINTRESOURCE(IDB_BITMAP13));	//����
		Paint(hbutton[2], MAKEINTRESOURCE(IDB_BITMAP3));	//��һ��
		Paint(hbutton[3], MAKEINTRESOURCE(IDB_BITMAP4));	//ֹͣ
		Paint(hbutton[4], MAKEINTRESOURCE(IDB_BITMAP5));	//��һ��
		Paint(hbutton[5], MAKEINTRESOURCE(IDB_BITMAP6));	//���ļ�
		Paint(hbutton[6], MAKEINTRESOURCE(IDB_BITMAP7));	//��С��
		Paint(hbutton[8], MAKEINTRESOURCE(IDB_BITMAP10));	//�ر�
		Paint(hbutton[9], MAKEINTRESOURCE(IDB_BITMAP11));	//����
		Paint(hbutton[10], MAKEINTRESOURCE(IDB_BITMAP12));	//�����
		Paint(hbutton[11], MAKEINTRESOURCE(IDB_BITMAP14));	//ѭ������
		Paint(hbutton[12], MAKEINTRESOURCE(IDB_BITMAP15));	//���ȫ��
		Paint(hbutton[13], MAKEINTRESOURCE(IDB_BITMAP16));	//ɾ������
		EndPaint(hWnd, &ps);
		break;

	//���ÿؼ�λ�ô�С
	case WM_SIZE:
		GetClientRect(hWnd, &ClientRect);
		Cx = ClientRect.right - ClientRect.left;
		Cy = ClientRect.bottom - ClientRect.top;
		MoveWindow(hstatic, 0, 0, 30, 30, TRUE);	//�Ի���
		MoveWindow(hbutton[0], 0.48*Cx+390, Cy - 52, 33, 33, TRUE);	//�Ի���
		MoveWindow(hbutton[1], 0.48*Cx, Cy-60, 45, 45, TRUE);	//����
		MoveWindow(hbutton[2], 0.48*Cx - 45, Cy-52, 33, 33, TRUE);	//��һ��
		MoveWindow(hbutton[3], 0.48*Cx - 88, Cy - 52, 33, 33, TRUE);	//ֹͣ
		MoveWindow(hbutton[4], 0.48*Cx + 57, Cy - 52, 33, 33, TRUE);	//��һ��
		MoveWindow(hbutton[5], 0.48*Cx + 97, Cy - 52, 33, 33, TRUE);	//�ļ�
		MoveWindow(hbutton[6], Cx - 100, 0, 30, 30, TRUE);					//��С��
		MoveWindow(hbutton[7], Cx - 65, 0, 30, 30, TRUE);					//���/�ָ�
		MoveWindow(hbutton[8], Cx - 30, 0, 30, 30, TRUE);					//�ر�
		MoveWindow(hbutton[9], 0.48*Cx + 180, Cy - 47, 12.5, 20, TRUE);	//����
		MoveWindow(hbutton[10], 0.48*Cx + 325, Cy - 47, 20, 20, TRUE);	//�����
		MoveWindow(hScroll[0], 0.48*Cx + 200, Cy - 42, 120, 10, TRUE);  //��������
		MoveWindow(hScroll[1], 0, Cy - 90, Cx-270, 15, TRUE);  //���ȵ���
		MoveWindow(hList, Cx - 270, 30, 270, Cy-120, TRUE);	//�б��	
		MoveWindow(hbutton[11], Cx - 40, Cy - 90, 25, 25, TRUE);	//ѭ������
		MoveWindow(hbutton[12], Cx - 80, Cy - 90, 25, 25, TRUE);	//���ȫ��
		MoveWindow(hbutton[13], Cx - 120, Cy - 90, 25, 25, TRUE);	//ɾ������
		MoveWindow(proc->hVideo, 0, 30, Cx - 270, Cy - 120, TRUE);	//��Ƶ
		return 0;

	//���ڵ����ź��ƶ�
	case WM_NCHITTEST:
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
		ScreenToClient(hWnd, &pt);
		GetClientRect(hWnd, &rcClient);

		if (pt.x<rcClient.left + 20 && pt.y<rcClient.top + 20)//���Ͻ�,�ж��ǲ��������Ͻǣ����ǿ���ǰ�����ǲ��Ǽ�������϶��ķ�Χ�ڣ������ϱ��϶��ķ�Χ�ڣ��������жϷ�������
		{
			return HTTOPLEFT;
		}
		else if (pt.x>rcClient.right - 20 && pt.y<rcClient.top + 20)//���Ͻ�
		{
			return HTTOPRIGHT;
		}
		else if (pt.x<rcClient.left + 20 && pt.y>rcClient.bottom - 20)//���½�
		{
			return HTBOTTOMLEFT;
		}
		else if (pt.x>rcClient.right - 20 && pt.y>rcClient.bottom - 20)//���½�
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
			return HTBOTTOM;          //�������ĸ����ϡ��¡������ĸ���
		}
		else
		{
			return HTCAPTION;
		}
		break;

	//��ʱ��
	/*case WM_TIMER:
		InvalidateRect(hWnd, 0, FALSE);
		//���õ�ǰʱ�䲢��00:00:00����ʽ��ʾ
		break;*/

	//�ؼ���Ϣ����
	case WM_COMMAND:
		switch (wParam)
		{
		case 0:	//������ͼ��
			DialogBox(hInst, MAKEINTRESOURCE(IDD_MYDIALOG), hWnd, MainDialogProc);
			break;

		case 1:	//����/��ͣ
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

		case 2:	//��һ��
			Index--;
			if (Index >= 0 && Index < total) {
				proc->szFileName = pBuffer_Directory[Index];
				proc->OpenVideoFile(proc->hVideo, m_streamstate, m_sdl);
				proc->EventProc(m_streamstate, m_sdl);
			}
			break;

		case 3:	//ֹͣ
			proc->status = QUIT;
			break;

		case 4:	//��һ��
			Index++;
			if (Index >= 0 && Index < total) {
				proc->szFileName = pBuffer_Directory[Index];
				proc->OpenVideoFile(proc->hVideo, m_streamstate, m_sdl);
				proc->EventProc(m_streamstate, m_sdl);
			}
			break;

		case 5:	//���ļ�
			OpenVideoFile(hWnd);
			break;

		case 6:	//��С��
			ShowWindow(hWnd, SW_MINIMIZE);
			break;

		case 7:	//���/�ָ�	
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

		case 8:	//�ر�
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

		case 9:	//����
			SetScrollPos(hScroll[0], SB_CTL, 0, TRUE);
			break;

		case 10://�������
			SetScrollPos(hScroll[0], SB_CTL, 1000, TRUE);
			break;

		case 11://ѭ������
			Count = 0;
			break;

		case 12://����б�
			SendMessage(hList, LB_RESETCONTENT, 0, 0);
			break;

		case 13://ɾ������
			SendMessage(hList, LB_DELETESTRING, id, 0);
			break;

		default:
			break;
		}
		break;

	//��������Ϣ����
	case WM_HSCROLL:
		if (GetWindowLong((HWND)lParam, GWL_ID) == 11)  //�������ڰ�ť
		{
			SetFocus(hScroll[0]);
			hWndParent = GetParent(hWnd);                   //�����ڵľ��
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
				//WM_VSCROLL��wParam�����ĸ��ֽ�Ϊ��������λ�ã����ֽ�Ϊ֪ͨ��
				break;
			}
			SetScrollPos(hScroll[0], SB_CTL, npos, TRUE);
			//ChangeVoice(g_MciDevId, npos);        //�������������������϶���λ��
		}
		else
		{	//���ȵ���
			Flag = 1;
			SetFocus(hScroll[1]);
			hWndParent = GetParent(hWnd);                   //�����ڵľ��
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
				//WM_VSCROLL��wParam�����ĸ��ֽ�Ϊ��������λ�ã����ֽ�Ϊ֪ͨ��
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

	//�б��С����
	case WM_MEASUREITEM://ODT_LISTBOX
		if (wParam == 14)
		{
			lpmis->itemWidth = 400;
			lpmis->itemHeight = 25;
		}
		break;

	//�б��������
	case WM_DRAWITEM:
		if (wParam == 14)
		{
			SetTextColor(pDI->hDC, RGB(255, 255, 255));
			SetBkColor(pDI->hDC, RGB(0, 0, 0));
			SendMessage(hList, LB_GETTEXT, pDI->itemID, (LPARAM)filename[Index]);

			//���ѡ���ѡ��״̬,���������ɫ
			if ((pDI->itemAction | ODA_SELECT) && (pDI->itemState & ODS_SELECTED))
			{
				SetTextColor(pDI->hDC, RGB(255, 255, 255));
				SetBkColor(pDI->hDC, RGB(54, 54, 54));
				FillRect(pDI->hDC, &pDI->rcItem, CreateSolidBrush(RGB(54, 54, 54)));
				id = (int)SendMessage(hList, LB_GETCURSEL, 0, 0);
			}

			//���ѡ���δѡ��״̬,�����������ɫ
			else
				FillRect(pDI->hDC, &pDI->rcItem, CreateSolidBrush(RGB(0, 0, 0)));

			const DWORD dwStyle = DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX | DT_END_ELLIPSIS;
			DrawText(pDI->hDC, filename[Index], lstrlen(filename[Index]), &pDI->rcItem, dwStyle);

			SetTextColor(pDI->hDC, RGB(0, 0, 0));	//�ָ�
			SetBkColor(pDI->hDC, RGB(255, 255, 255));
		}
		break;

	//�б���ɫ����
	case WM_CTLCOLORLISTBOX:
		hBrush = CreateSolidBrush(RGB(0, 0, 0));
		return (int)hBrush;
	}
	return DefWindowProc(hWnd, uiMessage, wParam, lParam);
}

//button��ͼ
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

//���ļ�
BOOL OpenFileDlg(HWND hWnd, TCHAR* szFileSelect)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	//��Ҫ����UNICODE����
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
		//�����ϸ����ķ�����ͬ��ʵ����ֻҪ��OK���ļ�·���ͱ�����ofn.lpstrFile������
		return TRUE;
	}
	return FALSE;
}

//��ѡ��Ŀ¼��ѡ��������򿪵���Ƶ
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

//����������С
void ChangeVoice(int iDevID, int value)
{
}

//�������Ž���
void ChangeWard(int iDevID, int nMinute, int nSecond)
{
}

//������Ļ����(δ�ܳɹ�ʵ�ָù���)
void ChangeBrightness(int iDevID, int bright)
{
	
}

//���ӰƬ��ǰ���ŵ�λ��
unsigned long getnowposition(int iDevID)
{
	unsigned long m_position;
	return m_position;
}

//��ʾ��WM_PAINT���������
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

//�йز����������õĶԻ�������
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
		//���Ӵ�����ʽ
		SetWindowLong(hWndParent, GWL_EXSTYLE, WS_EX_LAYERED);
		//���ô���͸����
		SetLayeredWindowAttributes(hWndParent, RGB(255, 255, 255), 255, LWA_ALPHA);
		hScroll = GetDlgItem(hWnd, IDC_SLIDER3);
		SendMessage(hScroll, TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(0, 255));
		break;

	case WM_HSCROLL:
		Value = (LONG)SendMessage(hScroll, TBM_GETPOS, 0, 0);
		//ChangeBrightness(g_MciDevId, Value);
		//SetLayeredWindowAttributes(hWndParent, RGB(255, 255, 255), Value, LWA_ALPHA);
		break;

	//�ؼ���Ϣ����
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

	//�Ի������
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

//�����б��
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
