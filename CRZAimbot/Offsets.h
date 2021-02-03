#pragma once

//#define OFFSET_RAN 0//0x34LLU
#define TOFFSET(d1) d1 //((d1)+offset_ran_var)

extern unsigned long long offset_ran_var;

#define ENTITY_SIZE_DEF				    0x430C			//last DT_Player offset + size of its type

#define OFFSET_ENTITYLIST		        0x18C62F8		//Miscellaneous cl_entitylist
#define OFFSET_LOCAL_ENT			    0x1C75558		//Miscellaneous LocalPlayer

#define OFFSET_CLIENT_STATE             0x1281D00       //Miscellaneous ClientState
#define OFFSET_CLIENT_SIGNON_STATE      (OFFSET_CLIENT_STATE + 0x0098)      //Miscellaneous SignonState
#define OFFSET_CLIENT_LEVEL_NAME        (OFFSET_CLIENT_STATE + 0x01b0)      //Miscellaneous LevelName

#define OFFSET_TEAM					    0x0450			//DT_BaseEntity.m_iTeamNum
#define OFFSET_HEALTH				    0x0440			//DT_Player.m_iHealth
#define OFFSET_NAME					    0x0589			//DT_BaseEntity.m_iName
#define OFFSET_SIG_NAME				    0x0580			//DT_BaseEntity.m_iSignifierName
#define OFFSET_SHIELD				    0x0170			//DT_BaseEntity.m_shieldHealth
#define OFFSET_MAX_SHIELD			    (OFFSET_SHIELD + 0x0004)        //DT_BaseEntity.m_shieldHealth +0x4
#define OFFSET_VISIBLE_TIME             0x1A4C          //lastVisibleTime or aLastvisibletim in disassmbler

#define OFFSET_LIFE_STATE			    0x0798			//DT_Player.m_lifeState
#define OFFSET_BLEED_OUT_STATE		    0x25f0			//DT_Player.m_bleedoutState

#define OFFSET_ITEM_ID				    0x1618			//DT_PropSurvival.m_customScriptInt

#define OFFSET_BULLET_SPEED			    0x1E50			//  aSpeedFIsGreate -> F3 0F 10 8B ?? ?? 00 00  F3 0F 10 83 9C 1C 00 00  1st movss   xmm1, dword ptr [rbx+OFFSETh] in disassembler
#define OFFSET_BULLET_GRAVITY		    (OFFSET_BULLET_SPEED + 0x0008)      //  speed + 0x0008 -> F3 0F 10 93 ?? ?? 00 00  F3 0F 5E C1 F3 0F 59 50  3rd movss   xmm2, dword ptr [rbx+OFFSETh]

#define OFFSET_CURRENT_WEAPON		    0x19EC			//DT_BaseCombatCharacter.m_latestPrimaryWeapons
#define OFFSET_ORIGIN				    0x014C			//C_BaseEntity.m_vecAbsOrigin
#define OFFSET_BONES				    0x0F38			//DT_BaseAnimating.m_nForceBone + 0x50-0x8 -> 48 8B 97 ?? ?? ?? ?? 48 8D 04 5B 48 C1 in disassembler
#define OFFSET_AIMPUNCH				    0x2390			//C_Player.m_vecPunchWeapon_Angle
#define OFFSET_CAMERAPOS			    0x1E30			//40 57 48 83 EC 20 F3 0F 10 0A 48 8B F9 0F 2E 89 first ucomiss result in disassembler
#define OFFSET_VIEWANGLES			    0x2470			//C_Player.m_ammoPoolCapacity - 0x14  -> 0F 57 C0 8B 86 ?? ?? ?? ?? F3 segundo uso de un 0x24AX -> movsd   xmm6, qword ptr [rsi+OFFSETh]
#define OFFSET_BREATH_ANGLES		    (OFFSET_VIEWANGLES - 0x10)
#define OFFSET_OBSERVER_MODE		    0x32A4			//DT_Player.m_iObserverMode
#define OFFSET_OBSERVING_TARGET		    (OFFSET_OBSERVER_MODE + 0x0004)     //DT_Player.m_hObserverTarget

#define OFFSET_HELMET_TYPE			    0x4270			//CPlayer.m_helmetType

#define GLOW_CONTEXT                    0x03C8 //Script_Highlight_SetCurrentContext
#define GLOW_LIFE_TIME                  0x03A4 //Script_Highlight_SetLifeTime + 4
#define GLOW_DISTANCE                   0x03B4 //Script_Highlight_SetFarFadeDist
#define GLOW_TYPE                       0x02C4 //Script_Highlight_GetState + 4
#define GLOW_COLOR                      0x01D0 //Script_CopyHighlightState mov tcx nº7 
#define GLOW_VISIBLE_TYPE               0x03D0 //Script_Highlight_SetVisibilityType
#define GLOW_FADE                       0x0388 //Script_Highlight_GetCurrentInsideOpacity 3º result of 3 offsets consecutive or first + 8~

#define OFFSET_MATRIX				    0x001B3BD0		//Miscellaneous ViewMatrix
#define OFFSET_RENDER				    0x04076FC0 	 	//Miscellaneous ViewRender