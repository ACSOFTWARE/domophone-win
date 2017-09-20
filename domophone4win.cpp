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
//---------------------------------------------------------------------------
USEFORM("main.cpp", fmain);
USEFORM("settings.cpp", fsettings);
USEFORM("about.cpp", fabout);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

        HANDLE Mutex = CreateMutex(NULL, true, ExtractFileName(Application->ExeName).c_str());

        if (Mutex != 0
            && GetLastError() == ERROR_ALREADY_EXISTS)
        {
               MessageBox(NULL, "Program jest ju¿ uruchomiony.", "", MB_OK|MB_ICONSTOP|MB_TASKMODAL);
               return 0;
        };

        try
        {
                 Application->ShowMainForm = false;
                 Application->Initialize();
                 Application->CreateForm(__classid(Tfmain), &fmain);
                 Application->CreateForm(__classid(Tfabout), &fabout);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        catch (...)
        {
                 try
                 {
                         throw Exception("");
                 }
                 catch (Exception &exception)
                 {
                         Application->ShowException(&exception);
                 }
        }

        ReleaseMutex(Mutex);
        return 0;
}
//---------------------------------------------------------------------------
