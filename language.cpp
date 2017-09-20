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
 
#pragma hdrstop

#include "language.h"
#include <Forms.hpp>

//---------------------------------------------------------------------------
 __fastcall Tlanguage::Tlanguage(void)
{
    STR = NULL;
    Count = 0;
};
//---------------------------------------------------------------------------
BYTE __fastcall Tlanguage::Load(BYTE LangID)
{
    if ( STR )
      {
        delete [] STR;
        STR = NULL;
      };

    Count = 0;

    AnsiString File = ExtractFilePath(Application->ExeName)+"Lang\\"+IntToStr(LangID)+".ini";

    if ( !FileExists(File) )
      {
         LangID = GetUserDefaultUILanguage()&0xFF;
         File = ExtractFilePath(Application->ExeName)+"Lang\\"+IntToStr(LangID)+".ini";


         if ( !FileExists(File) )
           {
              LangID = 9;
              File = ExtractFilePath(Application->ExeName)+"Lang\\9.ini";
           };
      };

    if ( FileExists(File) )
      {
         TIniFile *Ini = NULL;
         TStringList *SL = new TStringList();
         try
           {
              Ini = new TIniFile(File);
              Ini->ReadSections(SL);

              int a;
              for(a=0;a<SL->Count;a++)
                if ( SL->Strings[a] == "STR" )
                   {
                      LoadBase(Ini, SL->Strings[a], this->Count, &this->STR);
                   }
           }
         __finally
           {
              delete Ini;
           };
      };
      
return LangID;
};
__fastcall Tlanguage::~Tlanguage(void)
{
    if ( STR )
      {
        delete [] STR;
        STR = NULL;
      };
};
//---------------------------------------------------------------------------
void __fastcall Tlanguage::GetLanguages(TStringList *SL)
{
     TSearchRec sr;
     AnsiString Dir = ExtractFilePath(Application->ExeName)+"Lang\\";
     if (FindFirst(Dir+"*.ini", faAnyFile , sr) == 0)
       {
           do
             {
               if ( sr.Attr & faArchive )
                 {
                    int n = sr.Name.SubString(1, sr.Name.Length()-4).ToIntDef(0);
                    if ( n > 0 )
                      {
                        TIniFile *Ini = new TIniFile(Dir+sr.Name);
                        try
                          {
                            AnsiString L = Ini->ReadString("STR", "0", "");
                            if ( !L.IsEmpty() )
                              SL->Add(IntToStr(n)+"="+L);
                          }
                        __finally
                          {
                             delete Ini;
                          };
                      };

                 };

             } while (FindNext(sr) == 0);

           FindClose(sr);
       };
};
//---------------------------------------------------------------------------
void __fastcall Tlanguage::LoadBase(TIniFile *Ini, const AnsiString &Section, int &Count, AnsiString **STR)
{
      Count = 0;
      TStringList *SL = new TStringList();
      try
        {
            Ini->ReadSection(Section, SL);

            int a,x;

            for(a=0;a<SL->Count;a++)
              try
                {
                   x = SL->Strings[a].ToInt();

                   if ( x > Count )
                    Count = x;
                }
              catch(...) {};

            if ( Count > 0 )
              {
                 *STR = new AnsiString[Count];

                 for(a=0;a<Count;a++)
                   try
                     {
                        (*STR)[a] = Ini->ReadString(Section, IntToStr(a+1), IntToStr(a+1));
                     }
                   catch(...) {};
              };
        }
      __finally
        {
           delete SL;
        };
};
//---------------------------------------------------------------------------
AnsiString __fastcall Tlanguage::Get(int ID, AnsiString *STR, int Count)
{
    if ( ID <= 0
         || ID > Count
         || STR[ID-1].IsEmpty() ) return "!"+IntToStr(ID);

    return StringReplace(STR[ID-1], "\\n", "\n", TReplaceFlags() << rfReplaceAll);
};
//---------------------------------------------------------------------------
AnsiString __fastcall Tlanguage::Get(int ID)
{
   return Get(ID, this->STR, this->Count);
};
//---------------------------------------------------------------------------
AnsiString __fastcall Tlanguage::Get(int ID, const AnsiString &Param1)
{
   return StringReplace(Get(ID), "$1", Param1, TReplaceFlags() << rfReplaceAll);
};
//---------------------------------------------------------------------------
AnsiString __fastcall Tlanguage::Get(int ID, const AnsiString &Param1, const AnsiString &Param2)
{
   return StringReplace(Get(ID, Param1), "$2", Param2, TReplaceFlags() << rfReplaceAll);
};
//---------------------------------------------------------------------------
AnsiString __fastcall Tlanguage::Get(int ID, const AnsiString &Param1, const AnsiString &Param2, const AnsiString &Param3)
{
   return StringReplace(Get(ID, Param1, Param2), "$3", Param3, TReplaceFlags() << rfReplaceAll);
};
//---------------------------------------------------------------------------
AnsiString __fastcall Tlanguage::Get(int ID, const AnsiString &Param1, const AnsiString &Param2, const AnsiString &Param3, const AnsiString &Param4)
{
   return StringReplace(Get(ID, Param1, Param2, Param3), "$4", Param4, TReplaceFlags() << rfReplaceAll);
};
//---------------------------------------------------------------------------

#pragma package(smart_init)
