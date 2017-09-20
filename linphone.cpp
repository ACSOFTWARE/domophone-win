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

#include "linphone.h"
#include "main.h"
#pragma package(smart_init)

#define WIN32 1
#define DEFAULT_EXPIRES 600
#include "linphonecore.h"

TLinphone *Lin = NULL;

typedef LinphoneProxyConfig * __declspec(dllimport)_dll_linphone_proxy_config_new(void);
typedef LinphoneAddress * __declspec(dllimport)_dll_linphone_address_new(const char *uri);
typedef LinphoneCore *__declspec(dllimport)_dll_linphone_core_new(const LinphoneCoreVTable *vtable, const char *config_path, const char *factory_config, void* userdata);
typedef int __declspec(dllimport)_dll_linphone_proxy_config_set_identity(LinphoneProxyConfig *obj, const char *identity);
typedef int __declspec(dllimport)_dll_linphone_proxy_config_set_server_addr(LinphoneProxyConfig *obj, const char *server_addr);
typedef void __declspec(dllimport)_dll_linphone_proxy_config_enable_register(LinphoneProxyConfig *obj, bool_t val);
typedef void __declspec(dllimport)_dll_linphone_address_destroy(LinphoneAddress *u);
typedef int __declspec(dllimport)_dll_linphone_core_add_proxy_config(LinphoneCore *lc, LinphoneProxyConfig *config);
typedef void __declspec(dllimport)_dll_linphone_core_set_default_proxy(LinphoneCore *lc, LinphoneProxyConfig *config);
typedef void __declspec(dllimport)_dll_linphone_core_iterate(LinphoneCore *lc);
typedef void __declspec(dllimport)_dll_linphone_core_enable_logs(FILE *file);
typedef int __declspec(dllimport)_dll_linphone_core_get_default_proxy(LinphoneCore *lc, LinphoneProxyConfig **config);
typedef void __declspec(dllimport)_dll_linphone_proxy_config_edit(LinphoneProxyConfig *obj);
typedef int __declspec(dllimport)_dll_linphone_proxy_config_done(LinphoneProxyConfig *obj);
typedef int __declspec(dllimport)_dll_linphone_core_accept_call(LinphoneCore *lc, LinphoneCall *call);
typedef LinphoneCall *__declspec(dllimport)_dll_linphone_core_get_current_call(LinphoneCore *lc);
typedef int __declspec(dllimport)_dll_linphone_core_terminate_call(LinphoneCore *lc, LinphoneCall *call);
typedef int __declspec(dllimport)_dll_linphone_core_terminate_all_calls(LinphoneCore *lc);
typedef void __declspec(dllimport)_dll_linphone_core_mute_mic(LinphoneCore *lc, bool_t muted);
typedef void __declspec(dllimport)_dll_linphone_core_enable_video(LinphoneCore *lc, bool_t vcap_enabled, bool_t display_enabled);
typedef void __declspec(dllimport)_dll_linphone_core_enable_video_preview(LinphoneCore *lc, bool_t val);
typedef void __declspec(dllimport)_dll_linphone_core_enable_self_view(LinphoneCore *lc, bool_t val);
typedef unsigned long __declspec(dllimport)_dll_linphone_core_get_native_video_window_id(const LinphoneCore *lc);
typedef void __declspec(dllimport)_dll_linphone_core_destroy(LinphoneCore *lc);
typedef const char *__declspec(dllimport)_dll_linphone_proxy_config_get_identity(const LinphoneProxyConfig *obj);
typedef const char *__declspec(dllimport)_dll_linphone_proxy_config_get_addr(const LinphoneProxyConfig *obj);
typedef bool_t __declspec(dllimport)_dll_linphone_proxy_config_is_registered(const LinphoneProxyConfig *obj);
typedef const char * __declspec(dllimport)_dll_linphone_core_get_playback_device(LinphoneCore *lc);
typedef const char * __declspec(dllimport)_dll_linphone_core_get_capture_device(LinphoneCore *lc);
typedef int __declspec(dllimport)_dll_linphone_core_set_playback_device(LinphoneCore *lc, const char * devid);
typedef int __declspec(dllimport)_dll_linphone_core_set_capture_device(LinphoneCore *lc, const char * devid);
typedef const char**  __declspec(dllimport)_dll_linphone_core_get_sound_devices(LinphoneCore *lc);
typedef bool_t __declspec(dllimport)_dll_linphone_core_sound_device_can_capture(LinphoneCore *lc, const char *device);
typedef bool_t __declspec(dllimport)_dll_linphone_core_sound_device_can_playback(LinphoneCore *lc, const char *device);
typedef void __declspec(dllimport)_dll_linphone_core_clear_all_auth_info(LinphoneCore *lc);
typedef void __declspec(dllimport)_dll_linphone_core_clear_proxy_config(LinphoneCore *lc);
typedef int __declspec(dllimport)_dll_linphone_core_set_sip_transports(LinphoneCore *lc, const LCSipTransports *transports);
typedef void __declspec(dllimport)_dll_linphone_proxy_config_expires(LinphoneProxyConfig *obj, int expires);
typedef int __declspec(dllimport)_dll_linphone_core_get_sip_transports(LinphoneCore *lc, LCSipTransports *transports);
typedef void __declspec(dllimport)_dll_linphone_core_set_network_reachable(LinphoneCore* lc,bool_t value);
typedef void __declspec(dllimport)_dll_linphone_core_enable_logs_with_cb(OrtpLogFunc logfunc);


_dll_linphone_proxy_config_new *lin_proxy_config_new;
_dll_linphone_address_new *lin_address_new;
_dll_linphone_core_new *lin_core_new;
_dll_linphone_proxy_config_set_identity *lin_proxy_config_set_identity;
_dll_linphone_proxy_config_set_server_addr *lin_proxy_config_set_server_addr;
_dll_linphone_proxy_config_enable_register *lin_proxy_config_enable_register;
_dll_linphone_address_destroy *lin_address_destroy;
_dll_linphone_core_add_proxy_config *lin_core_add_proxy_config;
_dll_linphone_core_set_default_proxy *lin_core_set_default_proxy;
_dll_linphone_core_iterate *lin_core_iterate;
_dll_linphone_core_enable_logs *lin_core_enable_logs;
_dll_linphone_core_get_default_proxy *lin_core_get_default_proxy;
_dll_linphone_proxy_config_edit *lin_proxy_config_edit;
_dll_linphone_proxy_config_done *lin_proxy_config_done;
_dll_linphone_core_accept_call *lin_core_accept_call;
_dll_linphone_core_get_current_call *lin_core_get_current_call;
_dll_linphone_core_terminate_call *lin_core_terminate_call;
_dll_linphone_core_mute_mic *lin_core_mute_mic;
_dll_linphone_core_enable_video *lin_core_enable_video;
_dll_linphone_core_enable_video_preview *lin_core_enable_video_preview;
_dll_linphone_core_enable_self_view *lin_core_enable_self_view;
_dll_linphone_core_get_native_video_window_id *lin_core_get_native_video_window_id;
_dll_linphone_core_destroy *lin_core_destroy;
_dll_linphone_proxy_config_get_identity *lin_proxy_config_get_identity;
_dll_linphone_proxy_config_get_addr *lin_proxy_config_get_addr;
_dll_linphone_proxy_config_is_registered *lin_proxy_config_is_registered;
_dll_linphone_core_get_playback_device *lin_core_get_playback_device;
_dll_linphone_core_get_capture_device *lin_core_get_capture_device;
_dll_linphone_core_set_playback_device *lin_core_set_playback_device;
_dll_linphone_core_set_capture_device *lin_core_set_capture_device;
_dll_linphone_core_get_sound_devices *lin_core_get_sound_devices;
_dll_linphone_core_sound_device_can_capture *lin_core_sound_device_can_capture;
_dll_linphone_core_sound_device_can_playback *lin_core_sound_device_can_playback;
_dll_linphone_core_clear_all_auth_info *lin_core_clear_all_auth_info;
_dll_linphone_core_clear_proxy_config *lin_core_clear_proxy_config;
_dll_linphone_core_set_sip_transports *lin_core_set_sip_transports;
_dll_linphone_proxy_config_expires *lin_proxy_config_expires;
_dll_linphone_core_get_sip_transports *lin_core_get_sip_transports;
_dll_linphone_core_set_network_reachable *lin_core_set_network_reachable;
_dll_linphone_core_enable_logs_with_cb *lin_core_enable_logs_with_cb;
_dll_linphone_core_terminate_all_calls *lin_core_terminate_all_calls;

TThreadList *TL = new TThreadList();


LinphoneCoreVTable linphonec_vtable={0};
LinphoneCore *lc = NULL;

#ifdef _DEBUG
TStringList *LinLog = new TStringList();
void LinLogFunc(OrtpLogLevel lev, const char *fmt, va_list args)
{
     char buffer[2048];
     vsnprintf (buffer, 2048, fmt, args);

     LinLog->Add(buffer);
};
#endif

/*
void TLinphone::_ShowInterface(_LinphoneCore *lc)
{

};

void TLinphone::_InviteReceived(_LinphoneCore *lc, const char *from)
{
   lin_core_accept_call(lc, NULL);

   if ( Lin )
     {
       lin_core_mute_mic(lc, Lin->Muted);
     };
};

void TLinphone::_ByeReceived(_LinphoneCore *lc, const char *from)
{
    // Lin->Unregister();
};

void TLinphone::_NotifyPresenceReceived(_LinphoneCore *lc, LinphoneFriend * fid)
{

};

void TLinphone::_NewUnknownSubscriber(_LinphoneCore *lc, LinphoneFriend *lf, const char *url)
{

};

void TLinphone::_AuthInfoRequested(_LinphoneCore *lc, const char *realm, const char *username)
{

};

void TLinphone::_DisplayStatus(_LinphoneCore *lc, const char *message)
{
    AnsiString msg = AnsiString(message).LowerCase();

    if ( Lin && msg.AnsiPos("registration on") )
      {
          if ( msg.AnsiPos("successful") )
            {
                fmain->OnSipRegistered();
            }
          else if ( msg.AnsiPos("failed") )
            {
                if ( Lin->retryCounter > 0
                     && Lin->lastIdent != ""
                     && Lin->lastHost != ""  )
                  {
                     Lin->retryCounter--;
                     Lin->Register(Lin->lastIdent, Lin->lastHost);
                  }
                else
                  {
                     Lin->RegistrationInProgress = false;
                     fmain->SipTerminate();
                  };
            };
      };
};

void TLinphone::_DisplayMessage(_LinphoneCore *lc, const char *message)
{

};

void TLinphone::_DisplayWarning(_LinphoneCore *lc, const char *message)
{

};

void TLinphone::_DisplayUrl(_LinphoneCore *lc, const char *message, const char *url)
{

};

void TLinphone::_DisplayQuestion(_LinphoneCore *lc, const char *message)
{

};

void TLinphone::_CallLogUpdated(_LinphoneCore *lc, struct _LinphoneCallLog *newcl)
{

};

void TLinphone::_TextMessageReceived(_LinphoneCore *lc, LinphoneChatRoom *room, const char *from, const char *message)
{

};

void TLinphone::_GeneralStateChange(_LinphoneCore *lc, LinphoneGeneralState *gstate)
{
//    if ( gstate->new_state == GSTATE_POWER_OFF
//         && Lin )
//     Lin->Unregister();
};

void TLinphone::_DtmfReceived(_LinphoneCore* lc, int dtmf)
{

};

void TLinphone::_ReferReceived(_LinphoneCore *lc, const char *refer_to)
{

};

void TLinphone::_BuddyInfoUpdated(_LinphoneCore *lc, LinphoneFriend *lf)
{

};

void TLinphone::_NotifyReceived(_LinphoneCore *lc, const char *from, const char *msg)
{

};
*/

static void _call_state(LinphoneCore *lc, LinphoneCall* call, LinphoneCallState state,const char* message) {

    switch(state) {
        case LinphoneCallIncomingReceived:
            lin_core_accept_call(lc, call);
            break;
        case LinphoneCallStreamsRunning:
            fmain->OnVideoStarted();
            break;
        case LinphoneCallError:
        case LinphoneCallEnd:
        case LinphoneCallReleased:
            fmain->SipTerminate();
            break;
        default:
            break;
    }
}

static void _registration_state(LinphoneCore *lc, LinphoneProxyConfig* cfg, LinphoneRegistrationState state,const char* message) {

    switch(state) {
        case LinphoneRegistrationOk:
            if ( Lin )
              Lin->OnRegisterSuccess();
            break;
        //case LinphoneRegistrationCleared:
        case LinphoneRegistrationFailed:

             if ( Lin )
               Lin->OnRegisterFailed();

            break;
        case LinphoneRegistrationProgress:
            //[MainVC setConnectedStatusWithActInd:YES];
            break;
        default:
            break;
        
    }
}


__fastcall TLinphone::TLinphone(TComponent *FOwner)
        : TComponent(FOwner)
{
   Lin = this;
   FMuted = false;
   hWndLast = NULL;
}
//---------------------------------------------------------------------------
void __fastcall TLinphone::OnRegisterFailed(void)
{
      if ( retryCounter > 0
           && !lastIdent.IsEmpty()
           && !lastHost.IsEmpty() )
        {

          retryCounter--;
          Register(lastIdent, lastHost);

        }
      else
        {
          RegistrationInProgress = false;
          fmain->SipTerminate();
        };
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::OnRegisterSuccess(void)
{
       retryCounter = 0;
       RegistrationInProgress = false;
       fmain->OnSipRegistered();
};
//---------------------------------------------------------------------------
bool __fastcall TLinphone::LoadLib(void)
{
    hLinLib = LoadLibrary(AnsiString(ExtractFilePath(Application->ExeName)+"\\liblp-5.dll").c_str());
    lin_proxy_config_new = (_dll_linphone_proxy_config_new *)GetProcAddress(hLinLib, "linphone_proxy_config_new");
    lin_address_new = (_dll_linphone_address_new *)GetProcAddress(hLinLib, "linphone_address_new");
    lin_core_new = (_dll_linphone_core_new *)GetProcAddress(hLinLib, "linphone_core_new");
    lin_proxy_config_set_identity = (_dll_linphone_proxy_config_set_identity *)GetProcAddress(hLinLib, "linphone_proxy_config_set_identity");
    lin_proxy_config_set_server_addr = (_dll_linphone_proxy_config_set_server_addr *)GetProcAddress(hLinLib, "linphone_proxy_config_set_server_addr");
    lin_proxy_config_enable_register = (_dll_linphone_proxy_config_enable_register *)GetProcAddress(hLinLib, "linphone_proxy_config_enable_register");
    lin_address_destroy = (_dll_linphone_address_destroy *)GetProcAddress(hLinLib, "linphone_address_destroy");
    lin_core_add_proxy_config = (_dll_linphone_core_add_proxy_config *)GetProcAddress(hLinLib, "linphone_core_add_proxy_config");
    lin_core_set_default_proxy = (_dll_linphone_core_set_default_proxy *)GetProcAddress(hLinLib, "linphone_core_set_default_proxy");
    lin_core_iterate = (_dll_linphone_core_iterate *)GetProcAddress(hLinLib, "linphone_core_iterate");
    lin_core_enable_logs = (_dll_linphone_core_enable_logs *)GetProcAddress(hLinLib, "linphone_core_enable_logs");
    lin_core_get_default_proxy = (_dll_linphone_core_get_default_proxy *)GetProcAddress(hLinLib, "linphone_core_get_default_proxy");
    lin_proxy_config_edit = (_dll_linphone_proxy_config_edit *)GetProcAddress(hLinLib, "linphone_proxy_config_edit");
    lin_proxy_config_done = (_dll_linphone_proxy_config_done *)GetProcAddress(hLinLib, "linphone_proxy_config_done");
    lin_core_accept_call = (_dll_linphone_core_accept_call *)GetProcAddress(hLinLib, "linphone_core_accept_call");
    lin_core_get_current_call = (_dll_linphone_core_get_current_call *)GetProcAddress(hLinLib, "linphone_core_get_current_call");
    lin_core_terminate_call = (_dll_linphone_core_terminate_call *)GetProcAddress(hLinLib, "linphone_core_terminate_call");
    lin_core_terminate_all_calls = (_dll_linphone_core_terminate_all_calls *)GetProcAddress(hLinLib, "linphone_core_terminate_all_calls");
    lin_core_mute_mic = (_dll_linphone_core_mute_mic *)GetProcAddress(hLinLib, "linphone_core_mute_mic");
    lin_core_enable_video = (_dll_linphone_core_enable_video *)GetProcAddress(hLinLib, "linphone_core_enable_video");
    lin_core_enable_video_preview = (_dll_linphone_core_enable_video_preview *)GetProcAddress(hLinLib, "linphone_core_enable_video_preview");
    lin_core_enable_self_view = (_dll_linphone_core_enable_self_view *)GetProcAddress(hLinLib, "linphone_core_enable_self_view");
    lin_core_get_native_video_window_id = (_dll_linphone_core_get_native_video_window_id *)GetProcAddress(hLinLib, "linphone_core_get_native_video_window_id");
    lin_core_destroy = (_dll_linphone_core_destroy *)GetProcAddress(hLinLib, "linphone_core_destroy");
    lin_proxy_config_get_identity = (_dll_linphone_proxy_config_get_identity *)GetProcAddress(hLinLib, "linphone_proxy_config_get_identity");
    lin_proxy_config_get_addr = (_dll_linphone_proxy_config_get_addr *)GetProcAddress(hLinLib, "linphone_proxy_config_get_addr");
    lin_proxy_config_is_registered = (_dll_linphone_proxy_config_is_registered *)GetProcAddress(hLinLib, "linphone_proxy_config_is_registered");
    lin_core_get_playback_device = (_dll_linphone_core_get_playback_device *)GetProcAddress(hLinLib, "linphone_core_get_playback_device");
    lin_core_get_capture_device = (_dll_linphone_core_get_capture_device *)GetProcAddress(hLinLib, "linphone_core_get_capture_device");
    lin_core_set_playback_device = (_dll_linphone_core_set_playback_device *)GetProcAddress(hLinLib, "linphone_core_set_playback_device");
    lin_core_set_capture_device = (_dll_linphone_core_set_capture_device *)GetProcAddress(hLinLib, "linphone_core_set_capture_device");
    lin_core_get_sound_devices = (_dll_linphone_core_get_sound_devices *)GetProcAddress(hLinLib, "linphone_core_get_sound_devices");
    lin_core_sound_device_can_capture = (_dll_linphone_core_sound_device_can_capture *)GetProcAddress(hLinLib, "linphone_core_sound_device_can_capture");
    lin_core_sound_device_can_playback = (_dll_linphone_core_sound_device_can_playback *)GetProcAddress(hLinLib, "linphone_core_sound_device_can_playback");
    lin_core_clear_all_auth_info = (_dll_linphone_core_clear_all_auth_info *)GetProcAddress(hLinLib, "linphone_core_clear_all_auth_info");
    lin_core_clear_proxy_config = (_dll_linphone_core_clear_proxy_config*)GetProcAddress(hLinLib, "linphone_core_clear_proxy_config");
    lin_core_set_sip_transports = (_dll_linphone_core_set_sip_transports*)GetProcAddress(hLinLib, "linphone_core_set_sip_transports");
    lin_proxy_config_expires = (_dll_linphone_proxy_config_expires*)GetProcAddress(hLinLib, "linphone_proxy_config_expires");
    lin_core_get_sip_transports = (_dll_linphone_core_get_sip_transports*)GetProcAddress(hLinLib, "linphone_core_get_sip_transports");
    lin_core_set_network_reachable = (_dll_linphone_core_set_network_reachable*)GetProcAddress(hLinLib, "linphone_core_set_network_reachable");
    lin_core_enable_logs_with_cb = (_dll_linphone_core_enable_logs_with_cb*)GetProcAddress(hLinLib, "linphone_core_enable_logs_with_cb");

    return hLinLib
           && lin_proxy_config_new
           && lin_address_new
           && lin_core_new
           && lin_proxy_config_set_identity
           && lin_proxy_config_set_server_addr
           && lin_proxy_config_enable_register
           && lin_address_destroy
           && lin_core_add_proxy_config
           && lin_core_set_default_proxy
           && lin_core_iterate
           && lin_core_enable_logs
           && lin_core_get_default_proxy
           && lin_proxy_config_edit
           && lin_proxy_config_done
           && lin_core_accept_call
           && lin_core_get_current_call
           && lin_core_terminate_call
           && lin_core_terminate_all_calls
           && lin_core_mute_mic
           && lin_core_enable_video
           && lin_core_enable_video_preview
           && lin_core_enable_self_view
           && lin_core_get_native_video_window_id
           && lin_core_destroy
           && lin_proxy_config_get_identity
           && lin_proxy_config_get_addr
           && lin_proxy_config_is_registered
           && lin_core_get_playback_device
           && lin_core_get_capture_device
           && lin_core_set_playback_device
           && lin_core_set_capture_device
           && lin_core_get_sound_devices
           && lin_core_sound_device_can_capture
           && lin_core_sound_device_can_playback
           && lin_core_clear_all_auth_info
           && lin_core_clear_proxy_config
           && lin_core_set_sip_transports
           && lin_proxy_config_expires
           && lin_core_get_sip_transports
           && lin_core_set_network_reachable
           && lin_core_enable_logs_with_cb;
};
//---------------------------------------------------------------------------
bool __fastcall TLinphone::LinInitialize(void)
{
    RegistrationInProgress = false;
    /*
    linphonec_vtable.show = _ShowInterface;
    linphonec_vtable.inv_recv = _InviteReceived;
    linphonec_vtable.bye_recv = _ByeReceived;
    linphonec_vtable.notify_presence_recv = _NotifyPresenceReceived;
    linphonec_vtable.new_unknown_subscriber = _NewUnknownSubscriber;
    linphonec_vtable.auth_info_requested = _AuthInfoRequested;
    linphonec_vtable.display_status = _DisplayStatus;
    linphonec_vtable.display_message = _DisplayMessage;
    linphonec_vtable.display_warning = _DisplayWarning;
    linphonec_vtable.display_url = _DisplayUrl;
    linphonec_vtable.display_question = _DisplayQuestion;
    linphonec_vtable.call_log_updated = _CallLogUpdated;
    linphonec_vtable.text_received = _TextMessageReceived;
    linphonec_vtable.general_state = _GeneralStateChange;
    linphonec_vtable.dtmf_received = _DtmfReceived;
    linphonec_vtable.refer_received = _ReferReceived;
    linphonec_vtable.buddy_info_updated = _BuddyInfoUpdated;
    linphonec_vtable.notify_recv = _NotifyReceived;
    */
    linphonec_vtable.show =NULL;
    linphonec_vtable.call_state_changed =(LinphoneCallStateCb)_call_state;
    linphonec_vtable.registration_state_changed = _registration_state;
    linphonec_vtable.notify_recv = NULL;
    linphonec_vtable.new_subscription_request = NULL;
    linphonec_vtable.auth_info_requested = NULL;
    linphonec_vtable.display_status = NULL;
    linphonec_vtable.display_message=NULL;
    linphonec_vtable.display_warning=NULL;
    linphonec_vtable.display_url=NULL;
    linphonec_vtable.text_received=NULL;
    linphonec_vtable.dtmf_received=NULL;
    linphonec_vtable.transfer_state_changed=NULL;


    AnsiString FD = ExtractFilePath(Application->ExeName)+"\\lprc";
    lc = lin_core_new (&linphonec_vtable, "NUL", FileExists(FD) ? FD.c_str() : NULL, NULL);

    #ifdef _DEBUG
    lin_core_enable_logs_with_cb(LinLogFunc);
    #endif


    if ( lc )
      {
        lin_core_enable_video(lc, false, true);
//        lin_core_enable_video_preview(lc, true);
      };

return lc != NULL;
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::LinUninitialize(void)
{
//   Unregister();

   if ( lc )
     {
       lin_core_destroy(lc);
       lc = NULL;
     };
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::Register(AnsiString Ident, AnsiString Host)
{
    if ( RegistrationInProgress ) return;

    if ( Registered(Ident, Host) != 2 ) {

          RegistrationInProgress = true;
          lastIdent = Ident;
          lastHost = Host;

          //[MainVC setConnectedStatusWithActInd:YES];
          TerminateCall();
          Unregister();

          if ( lc )
            {
                lin_core_set_network_reachable(lc, true);

                lin_core_clear_all_auth_info(lc);
                lin_core_clear_proxy_config(lc);

                LCSipTransports transportValue;
                lin_core_get_sip_transports(lc, &transportValue);

                if (transportValue.tcp_port == 0) transportValue.tcp_port=transportValue.udp_port + transportValue.tls_port;
                transportValue.udp_port=0;
                transportValue.tls_port=0;
                lin_core_set_sip_transports(lc, &transportValue);

                LinphoneProxyConfig* proxy_cfg = lin_proxy_config_new();

                lin_proxy_config_set_identity(proxy_cfg, AnsiString("sip:"+Ident+"@"+Host).c_str());
                lin_proxy_config_set_server_addr(proxy_cfg, AnsiString("sip:"+Host).c_str());
                lin_proxy_config_enable_register(proxy_cfg, true);
                lin_proxy_config_expires(proxy_cfg, DEFAULT_EXPIRES);
                lin_core_add_proxy_config(lc,proxy_cfg);
                lin_core_set_default_proxy(lc,proxy_cfg);
            }

    } else {
        //[MainVC sipRegistered];
    }
    
};
//---------------------------------------------------------------------------
bool __fastcall TLinphone::Registered(AnsiString Ident, AnsiString Host)
{
    LinphoneProxyConfig* proxy_cfg = NULL;
    if ( lc )
      {
          lin_core_get_default_proxy(lc, &proxy_cfg);

          if ( proxy_cfg
               && lin_proxy_config_is_registered(proxy_cfg) == 1 )
            {
                AnsiString _Ident = lin_proxy_config_get_identity(proxy_cfg);
                AnsiString _Host = lin_proxy_config_get_addr(proxy_cfg);

                if ( _Ident == "sip:"+Ident+"@"+Host
                     && _Host == "sip:"+Host )
                 return true;
            };
      };

return false;
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::TerminateCall(void)
{
    if ( lc ) {
        lin_core_terminate_all_calls(lc);
    };
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::Unregister(void)
{
    TerminateCall();

    RegistrationInProgress = false;

    if ( lc )
      {
        LinphoneProxyConfig* proxy_cfg;
        lin_core_get_default_proxy(lc,&proxy_cfg);

        if ( proxy_cfg
             && lin_proxy_config_is_registered(proxy_cfg) == 1 )
          {
              lin_proxy_config_edit(proxy_cfg);
              lin_proxy_config_set_identity(proxy_cfg, AnsiString("sip:null@127.0.0.1").c_str());
              lin_proxy_config_set_server_addr(proxy_cfg, AnsiString("sip:127.0.0.1").c_str());
              lin_proxy_config_enable_register(proxy_cfg, false);
              lin_proxy_config_done(proxy_cfg);

              lin_core_clear_all_auth_info(lc);
              lin_core_clear_proxy_config(lc);
          };
      };
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::Mute(bool Muted)
{
    FMuted = Muted;
    
    if ( lc != NULL )
      lin_core_mute_mic(lc, Muted);
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::Start(void)
{
    if ( LoadLib() )
         LinInitialize();
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::Iterate(void)
{
    if (lc!=NULL)
      {
         HWND hWnd = VideoWindow();
         if ( hWnd != NULL
              && hWndLast != hWnd )
           {
              hWndLast = hWnd;

              LONG nStyle = GetWindowLong(hWnd, GWL_STYLE);
              SetWindowLong(hWnd, GWL_STYLE, nStyle & ~WS_SYSMENU & ~WS_BORDER & ~WS_CAPTION);

              fmain->SetVideoWindowPos();
              fmain->OnVideoStarted();
           };

         lin_core_iterate(lc);
         Application->ProcessMessages();
      };

};
//---------------------------------------------------------------------------
void __fastcall TLinphone::Finish(void)
{
          LinUninitialize();
};
//---------------------------------------------------------------------------
HWND __fastcall TLinphone::VideoWindow(void)
{
     return lc == NULL ? NULL : (HWND)lin_core_get_native_video_window_id(lc);
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::GetAudioDevices(TStringList *List)
{
   if ( lc != NULL )
     {
         const char **sound_devices=lin_core_get_sound_devices(lc);
         int n=0;
         while(sound_devices[n]!=NULL)
           {
              List->Add(UTF8Decode(AnsiString(sound_devices[n])));
              n++;
           };
     };
};
//---------------------------------------------------------------------------
bool __fastcall TLinphone::IsCaptureDevice(const AnsiString &DevName)
{
   return lc == NULL ? false : lin_core_sound_device_can_capture(lc, UTF8Encode(DevName).c_str());
};
//---------------------------------------------------------------------------
bool __fastcall TLinphone::IsPlaybackDevice(const AnsiString &DevName)
{
   return lc == NULL ? false : lin_core_sound_device_can_playback(lc, UTF8Encode(DevName).c_str());
};
//---------------------------------------------------------------------------
AnsiString __fastcall TLinphone::GetCaptureDevice(void)
{
  if ( lc != NULL )
    return UTF8Decode(lin_core_get_capture_device(lc));

return "";
};
//---------------------------------------------------------------------------
AnsiString __fastcall TLinphone::GetPlaybackDevice(void)
{
  if ( lc != NULL )
    return UTF8Decode(lin_core_get_playback_device(lc));

return "";
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::SetCaptureDevice(const AnsiString &DevName)
{
  if ( lc && !DevName.IsEmpty() )
    lin_core_set_capture_device(lc, UTF8Encode(DevName).c_str());
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::SetPlaybackDevice(const AnsiString &DevName)
{
  if ( lc && !DevName.IsEmpty() )
    lin_core_set_playback_device(lc, UTF8Encode(DevName).c_str());
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::SetWindowPosition(int Left, int Top, int Width, int Height)
{
     HWND hWnd = VideoWindow();
     if ( hWnd )
       {
          SetWindowPos(hWnd, HWND_TOPMOST, Left, Top, Width, Height, 0);
//          SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_NOACTIVATE);
       };
};
//---------------------------------------------------------------------------
void __fastcall TLinphone::ResetRetryCounter(void)
{
    retryCounter = 2;
}
//---------------------------------------------------------------------------
bool  __fastcall TLinphone::ActiveCall(void) {

    if ( lc ) {
        return lin_core_get_current_call(lc) != NULL;
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TLinphone::Clean(void) {

    retryCounter = 0;
    RegistrationInProgress = false;
    lastIdent = "";
    lastHost = "";
    TerminateCall();
    Unregister();

    if ( lc ) {
        lin_core_set_network_reachable(lc, false);
    };
}
