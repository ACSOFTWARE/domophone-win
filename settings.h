//---------------------------------------------------------------------------

#ifndef settingsH
#define settingsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdvSmoothPanel.hpp"
#include "AdvEdit.hpp"
#include "AdvSmoothSlider.hpp"
#include "AdvSmoothButton.hpp"
#include "AdvOfficeButtons.hpp"
#include "AdvSmoothComboBox.hpp"
#include "AdvSmoothListBox.hpp"
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class Tfsettings : public TForm
{
__published:	// IDE-managed Components
        TAdvSmoothPanel *AdvSmoothPanel1;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TAdvSmoothButton *bOK;
        TAdvSmoothButton *AdvSmoothButton2;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label6;
        TEdit *edHostName;
        TEdit *k1;
        TEdit *k2;
        TEdit *k3;
        TEdit *k4;
        TLabel *Label7;
        TLabel *Label8;
        TLabel *Label9;
        TAdvOfficeCheckBox *cbProxy;
        TAdvOfficeCheckBox *cbSound;
        TAdvOfficeCheckBox *cbShow;
        TAdvSmoothComboBox *DevIn;
        TLabel *Label10;
        TLabel *Label11;
        TAdvSmoothComboBox *DevOut;
        TShape *Shape1;
        TShape *Shape2;
        void __fastcall AdvSmoothButton2Click(TObject *Sender);
        void __fastcall edHostNameChange(TObject *Sender);
        void __fastcall bOKClick(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ssAllowProxyStateChanged(TObject *Sender,
          TAdvSmoothSliderState State, double Value);
        void __fastcall cbSoundClick(TObject *Sender);
        void __fastcall cbShowClick(TObject *Sender);
        void __fastcall DevInItemSelectionChanged(TObject *Sender,
          int previousitemindex, int itemindex);
private:	// User declarations
public:		// User declarations
        __fastcall Tfsettings(TComponent* Owner);

        char Key[8];

};
//---------------------------------------------------------------------------
extern PACKAGE Tfsettings *fsettings;
//---------------------------------------------------------------------------
#endif
