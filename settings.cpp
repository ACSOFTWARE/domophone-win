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

#include "settings.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvSmoothPanel"
#pragma link "AdvEdit"
#pragma link "AdvSmoothSlider"
#pragma link "AdvSmoothButton"
#pragma link "AdvOfficeButtons"
#pragma link "AdvSmoothComboBox"
#pragma link "AdvSmoothListBox"
#pragma resource "*.dfm"
Tfsettings *fsettings = NULL;
//---------------------------------------------------------------------------
__fastcall Tfsettings::Tfsettings(TComponent* Owner)
        : TForm(Owner)
{
    Caption = lngGet(13);
    Label10->Caption = lngGet(14);
    Label11->Caption = lngGet(15);
    Label7->Caption = lngGet(16);
    Label8->Caption = lngGet(17);
    Label9->Caption = lngGet(18);
    Label1->Caption = lngGet(19);
    Label2->Caption = lngGet(20);
    Label3->Caption = lngGet(21);
    bOK->Caption = lngGet(22);
    AdvSmoothButton2->Caption = lngGet(23);
}
//---------------------------------------------------------------------------
void __fastcall Tfsettings::AdvSmoothButton2Click(TObject *Sender)
{
    Close();        
}
//---------------------------------------------------------------------------
void __fastcall Tfsettings::edHostNameChange(TObject *Sender)
{
    bOK->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall Tfsettings::bOKClick(TObject *Sender)
{
     memset(Key, 0, 8);

     if ( k1->Text.Length() == 4
          && k2->Text.Length() == 4
          && k3->Text.Length() == 4
          && k4->Text.Length() == 4 )
       try
         {
            HexToBin(k1->Text.c_str(), &Key[0], 2);
            HexToBin(k2->Text.c_str(), &Key[2], 2);
            HexToBin(k3->Text.c_str(), &Key[4], 2);
            HexToBin(k4->Text.c_str(), &Key[6], 2);
         }
       catch(...)
         {
            memset(Key, 0, 8);
         };

     Tag = 1;
     Close();
}
//---------------------------------------------------------------------------
void __fastcall Tfsettings::FormShow(TObject *Sender)
{
     k1->Text = "";
     k2->Text = "";
     k3->Text = "";
     k4->Text = "";

     if ( Key[0] != 0
          || Key[1] != 0
          || Key[2] != 0
          || Key[3] != 0 )
       try
         {
            AnsiString TXT;
            TXT.SetLength(16);
            BinToHex(Key, TXT.c_str(), 8);
            if ( TXT.Length() == 16 )
              {
                 k1->Text = TXT.SubString(1, 4).UpperCase();
                 k2->Text = TXT.SubString(5, 4).UpperCase();
                 k3->Text = TXT.SubString(9, 4).UpperCase();
                 k4->Text = TXT.SubString(13, 4).UpperCase();
              };
         }
       catch(...) {};

       bOK->Enabled = false;
}
//---------------------------------------------------------------------------

void __fastcall Tfsettings::ssAllowProxyStateChanged(TObject *Sender,
      TAdvSmoothSliderState State, double Value)
{
     edHostNameChange(Sender);
}
//---------------------------------------------------------------------------

void __fastcall Tfsettings::cbSoundClick(TObject *Sender)
{
     if ( !cbSound->Checked
          && !cbShow->Checked )
      cbSound->Checked = true;

     edHostNameChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall Tfsettings::cbShowClick(TObject *Sender)
{
     if ( !cbSound->Checked
          && !cbShow->Checked )
      cbShow->Checked = true;

     edHostNameChange(Sender);
}
//---------------------------------------------------------------------------
void __fastcall Tfsettings::DevInItemSelectionChanged(TObject *Sender,
      int previousitemindex, int itemindex)
{
     edHostNameChange(Sender);
}
//---------------------------------------------------------------------------

