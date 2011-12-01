//Open Tuner: Copyright 2008, 2009 Mark Billington.

//This file is part of "Open Tuner".
//
//"Open Tuner" is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//"Open Tuner" is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with "Open Tuner".  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////////////////////////
// OpenTuner.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OpenTuner.h"
#include "OpenTunerDlg.h"

#ifdef _DEBUG
#ifdef _AFX
#define new DEBUG_NEW
#endif
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifdef  _UNICODE
int APIENTRY wWinMain( //Should be a better way of doing this, but _tWinMain does not seem to be working in CE:/
#else
int APIENTRY WinMain(
#endif
                       HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR     lpCmdLine,
                     int       nCmdShow)
{
    {
        COpenTunerDlg dlgMain = COpenTunerDlg(hInstance, 0);

        int nResponse = dlgMain.DoModal();
	    if (nResponse == IDOK)
	    {
	    }
	    else if (nResponse == IDCANCEL)
	    {
	    }
    }
    
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
