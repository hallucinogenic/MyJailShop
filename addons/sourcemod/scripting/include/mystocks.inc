/*
 * MyStocks - include file - function libary.
 * by: shanapu
 * https://github.com/shanapu/
 * 
 * Copyright (C) 2016-2017 Thomas Schmidt (shanapu)
 *
 * Version 12.0
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#if defined _mystocks_included_
	#endinput
#endif
#define _mystocks_included_


/******************************************************************************
                   STOCKS
******************************************************************************/


// Check for valid clients with bool for bots & dead player 
stock bool IsValidClient(int client, bool bAllowBots = false, bool bAllowDead = true)
{
	if (!(1 <= client <= MaxClients) || !IsClientInGame(client) || (IsFakeClient(client) && !bAllowBots) || IsClientSourceTV(client) || IsClientReplay(client) || (!bAllowDead && !IsPlayerAlive(client)))
	{
		return false;
	}
	return true;
}


// Easy precache & prepare download for decals
stock void PrecacheDecalAnyDownload(char[] sDecal)
{
	char sBuffer[256];
	Format(sBuffer, sizeof(sBuffer), "%s.vmt", sDecal);
	PrecacheDecal(sBuffer, true);
	Format(sBuffer, sizeof(sBuffer), "materials/%s.vmt", sDecal);
	AddFileToDownloadsTable(sBuffer);

	Format(sBuffer, sizeof(sBuffer), "%s.vtf", sDecal);
	PrecacheDecal(sBuffer, true);
	Format(sBuffer, sizeof(sBuffer), "materials/%s.vtf", sDecal);
	AddFileToDownloadsTable(sBuffer);
}


// Easy precache & prepare download for models (icons)
stock void PrecacheModelAnyDownload(char[] sModel)
{
	char sBuffer[256];
	Format(sBuffer, sizeof(sBuffer), "materials/%s.vmt", sModel);
	AddFileToDownloadsTable(sBuffer);
	PrecacheModel(sBuffer, true);
	Format(sBuffer, sizeof(sBuffer), "materials/%s.vtf", sModel);
	AddFileToDownloadsTable(sBuffer);
	PrecacheModel(sBuffer, true);
}


// Easy precache & prepare download for sounds
stock void PrecacheSoundAnyDownload(char[] sSound)
{
	char sBuffer[256];
	PrecacheSoundAny(sSound);
	Format(sBuffer, sizeof(sBuffer), "sound/%s", sSound);
	AddFileToDownloadsTable(sBuffer);
}


// Strip All Weapons & the knife slot twice for taser
stock void StripAllPlayerWeapons(int client)
{
	int weapon;
	for (int i = 0; i <= 4; i++)
	{
		if ((weapon = GetPlayerWeaponSlot(client, i)) != -1)
		{
			RemovePlayerItem(client, weapon);
			AcceptEntityInput(weapon, "Kill");
		}
	}

	if ((weapon = GetPlayerWeaponSlot(client, CS_SLOT_KNIFE)) != -1)   // strip knife slot 2 times for taser
	{
		RemovePlayerItem(client, weapon);
		AcceptEntityInput(weapon, "Kill");
	}
}


// Show overlay to a client with lifetime | 0.0 = no auto remove
stock void ShowOverlay(int client, char[] path, float lifetime)
{
	if (!IsClientInGame(client) || IsFakeClient(client) || IsClientSourceTV(client) || IsClientReplay(client))
		return;

	ClientCommand(client, "r_screenoverlay \"%s.vtf\"", path);

	if (lifetime != 0.0)
		CreateTimer(lifetime, DeleteOverlay, GetClientUserId(client));
}


// Show overlay to all clients with lifetime | 0.0 = no auto remove
stock void ShowOverlayAll(char[] path, float lifetime)
{
	for (int i = 1; i <= MaxClients; i++)
	{
		if (!IsClientInGame(i) || IsFakeClient(i) || IsClientSourceTV(i) || IsClientReplay(i))
			continue;

		ClientCommand(i, "r_screenoverlay \"%s.vtf\"", path);

		if (lifetime != 0.0)
			CreateTimer(lifetime, DeleteOverlay, GetClientUserId(i));
	}
}


// Remove overlay from a client
stock Action DeleteOverlay(Handle timer, any userid)
{
	int client = GetClientOfUserId(userid);
	if (client <= 0 || !IsClientInGame(client) || IsFakeClient(client) || IsClientSourceTV(client) || IsClientReplay(client))
		return;

	ClientCommand(client, "r_screenoverlay \"\"");
}


// Easy silent change of ConVars - Boolean
stock void SetCvar(char cvarName[64], int value)
{
	Handle IntCvar = FindConVar(cvarName);
	if (IntCvar == null) return;

	int flags = GetConVarFlags(IntCvar);
	flags &= ~FCVAR_NOTIFY;
	SetConVarFlags(IntCvar, flags);

	SetConVarInt(IntCvar, value);

	flags |= FCVAR_NOTIFY;
	SetConVarFlags(IntCvar, flags);
}


// Easy silent change of ConVars - Floats
stock void SetCvarFloat(char cvarName[64], float value)
{
	Handle FloatCvar = FindConVar(cvarName);
	if (FloatCvar == INVALID_HANDLE) return;

	new flags = GetConVarFlags(FloatCvar);
	flags &= ~FCVAR_NOTIFY;
	SetConVarFlags(FloatCvar, flags);

	SetConVarFloat(FloatCvar, value);

	flags |= FCVAR_NOTIFY;
	SetConVarFlags(FloatCvar, flags);
}


// Easy silent change of ConVars - Strings
stock void SetCvarString(char cvarName[64], char[] value)
{
	Handle cvar = FindConVar(cvarName);
	SetConVarString(cvar, value, true);
}


// Define filename & path for logging 
stock void SetLogFile(char LogFile[PLATFORM_MAX_PATH], char[] FileName, char[] FolderName)
{
	char sDate[12];
	FormatTime(sDate, sizeof(sDate), "%y-%m-%d");
	Format(LogFile, sizeof(LogFile), "logs/%s/%s-%s.log", FolderName, FileName, sDate);

	BuildPath(Path_SM, LogFile, sizeof(LogFile), LogFile);
}


// Set the players Ammo for active weapon
stock void SetPlayerAmmo(int client, int weaponEnt, int clip = -1, int ammo = -1)
{
	if (weaponEnt == INVALID_ENT_REFERENCE)
		return;

	if (clip != -1)
		SetEntProp(weaponEnt, Prop_Send, "m_iClip1", clip);

	if (ammo != -1)
	{
		int iOffset = FindDataMapInfo(client, "m_iAmmo") + (GetEntProp(weaponEnt, Prop_Data, "m_iPrimaryAmmoType") * 4);
		SetEntData(client, iOffset, ammo, 4, true);

		if (GetEngineVersion() == Engine_CSGO) 
		{
			SetEntProp(weaponEnt, Prop_Send, "m_iPrimaryReserveAmmoCount", ammo);
		}
	}
}


// Remove a players render color
stock Action Timer_RemoveColor(Handle timer, any client) 
{
	if (IsValidClient(client, true, true))
	{
		SetEntityRenderColor(client, 255, 255, 255, 255);
	}
}


// Get random alive & non-rebeling player of a team (need lastrequest.inc)
stock int GetRandomPlayerNonRebel(int team) 
{
	int[] clients = new int[MaxClients];
	int clientCount;

	for (int i = 1; i <= MaxClients; i++) if (IsClientInGame(i))
	{
		if ((GetClientTeam(i) == team) && IsPlayerAlive(i) && IsClientRebel(i))
		{
			clients[clientCount++] = i;
		}
	}

	return (clientCount == 0) ? -1 : clients[GetRandomInt(0, clientCount-1)];
}

// Get random alive player
stock int GetRandomPlayer(int team) 
{
	int[] clients = new int[MaxClients];
	int clientCount;

	for (int i = 1; i <= MaxClients; i++) if (IsClientInGame(i))
	{
		if ((GetClientTeam(i) == team) && IsPlayerAlive(i))
		{
			clients[clientCount++] = i;
		}
	}

	return (clientCount == 0) ? -1 : clients[GetRandomInt(0, clientCount-1)];
}


// Get alive player count of a team
stock int GetAlivePlayersCount(int team)
{
	int iCount, i; iCount = 0;

	for (i = 1; i <= MaxClients; i++)
		if (IsValidClient(i, true, false) && GetClientTeam(i) == team)
		iCount++;

	return iCount;
}


// Get alive & non-rebeling player count of a team (need lastrequest.inc)
stock int GetAlivePlayersCountNonRebel(int team)
{
	int iCount, i; iCount = 0;

	for (i = 1; i <= MaxClients; i++)
		if (IsValidClient(i, true, false) && !IsClientRebel(i) && GetClientTeam(i) == team)
		iCount++;

	return iCount;
}


// Get all player count of a team
stock int GetTeamPlayersCount(int team)
{
	int iCount, i; iCount = 0;

	for (i = 1; i <= MaxClients; i++)
		if (IsValidClient(i, true, true) && GetClientTeam(i) == team)
		iCount++;

	return iCount;
}

// Get all player count of a team
stock int GetAllPlayersCount()
{
	int iCount, i; iCount = 0;

	for (i = 1; i <= MaxClients; i++)
		if (IsValidClient(i, true, true))
		iCount++;

	return iCount;
}


// Get a alive random player of all teams
stock int GetRandomAlivePlayer() 
{
	int[] clients = new int[MaxClients];
	int clientCount;

	for (int i = 1; i <= MaxClients; i++) if (IsClientInGame(i))
	{
		if (IsPlayerAlive(i))
		{
			clients[clientCount++] = i;
		}
	}

	return (clientCount == 0) ? -1 : clients[GetRandomInt(0, clientCount-1)];
}


// Get last alive player of a team
stock int GetLastAlive(int team)
{
	int clientCount = 0;
	int client;

	for (int i = 1; i <= MaxClients; i++) if (IsValidClient(i, true, false)) if (GetClientTeam(i) == team)
	{
		clientCount++;
		client = i;
	}

	return (clientCount > 1) ? -1 : client;
}


// Get a player for a certain admin flag
stock bool CheckVipFlag(int client, char [] flagsNeed)
{
	int iCount = 0;
	char sflagNeed[22][8], sflagFormat[64];
	bool bEntitled = false;

	Format(sflagFormat, sizeof(sflagFormat), flagsNeed);
	ReplaceString(sflagFormat, sizeof(sflagFormat), " ", "");
	iCount = ExplodeString(sflagFormat, ",", sflagNeed, sizeof(sflagNeed), sizeof(sflagNeed[]));

	for (int i = 0; i < iCount; i++)
	{
		if ((GetUserFlagBits(client) & ReadFlagString(sflagNeed[i]) == ReadFlagString(sflagNeed[i])) || (GetUserFlagBits(client) & ADMFLAG_ROOT))
		{
			bEntitled = true;
			break;
		}
	}

	return bEntitled;
}


// Menu Handler for Panels
stock int Handler_NullCancel(Handle menu, MenuAction action, int param1, int param2)
{
	if (action == MenuAction_Select) 
	{
		switch (param2) 
		{
			default: // cancel
			{
				return;
			}
		}
	}

	return;
}

stock void StringToUpper(char [] sz)
{
	int len = strlen(sz);

	for (int i = 0; i < len; i++)
	{
		if (IsCharLower(sz[i]))
		{
			sz[i] = CharToUpper(sz[i]);
		}
	}
}

stock bool IsClientStuck(int client)
{
	float vOrigin[3];
	float vMins[3];
	float vMaxs[3];

	GetClientAbsOrigin(client, vOrigin);

	GetEntPropVector(client, Prop_Send, "m_vecMins", vMins);
	GetEntPropVector(client, Prop_Send, "m_vecMaxs", vMaxs);

	TR_TraceHullFilter(vOrigin, vOrigin, vMins, vMaxs, MASK_ALL, FilterOnlyPlayers, client);

	return TR_DidHit();
}

stock bool FilterOnlyPlayers(int entity, int contentsMask, any data)
{
	if(entity != data && entity > 0 && entity <= MaxClients)
		return true;
	
	return false;
}
