#ifndef _H_UTILITY_H_
#define _H_UTILITY_H_
#if __has_include("ep_private.h")
//#define USE_PRIVATE_INTERFACES
#endif
#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.data.xml.dom.h>
#include <accctrl.h>
#include <aclapi.h>
#include <sddl.h>
#include <Shobjidl.h>
#include <Shlobj_core.h>
#include <restartmanager.h>
#pragma comment(lib, "Rstrtmgr.lib")
#define _LIBVALINET_INCLUDE_UNIVERSAL
#ifndef __cplusplus
#include <valinet/universal/toast/toast.h>
#endif
#include "osutility.h"
#include "queryversion.h"
#pragma comment(lib, "Psapi.lib")
#include <activscp.h>
#include <netlistmgr.h>
#include <Psapi.h>
#include <stdbool.h>
#include "Localization.h"

#include "def.h"

#define WM_MSG_GUI_SECTION WM_USER + 1
#define WM_MSG_GUI_SECTION_GET 1

#ifdef __cplusplus
#define EP_INLINE inline
#else
#define EP_INLINE
#endif

#ifdef __cplusplus
extern "C" {
#endif

DEFINE_GUID(CLSID_ImmersiveShell,
    0xc2f03a33,
    0x21f5, 0x47fa, 0xb4, 0xbb,
    0x15, 0x63, 0x62, 0xa2, 0xf2, 0x39
);

DEFINE_GUID(IID_OpenControlPanel,
    0xD11AD862,
    0x66De, 0x4DF4, 0xBf, 0x6C,
    0x1F, 0x56, 0x21, 0x99, 0x6A, 0xF1
);

DEFINE_GUID(CLSID_VBScript,
    0xB54F3741, 
    0x5B07, 0x11CF, 0xA4, 0xB0, 
    0x00, 0xAA, 0x00, 0x4A, 0x55, 0xE8
);

DEFINE_GUID(CLSID_NetworkListManager,
    0xDCB00C01, 0x570F, 0x4A9B, 0x8D, 0x69, 0x19, 0x9F, 0xDB, 0xA5, 0x72, 0x3B);

DEFINE_GUID(IID_NetworkListManager,
    0xDCB00000, 0x570F, 0x4A9B, 0x8D, 0x69, 0x19, 0x9F, 0xDB, 0xA5, 0x72, 0x3B);

typedef struct _StuckRectsData
{
    int pvData[6];
    RECT rc;
    POINT pt;
} StuckRectsData;

HRESULT FindDesktopFolderView(REFIID riid, void** ppv);

HRESULT GetDesktopAutomationObject(REFIID riid, void** ppv);

HRESULT ShellExecuteFromExplorer(
    PCWSTR pszFile,
    PCWSTR pszParameters,
    PCWSTR pszDirectory,
    PCWSTR pszOperation,
    int nShowCmd
);

void ToggleTaskbarAutohide();

#pragma region "Enable old taskbar"
typedef interface ITrayUIHost ITrayUIHost;

typedef interface ITrayUI ITrayUI;

DEFINE_GUID(IID_ITrayUI,
    0x12b454e1,
    0x6e50, 0x42b8, 0xbc, 0x3e,
    0xae, 0x7f, 0x54, 0x91, 0x99, 0xd6
);

DEFINE_GUID(IID_ITrayUIComponent,
    0x27775f88,
    0x01d3, 0x46ec, 0xa1, 0xc1,
    0x64, 0xb4, 0xc0, 0x9b, 0x21, 0x1b
);

typedef HRESULT(*TrayUI_CreateInstance_t)(ITrayUIHost* host, REFIID riid, void** ppv);
EP_INLINE TrayUI_CreateInstance_t explorer_TrayUI_CreateInstanceFunc;
#pragma endregion

inline int FileExistsW(wchar_t* file)
{
    WIN32_FIND_DATAW FindFileData;
    HANDLE handle = FindFirstFileW(file, &FindFileData);
    int found = handle != INVALID_HANDLE_VALUE;
    if (found)
    {
        FindClose(handle);
    }
    return found;
}

// https://stackoverflow.com/questions/1672677/print-a-guid-variable
void printf_guid(GUID guid);

#ifdef _DEBUG
LRESULT CALLBACK BalloonWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

__declspec(dllexport) int CALLBACK ZZTestBalloon(HWND hWnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);

__declspec(dllexport) int CALLBACK ZZTestToast(HWND hWnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);
#endif

#ifndef EP_BUILD_SETUP
__declspec(dllexport) int CALLBACK ZZLaunchExplorer(HWND hWnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);

__declspec(dllexport) int CALLBACK ZZLaunchExplorerDelayed(HWND hWnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);

__declspec(dllexport) int CALLBACK ZZRestartExplorer(HWND hWnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow);
#endif

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

typedef LSTATUS(*t_SHRegGetValueFromHKCUHKLM)(
    PCWSTR pwszKey,
    PCWSTR pwszValue,
    int/*SRRF*/ srrfFlags,
    DWORD* pdwType,
    void* pvData,
    DWORD* pcbData
);
EP_INLINE t_SHRegGetValueFromHKCUHKLM SHRegGetValueFromHKCUHKLMFunc;

inline LSTATUS SHRegGetValueFromHKCUHKLMWithOpt(
    PCWSTR pwszKey,
    PCWSTR pwszValue,
    REGSAM samDesired,
    void* pvData,
    DWORD* pcbData
)
{
    LSTATUS lRes = ERROR_FILE_NOT_FOUND;
    HKEY hKey = NULL;

    RegOpenKeyExW(
        HKEY_CURRENT_USER,
        pwszKey,
        0,
        samDesired,
        &hKey
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        lRes = RegQueryValueExW(
            hKey,
            pwszValue,
            0,
            NULL,
            (LPBYTE)pvData,
            pcbData
        );
        RegCloseKey(hKey);
        if (lRes == ERROR_SUCCESS || lRes == ERROR_MORE_DATA)
        {
            return lRes;
        }
    }
    RegOpenKeyExW(
        HKEY_LOCAL_MACHINE,
        pwszKey,
        0,
        samDesired,
        &hKey
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        lRes = RegQueryValueExW(
            hKey,
            pwszValue,
            0,
            NULL,
            (LPBYTE)pvData,
            pcbData
        );
        RegCloseKey(hKey);
        if (lRes == ERROR_SUCCESS || lRes == ERROR_MORE_DATA)
        {
            return lRes;
        }
    }
    return lRes;
}

EP_INLINE HWND(WINAPI* CreateWindowInBand)(
    _In_ DWORD dwExStyle,
    _In_opt_ LPCWSTR lpClassName,
    _In_opt_ LPCWSTR lpWindowName,
    _In_ DWORD dwStyle,
    _In_ int X,
    _In_ int Y,
    _In_ int nWidth,
    _In_ int nHeight,
    _In_opt_ HWND hWndParent,
    _In_opt_ HMENU hMenu,
    _In_opt_ HINSTANCE hInstance,
    _In_opt_ LPVOID lpParam,
    DWORD band
);

EP_INLINE BOOL(WINAPI* GetWindowBand)(HWND hWnd, PDWORD pdwBand);

EP_INLINE BOOL(WINAPI* SetWindowBand)(HWND hWnd, HWND hwndInsertAfter, DWORD dwBand);

EP_INLINE INT64(*SetWindowCompositionAttribute)(HWND, void*);

// uxtheme.dll private functions

typedef enum IMMERSIVE_COLOR_TYPE
{
    // Defining only used ones
    IMCLR_SystemAccentLight2 = 2,
    IMCLR_SystemAccentDark2 = 6
} IMMERSIVE_COLOR_TYPE;

typedef struct IMMERSIVE_COLOR_PREFERENCE
{
    DWORD crStartColor;
    DWORD crAccentColor;
} IMMERSIVE_COLOR_PREFERENCE;

typedef enum IMMERSIVE_HC_CACHE_MODE
{
    IHCM_USE_CACHED_VALUE = 0,
    IHCM_REFRESH = 1
} IMMERSIVE_HC_CACHE_MODE;

typedef void(*GetThemeName_t)(void*, void*, void*); // 74
EP_INLINE GetThemeName_t GetThemeName;

typedef bool(*RefreshImmersiveColorPolicyState_t)(); // 104
EP_INLINE RefreshImmersiveColorPolicyState_t RefreshImmersiveColorPolicyState;

typedef bool(*GetIsImmersiveColorUsingHighContrast_t)(IMMERSIVE_HC_CACHE_MODE); // 106
EP_INLINE GetIsImmersiveColorUsingHighContrast_t GetIsImmersiveColorUsingHighContrast;

typedef HRESULT(*GetUserColorPreference_t)(IMMERSIVE_COLOR_PREFERENCE*, bool); // 120
EP_INLINE GetUserColorPreference_t GetUserColorPreference;

typedef DWORD(*GetColorFromPreference_t)(const IMMERSIVE_COLOR_PREFERENCE*, IMMERSIVE_COLOR_TYPE, bool, IMMERSIVE_HC_CACHE_MODE); // 121
EP_INLINE GetColorFromPreference_t GetColorFromPreference;

typedef bool(*ShouldAppsUseDarkMode_t)(); // 132
EP_INLINE ShouldAppsUseDarkMode_t ShouldAppsUseDarkMode;

typedef void(*AllowDarkModeForWindow_t)(HWND hWnd, BOOL bAllowDark); // 133
EP_INLINE AllowDarkModeForWindow_t AllowDarkModeForWindow;

typedef void(*SetPreferredAppMode_t)(BOOL bAllowDark); // 135
EP_INLINE SetPreferredAppMode_t SetPreferredAppMode;

typedef bool(*ShouldSystemUseDarkMode_t)(); // 138
EP_INLINE ShouldSystemUseDarkMode_t ShouldSystemUseDarkMode;

void* ReadFromFile(wchar_t* wszFileName, DWORD* dwSize);

int ComputeFileHash(LPCWSTR filename, LPSTR hash, DWORD dwHash);

int ComputeFileHash2(HMODULE hModule, LPCWSTR filename, LPSTR hash, DWORD dwHash);

void GetHardcodedHash(LPCWSTR wszPath, LPSTR hash, DWORD dwHash);

void LaunchPropertiesGUI(HMODULE hModule);

BOOL SystemShutdown(BOOL reboot);

LSTATUS RegisterDWMService(DWORD dwDesiredState, DWORD dwOverride);

char* StrReplaceAllA(const char* s, const char* oldW, const char* newW, int* dwNewSize);

WCHAR* StrReplaceAllW(const WCHAR* s, const WCHAR* oldW, const WCHAR* newW, int* dwNewSize);

HRESULT InputBox(BOOL bPassword, HWND hWnd, LPCWSTR wszPrompt, LPCWSTR wszTitle, LPCWSTR wszDefault, LPWSTR wszAnswer, DWORD cbAnswer, BOOL* bCancelled);

BOOL GetLogonSid(PSID* ppsid);

BOOL PrepareSecurityDescriptor(PSID pMainSid, DWORD dwMainPermissions, PSID pSecondarySid, DWORD dwSecondayPermissions, PSECURITY_DESCRIPTOR* ppSD);

inline BOOL IsHighContrast()
{
    HIGHCONTRASTW highContrast;
    ZeroMemory(&highContrast, sizeof(HIGHCONTRASTW));
    highContrast.cbSize = sizeof(highContrast);
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(highContrast), &highContrast, FALSE))
        return highContrast.dwFlags & HCF_HIGHCONTRASTON;
    return FALSE;
}

// https://codereview.stackexchange.com/questions/29198/random-string-generator-in-c
static inline WCHAR* rand_string(WCHAR* str, size_t size)
{
    const WCHAR charset[] = L"abcdefghijklmnopqrstuvwxyz";
    if (size) {
        --size;
        for (size_t n = 0; n < size; n++) {
            int key = rand() % (int)((sizeof(charset) / sizeof(WCHAR)) - 1);
            str[n] = charset[key];
        }
        str[size] = L'\0';
    }
    return str;
}

inline long long milliseconds_now() {
    LARGE_INTEGER s_frequency;
    BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return (1000LL * now.QuadPart) / s_frequency.QuadPart;
    }
    else {
        return GetTickCount();
    }
}

inline BOOL IsAppRunningAsAdminMode()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    // Allocate and initialize a SID of the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Determine whether the SID of administrators group is enabled in 
    // the primary access token of the process.
    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError)
    {
        return FALSE;
    }

    return fIsRunAsAdmin;
}

inline BOOL IsDesktopWindowAlreadyPresent()
{
    return (FindWindowExW(NULL, NULL, L"Progman", NULL) || FindWindowExW(NULL, NULL, L"Proxy Desktop", NULL));
}

// https://jiangsheng.net/2013/01/22/how-to-restart-windows-explorer-programmatically-using-restart-manager/
inline RM_UNIQUE_PROCESS GetExplorerApplication()
{
    HWND hwnd = FindWindow(L"Shell_TrayWnd", NULL);
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);

    RM_UNIQUE_PROCESS out = { 0, { (DWORD)-1, (DWORD)-1 } };
    DWORD bytesReturned;
    WCHAR imageName[MAX_PATH]; // process image name buffer
    DWORD processIds[2048]; // max 2048 processes (more than enough)

    // enumerate all running processes (usually around 60-70)
    EnumProcesses(processIds, sizeof(processIds), &bytesReturned);
    int count = bytesReturned / sizeof(DWORD); // number of processIds returned

    for (int i = 0; i < count; ++i)
    {
        DWORD processId = processIds[i];
        HANDLE hProc;
        if (processId == pid && (hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId)))
        {
            GetProcessImageFileNameW(hProc, imageName, MAX_PATH);
            FILETIME ftStart, ftExit, ftKernel, ftUser;
            GetProcessTimes(hProc, &ftStart, &ftExit, &ftKernel, &ftUser);

            if (ftStart.dwLowDateTime < out.ProcessStartTime.dwLowDateTime)
            {
                out.dwProcessId = processId;
                out.ProcessStartTime = ftStart;
            }
            CloseHandle(hProc);
        }
    }
    return out; // return count in pResults
}

static DWORD RmSession = -1;
static wchar_t RmSessionKey[CCH_RM_SESSION_KEY + 1];

// shuts down the explorer and is ready for explorer restart
inline DWORD WINAPI BeginExplorerRestart(LPVOID lpUnused)
{
    if (RmStartSession(&RmSession, 0, RmSessionKey) == ERROR_SUCCESS)
    {
        RM_UNIQUE_PROCESS rgApplications[] = { GetExplorerApplication() };
        RmRegisterResources(RmSession, 0, 0, 1, rgApplications, 0, 0);

        DWORD rebootReason;
        UINT nProcInfoNeeded, nProcInfo = 16;
        RM_PROCESS_INFO affectedApps[16];
        RmGetList(RmSession, &nProcInfoNeeded, &nProcInfo, affectedApps, &rebootReason);

        if (rebootReason == RmRebootReasonNone) // no need for reboot?
        {
            // shutdown explorer
            RmShutdown(RmSession, RmForceShutdown, 0);
        }
    }
    return 0;
}
// restarts the explorer
inline void FinishExplorerRestart()
{
    DWORD dwError;
    if (dwError = RmRestart(RmSession, 0, NULL))
        printf("\n RmRestart error: %d\n\n", dwError);

    RmEndSession(RmSession);
    RmSession = -1;
    RmSessionKey[0] = 0;
}

// https://stackoverflow.com/questions/5689904/gracefully-exit-explorer-programmatically
inline BOOL ExitExplorer()
{
    HWND hWndTray = FindWindowW(L"Shell_TrayWnd", NULL);
    return PostMessageW(hWndTray, 0x5B4, 0, 0);
}

inline void StartExplorerWithDelay(int delay, HANDLE userToken)
{
    WCHAR wszPath[MAX_PATH];
    ZeroMemory(wszPath, MAX_PATH * sizeof(WCHAR));
    GetWindowsDirectoryW(wszPath, MAX_PATH);
    wcscat_s(wszPath, MAX_PATH, L"\\explorer.exe");
    Sleep(delay);
    if (userToken != INVALID_HANDLE_VALUE)
    {
        HANDLE primaryUserToken = INVALID_HANDLE_VALUE;
        if (ImpersonateLoggedOnUser(userToken))
        {
            DuplicateTokenEx(userToken, MAXIMUM_ALLOWED, NULL, SecurityImpersonation, TokenPrimary, &primaryUserToken);
            RevertToSelf();
        }
        if (primaryUserToken != INVALID_HANDLE_VALUE)
        {
            PROCESS_INFORMATION processInfo;
            ZeroMemory(&processInfo, sizeof(processInfo));
            STARTUPINFOW startupInfo;
            ZeroMemory(&startupInfo, sizeof(startupInfo));
            startupInfo.cb = sizeof(startupInfo);
            BOOL processCreated = CreateProcessWithTokenW(
                primaryUserToken, LOGON_WITH_PROFILE, wszPath, NULL, 0, NULL, NULL, &startupInfo, &processInfo) != 0;
            CloseHandle(primaryUserToken);
            if (processInfo.hProcess != INVALID_HANDLE_VALUE)
            {
                CloseHandle(processInfo.hProcess);
            }
            if (processInfo.hThread != INVALID_HANDLE_VALUE)
            {
                CloseHandle(processInfo.hThread);
            }
            if (processCreated)
            {
                return;
            }
        }
    }
    ShellExecuteW(
        NULL,
        L"open",
        wszPath,
        NULL,
        NULL,
        SW_SHOWNORMAL
    );
}

inline void StartExplorer()
{

    /*PROCESSENTRY32 pe32 = {0};
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hSnapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPPROCESS,
        0
    );
    if (Process32First(hSnapshot, &pe32) == TRUE)
    {
        do
        {
            if (!wcscmp(pe32.szExeFile, TEXT("explorer.exe")))
            {
                HANDLE hSihost = OpenProcess(
                    PROCESS_TERMINATE,
                    FALSE,
                    pe32.th32ProcessID
                );
                TerminateProcess(hSihost, 1);
                CloseHandle(hSihost);
            }
        } while (Process32Next(hSnapshot, &pe32) == TRUE);
    }
    CloseHandle(hSnapshot);
    */
    wchar_t wszPath[MAX_PATH];
    ZeroMemory(
        wszPath,
        (MAX_PATH) * sizeof(wchar_t)
    );
    GetWindowsDirectoryW(
        wszPath,
        MAX_PATH
    );
    wcscat_s(
        wszPath,
        MAX_PATH,
        L"\\explorer.exe"
    );
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    if (CreateProcessW(
        NULL,
        wszPath,
        NULL,
        NULL,
        TRUE,
        CREATE_UNICODE_ENVIRONMENT,
        NULL,
        NULL,
        &si,
        &pi
    ))
    {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

inline BOOL IncrementDLLReferenceCount(HINSTANCE hinst)
{
    HMODULE hMod;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        (LPCWSTR)hinst,
        &hMod);
    return TRUE;
}

PVOID FindPattern(PVOID pBase, SIZE_T dwSize, LPCSTR lpPattern, LPCSTR lpMask);

#if _M_X64
inline BOOL FollowJnz(PBYTE pJnz, PBYTE* pTarget, DWORD* pJnzSize)
{
    // Check big jnz
    if (pJnz[0] == 0x0F && pJnz[1] == 0x85)
    {
        *pTarget = pJnz + 6 + *(int*)(pJnz + 2);
        *pJnzSize = 6;
        return TRUE;
    }
    // Check small jnz
    if (pJnz[0] == 0x75)
    {
        *pTarget = pJnz + 2 + *(char*)(pJnz + 1);
        *pJnzSize = 2;
        return TRUE;
    }
    return FALSE;
}
#endif

#if _M_ARM64
// https://github.com/CAS-Atlantic/AArch64-Encoding

__forceinline DWORD ARM64_ReadBits(DWORD value, int h, int l)
{
    return (value >> l) & ((1 << (h - l + 1)) - 1);
}

__forceinline int ARM64_SignExtend(DWORD value, int numBits)
{
    DWORD mask = 1 << (numBits - 1);
    if (value & mask)
        value |= ~((1 << numBits) - 1);
    return (int)value;
}

__forceinline int ARM64_ReadBitsSignExtend(DWORD insn, int h, int l)
{
    return ARM64_SignExtend(ARM64_ReadBits(insn, h, l), h - l + 1);
}

__forceinline BOOL ARM64_IsInRange(int value, int bitCount)
{
    int minVal = -(1 << (bitCount - 1));
    int maxVal = (1 << (bitCount - 1)) - 1;
    return value >= minVal && value <= maxVal;
}

__forceinline UINT_PTR ARM64_Align(UINT_PTR value, UINT_PTR alignment)
{
    return value & ~(alignment - 1);
}

__forceinline BOOL ARM64_IsCBZW(DWORD insn) { return ARM64_ReadBits(insn, 31, 24) == 0b00110100; }
__forceinline BOOL ARM64_IsCBNZW(DWORD insn) { return ARM64_ReadBits(insn, 31, 24) == 0b00110101; }
__forceinline BOOL ARM64_IsBL(DWORD insn) { return ARM64_ReadBits(insn, 31, 26) == 0b100101; }
__forceinline BOOL ARM64_IsADRP(DWORD insn) { return (ARM64_ReadBits(insn, 31, 24) & ~0b01100000) == 0b10010000; }

__forceinline DWORD* ARM64_FollowBL(DWORD* pInsnBL)
{
    DWORD insnBL = *pInsnBL;
    if (!ARM64_IsBL(insnBL))
        return NULL;
    int imm26 = ARM64_ReadBitsSignExtend(insnBL, 25, 0);
    return pInsnBL + imm26; // offset = imm26 * 4
}

__forceinline DWORD ARM64_MakeB(int imm26)
{
    if (!ARM64_IsInRange(imm26, 26))
        return 0;
    return 0b000101 << 26 | imm26 & (1 << 26) - 1;
}

__forceinline DWORD ARM64_CBZWToB(DWORD insnCBZW)
{
    if (!ARM64_IsCBZW(insnCBZW))
        return 0;
    int imm19 = ARM64_ReadBitsSignExtend(insnCBZW, 23, 5);
    return ARM64_MakeB(imm19);
}

__forceinline DWORD ARM64_CBNZWToB(DWORD insnCBNZW)
{
    if (!ARM64_IsCBNZW(insnCBNZW))
        return 0;
    int imm19 = ARM64_ReadBitsSignExtend(insnCBNZW, 23, 5);
    return ARM64_MakeB(imm19);
}

__forceinline DWORD ARM64_DecodeADD(DWORD insnADD)
{
    DWORD imm12 = ARM64_ReadBits(insnADD, 21, 10);
    DWORD shift = ARM64_ReadBits(insnADD, 22, 22);
    return imm12 << (shift * 12);
}

__forceinline DWORD ARM64_DecodeSTRBIMM(DWORD insnSTRBIMM)
{
    if (ARM64_ReadBits(insnSTRBIMM, 31, 22) != 0b0011100100)
        return (DWORD)-1;
    DWORD imm12 = ARM64_ReadBits(insnSTRBIMM, 21, 10);
    return imm12;
}

__forceinline DWORD ARM64_DecodeLDRBIMM(DWORD insnLDRBIMM)
{
    if (ARM64_ReadBits(insnLDRBIMM, 31, 22) != 0b0011100101)
        return (DWORD)-1;
    DWORD imm12 = ARM64_ReadBits(insnLDRBIMM, 21, 10);
    return imm12;
}

inline UINT_PTR ARM64_DecodeADRL(UINT_PTR offset, DWORD insnADRP, DWORD insnADD)
{
    if (!ARM64_IsADRP(insnADRP))
        return 0;

    UINT_PTR page = ARM64_Align(offset, 0x1000);

    DWORD adrp_immlo = ARM64_ReadBits(insnADRP, 30, 29);
    DWORD adrp_immhi = ARM64_ReadBits(insnADRP, 23, 5);
    DWORD adrp_imm = ((adrp_immhi << 2) | adrp_immlo) << 12;

    DWORD add_imm = ARM64_DecodeADD(insnADD);

    return page + adrp_imm + add_imm;
}
#endif

#if defined(WITH_MAIN_PATCHER) && WITH_MAIN_PATCHER
inline BOOL WINAPI PatchContextMenuOfNewMicrosoftIME(BOOL* bFound)
{
    // huge thanks to @Simplestas: https://github.com/valinet/ExplorerPatcher/issues/598
    HMODULE hInputSwitch = NULL;
    if (!GetModuleHandleExW(0, L"InputSwitch.dll", &hInputSwitch))
        return FALSE;

    MODULEINFO mi;
    GetModuleInformation(GetCurrentProcess(), hInputSwitch, &mi, sizeof(mi));

#if defined(_M_X64)
    // 44 38 ?? ?? 74 ?? ?? 8B CE E8 ?? ?? ?? ?? 85 C0
    //             ^^ Change jz into jmp
    PBYTE match = (PBYTE)FindPattern(
        hInputSwitch,
        mi.SizeOfImage,
        "\x44\x38\x00\x00\x74\x00\x00\x8B\xCE\xE8\x00\x00\x00\x00\x85\xC0",
        "xx??x??xxx????xx"
    );
    if (!match)
        return FALSE;

    DWORD dwOldProtect;
    if (!VirtualProtect(match + 4, 1, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        return FALSE;

    match[4] = 0xEB;

    VirtualProtect(match + 4, 1, dwOldProtect, &dwOldProtect);

    return TRUE;
#elif defined(_M_ARM64)
    // A8 43 40 39 C8 04 00 34 E0 03 14 AA
    //             ^^^^^^^^^^^ Change CBZ to B
    PBYTE match = (PBYTE)FindPattern(
        hInputSwitch,
        mi.SizeOfImage,
        "\xA8\x43\x40\x39\xC8\x04\x00\x34\xE0\x03\x14\xAA",
        "xxxxxxxxxxxx"
    );
    if (!match)
        return FALSE;

    match += 4;

    DWORD newInsn = ARM64_CBZWToB(*(DWORD*)match);
    if (!newInsn)
        return FALSE;

    DWORD dwOldProtect;
    if (!VirtualProtect(match, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        return FALSE;

    *(DWORD*)match = newInsn;

    VirtualProtect(match, 4, dwOldProtect, &dwOldProtect);

    return TRUE;
#endif
}
#endif

extern UINT PleaseWaitTimeout;
extern HHOOK PleaseWaitHook;
extern HWND PleaseWaitHWND;
extern void* PleaseWaitCallbackData;
extern BOOL (*PleaseWaitCallbackFunc)(void* data);
BOOL PleaseWait_UpdateTimeout(int timeout);
VOID CALLBACK PleaseWait_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
LRESULT CALLBACK PleaseWait_HookProc(int code, WPARAM wParam, LPARAM lParam);

BOOL DownloadAndInstallWebView2Runtime();

BOOL DownloadFile(LPCWSTR wszURL, DWORD dwSize, LPCWSTR wszPath);

BOOL IsConnectedToInternet();

#define SCRATCH_QCM_FIRST 1
#define SCRATCH_QCM_LAST  0x7FFF

#define SPOP_OPENMENU            1
#define SPOP_INSERTMENU_ALL      0b1111110000
#define SPOP_INSERTMENU_OPEN     0b0000010000
#define SPOP_INSERTMENU_NEXTPIC  0b0000100000
#define SPOP_INSERTMENU_LIKE     0b0001000000
#define SPOP_INSERTMENU_DISLIKE  0b0010000000
#define SPOP_INSERTMENU_INFOTIP1 0b0100000000
#define SPOP_INSERTMENU_INFOTIP2 0b1000000000
#define SPOP_CLICKMENU_FIRST     40000
#define SPOP_CLICKMENU_OPEN      40000
#define SPOP_CLICKMENU_NEXTPIC   40001
#define SPOP_CLICKMENU_LIKE      40002
#define SPOP_CLICKMENU_DISLIKE   40003
#define SPOP_CLICKMENU_LAST      40003

BOOL DoesOSBuildSupportSpotlight();

BOOL IsSpotlightEnabled();

void SpotlightHelper(DWORD dwOp, HWND hWnd, HMENU hMenu, LPPOINT pPt);

typedef struct _MonitorOverrideData
{
    DWORD cbIndex;
    DWORD dwIndex;
    HMONITOR hMonitor;
} MonitorOverrideData;

BOOL ExtractMonitorByIndex(HMONITOR hMonitor, HDC hDC, LPRECT lpRect, MonitorOverrideData* mod);
HRESULT SHRegGetBOOLWithREGSAM(HKEY key, LPCWSTR subKey, LPCWSTR value, REGSAM regSam, BOOL* data);
HRESULT SHRegGetDWORD(HKEY hkey, const WCHAR* pwszSubKey, const WCHAR* pwszValue, DWORD* pdwData);

inline BOOL MaskCompare(PVOID pBuffer, LPCSTR lpPattern, LPCSTR lpMask)
{
    for (PBYTE value = (PBYTE)pBuffer; *lpMask; ++lpPattern, ++lpMask, ++value)
    {
        if (*lpMask == 'x' && *(LPCBYTE)lpPattern != *value)
            return FALSE;
    }

    return TRUE;
}

inline __declspec(noinline) PVOID FindPatternHelper(PVOID pBase, SIZE_T dwSize, LPCSTR lpPattern, LPCSTR lpMask)
{
    for (SIZE_T index = 0; index < dwSize; ++index)
    {
        PBYTE pAddress = (PBYTE)pBase + index;

        if (MaskCompare(pAddress, lpPattern, lpMask))
            return pAddress;
    }

    return NULL;
}

inline PVOID FindPattern(PVOID pBase, SIZE_T dwSize, LPCSTR lpPattern, LPCSTR lpMask)
{
    dwSize -= strlen(lpMask);
    return FindPatternHelper(pBase, dwSize, lpPattern, lpMask);
}

inline UINT_PTR FileOffsetToRVA(PBYTE pBase, UINT_PTR offset)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pBase;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(pBase + pDosHeader->e_lfanew);
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeaders);
    for (int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSection++)
    {
        if (offset >= pSection->PointerToRawData && offset < pSection->PointerToRawData + pSection->SizeOfRawData)
            return offset - pSection->PointerToRawData + pSection->VirtualAddress;
    }
    return 0;
}

inline UINT_PTR RVAToFileOffset(PBYTE pBase, UINT_PTR rva)
{
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pBase;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)(pBase + pDosHeader->e_lfanew);
    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeaders);
    for (int i = 0; i < pNtHeaders->FileHeader.NumberOfSections; i++, pSection++)
    {
        if (rva >= pSection->VirtualAddress && rva < pSection->VirtualAddress + pSection->Misc.VirtualSize)
            return rva - pSection->VirtualAddress + pSection->PointerToRawData;
    }
    return 0;
}

inline HMODULE LoadGuiModule()
{
    wchar_t epGuiPath[MAX_PATH];
    ZeroMemory(epGuiPath, sizeof(epGuiPath));
    SHGetFolderPathW(NULL, SPECIAL_FOLDER, NULL, SHGFP_TYPE_CURRENT, epGuiPath);
    wcscat_s(epGuiPath, MAX_PATH, _T(APP_RELATIVE_PATH) L"\\ep_gui.dll");
    return LoadLibraryExW(epGuiPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
}

inline BOOL DoesWindows10StartMenuExist()
{
    if (!IsWindows11())
        return TRUE;

    wchar_t szPath[MAX_PATH];
    GetWindowsDirectoryW(szPath, MAX_PATH);
    wcscat_s(szPath, MAX_PATH, L"\\SystemApps\\Microsoft.Windows.StartMenuExperienceHost_cw5n1h2txyewy\\StartUI.dll");
    if (FileExistsW(szPath))
        return TRUE;

    GetWindowsDirectoryW(szPath, MAX_PATH);
    wcscat_s(szPath, MAX_PATH, L"\\SystemApps\\Microsoft.Windows.StartMenuExperienceHost_cw5n1h2txyewy\\StartUI_.dll");
    if (FileExistsW(szPath))
        return TRUE;

    return FALSE;
}

inline BOOL IsStockWindows10TaskbarAvailable()
{
    return true;
}

inline const WCHAR* PickTaskbarDll()
{
    DWORD b = global_rovi.dwBuildNumber;

    if (b == 15063 // Windows 10 1703
     || b == 16299 // Windows 10 1709
     || b == 17134 // Windows 10 1803
     || b == 17763 // Windows 10 1809
     || b >= 18362 && b <= 18363 // Windows 10 1903, 1909
     || b >= 19041 && b <= 19045) // Windows 10 20H2, 21H2, 22H2
    {
        return L"ep_taskbar.0.dll";
    }

    if (b >= 21343 && b <= 22000) // Windows 11 21H2
    {
        return L"ep_taskbar.1.dll";
    }

    if ((b >= 22621 && b <= 22635)  // 22H2-23H2 Release, Release Preview, and Beta channels
     || (b >= 23403 && b <= 25197)) // Early pre-reboot Dev channel until post-reboot Dev channel
    {
        return L"ep_taskbar.2.dll";
    }

    if (b >= 25201 && b <= 25915) // Pre-reboot Dev channel until early Canary channel, nuked ITrayComponentHost methods related to classic search box
    {
        return L"ep_taskbar.3.dll";
    }

    if (b >= 25921 && b <= 26040) // Canary channel with nuked classic system tray
    {
        return L"ep_taskbar.4.dll";
    }

    if (b >= 26052) // Same as 4 but with 2 new methods in ITrayComponentHost between GetTrayUI and ProgrammableTaskbarReportClick
    {
        return L"ep_taskbar.5.dll";
    }

    return NULL;
}

inline BOOL DoesTaskbarDllExist()
{
    const wchar_t* pszTaskbarDll = PickTaskbarDll();
    if (!pszTaskbarDll)
        return FALSE;

    wchar_t szPath[MAX_PATH];
    ZeroMemory(szPath, sizeof(szPath));
    SHGetFolderPathW(NULL, SPECIAL_FOLDER, NULL, SHGFP_TYPE_CURRENT, szPath);
    wcscat_s(szPath, MAX_PATH, _T(APP_RELATIVE_PATH) L"\\");
    wcscat_s(szPath, MAX_PATH, pszTaskbarDll);
    return FileExistsW(szPath);
}

inline void AdjustTaskbarStyleValue(DWORD* pdwValue)
{
    if (*pdwValue >= 2 && !DoesTaskbarDllExist())
    {
        *pdwValue = 1;
    }
    if (*pdwValue == 1 && !IsStockWindows10TaskbarAvailable())
    {
        *pdwValue = 0;
    }
}

#ifdef __cplusplus
}
#endif

#endif
