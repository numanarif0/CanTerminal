#include "pcan.hpp"
#include <iostream>

PCAN::PCAN(){
    m_bWasLoaded = false;
    m_hdll = NULL;
    LoadAPI();
}

PCAN::~PCAN(){
     UnloadAPI();
}

void PCAN::LoadAPI()
{
    InitializePointers();
    if(!LoadDllHandle())
    {
        std::cout<<"Unable to find the DLL PCANBasic.dll!"<<std::endl;
        return;
    }
    m_pInitialize = (fpInitialize)GetFunction("CAN_Initialize");
    m_pUninitialize = (fpOneParam)GetFunction("CAN_Uninitialize");
    m_pRead = (fpRead)GetFunction("CAN_Read");
    m_pWrite = (fpWrite)GetFunction("CAN_Write");
    m_pFilterMessages= (fpFilterMessages)GetFunction("CAN_FilterMessages");
    m_pSetValue = (fpGetSetValue)GetFunction("CAN_SetValue");
    m_pGetValue = (fpGetSetValue)GetFunction("CAN_GetValue");
    m_pGetErrorText = (fpGetErrorText)GetFunction("CAN_GetErrorText");
    m_pLookUpChannel = (fpLookUpChannel)GetFunction("CAN_LookUpChannel");
    m_pReset = (fpOneParam)GetFunction("CAN_Reset");
    m_pGetStatus = (fpOneParam)GetFunction("CAN_GetStatus");
    
    m_bWasLoaded = m_pInitialize && m_pUninitialize && m_pRead && m_pWrite && m_pFilterMessages && m_pSetValue && m_pGetValue && m_pGetErrorText && m_pLookUpChannel && m_pReset && m_pGetStatus;
    if(!m_bWasLoaded){
        std::cout<< "Dll dosyalari yüklenemedi" <<std::endl;
    }
}

void PCAN::UnloadAPI(){
    if (m_hdll != NULL)
        FreeLibrary(m_hdll);
    m_hdll = NULL;
    InitializePointers();
    m_bWasLoaded = false;
}

void PCAN::InitializePointers(){
    m_pInitialize = NULL;
    m_pUninitialize = NULL;
    m_pReset = NULL;
    m_pGetStatus = NULL;
    m_pRead = NULL;
    m_pWrite = NULL;
    m_pFilterMessages = NULL;
    m_pGetValue = NULL;
    m_pSetValue = NULL;
    m_pGetErrorText = NULL;
    m_pLookUpChannel = NULL;
}

bool PCAN::LoadDllHandle(){
    if(m_bWasLoaded){
        return true;
    }
    m_hdll = LoadLibraryA("PCANBasic");
    return (m_hdll != NULL);
}

FARPROC PCAN::GetFunction(LPCSTR name){
    if(m_hdll==NULL){
        return NULL;
    }
    return GetProcAddress(m_hdll,name);
}

TPCANStatus PCAN::Initialize(
        TPCANHandle Channel,
        TPCANBaudrate Btr0Btr1,
        TPCANType HwType,
        DWORD IOPort,
        WORD Interrupt ){
        if (!m_bWasLoaded){
             return PCAN_ERROR_UNKNOWN;
        }
    return (TPCANStatus)m_pInitialize(Channel, Btr0Btr1, HwType, IOPort, Interrupt);       
}

TPCANStatus PCAN::Uninitialize(TPCANHandle Channel){
    if (!m_bWasLoaded)
        return PCAN_ERROR_UNKNOWN;
    return (TPCANStatus)m_pUninitialize(Channel);
}

TPCANStatus PCAN::Reset(TPCANHandle Channel){
     if (!m_bWasLoaded)
        return PCAN_ERROR_UNKNOWN;
      return(TPCANStatus)m_pReset(Channel);  
}

TPCANStatus PCAN::GetStatus(TPCANHandle Channel){
    if(!m_bWasLoaded){
        return PCAN_ERROR_UNKNOWN;
    }
    return (TPCANStatus)m_pGetStatus(Channel);
}

TPCANStatus PCAN::Read(
        TPCANHandle Channel, 
        TPCANMsg* MessageBuffer, 
        TPCANTimestamp* TimestampBuffer
){
    if(!m_bWasLoaded){
        return PCAN_ERROR_UNKNOWN;
    }
    return (TPCANStatus)m_pRead(Channel, MessageBuffer, TimestampBuffer);
}

TPCANStatus PCAN::Write(
        TPCANHandle Channel, 
        TPCANMsg* MessageBuffer
){
    if(!m_bWasLoaded){
        return PCAN_ERROR_UNKNOWN;
    }
    return(TPCANStatus)m_pWrite(Channel,MessageBuffer);
}

TPCANStatus PCAN::FilterMessages(
        TPCANHandle Channel, 
        DWORD FromID, 
        DWORD ToID, 
        TPCANMode Mode
){
     if(!m_bWasLoaded){
        return PCAN_ERROR_UNKNOWN;
    }
    return(TPCANStatus)m_pFilterMessages(Channel,FromID,ToID,Mode);
}

TPCANStatus PCAN::GetValue(
        TPCANHandle Channel, 
        TPCANParameter Parameter,  
        void* Buffer, 
        DWORD BufferLength
){
     if(!m_bWasLoaded){
        return PCAN_ERROR_UNKNOWN;
    }
    return(TPCANStatus)m_pGetValue(Channel,Parameter,Buffer,BufferLength);
}

TPCANStatus PCAN::SetValue(
        TPCANHandle Channel,
        TPCANParameter Parameter,
        void* Buffer,
        DWORD BufferLength
){
     if(!m_bWasLoaded){
        return PCAN_ERROR_UNKNOWN;
    }
    return(TPCANStatus)m_pSetValue(Channel,Parameter,Buffer,BufferLength);
}

TPCANStatus PCAN::GetErrorText(
        TPCANStatus Error, 
        WORD Language, 
        LPSTR Buffer
){
    if(!m_bWasLoaded){
        return PCAN_ERROR_UNKNOWN;
    }
    return(TPCANStatus)m_pGetErrorText(Error,Language,Buffer);
}

TPCANStatus PCAN::LookUpChannel(
        LPSTR Parameters, 
        TPCANHandle* FoundChannel
){
    if(!m_bWasLoaded){
        return PCAN_ERROR_UNKNOWN;
    }
    return(TPCANStatus)m_pLookUpChannel(Parameters,FoundChannel);
}
