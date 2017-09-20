//---------------------------------------------------------------------------

#ifndef aboutH
#define aboutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "AdvSmoothButton.hpp"
#include "AdvSmoothPanel.hpp"
//---------------------------------------------------------------------------
class Tfabout : public TForm
{
__published:	// IDE-managed Components
        TAdvSmoothPanel *AdvSmoothPanel1;
        TAdvSmoothButton *bOK;
        TLabel *Label1;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *Label2;
        TLabel *Label6;
        TLabel *Label7;
        TLabel *Label8;
        void __fastcall Label2Click(TObject *Sender);
        void __fastcall bOKClick(TObject *Sender);
        void __fastcall Label4Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall Tfabout(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE Tfabout *fabout;
//---------------------------------------------------------------------------
#endif
