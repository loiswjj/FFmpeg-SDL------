#include<Windows.h>
#include<windowsx.h>
#include<string>
#include<stdlib.h>
//#include<mciapi.h>
//#include<MciAvi.h>
#include<WinUser.h>
#include<mmsystem.h>
#include<Digitalv.h>
#include"resource.h"
#include"resource3.h"
#include<Shlobj.h>										// ���� ����ļ���
#include<shellapi.h>									// ���� NOTIFYICONDATA
#include<commdlg.h>									// ���� OPENFILENAME
#include<CommCtrl.h>
#include<Richedit.h>
#include<shlwapi.h>
#include<iostream>
#include "FFmpegPlayer.h"

#pragma comment(lib, "WINMM.LIB") 
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"GDI32.lib")
#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"shlwapi.lib")

using namespace std;

#define MAX_PATH_LEN  1000
#define ID_STATIC 40000

//�ؼ�����
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

TCHAR szFileFilter[MAX_PATH_LEN] = TEXT("��ý���ļ�(*.avi;*.mpg;*.mpeg;*.asf;*.wmv)\0")\
TEXT("*.avi;*.mpg;*.asf;*.wmv\0")\
TEXT("��Ӱ(*.avi;*.mpg;*.mpeg)\0*.avi;*.mpg;*.mpeg\0")\
TEXT("windows��ý��(*.asf;*.wmv)\0*.asf;*.wmv\0");

HINSTANCE hInst;
VideoProcess *proc = new VideoProcess();

static RECT ClientRect;
static HWND hList;
static TCHAR pBuffer_Directory[MAX_PATH][MAX_PATH];								//����ļ���Ϣ
static TCHAR filename[MAX_PATH][MAX_PATH];
static TCHAR szOpenFileNames[80 * MAX_PATH];				// ��ʼ��ʱ������ȡ�ļ����ı���
static TCHAR szFileName_Path[255][MAX_PATH];				// ��¼�����ľ���·���������ļ���
static TCHAR szFile[MAX_PATH_LEN];
static int Cx, Cy, flag = 0;
static int i = 1, j = 1, k = 1;
static float cxScreen = GetSystemMetrics(SM_CXSCREEN), cyScreen = GetSystemMetrics(SM_CYSCREEN);
static int total;	//���ļ�����
static int Index = 0;