/*
 Copyright (C) AC SOFTWARE SP. Z O.O.
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
#include <vcl.h>
#pragma hdrstop

#include "domophoneconnection.h"
#include "socketdata.h"
#include "dconnection.h"
#include "main.h"
#include <SysUtils.hpp>
#include <tchar.h>
#pragma package(smart_init)

#define RBSIZE 1024
#define DEFAUL_TCP_PORT 465
#define	MSG_DONTWAIT	0

  typedef struct tagTHREADNAME_INFO
  {
    DWORD dwType;     // must be 0x1000
    LPCSTR szName;    // pointer to name (in user addr space)
    DWORD dwThreadID; // thread ID (-1=caller thread)
    DWORD dwFlags;    // reserved for future use, must be zero
  } THREADNAME_INFO;

TDomophoneConnection *ConnectionPtr = NULL;

#ifdef _DEBUG
TStringList *LogSL = new TStringList();
#endif
CRITICAL_SECTION CS;


//---------------------------------------------------------------------------
__fastcall TDomophoneConnection::TDomophoneConnection(bool CreateSuspended, BYTE LangID, char *AuthKey, char *SerialKey, char *ClientID, AnsiString HostName, bool AllowProxy, void *dc)
        : TThread(CreateSuspended)
{
  char ComputerName[1024];
  DWORD Size = 1023;
  GetComputerName(ComputerName, &Size);
  ComputerName[Size] = 0;
  FAllowProxy = AllowProxy;

  #ifdef _DEBUG
  InitializeCriticalSection(&CS);
  #endif

  if ( dc == NULL )
    {
      char Language = LANG_UNKNOWN;
      switch(LangID)
        {
           case 9:
                Language = LANG_EN;
                break;
           case 21:
                Language = LANG_PL;
                break;
        };
      this->dc = dconnection_init(OSTYPE_WINDOWS, Language, AuthKey, SerialKey, ClientID, WideString(ComputerName), AllowProxy ? USEPROXY_ALWAYS : USEPROXY_NONE);
    }
  else
    {
      this->dc = dc;
    };

  FHostName = HostName;
  FOnAuthorize = NULL;
  FOnUnauthorize = NULL;
  FOnConnect = NULL;
  FOnDisconnect = NULL;
  FOnEvent = NULL;
  FOnSysState = NULL;
  FOnLocked = NULL;
  FOnVersionError = NULL;
}
//---------------------------------------------------------------------------
__fastcall TDomophoneConnection::~TDomophoneConnection(void)
{
   dconnection_release(dc);

   if ( ConnectionPtr == this )
     ConnectionPtr = NULL;
};
//---------------------------------------------------------------------------
bool __fastcall TDomophoneConnection::pzInternetReadFile(TStream *Stream,
                const char *Server, const char *Action, const char *Data)
{
     Stream->Size = 0;

     const char *hdr = "Content-Type: application/x-www-form-urlencoded";
     const TCHAR * strAcceptT[] = { _T("*/*"), NULL };

     HINTERNET ISession = InternetOpen(AnsiString("domophoneClient[Win32]").c_str(), INTERNET_OPEN_TYPE_PRECONFIG ,NULL,NULL, NULL);

     if ( ! ISession )
      return false;

     try
       {
           HINTERNET HConn = InternetConnect(ISession, Server , INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP,0, NULL);

           if ( !HConn  )
               {
                  return false;
               }
            else
               {
                  try
                    {
                        HINTERNET  HReq = HttpOpenRequest(HConn, "POST", Action, NULL, NULL, strAcceptT, INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID , NULL);

                        if ( !HReq  )
                          {
                            return false;
                          }
                        else
                          {
                             try
                               {

                                   DWORD dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_REVOCATION ;
                                   InternetSetOption(HReq, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));

                                   if ( HttpSendRequest(HReq, hdr, strlen(hdr), (char *)Data, strlen(Data) ) )
                                     {

                                        AnsiString ContentType = "";

                                        DWORD l = RBSIZE;
                                        BYTE readbuffer[RBSIZE];

                                        if ( HttpQueryInfo(HReq, HTTP_QUERY_CONTENT_TYPE  , readbuffer, &l ,NULL) )
                                          ContentType = (char *)readbuffer;

                                        l = RBSIZE;

                                        bool CanRead = true;

                                        while ( CanRead  )
                                           if ( InternetReadFile(HReq,&readbuffer,RBSIZE,&l) )
                                             {
                                               if ( l )
                                                   Stream->WriteBuffer(&readbuffer, l);
                                               else
                                                   break;
                                             }
                                           else
                                             {
                                               Stream->Size = 0;
                                               break;
                                             };

                                     }
                                    else
                                     {
                                       return false;
                                     };
                               }
                             __finally
                               {
                                  InternetCloseHandle(HReq);
                               };
                          };
                    }
                  __finally
                    {
                       InternetCloseHandle(HConn);
                    };
               };
       }
     __finally
       {
         InternetCloseHandle(ISession);
       };

return Stream->Size > 0;
};
//---------------------------------------------------------------------------
AnsiString __fastcall TDomophoneConnection::GetProxyHostName(void)
{
    AnsiString Result;
    TStringStream *SS = new TStringStream("");
    try
      {
          if ( pzInternetReadFile(SS, "www.acsoftware.pl", "support/domophone.php", "Action=GetProxyAddr") )
            {
               SS->Position = 0;
               Result = SS->ReadString(SS->Size).Trim();
            };
      }
    __finally
      {
        delete SS;
      };
      
return Result;
};
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::Disconnect(void)
{
    if ( clientSock )
      {
        closesocket(clientSock);
        WSACleanup();
        #ifdef _DEBUG
        Log("TDomophoneConnection::Disconnect dconnection_setdisconnected");
        #endif
        dconnection_setdisconnected(dc, 1);
      };
};
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::TryConnect(void)
{
    Disconnect();

    int Port = DEFAUL_TCP_PORT;

    if ( FHostName.AnsiPos(":") )
      {
         int n = FHostName.AnsiPos(":");
         Port = FHostName.SubString(n+1, FHostName.Length()).ToIntDef(DEFAUL_TCP_PORT);
         FHostName = FHostName.SubString(1, n-1);
      };

    WORD ver = MAKEWORD(2,2);
    WSADATA wsaData;
    LPHOSTENT hostEnt;
    SOCKADDR_IN serverInfo;

    WSAStartup(ver,(LPWSADATA)&wsaData);
    hostEnt = gethostbyname(FHostName.c_str());

    if( hostEnt )
      {
          clientSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

          if( clientSock != SOCKET_ERROR )
            {
                serverInfo.sin_family = PF_INET;
                serverInfo.sin_addr = *((LPIN_ADDR)*hostEnt->h_addr_list);
                serverInfo.sin_port = htons(Port);

                int retVal=0;
                retVal=connect(clientSock, (LPSOCKADDR)&serverInfo, sizeof(serverInfo));

                if(retVal != SOCKET_ERROR)
                  {
                      u_long iMode=1;
                      ioctlsocket(clientSock,FIONBIO,&iMode); // Non blocikng mode
                  };
            };
      };
};
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::StopProxyConnection(TDomophoneConnection **Proxy)
{
     if ( *Proxy )
       {
           (*Proxy)->Terminate();
           (*Proxy)->WaitFor();
           delete *Proxy;
           *Proxy = NULL;
       };
};
//---------------------------------------------------------------------------
#ifdef _DEBUG
void __fastcall TDomophoneConnection::Log(AnsiString L)
{
     EnterCriticalSection(&CS);
     LogSL->Add(Now().FormatString("hh:nn:ss")+""+IntToStr(GetTickCount())+" "+AnsiString(FAllowProxy ? "master" : "proxy" )+" "+L);
     LeaveCriticalSection(&CS);

};
#endif
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::Execute()
{
      /*
      THREADNAME_INFO info;
      info.dwType = 0x1000;
      info.szName = Tag == 1 ? "Proxy" : "Master";
      info.dwThreadID = -1;
      info.dwFlags = 0;

      __try
      {
               RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD),(DWORD*)&info );
      }
      __except (EXCEPTION_CONTINUE_EXECUTION)
      {
      }
      */

    TDomophoneConnection *Proxy = NULL;

    _is_proxy = dconnection_isproxy(dc) != 0;

    char ComputerName[1024];
    {
       DWORD Size = 1023;
       GetComputerName(ComputerName, &Size);
       ComputerName[Size] = 0;
    };

    clientSock = NULL;

    while(!Terminated)
      {
         int wr = dconnection_work(dc);

         switch(wr)
           {
              case WRESULT_ONCONNECT:
                   #ifdef _DEBUG
                   Log("WRESULT_ONCONNECT");
                   #endif
                   Synchronize(_FOnConnect);
                   break;
              case WRESULT_ONDISCONNECT:
                   #ifdef _DEBUG
                   Log("WRESULT_ONDISCONNECT");
                   #endif
                   Disconnect();
                   Synchronize(_FOnDisconnect);
                   break;
              case WRESULT_ONAUTHORIZE:
                   #ifdef _DEBUG
                   Log("WRESULT_ONAUTHORIZE");
                   #endif
                   Synchronize(_FOnAuthorize);
                   break;
              case WRESULT_ONUNAUTHORIZE:
                   #ifdef _DEBUG
                   Log("WRESULT_ONUNAUTHORIZE");
                   #endif
                   if ( !_is_proxy ) {
                     Synchronize(_FOnUnauthorize);
                   }
                   break;
              case WRESULT_ONEVENT:
                   #ifdef _DEBUG
                   Log("WRESULT_ONEVENT");
                   #endif
                   Synchronize(_FOnEvent);
                   break;
              case WRESULT_TRYCONNECT:
                   #ifdef _DEBUG
                   Log("WRESULT_TRYCONNECT");
                   #endif
                   TryConnect();
                   dconnection_setconnecting(dc);
                   break;
              case WRESULT_WAITFORDATA:
                   {
                     char buffer[1024];
                     int size;
                     size = recv(clientSock, buffer, 1024, MSG_DONTWAIT);
                     if ( size > 0 )
                       {
                           dconnection_appendrecvbuffer(dc, buffer, size);
                       }
                     else if ( size == 0 )
                       {
                           #ifdef _DEBUG
                           Log("WRESULT_WAITFORDATA dconnection_setdisconnected");
                           #endif
                           dconnection_setdisconnected(dc, 0);
                       };
                   }
                   break;
              case WRESULT_TRYSENDDATA:
                   {
                       int buff_size;
                       char *buff = dconnection_getsentbuffer(dc, &buff_size);

                       if ( buff && buff_size > 0 )
                         {
                            buff_size = send(clientSock, buff, buff_size, 0);
                            free(buff);

                            if ( buff_size == 0 )
                              {
                                 #ifdef _DEBUG
                                 Log("WRESULT_TRYSENDDATA dconnection_setdisconnected");
                                 #endif
                                 dconnection_setdisconnected(dc, 0);
                              };
                         }
                   }
                   break;
              case WRESULT_RESPONSETIMEOUT:
                   #ifdef _DEBUG
                   Log("WRESULT_RESPONSETIMEOUT");
                   #endif
                   break;
              case WRESULT_ONRESPONSE:
                   #ifdef _DEBUG
                   Log("WRESULT_ONRESPONSE");
                   #endif
                   break;
              case WRESULT_ONSYSSTATE:
                   Synchronize(_FOnSysState);
                   break;
              case WRESULT_LOCKED:
                   Synchronize(_FOnLocked);
                   break;
              case WRESULT_PROXYCONNECT:
                   {
                       #ifdef _DEBUG
                       Log("WRESULT_PROXYCONNECT");
                       #endif
                       AnsiString Host = GetProxyHostName();

                       if ( !Proxy
                            && !Host.IsEmpty() )
                         {
                            Proxy = new TDomophoneConnection(true, 0, NULL, NULL, NULL, Host.c_str(), false, pconnection_proxyinit(dc));
                            Proxy->FreeOnTerminate = false;
                            Proxy->Priority = tpLower;
                            Proxy->OnAuthorize = OnAuthorize;
                            Proxy->OnUnauthorize = OnUnauthorize;
                            Proxy->OnDisconnect = OnDisconnect;
                            Proxy->OnEvent = OnEvent;
                            Proxy->OnSysState = OnSysState;
                            Proxy->OnLocked = OnLocked;
                            Proxy->OnVersionError = OnVersionError;
                            Proxy->Resume();
                         };
                   }
                   break;
              case WRESULT_PROXYDISCONNECT:
                   #ifdef _DEBUG
                   Log("WRESULT_PROXYDISCONNECT");
                   #endif
                   StopProxyConnection(&Proxy);
                   break;
              case WRESULT_DEVICENOTFOUND:
                   #ifdef _DEBUG
                   Log("WRESULT_DEVICENOTFOUND");
                   #endif
                   break;
              case WRESULT_VERSIONERROR:
                   Synchronize(_FOnVersionError);
                   break;
              case WRESULT_NONE:
                   Sleep(100);
                   break;
           };
      };

    StopProxyConnection(&Proxy);

    #ifdef _DEBUG
    LogSL->SaveToFile("d:\\log.txt");
    DeleteCriticalSection(&CS);
    #endif
}
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::_FOnAuthorize(void)
{
   if ( FOnAuthorize )
     {
        TConnectionSettings Settings;
        dconnection_getconnectionsettings(dc, &Settings);
        FOnAuthorize(this, Settings);
     };
};
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::_FOnUnauthorize(void)
{
   if ( FOnUnauthorize )
        FOnUnauthorize(this);
};
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::_FOnConnect(void)
{
   if ( FOnConnect )
      FOnConnect(this);
};
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::_FOnDisconnect(void)
{
   if ( FOnDisconnect )
      FOnDisconnect(this);
};
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::_FOnEvent(void)
{
   if ( FOnEvent )
     {
        TdEvent Event;
        unsigned char duplicate = 0;

        if ( dconnection_getevent(dc, &Event, &duplicate) == 1
             && duplicate == 0 )
          {
             FOnEvent(this, Event);

             if ( Event.SenderName )
               free(Event.SenderName);
          }
        else
          {
             int a=10;
             a=10;
          };
     };
};
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::_FOnSysState(void)
{
     int state;
     if ( FOnSysState
          && dconnection_get_sys_state(dc, &state) )
       {
           FOnSysState(this, state);
       };
};
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::_FOnLocked(void)
{
     if ( FOnLocked )
       {
          TDataPacket DP;
          dconnection_getresponse(dc, &DP);

          if ( DP.Param2 == 0 ) {
              wchar_t *SenderName;
              dconnection_extract_name_and_id(&DP, &SenderName, NULL);

              FOnLocked(this, SenderName ? AnsiString(SenderName) : AnsiString(""));

              if ( SenderName ) {
                  free(SenderName);
              }
          }
       };
};
//---------------------------------------------------------------------------
void __fastcall TDomophoneConnection::_FOnVersionError(void)
{
    if ( FOnVersionError )
      FOnVersionError(this);
};
//---------------------------------------------------------------------------
void TDomophoneConnection::OpenGate(void)
{
    dconnection_opengate(dc, 2);
};
//---------------------------------------------------------------------------
void TDomophoneConnection::OpenGateway(void)
{
    dconnection_opengate(dc, 1);
};
//---------------------------------------------------------------------------
void TDomophoneConnection::SipConnect(bool videoEnabled, bool speakerOn)
{
    if ( dc )
        dconnection_sipconnect(dc, speakerOn ? 1 : 0, videoEnabled ? 1 : 0);
};
//---------------------------------------------------------------------------
void TDomophoneConnection::SipDisconnect(void)
{
    if ( dc )
        dconnection_sipdisconnect(dc);
};
//---------------------------------------------------------------------------
void TDomophoneConnection::SetSpeakerOn(bool On)
{
    if ( dc )
        dconnection_setspeakeronoff(dc, On ? 1 : 0);
};
//---------------------------------------------------------------------------
AnsiString TDomophoneConnection::SipServer(void)
{
    TConnectionSettings cs;
    if ( dc
        && dconnection_getconnectionsettings(dc, &cs) ) {
        return AnsiString(cs.Sip.Host);
    }
    
    return "";
}

