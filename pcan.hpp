#ifndef PCAN_HPP
#define PCAN_HPP

// 1. ÖNCE Windows'un temel tanımları
#include <windows.h>

#include "PCANBasic.h"

typedef TPCANStatus (__stdcall *fpInitialize)(TPCANHandle, TPCANBaudrate, TPCANType, DWORD, WORD);
typedef TPCANStatus (__stdcall *fpRead)(TPCANHandle, TPCANMsg*, TPCANTimestamp*); 
typedef TPCANStatus (__stdcall *fpWrite)(TPCANHandle, TPCANMsg*);
typedef TPCANStatus (__stdcall *fpFilterMessages)(TPCANHandle, DWORD, DWORD, TPCANMode); 
typedef TPCANStatus (__stdcall *fpGetSetValue)(TPCANHandle, TPCANParameter, void*, DWORD);
typedef TPCANStatus (__stdcall *fpGetErrorText)(TPCANStatus, WORD, LPSTR);
typedef TPCANStatus (__stdcall *fpLookUpChannel)(LPSTR, TPCANHandle*);
typedef TPCANStatus (__stdcall *fpOneParam)(TPCANHandle);

#define fpUninitialize fpOneParam
#define fpReset fpOneParam
#define fpGetStatus fpOneParam
#define fpGetValue fpGetSetValue
#define fpSetValue fpGetSetValue

class PCAN {
private:
    HINSTANCE m_hdll;
    fpInitialize m_pInitialize;
    fpRead m_pRead;
    fpWrite m_pWrite;
    fpFilterMessages m_pFilterMessages;
    fpGetErrorText m_pGetErrorText;
    fpLookUpChannel m_pLookUpChannel;
    fpUninitialize m_pUninitialize;
    fpReset m_pReset;
    fpGetStatus m_pGetStatus;
    fpGetValue m_pGetValue;
    fpSetValue m_pSetValue;

    bool m_bWasLoaded;
    void LoadAPI();
    void UnloadAPI();
    void InitializePointers();
    bool LoadDllHandle();
    FARPROC GetFunction(LPCSTR szName);

public:
    PCAN();
    ~PCAN();

    TPCANStatus Initialize(TPCANHandle Channel, TPCANBaudrate Btr0Btr1, TPCANType HwType = 0, DWORD IOPort = 0, WORD Interrupt = 0);
    TPCANStatus Uninitialize(TPCANHandle Channel);
    TPCANStatus Reset(TPCANHandle Channel);
    TPCANStatus GetStatus(TPCANHandle Channel);
    TPCANStatus Read(TPCANHandle Channel, TPCANMsg* MessageBuffer, TPCANTimestamp* TimestampBuffer); 
    TPCANStatus Write(TPCANHandle Channel, TPCANMsg* MessageBuffer);
    TPCANStatus FilterMessages(TPCANHandle Channel, DWORD FromID, DWORD ToID, TPCANMode Mode);
    TPCANStatus GetValue(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength);
    TPCANStatus SetValue(TPCANHandle Channel, TPCANParameter Parameter, void* Buffer, DWORD BufferLength);
    TPCANStatus GetErrorText(TPCANStatus Error, WORD Language, LPSTR Buffer);
    TPCANStatus LookUpChannel(LPSTR Parameters, TPCANHandle* FoundChannel);
};

#endif