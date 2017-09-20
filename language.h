//---------------------------------------------------------------------------

#ifndef pzLanguageH
#define pzLanguageH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <IniFiles.hpp>

#define lngGet Lang->Get
#define lngSet Lang->Set
#define lngName Lang->Name
#define lngShortName Lang->ShortName

class PACKAGE Tlanguage : public TObject
{
private:

   AnsiString *STR;
   int Count;

   AnsiString __fastcall Get(int ID, AnsiString *STR, int Count);
   void __fastcall LoadBase(TIniFile *Ini, const AnsiString &Section, int &Count, AnsiString **STR);

protected:
public:

    __fastcall Tlanguage::Tlanguage(void);
    __fastcall Tlanguage::~Tlanguage(void);

    BYTE __fastcall Load(BYTE LangID);
    void __fastcall GetLanguages(TStringList *SL);

    AnsiString __fastcall Get(int ID);
    AnsiString __fastcall Get(int ID, const AnsiString &Param1);
    AnsiString __fastcall Get(int ID, const AnsiString &Param1, const AnsiString &Param2);
    AnsiString __fastcall Get(int ID, const AnsiString &Param1, const AnsiString &Param2, const AnsiString &Param3);
    AnsiString __fastcall Get(int ID, const AnsiString &Param1, const AnsiString &Param2, const AnsiString &Param3, const AnsiString &Param4);

};

#endif
