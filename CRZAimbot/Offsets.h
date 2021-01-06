#pragma once

//#define OFFSET_RAN 0//0x34LLU
#define TOFFSET(d1) d1 //((d1)+offset_ran_var)

extern unsigned long long offset_ran_var;

#define ENTITY_SIZE_DEF				    0x43C4			//biggest entity offset
#define OFFSET_VISIBLE_TIME             0x1AEC          //target visible counter

#define OFFSET_ENTITYLIST			    0x18CDCE8		//cl_entitylist
#define OFFSET_LOCAL_ENT			    0x1C7Cf28		//LocalPlayer

#define OFFSET_CLIENT_STATE             0x1281D00
#define OFFSET_CLIENT_SIGNON_STATE      (OFFSET_CLIENT_STATE + 0x0098)
#define OFFSET_CLIENT_LEVEL_NAME        (OFFSET_CLIENT_STATE + 0x01b0)

#define OFFSET_TEAM					    0x04B0			//m_iTeamNum
#define OFFSET_HEALTH				    0x04A0			//m_iHealth
#define OFFSET_NAME					    0x05E9			//m_iName
#define OFFSET_SIG_NAME				    0x05E0			//m_iSignifierName
#define OFFSET_SHIELD				    0x0170			//m_shieldHealth
#define OFFSET_MAX_SHIELD			    0x0174			//m_shieldHealth +0x4

#define OFFSET_LIFE_STATE			    0x07F8			//m_lifeState
#define OFFSET_BLEED_OUT_STATE		    0x26B0			//m_bleedoutState

#define OFFSET_ITEM_ID				    0x1688			//m_customScriptInt

#define OFFSET_BULLET_SPEED			    0x1EA8			//m_flProjectileSpeed
#define OFFSET_BULLET_GRAVITY		    0x1EB0			//m_flProjectileScale

#define OFFSET_CURRENT_WEAPON		    0x1A8C			//m_latestPrimaryWeapons
#define OFFSET_ORIGIN				    0x014C			//m_vecAbsOrigin
#define OFFSET_BONES				    0x0F98			//m_nForceBone + 0x50-0x8 -> 48 8B 97 ?? ?? ?? ?? 48 8D 04 5B 48 C1
#define OFFSET_AIMPUNCH				    0x2450			//m_vecPunchWeapon_Angle
#define OFFSET_CAMERAPOS			    0x1EF0			//40 57 48 83 EC 20 F3 0F 10 0A 48 8B F9 0F 2E 89 first ucomiss result
#define OFFSET_VIEWANGLES			    0x2528			//m_ammoPoolCapacity - 0x14  -> 0F 57 C0 8B 86 ?? ?? ?? ?? F3 segundo uso de un 0x24AX
#define OFFSET_BREATH_ANGLES		    (OFFSET_VIEWANGLES - 0x10)
#define OFFSET_OBSERVER_MODE		    0x3364			//m_iObserverMode
#define OFFSET_OBSERVING_TARGET		    0x3368			//m_hObserverTarget

#define OFFSET_HELMET_TYPE			    0x4328			//m_helmetType

#define GLOW_CONTEXT                    0x03C8 //Script_Highlight_SetCurrentContext
#define GLOW_LIFE_TIME                  0x03A4 //Script_Highlight_SetLifeTime + 4
#define GLOW_DISTANCE                   0x03BC //Script_Highlight_SetFarFadeDist
#define GLOW_TYPE                       0x02C4 //Script_Highlight_GetState + 4
#define GLOW_COLOR                      0x01D0 //Script_CopyHighlightState mov tcx nº7 
#define GLOW_VISIBLE_TYPE               0x03D0 //Script_Highlight_SetVisibilityType
#define GLOW_FADE                       0x0388 //Script_Highlight_GetCurrentInsideOpacity 3º result of 3 offsets consecutive or first + 8~

#define OFFSET_MATRIX				    0x001B3BD0		//ViewMatrix
#define OFFSET_RENDER				    0x0407DEF8 	 	//ViewRender