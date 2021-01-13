#pragma warning(disable : 6031)

#include "Main.h"

uintptr_t GamePid = 0;
uintptr_t GameBaseAddress = 0;
uintptr_t entitylist = 0;

uintptr_t nextAim = 0;
uintptr_t AimTarget = 0;
uintptr_t nextEntityInfoUpdate = 0;
uintptr_t nextAppUpdate = 0;
uintptr_t nextConsoleUpdate = 0;

float current_fov_limiter = 999.f;
bool printableOut = false;
bool targetDummiesToggled = 0;

int SMOOTH = 12;
int Spectators = 0;

bool enable_glow_hack = true;
bool enableTargetDummies = false;
bool enableTargetTeammate = false;

uintptr_t entityNum = 70;

float* vis_old = new float[(entityNum + (uintptr_t)1)];
typedef bool (Entity::* EntityPtrDef)();


uintptr_t eptr(EntityPtrDef method) {
    return *(uintptr_t*)&method;
}

void LoadProtectedFunctions() {
    uintptr_t t = milliseconds_now();
    BYTE xorkey = 0x0;
    for (DWORD i = 0; i < 8; i++) {
        xorkey = ((BYTE*)&t)[i];
        if (xorkey > 0x3 && xorkey < 0xf0) {
            break;
        }
    }
    if (xorkey <= 0x3 || xorkey >= 0xf0) {
        xorkey = 0x56;
    }

    addFunc({ LoadProtectedFunctions, (uintptr_t)CheckDriverStatus - (uintptr_t)LoadProtectedFunctions - 0x3, xorkey, false });
    addFunc({ CheckDriverStatus, (uintptr_t)GetProcessIdByName - (uintptr_t)CheckDriverStatus - 0x3, xorkey, false });
    addFunc({ GetProcessIdByName, (uintptr_t)milliseconds_now - (uintptr_t)GetProcessIdByName - 0x3, xorkey, false });
    addFunc({ milliseconds_now, (uintptr_t)ProcessPlayer - (uintptr_t)milliseconds_now - 0x3, xorkey, false });
    addFunc({ ProcessPlayer, (uintptr_t)UpdatePlayersInfo - (uintptr_t)ProcessPlayer - 0x3, xorkey, false });
    addFunc({ UpdatePlayersInfo, (uintptr_t)PredictPosition - (uintptr_t)UpdatePlayersInfo - 0x3, xorkey, false });
    addFunc({ PredictPosition, (uintptr_t)AutoBoneSwitch - (uintptr_t)PredictPosition - 0x3, xorkey, false });
    addFunc({ AutoBoneSwitch, (uintptr_t)SmoothType_Asist - (uintptr_t)AutoBoneSwitch - 0x3, xorkey, false });
    addFunc({ SmoothType_Asist, (uintptr_t)AimAngles - (uintptr_t)SmoothType_Asist - 0x3, xorkey, false });
    addFunc({ AimAngles, (uintptr_t)RunApp - (uintptr_t)AimAngles - 0x3, xorkey, false });
    addFunc({ RunApp, (uintptr_t)Configure - (uintptr_t)RunApp - 0x3, xorkey, false });
    addFunc({ Configure, (uintptr_t)mainThread - (uintptr_t)Configure - 0x3, xorkey, false });
    addFunc({ mainThread, (uintptr_t)DllMain - (uintptr_t)mainThread - 0x3, xorkey, false });
    addFunc({ DllMain, (uintptr_t)std::_Narrow_char_traits<char, int>::eof - (uintptr_t)DllMain - 0x3, xorkey, false });


    addFunc({ Driver::SendCommand, (uintptr_t)Driver::GetBaseAddress - (uintptr_t)Driver::SendCommand - 0x3, xorkey, false });
    addFunc({ Driver::GetBaseAddress, (uintptr_t)Driver::copy_memory - (uintptr_t)Driver::GetBaseAddress - 0x3, xorkey, false });
    addFunc({ Driver::copy_memory, (uintptr_t)GetKernelModuleExport - (uintptr_t)Driver::copy_memory - 0x3, xorkey, false });
    addFunc({ GetKernelModuleExport, (uintptr_t)GetKernelModuleAddress - (uintptr_t)GetKernelModuleExport - 0x3, xorkey, false });
    addFunc({ GetKernelModuleAddress, (uintptr_t)Driver::initialize - (uintptr_t)GetKernelModuleAddress - 0x3, xorkey, false });
    addFunc({ Driver::initialize, (uintptr_t)Driver::read_memory - (uintptr_t)Driver::initialize - 0x3, xorkey, false });
    addFunc({ Driver::read_memory, (uintptr_t)Driver::write_memory - (uintptr_t)Driver::read_memory - 0x3, xorkey, false });
    addFunc({ Driver::write_memory, (uintptr_t)getEntity - (uintptr_t)Driver::write_memory - 0x3, xorkey, false });
    addFunc({ getEntity, (uintptr_t)paddingsub - (uintptr_t)getEntity - 0x3, xorkey, false });

    
    addFunc({ (void*)eptr(&Entity::isPlayer), eptr((EntityPtrDef)&Entity::getBonePosition) - eptr(&Entity::isPlayer) - 0x3, xorkey, false });
    addFunc({ (void*)eptr((EntityPtrDef)&Entity::getBonePosition), eptr((EntityPtrDef)&Entity::GetViewAngles) - eptr((EntityPtrDef)&Entity::getBonePosition) - 0x3, xorkey, false });
    addFunc({ (void*)eptr((EntityPtrDef)&Entity::Observing), eptr((EntityPtrDef)&Entity::getTeamId) - eptr((EntityPtrDef)&Entity::Observing) - 0x3, xorkey, false });
    
    for (size_t i = 0; i < funcCount; i++) {
        if (functions[i].address != LoadProtectedFunctions)
            Protect(functions[i].address);
    }
    Unprotect(_ReturnAddress());
}

bool CheckDriverStatus() {
    int icheck = 29;
    NTSTATUS status = 0;
    SLog(L"getting base");
    Unprotect(Driver::GetBaseAddress);
    uintptr_t BaseAddr = Driver::GetBaseAddress(Driver::currentProcessId);
    SLog(std::wstring(L"base: "+std::to_wstring(BaseAddr)).c_str());
    if (BaseAddr == 0) {
        return false;
    }
    Protect(Driver::GetBaseAddress);

    SLog(L"reading var");
    int checked = Driver::read<int>(Driver::currentProcessId, (uintptr_t)&icheck, &status);
    SLog(std::wstring(L"read: " + std::to_wstring(checked)).c_str());
    if (checked != icheck) {
        return false;
    }
    
    return true;
}

DWORD GetProcessIdByName(wchar_t * name) {
    Protect(_ReturnAddress());

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (_wcsicmp(entry.szExeFile, name) == 0) {
                Unprotect(_ReturnAddress());
                return entry.th32ProcessID;
            }
        }
    }

    CloseHandle(snapshot);
    Unprotect(_ReturnAddress());
    return 0;
}

uintptr_t milliseconds_now() {
    static LARGE_INTEGER s_frequency;
    static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
    }
    else {
        return GetTickCount();
    }
}

struct GColor {
    float r, g, b;
};
struct GlowMode {
    BYTE GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel;
};
struct Fade {
    int a, b;
    float c, d, e, f;
};

/// <summary>
/// Update spectator count, aim to target, and the target's glow
/// </summary>
/// <param name="LPlayer">local player</param>
/// <param name="target">target player</param>
/// <param name="entitylist">list of entities</param>
/// <param name="index">current target's index id</param>
void ProcessPlayer(Entity* LPlayer, Entity* target, UINT64 entitylist, int index) {
    Protect(_ReturnAddress());
    auto fptr = &Entity::Observing;
    Unprotect((void*)*(uintptr_t*)&fptr);
    intptr_t obser = target->Observing(GamePid, entitylist);
    Protect((void*)*(uintptr_t*)&fptr);
    if (obser == LPlayer->ptr) {
        Spectators++;
    }
    if (obser != 0) { // Is an observer... nothing to do
        Unprotect(_ReturnAddress());
        return;
    }
    auto fptrBone = &Entity::getBonePosition;
    Unprotect((void*)*(uintptr_t*)&fptrBone);
    Vector BonePosition = target->getBonePosition(GamePid, 3);
    Protect((void*)*(uintptr_t*)&fptrBone);
    Vector LocalPlayerPosition = LPlayer->getPosition();
    float dist = LocalPlayerPosition.DistTo(BonePosition) / 39.62f;
    if (dist > Max_Distance || BonePosition.z > 10000.f) {
        Unprotect(_ReturnAddress());
        return;
    }

    int health = target->getHealth();
    if (health < 0 || health > 100) {
        Unprotect(_ReturnAddress());
        return;
    }

    int entity_team = target->getTeamId();
    if (entity_team < 0 || (entity_team > 31 && entity_team != 97)) {
        Unprotect(_ReturnAddress());
        return;
    }

    if (LocalPlayerPosition.z < 10000.f)
    {
        if (enable_glow_hack)
        {
            if ((int)target->buffer[GLOW_CONTEXT] != 1 || (int)target->buffer[GLOW_VISIBLE_TYPE] != 1 || (int)target->buffer[GLOW_FADE] != 872415232)
            {
                float currentEntityTime = 5000.f;//(float)target->buffer[0xEE4];
                if (!isnan(currentEntityTime) && currentEntityTime > 0.f)
                {
                    GlowMode mode = { 101,102,96,75 };
                    GColor color;
                    if ((target->getTeamId() == LPlayer->getTeamId()) && !enableTargetTeammate)
                    {
                        color = { 0.f, 2.f, 3.f };
                    }
                    else if (!(enableTargetDummies) && (target->isBleedOut() || !target->isOkLifeState()))
                    {
                        color = { 3.f, 3.f, 3.f };
                    }
                    else if (target->vis_time() > vis_old[index] || target->vis_time() < 0.f && vis_old[index] > 0.f)
                    {
                        color = { 0.f, 2.f, 0.f };
                    }
                    else
                    {
                        int shield = target->getShield();
                        //color = { 0.f, 2.f, 0.f };
                        if (shield > 100)
                        { //Heirloom armor - Red
                            color = { 3.f, 0.f, 0.f };
                        }
                        else if (shield > 75)
                        { //Purple armor - Purple
                            color = { 1.84f, 0.46f, 2.07f };
                        }
                        else if (shield > 50)
                        { //Blue armor - Light blue
                            color = { 0.39f, 1.77f, 2.85f };
                        }
                        else if (shield > 0)
                        { //White armor - White
                            color = { 2.f, 2.f, 2.f };
                        }
                        else if (health > 50)
                        { //Above 50% HP - Orange
                            color = { 3.5f, 1.8f, 0.f };
                        }
                        else
                        { //Below 50% HP - Light Red
                            color = { 3.28f, 0.78f, 0.63f };
                        }
                    }
                    //printf("Changed: %p\n", target->ptr);
                    Driver::write<GlowMode>(GamePid, target->ptr + GLOW_TYPE, mode);
                    Driver::write<GColor>(GamePid, target->ptr + GLOW_COLOR, color);

                    Driver::write<float>(GamePid, target->ptr + GLOW_DISTANCE, 40000.f);
                    Driver::write<float>(GamePid, target->ptr + GLOW_LIFE_TIME, currentEntityTime);
                    currentEntityTime -= 1.f;
                    Driver::write<int>(GamePid, target->ptr + GLOW_CONTEXT, 1);
                    Driver::write<int>(GamePid, target->ptr + GLOW_VISIBLE_TYPE, 1);
                    Driver::write<Fade>(GamePid, target->ptr + GLOW_FADE, { 872415232, 872415232, currentEntityTime, currentEntityTime, currentEntityTime, currentEntityTime });

                }
            }
        }
        else if (!enable_glow_hack)
        {
            if ((int)target->buffer[GLOW_CONTEXT] == 1)
            {
                Driver::write<int>(GamePid, target->ptr + GLOW_CONTEXT, 0);
            }
        }
    }

    if (!(enableTargetDummies) && (target->isBleedOut() || !target->isOkLifeState())) {
        Unprotect(_ReturnAddress());
        return;
    }

    if ((entity_team == LPlayer->getTeamId()) && !enableTargetTeammate) {
        Unprotect(_ReturnAddress());
        return;
    }

    if (target->vis_time() > vis_old[index] || target->vis_time() < 0.f && vis_old[index] > 0.f) {
        Vector BreathAngles = LPlayer->GetBreathAngles();
        Vector LocalCamera = LPlayer->GetCamPos();
        Vector Angle = Math::CalcAngle(LocalCamera, BonePosition);
        float fov = (float)Math::GetFov(BreathAngles, Angle, dist);
        if (fov < current_fov_limiter) {
            AimTarget = target->ptr;
            current_fov_limiter = fov;
        }
    }
    Unprotect(_ReturnAddress());
}

/// <summary>
/// For each player, process the player and update visible time
/// </summary>
/// <param name="LocalPlayer"></param>
void UpdatePlayersInfo(Entity * LocalPlayer) {
    Protect(_ReturnAddress());

    current_fov_limiter = 999.f;
    AimTarget = 0;
    Spectators = 0;
    
    if (targetDummiesToggled)
    {
        entityNum = enableTargetDummies ? 10000 : 70;
        delete[] vis_old;
        vis_old = nullptr;
        vis_old = new float[(entityNum + (uintptr_t)1)];
        targetDummiesToggled = false;
    }

    for (int i = 0; i <= entityNum; i++) { // 70
        uintptr_t centity = Driver::read<uintptr_t>(GamePid, entitylist + ((uintptr_t)i << 5));

        Unprotect(getEntity);
        Entity* Target = getEntity(GamePid, centity);
        Protect(getEntity);

        auto fptr = &Entity::isPlayer;
        Unprotect((void*)*(uintptr_t*)&fptr);
        if (!Target->isPlayer())
        {
            Protect((void*)*(uintptr_t*)&fptr);
            delete Target;
            continue;
        }
        Protect((void*)*(uintptr_t*)&fptr);

        if (centity == 0) continue;
        if (LocalPlayer->ptr == centity) continue;

        Unprotect(ProcessPlayer);
        ProcessPlayer(LocalPlayer, Target, entitylist, i);
        Protect(ProcessPlayer);
        vis_old[i] = Target->vis_time();
        delete Target;
    }

    Unprotect(_ReturnAddress());
}

void PredictPosition(Entity* LocalPlayer, Entity* target, Vector* BonePosition) {
    Protect(_ReturnAddress());
    uintptr_t current_weapon = LocalPlayer->CurrentWeapon(GamePid, entitylist);
    if (current_weapon != 0) { //if weapon found apply gravity & speed calculation
        float bulletSpeed = Driver::read<float>(GamePid, current_weapon + TOFFSET(OFFSET_BULLET_SPEED));
        float bulletGravity = Driver::read<float>(GamePid, current_weapon + TOFFSET(OFFSET_BULLET_GRAVITY));

        if (bulletSpeed > 1.0f) { //fix for charge rifle
            Vector muzzle = LocalPlayer->GetCamPos();
            float Time = BonePosition->DistTo(muzzle) / bulletSpeed;
            BonePosition->z += (700.f * bulletGravity * 0.5f) * (Time * Time);
            Vector velocity_delta = (target->GetVelocity() * Time);
            BonePosition->x += velocity_delta.x;
            BonePosition->y += velocity_delta.y;
            BonePosition->z += velocity_delta.z;
        }
    }
    Unprotect(_ReturnAddress());
}

uintptr_t nextBoneSwitch = 0;
uintptr_t StartTimeToAim = 0;
int CurrentTargetBone = 7;
int targets[] = {7,5,7}; // { 7,5,3,2 }
int action = 1;
int boneIndex = 0;
Vector lastSet;
bool TargetLocked = false;

void AutoBoneSwitch() {
    Protect(_ReturnAddress());
    Unprotect(milliseconds_now);
    if (nextBoneSwitch < milliseconds_now()) {
        boneIndex += action;
        if (boneIndex == 2) {
            action = -1;
        }
        else if (boneIndex == 0) {
            action = 1;
        }
        CurrentTargetBone = targets[boneIndex];
        nextBoneSwitch = milliseconds_now() + 100;
    }
    Protect(milliseconds_now);
    Unprotect(_ReturnAddress());
}

void SmoothType_Asist(float fov, float TargetDistance, Vector* Delta, int smooth_multiplier) {
    Protect(_ReturnAddress());
    float smooth = 0.f;
    if (TargetDistance < 10.f) {
        smooth = 6.f + (smooth_multiplier - 1.f) * 3.f;
    }
    else {
        smooth = 6.f + (smooth_multiplier + 1.f) * fov;
    }
    if (smooth > 0.1f) {
        Delta->x /= smooth;
        Delta->y /= smooth;
        Delta->z /= smooth;
    }
    Unprotect(_ReturnAddress());
}

int AimAngles(Entity* LocalPlayer, Entity* target, Vector * out) {
    Protect(_ReturnAddress());
    Vector LocalCamera = LocalPlayer->GetCamPos();
    auto fptr = &Entity::getBonePosition;
    Unprotect((void*)*(uintptr_t*)&fptr);
    Vector BonePosition = target->getBonePosition(GamePid, CurrentTargetBone);
    Protect((void*)*(uintptr_t*)&fptr);
    Vector EntityPosition = target->getPosition();
    if (BonePosition.x == 0 || BonePosition.y == 0 || //check wrong player position and bone position
        LocalCamera.x == 0 || LocalCamera.y == 0 || //check wrong camera
        (BonePosition.x == EntityPosition.x && BonePosition.y == EntityPosition.y) //checks wrong bone position
        ) {
        Unprotect(_ReturnAddress());
        return 0;
    }
    Unprotect(PredictPosition);
    PredictPosition(LocalPlayer, target, &BonePosition);
    Protect(PredictPosition);

    Vector CalculatedAngles = Math::CalcAngle(LocalCamera, BonePosition);
    Vector ViewAngles = LocalPlayer->GetViewAngles();
    Vector DynBreath = LocalPlayer->GetBreathAngles();

    if (DynBreath.x == 0 || DynBreath.y == 0 || //Something was wrong
        ViewAngles.x == 0 || ViewAngles.y == 0) {
        Unprotect(_ReturnAddress());
        return 0;
    }
    Math::NormalizeAngles(DynBreath);

    Vector LocalPlayerPosition = LocalPlayer->getPosition();
    float TargetDistance = LocalPlayerPosition.DistTo(EntityPosition) / 39.62f;

    double aimDistFromTarget = Math::GetFov(DynBreath, CalculatedAngles, TargetDistance); //fov based in distance to the target and angles (like create an sphere around the target, fov is the radius
    if (aimDistFromTarget > 1.5f || TargetDistance > Max_Distance) {
        Unprotect(_ReturnAddress());
        return 0;
    }

    Vector Delta = CalculatedAngles - DynBreath;
    Math::NormalizeAngles(Delta);

    Unprotect(AutoBoneSwitch);
    AutoBoneSwitch();
    Protect(AutoBoneSwitch);

    Vector RecoilVec = LocalPlayer->GetRecoil();
    if (RecoilVec.x != 0 || RecoilVec.y != 0) {
        Delta -= (RecoilVec * (SMOOTH * 0.025)); //Scale up/down based on smooth
        Math::NormalizeAngles(Delta);
    }

    float aimDistFromTargetBone = (float)Math::GetFov2(DynBreath, CalculatedAngles);
    Unprotect(SmoothType_Asist);
    SmoothType_Asist(aimDistFromTargetBone, TargetDistance , &Delta, SMOOTH);
    Protect(SmoothType_Asist);

    Math::NormalizeAngles(Delta);

    Vector SmoothedAngles = ViewAngles + Delta;
    Math::NormalizeAngles(SmoothedAngles);
    if (lastSet == SmoothedAngles) {
        Unprotect(_ReturnAddress());
        return 2;
    }
    out->x = SmoothedAngles.x;
    out->y = SmoothedAngles.y;
    out->z = SmoothedAngles.z;
    Unprotect(_ReturnAddress());
    return 1;
}

void RunApp() {
    Protect(_ReturnAddress());
    entitylist = GameBaseAddress + TOFFSET(OFFSET_ENTITYLIST);
    uintptr_t lastAimTarget = 0;
    int activeKey = VK_RBUTTON;
    int altActiveKey = 0x36; // 6 key;
    
    while (true) {
        uintptr_t lptr = Driver::read<uintptr_t>(GamePid, GameBaseAddress + TOFFSET(OFFSET_LOCAL_ENT));
        if (lptr == 0) break;

        int signOnState = Driver::read<uintptr_t>(GamePid, GameBaseAddress + TOFFSET(OFFSET_CLIENT_SIGNON_STATE));
        // -1150273734 at first start 
        bool isConnected = (signOnState == 8);
        //std::cout <<         "\nsignOnState: " << signOnState;
        //if (signOnState >= 0 && signOnState <= 9)
        //{ 
        //    switch (signOnState)
        //    {
        //        case 0: 
        //            std::cout << " : None";
        //            break;
        //        case 1:
        //            std::cout << " : Challenge";
        //            break;
        //        case 2:
        //            std::cout << " : Connected";
        //            break;
        //        case 3:
        //            std::cout << " : StateNew";
        //            break;
        //        case 4:
        //            std::cout << " : Prespawn";
        //            break;
        //        case 5:
        //            std::cout << " : GettingData";
        //            break;
        //        case 6:
        //            std::cout << " : Spawn";
        //            break;
        //        case 7:
        //            std::cout << " : FirstSnap";
        //            break;
        //        case 8:
        //            std::cout << " : Full";
        //            break;
        //        case 9:
        //            std::cout << " : ChangeLevel";
        //            break;
        //    }
        //}
        //std::cout << std::endl;

        if (isConnected)
        {
            // game is connected
            uintptr_t currentArea = {};
            currentArea = Driver::read<uintptr_t>(GamePid, GameBaseAddress + TOFFSET(OFFSET_CLIENT_LEVEL_NAME));

            if (char* lvlStr = (char*) &currentArea)
            {
                // has valid area string
                if (!(strcmp(lvlStr, "mp_lobby") == 0))
                {
                    // not in lobby
                    Unprotect(getEntity);
                    Entity* LocalPlayer = getEntity(GamePid, lptr);
                    Protect(getEntity);

                    auto fptr = &Entity::isPlayer;
                    Unprotect((void*)*(uintptr_t*)&fptr);
                    if (!LocalPlayer->isPlayer()) {
                        Protect((void*)*(uintptr_t*)&fptr);
                        delete LocalPlayer;
                        break;
                    }
                    Protect((void*)*(uintptr_t*)&fptr);

                    Unprotect(milliseconds_now);
                    if (nextEntityInfoUpdate < milliseconds_now()) {
                        Protect(milliseconds_now);

                        Unprotect(UpdatePlayersInfo);
                        UpdatePlayersInfo(LocalPlayer);
                        Protect(UpdatePlayersInfo);
            
                        Unprotect(milliseconds_now);
                        nextEntityInfoUpdate = milliseconds_now() + 50; //update info
                        Protect(milliseconds_now);
                    }

                    // update app
                    Unprotect(milliseconds_now);
                    if (nextConsoleUpdate < milliseconds_now())
                    {
                        Protect(milliseconds_now);
                        // print msgs
                        char sp_str[] = { '\n', 'S','p','e','c','t','a','t','o','r','s',':',' ','%','u','\n','\n','\0' };
                        printf(sp_str, Spectators);
                        memset(sp_str, 0, sizeof(sp_str));

                        char smthMsg[] = { 'S','m','o','o','t','h',' ','(','+','/','-',')',' ',':',' ','%','u','\n','\0' };
                        printf(smthMsg, SMOOTH);
                        memset(smthMsg, 0, sizeof(smthMsg));
                        
                        Unprotect(milliseconds_now);
                        nextConsoleUpdate = milliseconds_now() + 2000;
                        Protect(milliseconds_now);
                    }

                    Unprotect(milliseconds_now);
                    bool primaryKeyPressed = (GetKeyState(activeKey) & 0x8000);
                    bool key_pressed = primaryKeyPressed ? true : (GetKeyState(altActiveKey) & 0x8000);
                    //bool key_pressed = (GetKeyState(altActiveKey) & 0x8000);

                    if (AimTarget > 0 && key_pressed && nextAim < milliseconds_now()) {
                        Protect(milliseconds_now);

                        if (lastAimTarget != AimTarget) {
                            TargetLocked = false;
                            Unprotect(milliseconds_now);
                            StartTimeToAim = milliseconds_now();
                            Protect(milliseconds_now);
                            lastAimTarget = AimTarget;
                        }
                        Unprotect(getEntity);
                        Entity* target = getEntity(GamePid, AimTarget);
                        Protect(getEntity);

                        Vector result = { 0.f,0.f,0.f };

                        Unprotect(AimAngles);
                        int status = AimAngles(LocalPlayer, target, &result);
                        Protect(AimAngles);
                        if (status == 1) { // 1 = movement needed, 2 = view already there, 0 = some out of aimbot params
                            LocalPlayer->SetViewAngles(GamePid, result);
                        }
                        else if (status == 0) {
                            TargetLocked = false;
                            Unprotect(milliseconds_now);
                            StartTimeToAim = milliseconds_now();
                            Protect(milliseconds_now);
                        }

                        delete target;
                        Unprotect(milliseconds_now);
                        nextAim = milliseconds_now() + 16; //16 = ~60 movements per second
                        Protect(milliseconds_now);
                    }
                    else if (!key_pressed || AimTarget == 0) {

                        TargetLocked = false;
                        Unprotect(milliseconds_now);
                        StartTimeToAim = milliseconds_now();
                        Protect(milliseconds_now);

                        //ProtectedSleep(2);
                    }
                    Protect(milliseconds_now);
        
                    delete LocalPlayer;
                }
            }

            #pragma region SettingsUpdate

            Unprotect(milliseconds_now);
            if (nextAppUpdate < milliseconds_now())
            {
                Protect(milliseconds_now);
                // update smooth value (NUM +/-)
                if ((GetAsyncKeyState(VK_ADD) & 0x1) != 0)
                {
                    SMOOTH += 1;
                    if (SMOOTH > 20) SMOOTH = 20;

                    char smthMsg[] = { 'S','m','o','o','t','h',' ','(','+','/','-',')',' ',':',' ','%','u','\n','\0' };
                    printf(smthMsg, SMOOTH);
                    memset(smthMsg, 0, sizeof(smthMsg));
                }

                if ((GetAsyncKeyState(VK_SUBTRACT) & 0x1) != 0)
                {
                    SMOOTH -= 1;
                    if (SMOOTH < 1) SMOOTH = 1;

                    char smthMsg[] = { 'S','m','o','o','t','h',' ','(','+','/','-',')',' ',':',' ','%','u','\n','\0' };
                    printf(smthMsg, SMOOTH);
                    memset(smthMsg, 0, sizeof(smthMsg));
                }

                // update glow toggle (NUM-*)
                if ((GetAsyncKeyState(VK_MULTIPLY) & 0x1) != 0)
                {
                    enable_glow_hack = !enable_glow_hack;

                    char glwMsg[] = { 'G','l','o','w',' ','(','*',')',' ',':',' ','%','u','\n','\0' };
                    printf(glwMsg, enable_glow_hack);
                    memset(glwMsg, 0, sizeof(glwMsg));
                }

                // update target dummie toggle (NUM-/)
                if (!targetDummiesToggled && ((GetAsyncKeyState(VK_DIVIDE) & 0x1) != 0))
                {
                    enableTargetDummies = !enableTargetDummies;
                    targetDummiesToggled = true;

                    char rngMsg[] = { 'T','a','r','g','e','t',' ','R','a','n','g','e',' ','(','/',')',' ',':',' ','%','u','\n','\0' };
                    printf(rngMsg, enableTargetDummies);
                    memset(rngMsg, 0, sizeof(rngMsg));
                }

                // update target teammates (NUM-7)
                if ((GetAsyncKeyState(VK_NUMPAD7) & 0x1) != 0)
                {
                    enableTargetTeammate = !enableTargetTeammate;

                    char tmmtMsg[] = { 'T','a','r','g','e','t',' ','T','e','a','m','m','a','t','e','s',' ','(','N','U','M','-','7',')',' ',':',' ','%','u','\n','\0' };
                    printf(tmmtMsg, enableTargetTeammate);
                    memset(tmmtMsg, 0, sizeof(tmmtMsg));
                }

                Unprotect(milliseconds_now);
                nextAppUpdate = milliseconds_now() + 500;
                Protect(milliseconds_now);
            }
            #pragma endregion
        }
        else
        {
            const char msg[] = { 'W','a','i','t','i','n','g',' ','t','o',' ','C','o','n','n','e','c','t','.',' ','.',' ','.','\t','\0' };
            std::cout << msg;
            ProtectedSleep(2000);
        }
    }
    Unprotect(_ReturnAddress());
}

void Configure() {

    HWND consoleWnd = GetConsoleWindow();
    if (consoleWnd == NULL)
    {
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
    }
    else {
        printableOut = true;
    }

    std::cout << '\n';

    if (consoleWnd == NULL) {
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);
        FreeConsole();
    }

}

DWORD WINAPI mainThread(PVOID) {
    SLog(L"Protecting");
    LoadProtectedFunctions();
    Protect(LoadProtectedFunctions);
    SLog(L"Connecting driver");
    Unprotect(Driver::initialize);
    Unprotect(CheckDriverStatus);
    if (!Driver::initialize() || !CheckDriverStatus()) {
        wchar_t VarName[] = { 'F','a','s','t','B','o','o','t','O','p','t','i','o','n','\0' };
        UNICODE_STRING FVariableName = UNICODE_STRING();
        FVariableName.Buffer = VarName;
        FVariableName.Length = 28;
        FVariableName.MaximumLength = 30;
        //UNICODE_STRING VariableName = RTL_CONSTANT_STRING(VARIABLE_NAME);
        myNtSetSystemEnvironmentValueEx(
            &FVariableName,
            &DummyGuid,
            0,
            0,
            ATTRIBUTES);//delete var
        memset(VarName, 0, sizeof(VarName));
        //memset(VariableName.Buffer, 0, VariableName.Length);
        //VariableName.Length = 0;
        Beep(600, 1000);
        char tx[] = { 'N','O',' ','E','F','I',' ',';','(','\n', 0 };
        printf(tx);
        ProtectedSleep(3000);
        exit(1);
        return 1;
    }
    Protect(Driver::initialize);
    Protect(CheckDriverStatus);

    //Beep(900, 200);
    //Beep(1100, 200);
    Beep(1300, 300);
    //Beep(1500, 300);

    Unprotect(Configure);
    Configure();
    Protect(Configure);

    while (true) {
        wchar_t name[] = { 'r','5','a','p','e','x','.','e','x','e', 0 };
        //wchar_t name[] = { 'E','a','s','y','A','n','t','i','C','h','e','a','t','_','l','a','u','n','c','h','e','r','.','e','x','e', 0 };
        Unprotect(GetProcessIdByName);
        DWORD pid = GetProcessIdByName(name);
        Protect(GetProcessIdByName);
        memset(name, 0, sizeof(name));

        Unprotect(Driver::GetBaseAddress);
        uintptr_t BaseAddr = Driver::GetBaseAddress(pid);
        Protect(Driver::GetBaseAddress);
       
        if (BaseAddr != 0) {
            GamePid = pid;
            GameBaseAddress = BaseAddr;
            
            Unprotect(RunApp);
            RunApp();
            Protect(RunApp);

            GamePid = 0;
            GameBaseAddress = 0;
        }
        char msg[] = { 'W','a','i','t','i','n','g',' ','f','o','r',' ','g','a','m','e','.',' ','.',' ','.','\t','\0' };
        std::cout << msg;
        ProtectedSleep(2000);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE, DWORD ul_reason_for_call, LPVOID) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        SLog(L"Attach");
        HANDLE thread = CreateThread(NULL, NULL, mainThread, NULL, NULL, NULL);
        if (thread) 
            CloseHandle(thread);
    }
    return TRUE;
}

