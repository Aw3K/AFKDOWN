
/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <dedkam2@wp.pl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <tchar.h>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <cstring>
#pragma comment (lib, "Wininet.lib");
#include <wininet.h>
#include <vector>
#include <math.h>
using namespace std;

#define IDT_TIMER 1
#define IDT_TIMER2 2
#define MAIN_LOOP 3
#define IDT_TIMER3 4
#define SHUTDOWN 5
#define IDC_PICK1 6
#define IDC_PICK2 7
#define IDC_PICK3 8
#define IDC_SAVE 9
#define IDC_START 10
#define IDC_RESET 11
#define SLEEPP 12
#define SLEEPPANIM 13
#define IDC_STOP 14
#define IDC_STARTUP 15
#define QUICK 16
#define MINIMALIZE 17

WNDCLASSEX wc;
HWND hwnd, B1, B2, B3, B4, B5, B6, B7, B8, INACTIVET, INACTIVED, INACTIVEDD, BSAVE, yes;
MSG msg;
LASTINPUTINFO key;
int SLEEP = 0, SHUTDOWNTIMEOUT = 0, AFKMODE, ANIMATION = 1;
DWORD keyOLD;
bool SAVE = false, MIN = false;
LPSTR CMD;

BOOL FileExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
         !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

bool STARTUP()
{
	fstream check;
	check.open("config.cfg", ios::in);
	check >> AFKMODE;
	SetTimer(hwnd, QUICK, 5000, NULL);
	if (check.is_open())
	{
		check.close();
		return true;
	} else {
		check.close();
		return false;
	}
}

void FLASH()
{
	FLASHWINFO obj;
	ZeroMemory( & obj, sizeof( FLASHWINFO ) );
	obj.cbSize = sizeof( FLASHWINFO );
	obj.dwFlags = FLASHW_TRAY;
	obj.hwnd = hwnd;
	obj.uCount = 2;
	obj.dwTimeout = 500;
	FlashWindowEx( & obj );
}

void DESTROY()
{
	DestroyWindow(B1);
	DestroyWindow(B2);
	DestroyWindow(B3);
	DestroyWindow(B4);
	DestroyWindow(B5);
	DestroyWindow(B6);
	DestroyWindow(B7);
	DestroyWindow(INACTIVET);
	DestroyWindow(INACTIVED);
	DestroyWindow(BSAVE);
	DestroyWindow(INACTIVEDD);
	DestroyWindow(yes);
}

void KILLTIMERS()
{
	KillTimer(hwnd, IDT_TIMER);
	KillTimer(hwnd, IDT_TIMER2);
	KillTimer(hwnd, IDT_TIMER3);
	KillTimer(hwnd, MAIN_LOOP);
	KillTimer(hwnd, SLEEPP);
	KillTimer(hwnd, SLEEPPANIM);
}

void RUNAPP()
{
	DESTROY();
	INACTIVET = CreateWindow(
				"STATIC",
				"",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | ES_READONLY,
				10, 10, 190, 20,
				hwnd, NULL, NULL, NULL);
	BSAVE = CreateWindow(
				"BUTTON",
				"Change Config",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
				10, 30, 190, 20,
				hwnd, (HMENU)IDC_RESET, NULL, NULL);
	INACTIVEDD = CreateWindow(
				"STATIC",
				"---",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | ES_READONLY,
				10, 50, 190, 100,
				hwnd, NULL, NULL, NULL);
	EnableWindow(INACTIVET, TRUE);	
	HFONT hFont = CreateFont (110, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
   	SendMessage (INACTIVEDD, WM_SETFONT, WPARAM (hFont), TRUE);
	SetTimer(hwnd, IDT_TIMER, 100, (TIMERPROC) NULL);
	SetTimer(hwnd, IDT_TIMER2, 1000, (TIMERPROC) NULL);
	SetTimer(hwnd, MAIN_LOOP, 1000, (TIMERPROC) NULL);
	ANIMATION = 1;
}

string intToStr(int n)
{
    string tmp;
    if(n < 0)
	{
      	tmp = "-";
      	n = -n;
    }
    if (n > 9) tmp += intToStr(n / 10);
    tmp += n % 10 + 48;
    return tmp;
}

void PREPARE_END()
{
	yes = CreateWindow(
			"BUTTON",
			"STOP SHUTDOWN",
			WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
			10, 0, 190, 45,
			hwnd, (HMENU)IDC_STOP, NULL, NULL);
	DestroyWindow(INACTIVET);
	DestroyWindow(BSAVE);
	BringWindowToTop(yes);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch(Message)
	{
		case WM_DESTROY:
		{
			KILLTIMERS();
			PostQuitMessage(0);
			break;
		}
		
		case WM_CREATE:
		{
			SHELLEXECUTEINFO sei = { sizeof(sei) };
			sei.lpVerb = "open";
			sei.lpFile = "cmd.exe";
			sei.lpParameters = "/C COPY %USERPROFILE%\\Documents\\AFKDOWN\\config.cfg .\\";
			sei.nShow = SW_HIDE;
			ShellExecuteExA(&sei);
			DESTROY();
			INACTIVED = CreateWindow(
						"STATIC",
						"A",
						WS_VISIBLE | WS_CHILD | ES_CENTER,
						10, 10, 190, 150,
						hwnd, NULL, NULL, NULL);
			HFONT hFont = CreateFont (140, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
			SendMessage (INACTIVED, WM_SETFONT, WPARAM (hFont), TRUE);
			SetTimer(hwnd, SLEEPP, 3000, (TIMERPROC) NULL);
			SetTimer(hwnd, SLEEPPANIM, 1000, (TIMERPROC) NULL);
			break;
		}
		
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_PICK1:
				{
					AFKMODE = 1;
					break;
				}
				case IDC_PICK2:
				{
					AFKMODE = 2;
					break;
				}
				case IDC_PICK3:
				{
					AFKMODE = 3;
					break;
				}
				case IDC_SAVE:
				{
					SAVE = !SAVE;
					break;
				}
				case IDC_START:
				{
					if (AFKMODE == 0)
					{
						MessageBox(hwnd, "Pick \"AFK detection aggresion\" before continue!", "ERROR", MB_OK);
					} else {
						if (SAVE)
						{
							fstream save;
							save.open("config.cfg", ios::out);
							save << AFKMODE;
							save.close();
							ShellExecute(0, "open", "cmd.exe", "/C MKDIR %USERPROFILE%\\Documents\\AFKDOWN&COPY config.cfg %USERPROFILE%\\Documents\\AFKDOWN\\&DEL /Q config.cfg", 0, SW_HIDE);
							RUNAPP();
						} else {
							RUNAPP();
						}
					}
					break;
				}
				case IDC_RESET:
				{
					ShellExecute(0, "open", "cmd.exe", "/C DEL /Q %USERPROFILE%\\Documents\\AFKDOWN", 0, SW_HIDE);
					KILLTIMERS();
					DESTROY();
					INACTIVED = CreateWindow(
								"STATIC",
								"A",
								WS_VISIBLE | WS_CHILD | ES_CENTER,
								10, 10, 190, 140,
								hwnd, NULL, NULL, NULL);
					HFONT hFont = CreateFont (130, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
					SendMessage (INACTIVED, WM_SETFONT, WPARAM (hFont), TRUE);
					SetTimer(hwnd, SLEEPP, 3000, (TIMERPROC) NULL);
					SetTimer(hwnd, SLEEPPANIM, 1000, (TIMERPROC) NULL);
					break;
				}
				case IDC_STOP:
				{
					KILLTIMERS();
					RUNAPP();
					SetWindowText(INACTIVED, "---");
					break;
				}
				case IDC_STARTUP:
				{
					ShellExecute(0, "open", "cmd.exe", "/C cd %appdata%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup&cd >> appdata.txt", 0, SW_HIDE);
					ShellExecute(0, "open", "cmd.exe", "/C COPY \"%appdata%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\appdata.txt\" .\\&DEL /Q \"%appdata%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\appdata.txt\"", 0, SW_HIDE);
					Sleep(500);
					fstream check;
					check.open("appdata.txt", ios::in);
					string path;
					getline(check, path);
					check.close();
					path += "\\AFKDOWN.vbs";
					Sleep(500);
					ShellExecute(0, "open", "cmd.exe", "/C DEL appdata.txt", 0, SW_HIDE);
					if (FileExists(path.c_str()))
					{
						ShellExecute(0, "open", "cmd.exe", "/C DEL /Q \"%appdata%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\\AFKDOWN.vbs\"", 0, SW_HIDE);
						MessageBox(hwnd, "AUTORUN entry successfully deleted.", "INFO", MB_OK);
					} else {
						TCHAR szFileName[MAX_PATH];
						GetModuleFileName(NULL, szFileName, MAX_PATH);
						string SCS = string(szFileName);
						for (int i = SCS.length()-1; i>0; i--)
						{
							if (SCS[i] == '\\')
							{
								SCS[i] = '&';
								break;
							}
						}
						fstream SC;
						SC.open("AFKDOWN.vbs", ios::out);
						SC << "Set objShell = WScript.CreateObject(\"WScript.Shell\")\n";
						SC << "objShell.Run \"cmd /c cd " + SCS + " -sc\", 0, True";
						SC.close();
						ShellExecute(0, "open", "cmd.exe", "/C COPY AFKDOWN.vbs \"%appdata%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup\"&DEL /Q AFKDOWN.vbs", 0, SW_HIDE);
						MessageBox(hwnd, "AUTORUN entry successfully created.", "INFO", MB_OK);
					}
					break;
				}
				break;
			}
			
			case WM_TIMER:
			{
				switch (wParam) 
				{ 
					case QUICK:
					{
						ShellExecute(0, "open", "cmd.exe", "/C DEL /Q config.cfg", 0, SW_HIDE);
						KillTimer(hwnd, QUICK);
						break;
					}
					case SLEEPP:
					{
						KillTimer(hwnd, SLEEPPANIM);
						DestroyWindow(INACTIVED);
						KillTimer(hwnd, SLEEPP);
						if (STARTUP())
						{
							RUNAPP();
							SetWindowText(INACTIVED, "---");
						} else {
							B1 = CreateWindow(
								"BUTTON",
								" AFK detection aggresion ",
								WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | BS_GROUPBOX,
								10, 10, 190, 100,
								hwnd, NULL, NULL, NULL);
							B2 = CreateWindow(
								"BUTTON",
								"LOW (60/30)",
								WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
								30, 30, 150, 20,
								hwnd, (HMENU)IDC_PICK1, NULL, NULL);
							B3 = CreateWindow(
								"BUTTON",
								"MEDIUM (30/15)",
								WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
								30, 55, 150, 20,
								hwnd, (HMENU)IDC_PICK2, NULL, NULL);
							B4 = CreateWindow(
								"BUTTON",
								"HIGH (20/5)",
								WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | BS_AUTORADIOBUTTON | BS_PUSHLIKE,
								30, 80, 150, 20,
								hwnd, (HMENU)IDC_PICK3, NULL, NULL);
							B5 = CreateWindow(
								"BUTTON",
								"Save config.",
								WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | BS_CHECKBOX,
								10, 110, 120, 20,
								hwnd, (HMENU)IDC_SAVE, NULL, NULL);
							B6 = CreateWindow(
								"BUTTON",
								"STARTUP",
								WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
								130, 110, 70, 20,
								hwnd, (HMENU)IDC_STARTUP, NULL, NULL);
							B7 = CreateWindow(
								"BUTTON",
								"RUN PROGRAM",
								WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
								10, 135, 190, 20,
								hwnd, (HMENU)IDC_START, NULL, NULL);
						}
					}
					case SLEEPPANIM:
					{
						if (ANIMATION == 1) SetWindowText(INACTIVED, "F");
						else if (ANIMATION == 2) SetWindowText(INACTIVED, "K");
						ANIMATION++;
					}
					case IDT_TIMER: 
					{
					    key.cbSize = sizeof(LASTINPUTINFO);
					    GetLastInputInfo(&key);
					    if (key.dwTime == keyOLD){}
						else {
							SLEEP = 0;
						}
						keyOLD = key.dwTime;
						string x = "IDLE TIME: ";
						x += intToStr(SLEEP);
						x+= "s";
						SetWindowText(INACTIVET, x.c_str());
					    break;
					}
					case IDT_TIMER2: 
					{
						SLEEP++;
						SHUTDOWNTIMEOUT++;
					    break;
					}
					case IDT_TIMER3:
					{
						FLASH();
						if (AFKMODE == 1)
						{
							float i = (1800-(float)SHUTDOWNTIMEOUT)/60;
							string y = intToStr(ceil(i));
							SetWindowText(INACTIVEDD, y.c_str());
							if (SHUTDOWNTIMEOUT == 3000)
							{
								SetTimer(hwnd, SHUTDOWN, 1000, (TIMERPROC) NULL);
								KillTimer(hwnd, IDT_TIMER3);
							}
						}
						else if (AFKMODE == 2)
						{
							float i = (900-(float)SHUTDOWNTIMEOUT)/60;
							string y = intToStr(ceil(i));
							SetWindowText(INACTIVEDD, y.c_str());
							if (SHUTDOWNTIMEOUT == 1800)
							{
								SetTimer(hwnd, SHUTDOWN, 1000, (TIMERPROC) NULL);
								KillTimer(hwnd, IDT_TIMER3);
							}
						}
						else if (AFKMODE == 3)
						{
							float i = (300-(float)SHUTDOWNTIMEOUT)/60;
							string y = intToStr(ceil(i));
							SetWindowText(INACTIVEDD, y.c_str());
							if (SHUTDOWNTIMEOUT == 300)
							{
								SetTimer(hwnd, SHUTDOWN, 1000, (TIMERPROC) NULL);
								KillTimer(hwnd, IDT_TIMER3);
							}
						}
						else {
							MessageBox(NULL, "AFKMODE: wrong specified/equals NULL", "ERROR", MB_OK);
							KILLTIMERS();
							PostQuitMessage(0);
						}
					}
					case SHUTDOWN:
					{
						ShellExecute(0, "open", "cmd.exe", "/C SHUTDOWN -s -t 1", 0, SW_HIDE);
						break;
					}
					case MAIN_LOOP:
					{
						if (MIN) {
							SendMessageA(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
							MIN = false;
						}
						if (AFKMODE == 1)
						{
							float i = (3600-(float)SLEEP)/60;
							string y = intToStr(ceil(i));
							SetWindowText(INACTIVEDD, y.c_str());
							if (SLEEP == 6000)
							{
								SHUTDOWNTIMEOUT = 0;
								SetTimer(hwnd, IDT_TIMER3, 500, (TIMERPROC) NULL);
								PREPARE_END();
							}
						}
						else if (AFKMODE == 2)
						{
							float i = (1800-(float)SLEEP)/60;
							string y = intToStr(ceil(i));
							SetWindowText(INACTIVEDD, y.c_str());
							if (SLEEP == 3600)
							{
								SHUTDOWNTIMEOUT = 0;
								SetTimer(hwnd, IDT_TIMER3, 500, (TIMERPROC) NULL);
								PREPARE_END();
							}
						}
						else if (AFKMODE == 3)
						{
							double i = (1200-(float)SLEEP)/60;
							string y = intToStr(ceil(i));
							SetWindowText(INACTIVEDD, y.c_str());
							if (SLEEP == 1200)
							{
								SHUTDOWNTIMEOUT = 0;
								SetTimer(hwnd, IDT_TIMER3, 500, (TIMERPROC) NULL);
								PREPARE_END();
							}
						}
						else {
							MessageBox(NULL, "AFKMODE: wrong specified/equals NULL", "ERROR", MB_OK);
							KILLTIMERS();
							PostQuitMessage(0);
						}
					}
				} 
			}
			break;
		}
		
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (string(lpCmdLine) == "-sc")
	{
		Sleep(60000);
		MIN = true;
	}
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc;
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);

	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","AFKDOWN",
		WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX | WM_TIMER,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		220,
		190,
		NULL,NULL,hInstance,NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
