//---------------------------------------------------------------------------

#ifndef domophoneconnectionH
#define domophoneconnectionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <winsock.h>
#include "dconnection.h"
//---------------------------------------------------------------------------

typedef void __fastcall(__closure *TdcEvent)(TObject *Sender, const TdEvent &Event);
typedef void __fastcall(__closure *TdcSysState)(TObject *Sender, int SysState);
typedef void __fastcall(__closure *TdcLocked)(TObject *Sender, AnsiString Name);
typedef void __fastcall(__closure *TdcAuthorize)(TObject *Sender, const TConnectionSettings &Settings);

class TDomophoneConnection : public TThread
{
private:
protected:
        void __fastcall Execute();
        
        void *dc;
        bool _is_proxy;
        AnsiString FHostName;

        TdcAuthorize FOnAuthorize;
        TNotifyEvent FOnUnauthorize;
        TNotifyEvent FOnConnect;
        TNotifyEvent FOnDisconnect;
        TdcEvent FOnEvent;
        TdcLocked FOnLocked;
        TdcSysState FOnSysState;
        TNotifyEvent FOnVersionError;

        void __fastcall _FOnAuthorize(void);
        void __fastcall _FOnUnauthorize(void);
        void __fastcall _FOnConnect(void);
        void __fastcall _FOnDisconnect(void);
        void __fastcall _FOnEvent(void);
        void __fastcall _FOnSysState(void);
        void __fastcall _FOnLocked(void);
        void __fastcall _FOnVersionError(void);

        AnsiString __fastcall GetProxyHostName(void);

        bool __fastcall pzInternetReadFile(TStream *Stream,
                const char *Server, const char *Action, const char *Data);

        int clientSock;
        void __fastcall TryConnect(void);
        void __fastcall Disconnect(void);
        void __fastcall StopProxyConnection(TDomophoneConnection **Proxy);
        bool FAllowProxy;
        #ifdef _DEBUG
        void __fastcall Log(AnsiString L);
        #endif

public:

        __fastcall TDomophoneConnection(bool CreateSuspended, BYTE LangID, char *AuthKey, char *SerialKey, char *ClientID, AnsiString HostName, bool AllowProxy, void *dc = NULL);
        __fastcall ~TDomophoneConnection(void);

        void OpenGate(void);
        void OpenGateway(void);
        void SipConnect(bool videoEnabled, bool speakerOn);
        void SipDisconnect(void);
        void SetSpeakerOn(bool On);
        AnsiString SipServer(void);

        __property TNotifyEvent OnConnect = { read = FOnConnect, write = FOnConnect };
        __property TNotifyEvent OnDisconnect = { read = FOnDisconnect, write = FOnDisconnect };
        __property TdcAuthorize OnAuthorize = { read = FOnAuthorize, write = FOnAuthorize };
        __property TNotifyEvent OnUnauthorize = { read = FOnUnauthorize, write = FOnUnauthorize };
        __property TdcEvent OnEvent = { read = FOnEvent, write = FOnEvent };
        __property TdcSysState OnSysState = { read = FOnSysState, write = FOnSysState };
        __property TdcLocked OnLocked = { read = FOnLocked, write = FOnLocked };
        __property TNotifyEvent OnVersionError = { read = FOnVersionError, write = FOnVersionError };

};
//---------------------------------------------------------------------------

extern TDomophoneConnection *ConnectionPtr;


#endif
