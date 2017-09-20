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
//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "about.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "AdvSmoothButton"
#pragma link "AdvSmoothPanel"
#pragma resource "*.dfm"
Tfabout *fabout;
//---------------------------------------------------------------------------
__fastcall Tfabout::Tfabout(TComponent* Owner)
        : TForm(Owner)
{
   Caption = lngGet(11);
   bOK->Caption = lngGet(22);
}
//---------------------------------------------------------------------------
void __fastcall Tfabout::Label2Click(TObject *Sender)
{
   ShellExecute(GetDesktopWindow(), "open", "http://www.acsoftware.pl", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall Tfabout::Label4Click(TObject *Sender)
{
   ShellExecute(GetDesktopWindow(), "open", "http://www.domophone.eu", NULL, NULL, SW_SHOWNORMAL);
}
//---------------------------------------------------------------------------
void __fastcall Tfabout::bOKClick(TObject *Sender)
{
    Close();
}
//---------------------------------------------------------------------------


