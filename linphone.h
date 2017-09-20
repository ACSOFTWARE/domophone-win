//---------------------------------------------------------------------------

#ifndef linphoneH
#define linphoneH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------

/*
typedef struct _LinphoneCore LinphoneCore;
typedef struct _LinphoneFriend LinphoneFriend;
typedef struct _LinphoneChatRoom LinphoneChatRoom;
typedef struct _LinphoneGeneralState LinphoneGeneralState;
typedef struct _LinphoneCall LinphoneCall;
typedef struct _LinphoneCallState LinphoneCallState;
typedef struct _LinphoneProxyConfig LinphoneProxyConfig;
*/

class TLinphone : public TComponent
{
private:
  HINSTANCE hLinLib;

  bool __fastcall LoadLib(void);
  bool __fastcall LinInitialize(void);
  void __fastcall LinUninitialize(void);
  void __fastcall TerminateCall(void);
protected:
  HWND hWndLast;
  bool FMuted;

  int retryCounter;
  bool RegistrationInProgress;
  AnsiString lastIdent;
  AnsiString lastHost;

  /*
  static void _ShowInterface(_LinphoneCore *lc);
  static void _InviteReceived(_LinphoneCore *lc, const char *from);
  static void _ByeReceived(_LinphoneCore *lc, const char *from);
  static void _NotifyPresenceReceived(_LinphoneCore *lc, LinphoneFriend * fid);
  static void _NewUnknownSubscriber(_LinphoneCore *lc, LinphoneFriend *lf, const char *url);
  static void _AuthInfoRequested(_LinphoneCore *lc, const char *realm, const char *username);
  static void _DisplayStatus(_LinphoneCore *lc, const char *message);
  static void _DisplayMessage(_LinphoneCore *lc, const char *message);
  static void _DisplayWarning(_LinphoneCore *lc, const char *message);
  static void _DisplayUrl(_LinphoneCore *lc, const char *message, const char *url);
  static void _DisplayQuestion(_LinphoneCore *lc, const char *message);
  static void _CallLogUpdated(_LinphoneCore *lc, struct _LinphoneCallLog *newcl);
  static void _TextMessageReceived(_LinphoneCore *lc, LinphoneChatRoom *room, const char *from, const char *message);
  static void _GeneralStateChange(_LinphoneCore *lc, LinphoneGeneralState *gstate);
  static void _DtmfReceived(_LinphoneCore* lc, int dtmf);
  static void _ReferReceived(_LinphoneCore *lc, const char *refer_to);
  static void _BuddyInfoUpdated(_LinphoneCore *lc, LinphoneFriend *lf);
  static void _NotifyReceived(_LinphoneCore *lc, const char *from, const char *msg);
  */

public:
        __fastcall TLinphone(TComponent *FOwner);

        void __fastcall OnRegisterFailed(void);
        void __fastcall OnRegisterSuccess(void);

        void __fastcall Unregister(void);

        void __fastcall Start(void);
        void __fastcall Iterate(void);
        void __fastcall Finish(void);
        void __fastcall Mute(bool Muted);

        void __fastcall Register(AnsiString Ident, AnsiString Host);
        bool __fastcall Registered(AnsiString Ident, AnsiString Host);
        void __fastcall ResetRetryCounter(void);

        HWND __fastcall VideoWindow(void);
        void __fastcall SetWindowPosition(int Left, int Top, int Width, int Height);

        void __fastcall GetAudioDevices(TStringList *List);
        bool __fastcall IsCaptureDevice(const AnsiString &DevName);
        bool __fastcall IsPlaybackDevice(const AnsiString &DevName);

        AnsiString __fastcall GetCaptureDevice(void);
        AnsiString __fastcall GetPlaybackDevice(void);
        void __fastcall SetCaptureDevice(const AnsiString &DevName);
        void __fastcall SetPlaybackDevice(const AnsiString &DevName);

        bool  __fastcall ActiveCall(void);
        void __fastcall Clean(void);

        __property bool Muted = { read=FMuted };


};
//---------------------------------------------------------------------------

extern TLinphone *Lin;

#ifdef _DEBUG
extern TStringList *LinLog;
#endif

#endif
