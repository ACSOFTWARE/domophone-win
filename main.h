//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "linphone.h"
#include <ExtCtrls.hpp>
#include "AdvGlowButton.hpp"
#include "AdvPanel.hpp"
#include "AdvOfficeStatusBar.hpp"
#include "AdvOfficeStatusBarStylers.hpp"
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
#include "AdvSmoothToggleButton.hpp"
#include "frxpngimage.hpp"
#include <jpeg.hpp>
#include "HTMLabel.hpp"
#include "AdvSmoothProgressBar.hpp"
#include "AdvCircularProgress.hpp"
#include <ImgList.hpp>
#include "AdvMenus.hpp"
#include <Menus.hpp>
#include "AdvGlassButton.hpp"
#include "language.h"
#include "dconnection.h"
#include "PictureContainer.hpp"
#include "AdvPicture.hpp"
#include <Graphics.hpp>
#include "AdvMenuStylers.hpp"
//---------------------------------------------------------------------------
#define ST_CONNECTING       0
#define ST_AUTHERROR        1
#define ST_CONNECTIONERROR  2
#define ST_COMPATERROR      3
#define ST_CONNECTED        4
#define ST_OPENING          5
#define ST_WAITING          6

class TActionBtn 
{
private:
   TImage *FImage;

   short FBTN;

   int FIdxDisabled;
   int FIdxEnabled;
   int FIdxSelected;
   int FIdxMouseOver;

   bool FEnabled;
   bool FSelected;
   void __fastcall FSetEnabled(bool Enabled);
   void __fastcall FSetSelected(bool Selected);

   void __fastcall ImgMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);

public:

  TActionBtn(short BTN, TImage *Img, int IdxDisabled, int IdxEnabled, int IdxMouseOver, int IdxSelected);
  TActionBtn();
  ~TActionBtn(void);
  void __fastcall Update(void);
  void __fastcall UpdateAll(void);
  bool __fastcall AnyEnabled(void);
  void DisableAll(void);

  TActionBtn *GetBtn(short BTN);

   __property bool Enabled = {read=FEnabled, write=FSetEnabled};
   __property bool Selected = {read=FSelected, write=FSetSelected};

};

class Tfmain : public TForm
{
__published:	// IDE-managed Components
        TTimer *LinphoneTimer;
        TImage *Image1;
        TImageList *ImageList1;
        TTimer *AlertTimer;
        TAdvPopupMenu *popup1;
        TMenuItem *Zamknij1;
        TTimer *AutoHideTimer;
        TMenuItem *N1;
        TMenuItem *Ustawienia1;
        TMenuItem *Jzyk1;
        TMenuItem *Oprogramie1;
        TImage *Image2;
        TImage *ImgView;
        TImage *ImgVoice;
        TImage *ImgGateway;
        TImage *ImgGate;
        TImageList *ImageList2;
        TAdvPicture *AdvPicture1;
        TLabel *lStatus;
        TImage *Image3;
        TAdvMenuOfficeStyler *AdvMenuOfficeStyler1;
        TTimer *OpenTimeoutTimer;
        TTimer *SipTimeoutTimer;
        void __fastcall LinphoneTimerTimer(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall PanelMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall Image1Click(TObject *Sender);
        void __fastcall Zamknij1Click(TObject *Sender);
        void __fastcall AutoHideTimerTimer(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall Ustawienia1Click(TObject *Sender);
        void __fastcall popup1Popup(TObject *Sender);
        void __fastcall Oprogramie1Click(TObject *Sender);
        void __fastcall btnOpen2Click(TObject *Sender);
        void __fastcall Image2MouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall ImgViewClick(TObject *Sender);
        void __fastcall ImgGatewayClick(TObject *Sender);
        void __fastcall ImgGateClick(TObject *Sender);
        void __fastcall OpenTimeoutTimerTimer(TObject *Sender);
        void __fastcall SipTimeoutTimerTimer(TObject *Sender);
        void __fastcall AlertTimerTimer(TObject *Sender);
private:	// User declarations
        TWndMethod OldWndProc;
        void __fastcall _WndProc(Messages::TMessage &Message);

        TConnectionSettings LastCS;

        char sAuthKey[AUTHKEY_SIZE];
        char sSerialKey[ID_SIZE];
        char sID[ID_SIZE];

        AnsiString sHostName;
        bool sAllowProxy;
        bool sShowApp;
        bool sPlaySound;
        BYTE LangID;

        void __fastcall SetLang(void);
        void __fastcall SaveSettings(void);
        void __fastcall LoadSettings(void);
        void __fastcall LoadSettings_AudioDevice(TRegistry *Reg, const AnsiString &RegName, bool Capture);
        void __fastcall OnAuthorize(TObject *Sender, const TConnectionSettings &Settings);
        void __fastcall OnDisconnect(TObject *Sender);
        void __fastcall OnUnauthorize(TObject *Sender);
        void __fastcall OnRing(TObject *Sender);
        void __fastcall OnEvent(TObject *Sender, const TdEvent &Event);
        void __fastcall OnSysState(TObject *Sender, int SysState);
        void __fastcall OnLocked(TObject *Sender, AnsiString Name);
        void __fastcall OnVersionError(TObject *Sender);
        void __fastcall SetStatus(short Status, AnsiString Msg, int Img, bool ActInd = false);
        void __fastcall ShowStatus(short _ShowStatus);
        void __fastcall ShowAlert(AnsiString Msg, int ImgIdx, int Time, int BlinkInterval = 100);
        void __fastcall HideAlert(void);  
        int __fastcall GetStatus(void);
        void __fastcall TerminateConnection(bool Wait = true);
        void __fastcall InitConnection(void);
        void __fastcall _Show(void);
        TLinphone* __fastcall GetLin(void);
        AnsiString __fastcall getSipIdent(void);
        void __fastcall et_onend_es_open(void);

        void __fastcall LangOnClick(TObject *Sender);
        int Caps;

public:		// User declarations
        __fastcall Tfmain(TComponent* Owner);
        __fastcall ~Tfmain(void);
        void __fastcall SetVideoWindowPos(void);
        void __fastcall SipTerminateWithSendAction(bool SendAction);
        void __fastcall SipTerminate(void);
        void __fastcall OnSipRegistered(void);
        void __fastcall OnVideoStarted(void);

};
//---------------------------------------------------------------------------
extern PACKAGE Tfmain *fmain;
extern Tlanguage *Lang;

//---------------------------------------------------------------------------
#endif
