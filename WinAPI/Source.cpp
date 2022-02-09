#include<Windows.h>
#include"resource.h"

CONST CHAR INVITATION_LOGIN[] = "Введите логин";
CONST CHAR INVITATION_PASSWORD[] = "Введите пароль";

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	/*MessageBox(FindWindow(NULL,"WinAPI - Microsoft Visual Studio"), "Привет! Это мое первое окно сообщения", "Hello World",
		MB_DEFBUTTON1|MB_ICONWARNING | MB_YESNOCANCEL|MB_HELP| MB_SYSTEMMODAL);*/

	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc, 0);

	return 0;
}
BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:   //Инциализация окна (создание значков и т.д.)
	{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
		SendMessage(hwnd, WM_SETICON, 0, (LPARAM)hIcon);
		SendMessage(GetDlgItem(hwnd, IDC_EDIT_LOGIN), WM_SETTEXT, 0, (LPARAM)INVITATION_LOGIN);
		SendMessage(GetDlgItem(hwnd, IDC_EDIT_PASSWORD), WM_SETTEXT, 0, (LPARAM)INVITATION_PASSWORD);
	}
	break;
	case WM_COMMAND:     //Обработка команд нажатия кнопок и т.д.
	{
		//WORD = 2 BYTES
		// DWORD = 4 BYTES
		//LOWORD - 2 младший Байта в двойном слове
		//HIWORD - 2 старших брата в двойном слове
		switch (LOWORD(wParam))
		{
		case IDC_BTN_COPY:
		{
			CONST INT SIZE = 256;
			CHAR buffer[SIZE]{};
			HWND hEdit = GetDlgItem(hwnd, IDC_EDIT_LOGIN);//Получаем HWND элемента окна по его ResourseID
			SendMessage(hEdit, WM_GETTEXT, SIZE, (LPARAM)buffer);
			SendMessage(GetDlgItem(hwnd, IDC_EDIT_PASSWORD), WM_SETTEXT, 0, (LPARAM)buffer);

		}
		break;
		case IDOK:MessageBox(hwnd, "Было нажата кнопка ОК", "Info",
			MB_ICONINFORMATION | MB_OK); break;
		case IDCANCEL:
			EndDialog(hwnd, 0); break;
		case IDC_EDIT_LOGIN:
		{
			CONST INT SIZE = 256;
			CHAR buffer[SIZE]{};
				SendMessage(GetDlgItem(hwnd, IDC_EDIT_LOGIN), WM_GETTEXT, SIZE, (LPARAM)buffer);
			switch (HIWORD(wParam))
			{
			case EN_SETFOCUS:
				if (strcmp(buffer, INVITATION_LOGIN) == 0)
					SendMessage(GetDlgItem(hwnd, IDC_EDIT_LOGIN), WM_SETTEXT, 0, (LPARAM)"");
				break;
			case EN_KILLFOCUS:
				if (strcmp(buffer, "") == 0)
					SendMessage(GetDlgItem(hwnd, IDC_EDIT_LOGIN), WM_SETTEXT, 0, (LPARAM)INVITATION_LOGIN);
				break;
			}
		}
		}break;
	}
	break;
	case WM_CLOSE:       //Закрытие окна:
		EndDialog(hwnd, 0);
	}
	return FALSE;
}