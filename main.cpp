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

#define WM_SYSTEM_TRAY_NOTIFY (WM_USER + 1)
#define AUTOHIDE_TIME  5

#define COLOR_DOWN   0x0000E100
#define COLOR_UP     0x00FF870F

#include "main.h"
#include "settings.h"
#include "mmsystem.h"
#include "about.h"
#include "domophoneconnection.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvGlowButton"
#pragma link "AdvPanel"
#pragma link "AdvOfficeStatusBar"
#pragma link "AdvOfficeStatusBarStylers"
#pragma link "AdvSmoothButton"
#pragma link "AdvSmoothPanel"
#pragma link "AdvSmoothToggleButton"
#pragma link "frxpngimage"
#pragma link "HTMLabel"
#pragma link "AdvSmoothProgressBar"
#pragma link "AdvCircularProgress"
#pragma link "AdvMenus"
#pragma link "AdvGlassButton"
#pragma link "PictureContainer"
#pragma link "AdvPicture"
#pragma link "AdvMenuStylers"
#pragma resource "*.dfm"
Tfmain *fmain;
Tlanguage *Lang;
//---------------------------------------------------------------------------

#define BTN_VIEW       0
#define BTN_VOICE      1
#define BTN_GATEWAY    2
#define BTN_GATE       3


TList *Btns = new TList();

TActionBtn::TActionBtn(short BTN, TImage *Img, int IdxDisabled, int IdxEnabled, int IdxMouseOver, int IdxSelected)
{
    Btns->Add(this);
    FBTN = BTN;
    FEnabled = false;
    FSelected = false;
    FImage = Img;
    FImage->OnMouseMove = ImgMouseMove;
    FImage->Tag = -1;
    FImage->Enabled = false;
    FIdxDisabled = IdxDisabled;
    FIdxEnabled = IdxEnabled;
    FIdxSelected = IdxSelected;
    FIdxMouseOver = IdxMouseOver;

    Update();
}

//---------------------------------------------------------------------------
TActionBtn::TActionBtn(void)
{
    FImage = NULL;
};
//---------------------------------------------------------------------------

TActionBtn::~TActionBtn(void)
{
    Btns->Remove(this);
}

//---------------------------------------------------------------------------
void __fastcall TActionBtn::Update(void)
{
     if ( FImage == NULL ) return;

     int Idx;
     TPoint P;
     GetCursorPos(&P);
     P = FImage->ScreenToClient(P);
     bool FMouseOver = P.x > 0 && P.y > 0 && P.x <= FImage->Width && P.y <= FImage->Height;

     if ( Enabled )
       {
          if ( FSelected )
            {
                Idx = FIdxSelected;
            }
          else if ( FMouseOver )
            {
               Idx = FIdxMouseOver;
            }
          else
            {
               Idx = FIdxEnabled;
            }

          if ( FImage->Cursor != crHandPoint )
            {
              FImage->Cursor = crHandPoint;
            }
       }
     else
       {
          Idx = FIdxDisabled;

          if ( FImage->Cursor != crNone )
            {
              FImage->Cursor = crArrow;
            }
       };

     if ( Idx != FImage->Tag )
       {
          FImage->Tag = Idx;
          FImage->Picture = NULL;
          fmain->ImageList2->GetBitmap(Idx, FImage->Picture->Bitmap);
//          FImage->Repaint();
       };

};
//---------------------------------------------------------------------------
void __fastcall TActionBtn::UpdateAll(void)
{
     Update();

     for(int a=0;a<Btns->Count;a++)
       ((TActionBtn*)Btns->Items[a])->Update();
};
//---------------------------------------------------------------------------
void __fastcall TActionBtn::ImgMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
     UpdateAll();
}
//---------------------------------------------------------------------------
void __fastcall TActionBtn::FSetEnabled(bool Enabled)
{
     FEnabled = Enabled;
     FImage->Enabled = Enabled;
     Update();
};
//---------------------------------------------------------------------------
void __fastcall TActionBtn::FSetSelected(bool Selected)
{
     FSelected = Selected;
     Update();
};
//---------------------------------------------------------------------------
TActionBtn* TActionBtn::GetBtn(short BTN)
{
     for(int a=0;a<Btns->Count;a++)
       if ( ((TActionBtn*)Btns->Items[a])->FBTN == BTN )
         {
            return (TActionBtn*)Btns->Items[a];
         };

return NULL;
};
//---------------------------------------------------------------------------
void TActionBtn::DisableAll(void)
{
     for(int a=0;a<Btns->Count;a++)
       ((TActionBtn*)Btns->Items[a])->Enabled = false;
};
//---------------------------------------------------------------------------
bool __fastcall TActionBtn::AnyEnabled(void)
{
     for(int a=0;a<Btns->Count;a++)
       if ( ((TActionBtn*)Btns->Items[a])->Enabled ) return true;

return false;
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


__fastcall Tfmain::Tfmain(TComponent* Owner)
        : TForm(Owner)
{
    Lang = new Tlanguage();
    Visible = true;
    sAllowProxy = false;
    sPlaySound = true;
    sShowApp = true;
    Caps = 0;

    new TActionBtn(BTN_VIEW, ImgView, 0, 1, 2, 3);
    new TActionBtn(BTN_VOICE, ImgVoice, 4, 5, 6, 7);
    new TActionBtn(BTN_GATEWAY, ImgGateway, 8, 9, 10, 11);
    new TActionBtn(BTN_GATE, ImgGate, 12, 13, 14, 15);

    TStringList *SL = new TStringList();
    try
      {
         Lang->GetLanguages(SL);
         TMenuItem *MI;
         for(int a=0;a<SL->Count;a++)
           {
              MI = new TMenuItem(popup1->Items);
              MI->Tag = SL->Names[a].ToInt();
              MI->Caption = SL->Values[SL->Names[a]];
              MI->OnClick = LangOnClick;
              Jzyk1->Add(MI);
           };
      }
    __finally
      {
         delete SL;
      };

    memset(sAuthKey, 0, AUTHKEY_SIZE);

    OldWndProc = WindowProc;
    WindowProc = _WndProc;

    Lin = NULL;

    NOTIFYICONDATA nid;

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = Handle;
    nid.uID = 1;
    nid.uCallbackMessage = WM_SYSTEM_TRAY_NOTIFY;
    nid.uFlags = NIF_MESSAGE | NIF_ICON;
    TIcon *I = new TIcon();
    try
      {
        nid.hIcon = Application->Icon->Handle;
        Shell_NotifyIcon(NIM_ADD, &nid);
      }
    __finally
      {
        delete I;
      };

    Left = Screen->WorkAreaWidth-Width;
    Top = Screen->WorkAreaHeight-Height;


    try
      {
        LoadSettings();
        InitConnection();
      }
    catch(...) {};

}
//---------------------------------------------------------------------------
__fastcall Tfmain::~Tfmain(void)
{
   delete Lang;
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::LinphoneTimerTimer(TObject *Sender)
{
     if ( Lin )
       {
          Lin->Iterate();
       };

}
//---------------------------------------------------------------------------
void __fastcall Tfmain::OpenTimeoutTimerTimer(TObject *Sender)
{
    OpenTimeoutTimer->Enabled = false;
    TdEvent Event;
    Event.Scope = ES_OPEN;
    Event.Type = ET_ONEND;

    OnEvent(Sender, Event);

}
//---------------------------------------------------------------------------
void __fastcall Tfmain::FormClose(TObject *Sender, TCloseAction &Action)
{
    if ( Tag == 1 )
      {
         NOTIFYICONDATA nid;

         nid.cbSize = sizeof(NOTIFYICONDATA);
         nid.hWnd = Handle;
         nid.uID = 1;

         Shell_NotifyIcon(NIM_DELETE, &nid);

         TerminateConnection();

         if ( Lin )
           Lin->Finish();

         #ifdef _DEBUG
           LinLog->SaveToFile("d:\\lin_log.txt");
         #endif

         LinphoneTimer->Enabled = false;
      }
    else
      {
         Action = Forms::caNone;
         Visible = false;
         SipTerminate();
      };
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::_Show(void)
{
    if ( !Visible )
      {
          Visible = true;
          Application->BringToFront();
          ShowWindow(Application->Handle, SW_HIDE);
      };
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::_WndProc(Messages::TMessage &Message)
{
   if ( Message.Msg == WM_SYSTEM_TRAY_NOTIFY )
     {
        if ( Message.LParam == WM_LBUTTONDOWN )
          {
             _Show();
          }
        else if ( Message.LParam == WM_RBUTTONDOWN )
          {
             popup1->PopupAtCursor();
          };
     }
   else
     {
          if ( Message.Msg == WM_MOVE )
            {
               SetVideoWindowPos();
            }
          else if ( Message.Msg == WM_NCHITTEST )
            {
               int a=0;
            };

      };

   if ( OldWndProc ) OldWndProc(Message);
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::SetVideoWindowPos(void)
{
    if ( Lin )
      Lin->SetWindowPosition(Left, Top-Width, Width, Width-2);
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::PanelMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    ReleaseCapture();
    SendMessage(Handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::TerminateConnection(bool Wait)
{
    if ( ConnectionPtr )
      {
         ConnectionPtr->Terminate();

         while(ConnectionPtr && Wait)
           {
             Application->ProcessMessages();
             Sleep(200);
           };
      };
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::InitConnection(void)
{
    TerminateConnection();

    lStatus->Tag = -1;

    if ( sHostName.Length() > 1 )
      {
          ConnectionPtr = new TDomophoneConnection(true, LangID, sAuthKey, sSerialKey, sID, sHostName, sAllowProxy);
          ConnectionPtr->FreeOnTerminate = true;
          ConnectionPtr->Priority = tpLower;
          ConnectionPtr->OnAuthorize = OnAuthorize;
          ConnectionPtr->OnUnauthorize = OnUnauthorize;
          ConnectionPtr->OnDisconnect = OnDisconnect;
          ConnectionPtr->OnEvent = OnEvent;
          ConnectionPtr->OnSysState = OnSysState;
          ConnectionPtr->OnLocked = OnLocked;
          ConnectionPtr->OnVersionError = OnVersionError;
          ConnectionPtr->Resume();
      }
    else
      {
         ShowStatus(ST_CONNECTIONERROR);
      };
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::Image1Click(TObject *Sender)
{
    if ( ConnectionPtr )
      ConnectionPtr->SipDisconnect();

    Close();
}
//---------------------------------------------------------------------------

void __fastcall Tfmain::SaveSettings(void)
{
  TRegistry *Reg = new TRegistry;
  try
    {
      Reg->RootKey = HKEY_CURRENT_USER;
      if ( Reg->OpenKey("\\Software\\Ac.Software\\DOMOPHONE", true) )
        {
          Reg->WriteString("Host", sHostName);
          Reg->WriteBinaryData("AuthKey", sAuthKey, sizeof(sAuthKey));
          Reg->WriteBool("Proxy", sAllowProxy);
          Reg->WriteBool("PlaySound", sPlaySound);
          Reg->WriteBool("ShowApp", sShowApp);
          Reg->WriteString("CaptureDevice", GetLin()->GetCaptureDevice());
          Reg->WriteString("PlaybackDevice", GetLin()->GetPlaybackDevice());
          Reg->WriteInteger("LangID", LangID);
          Reg->CloseKey();
        };
    }
  __finally
    {
      delete Reg;
    }
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::LoadSettings_AudioDevice(TRegistry *Reg, const AnsiString &RegName, bool Capture)
{
     AnsiString Dev;

     if ( Reg->ValueExists(RegName) )
          Dev = Reg->ReadString(RegName);

     TStringList *SL = new TStringList();
     GetLin()->GetAudioDevices(SL);

     if ( SL->IndexOf(Dev) == -1 )
       Dev = "";

     int a;

     if ( Dev.IsEmpty() )
       for(a=0;a<SL->Count;a++)
         if ( (  Capture
                 && GetLin()->IsCaptureDevice(SL->Strings[a]) )
              || ( !Capture
                   && GetLin()->IsPlaybackDevice(SL->Strings[a]) ) )
            {
               Dev = SL->Strings[a];
               break;
            };


     if ( Dev.IsEmpty() )
       for(a=0;a<SL->Count;a++)
         if ( ( Capture && GetLin()->IsCaptureDevice(SL->Strings[a]) )
              || ( !Capture && GetLin()->IsPlaybackDevice(SL->Strings[a]) ) )
            {
               Dev = SL->Strings[a];
               break;
            };

     delete SL;

     Capture ? GetLin()->SetCaptureDevice(Dev) : GetLin()->SetPlaybackDevice(Dev);
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::LoadSettings(void)
{
  LangID = 0;
  TRegistry *Reg = new TRegistry;
  try
    {
      Reg->RootKey = HKEY_CURRENT_USER;
      if ( Reg->OpenKey("\\Software\\Ac.Software\\DOMOPHONE", true) )
        {
          if ( Reg->ValueExists("Host")
               && Reg->ValueExists("AuthKey")
               && Reg->ValueExists("Proxy") )
            {
              sHostName = Reg->ReadString("Host");
              Reg->ReadBinaryData("AuthKey", sAuthKey, sizeof(sAuthKey));
              sAllowProxy = Reg->ReadBool("Proxy");
              sShowApp = Reg->ReadBool("PlaySound");
              sPlaySound = Reg->ReadBool("ShowApp");
            };

          if ( !Reg->ValueExists("ID")
               || Reg->ReadBinaryData("ID", sID, sizeof(sID)) != sizeof(sID) )
            {
               for(int a=0;a<ID_SIZE;a++)
                 sID[a] = (GetTickCount()+rand())%255;

               Reg->WriteBinaryData("ID", sID, sizeof(sID));
            };

          if ( Reg->ValueExists("LangID") )
            LangID = Reg->ReadInteger("LangID");

          LoadSettings_AudioDevice(Reg, "CaptureDevice", true);
          LoadSettings_AudioDevice(Reg, "PlaybackDevice", false);

          Reg->CloseKey();

        };
    }
  __finally
    {
      delete Reg;
    }

   LangID = Lang->Load(LangID);
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::SetStatus(short Status, AnsiString Msg, int Img, bool ActInd)
{
    lStatus->Tag = Status;
    lStatus->BringToFront();

    Image3->Visible = !ActInd;
    AdvPicture1->Animate = ActInd;
    AdvPicture1->Visible = ActInd;
    lStatus->Caption = Msg;

    if ( !ActInd )
       {
         Image3->Picture = NULL;
         ImageList1->GetBitmap(Img, Image3->Picture->Bitmap);
       };

};
//---------------------------------------------------------------------------
void __fastcall Tfmain::ShowStatus(short Status)
{
    if (  AlertTimer->Enabled )
      {
         lStatus->Tag = Status;
         return;
      };

    if ( lStatus->Tag != Status
         && lStatus->Tag != ST_AUTHERROR )
      {

          switch(Status)
            {
                case ST_CONNECTING:
                     SetStatus(Status, lngGet(6), 0, true); //"£¹czenie..."
                     break;
                case ST_AUTHERROR:
                     TActionBtn().DisableAll();
                     SetStatus(Status, lngGet(2), 1, false); //*B³êdny klucz autoryzacji*/
                     _Show();
                     break;
                case ST_CONNECTIONERROR:
                     TActionBtn().DisableAll();
                     SetStatus(Status, lngGet(1), 0, false); //*Brak po³¹czenia*/
                     break;
                case ST_COMPATERROR:
                     TActionBtn().DisableAll();
                     SetStatus(Status, lngGet(24), 0, false); //*B³¹d kompatybilnoœci*/
                     break;
                case ST_CONNECTED:
                     SetStatus(Status, lngGet(5), 2, false); //*Po³¹czony*/
                     break;
                case ST_OPENING:
                     SetStatus(Status, lngGet(7), 0, true); //"Otwieram..."
                     break;
                case ST_WAITING:
                     SetStatus(Status, lngGet(8), 0, true); //"Oczekiwanie..."
                     break;
            };
      };

};
//---------------------------------------------------------------------------
void __fastcall Tfmain::ShowAlert(AnsiString Msg, int ImgIdx, int Time, int BlinkInterval)
{
    lStatus->BringToFront();
    lStatus->Caption = Msg;

    Image3->Picture = NULL;
    ImageList1->GetBitmap(ImgIdx, Image3->Picture->Bitmap);

    AlertTimer->Interval = BlinkInterval;
    AlertTimer->Tag = Time * 1000 / BlinkInterval;
    AlertTimer->Enabled = true;
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::HideAlert(void)
{
      AlertTimer->Enabled = false;
      int s = lStatus->Tag;
      lStatus->Tag = -1;
      ShowStatus(s);
};
//---------------------------------------------------------------------------
int __fastcall Tfmain::GetStatus(void)
{
    return lStatus->Tag;
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::OnAuthorize(TObject *Sender, const TConnectionSettings &Settings)
{
     bool Save = false;

     if ( memcmp(Settings.AuthKey, sAuthKey, AUTHKEY_SIZE) != 0 )
       {
          memcpy(sAuthKey, Settings.AuthKey, AUTHKEY_SIZE);
          Save = true;
       };

     if ( memcmp(Settings.SerialKey, sSerialKey, ID_SIZE ) != 0 )
       {
          memcpy(sSerialKey, Settings.SerialKey, ID_SIZE );
          Save = true;
       };


     if ( Save )
       {
         SaveSettings();
       };

     TActionBtn().GetBtn(BTN_VOICE)->Enabled = Settings.Caps & CAP_AUDIO;
     TActionBtn().GetBtn(BTN_VIEW)->Enabled = Settings.Caps & CAP_VIDEO;
     TActionBtn().GetBtn(BTN_GATEWAY)->Enabled = Settings.Caps & CAP_OPEN1;
     TActionBtn().GetBtn(BTN_GATE)->Enabled = Settings.Caps & CAP_OPEN2;

     LastCS = Settings;
     ShowStatus(ST_CONNECTED);
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::OnUnauthorize(TObject *Sender)
{
     ShowStatus(ST_AUTHERROR);
     TerminateConnection(false);
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::OnDisconnect(TObject *Sender)
{
    TActionBtn().DisableAll();
    ShowStatus(ST_CONNECTING);
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::OnEvent(TObject *Sender, const TdEvent &Event)
{

    if ( Event.Type == ET_ONBEGIN
        && Event.Scope == ES_RING ) {

          ShowAlert(lngGet(4), 3, 10);

          if ( sShowApp )
            _Show();

          if ( sPlaySound )
            PlaySound(AnsiString(ExtractFilePath(Application->ExeName)+"\\ring.wav").c_str(), Application->Handle, SND_ASYNC|SND_FILENAME);

    } else if ( Event.Type == ET_ONBEGIN ) {

        if ( Event.Scope == ES_OPEN ) {
            ShowStatus(ST_OPENING);
        }

        if ( !Event.Owner ) {
            if (  Event.Scope == ES_OPEN
                  && ( Event.Param1 == ACTION_OPEN1
                      || Event.Param1 == ACTION_OPEN2 ) ) {
               // [self insertLogItem: Event->Param1 == ACTION_OPEN1 ? NSLocalizedString(@"Gateway opening", nil) : NSLocalizedString(@"Opening / closing the gate", nil) senderName:[NSString stringWithWideString:Event->SenderName] showLog:YES];
            } else if ( Event.Scope == ES_SIP ) {
               // [self insertLogItem: NSLocalizedString(@"Audio/video connection started", nil) senderName:[NSString stringWithWideString:Event->SenderName] showLog:YES];
            }
        }



    } else if ( Event.Type == ET_ONEND
               && Event.Scope == ES_OPEN ) {
        if ( GetStatus() == ST_OPENING )
           {
              et_onend_es_open();
           };
    } else if ( Event.Type == ET_ONEND
                && Event.Scope == ES_SIP ) {
        if ( Event.Owner == 1 ) {
           SipTerminate();
        } else {
          //  [self insertLogItem: NSLocalizedString(@"Audio/video connection finished", nil) senderName:[NSString stringWithWideString:Event->SenderName] showLog:YES];
        }

    }

};
//---------------------------------------------------------------------------
void __fastcall Tfmain::OnSysState(TObject *Sender, int SysState)
{

    if ( GetStatus() == ST_OPENING
        && !(SysState & SYS_STATE_OPENING1)
        && !(SysState & SYS_STATE_OPENING2)
        && !(SysState & SYS_STATE_OPENING3) ) {

          et_onend_es_open();
          ShowStatus(ST_CONNECTED);

          TActionBtn().GetBtn(BTN_GATE)->Selected = false;
          TActionBtn().GetBtn(BTN_GATEWAY)->Selected = false;
          OpenTimeoutTimer->Enabled = false;
    }

}
//---------------------------------------------------------------------------
void __fastcall Tfmain::et_onend_es_open(void)
{
    ShowStatus(ST_CONNECTED);

    TActionBtn().GetBtn(BTN_GATE)->Selected = false;
    TActionBtn().GetBtn(BTN_GATEWAY)->Selected = false;
    OpenTimeoutTimer->Enabled = false;
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::OnLocked(TObject *Sender, AnsiString Name)
{
    ShowAlert(lngGet(25)+" "+Name, 4, 1, 100);
    et_onend_es_open();
    SipTerminateWithSendAction(false);
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::OnVersionError(TObject *Sender)
{
     ShowStatus(ST_COMPATERROR);
     TerminateConnection(false);
}
//---------------------------------------------------------------------------

void __fastcall Tfmain::Zamknij1Click(TObject *Sender)
{
    Tag = 1;
    Close();
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::AutoHideTimerTimer(TObject *Sender)
{
     POINT P;
     GetCursorPos(&P);

     if ( GetStatus() == ST_CONNECTED
          && !AlertTimer->Enabled
          && !TActionBtn().GetBtn(BTN_VOICE)->Selected
          && !TActionBtn().GetBtn(BTN_VIEW)->Selected
          && ( P.x < Left
               || P.y < Top
               || P.x > Left+Width
               || P.y > Top+Height ) )
      {
        AutoHideTimer->Tag = AutoHideTimer->Tag-1;
      }
     else
      {
        AutoHideTimer->Tag = AUTOHIDE_TIME;
      };

     if ( AutoHideTimer->Tag <= 0 )
       {
         AutoHideTimer->Enabled = false;
         Close();
       };
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::FormShow(TObject *Sender)
{
   AutoHideTimer->Tag = AUTOHIDE_TIME;
   AutoHideTimer->Enabled = true;
}
//---------------------------------------------------------------------------
TLinphone* __fastcall Tfmain::GetLin(void)
{
      if ( !Lin )
        {
           Lin = new TLinphone(this);
           Lin->Start();
        };
        
return Lin;
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::Ustawienia1Click(TObject *Sender)
{
    if ( fsettings )
      {
        fsettings->BringToFront();
        fsettings->SetFocus();
        return;
      };

    fsettings = new Tfsettings(this);
    try
      {
          TStringList *SL = new TStringList();
          GetLin()->GetAudioDevices(SL);
          TAdvSmoothListBoxItem *I;

          fsettings->DevIn->Items->Clear();
          fsettings->DevOut->Items->Clear();

          for(int a=0;a<SL->Count;a++)
            {
                if ( GetLin()->IsCaptureDevice(SL->Strings[a]) )
                      fsettings->DevIn->Items->Add()->Caption = SL->Strings[a];

                if ( GetLin()->IsPlaybackDevice(SL->Strings[a]) )
                      fsettings->DevOut->Items->Add()->Caption = SL->Strings[a];
            };

          fsettings->DevIn->SelectedItemIndex = fsettings->DevIn->Items->IndexOfCaption(GetLin()->GetCaptureDevice());
          fsettings->DevOut->SelectedItemIndex = fsettings->DevOut->Items->IndexOfCaption(GetLin()->GetPlaybackDevice());

          fsettings->edHostName->Text = sHostName;
          memcpy(fsettings->Key, sAuthKey, AUTHKEY_SIZE);
          fsettings->cbProxy->Checked = sAllowProxy;
          fsettings->cbShow->Checked = sShowApp;
          fsettings->cbSound->Checked = sPlaySound;

          delete SL;
          fsettings->ShowModal();

          if ( fsettings->Tag )
            {
               sHostName = fsettings->edHostName->Text;
               memcpy(sAuthKey, fsettings->Key, AUTHKEY_SIZE);
               sAllowProxy = fsettings->cbProxy->Checked;
               sPlaySound = fsettings->cbSound->Checked;
               sShowApp = fsettings->cbShow->Checked;

               if ( fsettings->DevIn->SelectedItemIndex > -1 )
                 GetLin()->SetCaptureDevice(fsettings->DevIn->Items->Items[fsettings->DevIn->SelectedItemIndex]->Caption);

               if ( fsettings->DevOut->SelectedItemIndex > -1 )
                 GetLin()->SetPlaybackDevice(fsettings->DevOut->Items->Items[fsettings->DevOut->SelectedItemIndex]->Caption);

               SaveSettings();
               InitConnection();
            };
      }
    __finally
      {
        delete fsettings;
        fsettings = NULL;
      };
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::OnSipRegistered(void)
{
    if ( ConnectionPtr
        && GetLin()
        && !GetLin()->ActiveCall()
        && ( TActionBtn().GetBtn(BTN_VIEW)->Selected
             || TActionBtn().GetBtn(BTN_VOICE)->Selected ) ) {
            ShowStatus(ST_CONNECTED);
            ConnectionPtr->SipConnect(TActionBtn().GetBtn(BTN_VIEW)->Selected, TActionBtn().GetBtn(BTN_VOICE)->Selected);
    };
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::SipTerminateWithSendAction(bool SendAction) {

    if ( SendAction
         && ConnectionPtr )
      {
         ConnectionPtr->SipDisconnect();
      };

    if ( GetLin() )
         GetLin()->Clean();

    TActionBtn().GetBtn(BTN_VIEW)->Selected = false;
    TActionBtn().GetBtn(BTN_VOICE)->Selected = false;

     if ( GetStatus() == ST_CONNECTED
          && AdvPicture1->Visible )
       {
          Image3->Visible = true;
          AdvPicture1->Visible = false;
          AdvPicture1->Animate = false;
       };
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::SipTerminate(void)
{
    SipTerminateWithSendAction(true);
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::popup1Popup(TObject *Sender)
{
    Ustawienia1->Caption = lngGet(9);
    Jzyk1->Caption = lngGet(10);
    Oprogramie1->Caption = lngGet(11);
    Zamknij1->Caption = lngGet(12);

    for(int a=0;a<Jzyk1->Count;a++)
       Jzyk1->Items[a]->Checked = Jzyk1->Items[a]->Tag == LangID;
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::LangOnClick(TObject *Sender)
{
    LangID = dynamic_cast<TMenuItem*>(Sender)->Tag;
    SaveSettings();
    Lang->Load(LangID);
};

void __fastcall Tfmain::Oprogramie1Click(TObject *Sender)
{
     Tfabout *F = new Tfabout(this);
     try
       {
          F->ShowModal();
       }
     __finally
       {
          delete F;
       };
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::btnOpen2Click(TObject *Sender)
{
    if ( ConnectionPtr )
        ConnectionPtr->OpenGate();
}
//---------------------------------------------------------------------------

void __fastcall Tfmain::Image2MouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
     TActionBtn().UpdateAll();        
}
//---------------------------------------------------------------------------
AnsiString __fastcall Tfmain::getSipIdent(void)
{
    AnsiString TXT1, TXT2;
    TXT1.SetLength(16);
    TXT2.SetLength(16);
    BinToHex(sID, TXT1.c_str(), 8);
    BinToHex(sSerialKey, TXT2.c_str(), 8);

    return TXT1+"-"+TXT2;
};
//---------------------------------------------------------------------------
void __fastcall Tfmain::ImgViewClick(TObject *Sender)
{
    if ( ( Sender == ImgView
           && !TActionBtn().GetBtn(BTN_VIEW)->Selected )
          || ( Sender == ImgVoice
           && !TActionBtn().GetBtn(BTN_VOICE)->Selected ) )
       {
          HideAlert();
          
          if ( ConnectionPtr )
            {
                if ( !TActionBtn().GetBtn(BTN_VIEW)->Selected )
                  {
                      Image3->Visible = false;
                      AdvPicture1->Visible = true;
                      AdvPicture1->Animate = true;

                      SipTimeoutTimer->Enabled = false;
                      SipTimeoutTimer->Enabled = true;

                      GetLin()->ResetRetryCounter();
                      GetLin()->Register(getSipIdent(),  ConnectionPtr->SipServer());
                  }
                else
                  {
                      ConnectionPtr->SetSpeakerOn(true);
                  };

                TActionBtn().GetBtn(BTN_VIEW)->Selected = true;

                if ( Sender == ImgVoice )
                  {
                     TActionBtn().GetBtn(BTN_VOICE)->Selected = true;
                  };

            };
       }
     else
       {
          SipTerminate();
       };
}
//---------------------------------------------------------------------------

void __fastcall Tfmain::ImgGatewayClick(TObject *Sender)
{
    HideAlert();

    if ( !TActionBtn().GetBtn(BTN_GATEWAY)->Selected
         && ConnectionPtr )
      {
         ShowStatus(ST_OPENING);
         OpenTimeoutTimer->Enabled = false;
         OpenTimeoutTimer->Enabled = true;
         TActionBtn().GetBtn(BTN_GATEWAY)->Selected = true;
         ConnectionPtr->OpenGateway();
      };
}
//---------------------------------------------------------------------------

void __fastcall Tfmain::ImgGateClick(TObject *Sender)
{
    HideAlert();

    if ( !TActionBtn().GetBtn(BTN_GATE)->Selected
         && ConnectionPtr )
      {
         ShowStatus(ST_OPENING);
         OpenTimeoutTimer->Enabled = false;
         OpenTimeoutTimer->Enabled = true;
         TActionBtn().GetBtn(BTN_GATE)->Selected = true;
         ConnectionPtr->OpenGate();
      };
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::SipTimeoutTimerTimer(TObject *Sender)
{
     if ( GetStatus() == ST_CONNECTED )
       {
          Image3->Visible = true;
          AdvPicture1->Visible = false;
          AdvPicture1->Animate = false;
       };

     SipTerminate();
}
//---------------------------------------------------------------------------
void __fastcall Tfmain::OnVideoStarted(void)
{
     SipTimeoutTimer->Enabled = false;
     
     if ( GetStatus() == ST_CONNECTED )
       {
          Image3->Visible = true;
          AdvPicture1->Visible = false;
          AdvPicture1->Animate = false;
       };
};

void __fastcall Tfmain::AlertTimerTimer(TObject *Sender)
{
   if ( AlertTimer->Tag <= 0 )
     {
        HideAlert();
     }
   else
     {
        Image3->Visible = !Image3->Visible;
        AlertTimer->Tag--;
     };
}
//---------------------------------------------------------------------------


