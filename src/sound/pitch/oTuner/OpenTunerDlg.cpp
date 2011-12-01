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
// OpenTunerDlg.cpp : implementation file
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef UNDER_CE
#include <Winuser.h>
#include <aygshell.h>
#endif
#include <limits> //FLT_MAX
#include <deque>
#include <numeric>
#include "resource.h"
#include "OpenTunerDlg.h"
#include "tartini_core\tartinilib\tartini_core.h"
#include "cBscFileVersion.h"

#ifdef _DEBUG
#ifdef _AFX
#define new DEBUG_NEW
#endif
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                          WinCE Positioning Helper
//--------------------------------------------------------------------------------------------------
void COpenTunerDlg::SortOutMenuBarEtcIfRunningWinCE()
{
#ifdef UNDER_CE
    HMENU hSysMenu = NULL;//Don't want this in CE!  GetSystemMenu(m_hWnd, FALSE); //CE compiler won't let me use :: prefix!
    SHINITDLGINFO shido;//Make our dialog appear fullscreen without obscuring the softkey-bar
    ZeroMemory(&shido, sizeof(shido));
    shido.dwMask = SHIDIM_FLAGS;
    shido.dwFlags = SHIDIF_SIZEDLGFULLSCREEN;//| SHIDIF_DONEBUTTON;

    shido.hDlg = m_hWnd;
    SHInitDialog(&shido);

    SHMENUBARINFO mbi; //The softkey menu bar is an ugly white surrounding the keypad icon unless we do this
    ZeroMemory(&mbi, sizeof(SHMENUBARINFO));

    mbi.cbSize     = sizeof(SHMENUBARINFO);
    mbi.hwndParent = m_hWnd;
    mbi.nToolBarId = 0; //!!!Or I could add the play/stop buttons I guess... http://msdn2.microsoft.com/en-us/library/aa453678.aspx http://msdn2.microsoft.com/en-us/library/ms838268.aspx
    mbi.hInstRes   = m_hInstance;
    mbi.dwFlags    = SHCMBF_EMPTYBAR;

    SHCreateMenuBar(&mbi);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                         Construction/ Destruction
//--------------------------------------------------------------------------------------------------
COpenTunerDlg::COpenTunerDlg(HINSTANCE hInstance, HWND hParent) :
    
    CBscBblDlg(IDD_OpenTuner_DIALOG, hInstance, hParent, false, true),
    m_hIcon((HICON)LoadImage(m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON,0,0, 0x0040/*LR_DEFAULTSIZE*/)),
    m_hdcBmpTitleLHS   (0), m_hOldObjBmpTitleLHS   (0), m_hBmpTitleLHS   ((HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_LHS), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR)), //###currently 24bpp! is that best? probably yes, on ARM
    m_BmpTitleLHSWidth(110),  //###hardcoded!
    m_BmpTitleLHSHeight(24),
    m_hdcBmpTitleRHS   (0), m_hOldObjBmpTitleRHS   (0), m_hBmpTitleRHS   ((HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_RHS), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR)), //###currently 24bpp! is that best? probably yes, on ARM
    m_BmpTitleRHSWidth(36),
    m_BmpTitleRHSHeight(24),

    m_hdcBmpVolume   (0), m_hOldObjBmpVolume   (0), m_hBmpVolume   ((HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_VU), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR)), //###currently 24bpp! is that best? probably yes, on ARM
    m_BmpVolumeWidth(6),
    m_BmpVolumeHeight(48),

    m_hdcBmpArrowEmpLHS   (0), m_hOldObjBmpArrowEmpLHS   (0), m_hBmpArrowEmpLHS   ((HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_RARROW_EMP), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR)), //###currently 24bpp! is that best? probably yes, on ARM
    m_BmpArrowEmpLHSWidth(18),
    m_BmpArrowEmpLHSHeight(24),
    m_hdcBmpArrowEmpRHS   (0), m_hOldObjBmpArrowEmpRHS   (0), m_hBmpArrowEmpRHS   ((HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_LARROW_EMP), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR)), //###currently 24bpp! is that best? probably yes, on ARM
    m_BmpArrowEmpRHSWidth(18),
    m_BmpArrowEmpRHSHeight(24),

    m_hdcBmpArrowFulLHS   (0), m_hOldObjBmpArrowFulLHS   (0), m_hBmpArrowFulLHS   ((HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_RARROW_FUL), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR)), //###currently 24bpp! is that best? probably yes, on ARM
    m_BmpArrowFulLHSWidth(18),
    m_BmpArrowFulLHSHeight(24),
    m_hdcBmpArrowFulRHS   (0), m_hOldObjBmpArrowFulRHS   (0), m_hBmpArrowFulRHS   ((HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_LARROW_FUL), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR)), //###currently 24bpp! is that best? probably yes, on ARM
    m_BmpArrowFulRHSWidth(18),
    m_BmpArrowFulRHSHeight(24),

    m_hdcBmpCentreFul   (0), m_hOldObjBmpCentreFul   (0), m_hBmpCentreFul   ((HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_CENTRE_FUL), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR)), //###currently 24bpp! is that best? probably yes, on ARM
    m_BmpCentreFulWidth(20),
    m_BmpCentreFulHeight(24),
    m_hdcBmpCentreEmp   (0), m_hOldObjBmpCentreEmp   (0), m_hBmpCentreEmp   ((HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_CENTRE_EMP), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR)), //###currently 24bpp! is that best? probably yes, on ARM
    m_BmpCentreEmpWidth(20),
    m_BmpCentreEmpHeight(24),
    
    m_BmpTitleNumberWidth (18),
    m_BmpTitleNumberHeight(24),

    m_BmpLettersWidth (36),
    m_BmpLettersHeight(48),

    m_iNoteName(-1),
    m_iPitchIndicator8IsCentre(-1),

	m_volume(0),

    m_hFontOld(0),
    m_fntFixed(0),
    m_hgtFont (0),

    m_DroppedBufs(0),
    m_nUnstableCount(0),
    m_fPrevPitch (0.0f),
    m_fAvPitch   (0.0f),

    m_ReferenceAFrequencyHundreds(4), //!!!Currently changing this has no effect (other than to change label on UI)
    m_ReferenceAFrequencyTens    (4), //!!!Currently changing this has no effect (other than to change label on UI)
    m_ReferenceAFrequencyOnes    (0) //!!!Currently changing this has no effect (other than to change label on UI)
{
    memset(&m_ahdcBmpNumbers    , 0, sizeof(m_ahdcBmpNumbers    ));
    memset(&m_ahOldObjBmpNumbers, 0, sizeof(m_ahOldObjBmpNumbers));

    m_ahBmpNumbers[0] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_0  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpNumbers[1] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_1  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpNumbers[2] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_2  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpNumbers[3] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_3  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpNumbers[4] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_4  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpNumbers[5] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_5  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpNumbers[6] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_6  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpNumbers[7] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_7  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpNumbers[8] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_8  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpNumbers[9] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_TITLE_9  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    
    
    memset(&m_ahdcBmpLetters    , 0, sizeof(m_ahdcBmpLetters    ));
    memset(&m_ahOldObjBmpLetters, 0, sizeof(m_ahOldObjBmpLetters));

    m_ahBmpLetters[ 0] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_C  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[ 1] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_C_SHARP ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[ 2] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_D  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[ 3] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_D_SHARP  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[ 4] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_E  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[ 5] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_F  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[ 6] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_F_SHARP  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[ 7] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_G  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[ 8] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_G_SHARP  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[ 9] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_A  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[10] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_A_SHARP  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[11] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_B  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);
    m_ahBmpLetters[12] = (HBITMAP)LoadImage(m_hInstance, MAKEINTRESOURCE(IDB_BITMAP_X  ), IMAGE_BITMAP,0,0, LR_DEFAULTCOLOR);

    memset(&m_rcUpdateRgn, 0, sizeof(m_rcUpdateRgn));
    memset( m_strVol     , 0, sizeof(m_strVol     ));
    memcpy(m_strTune, _T("[No Input                |No Input                ]"), sizeof(m_strTune));

    //Message Map
#ifdef UNDER_CE
    ADD_MSG_HANDLER(WM_SIZE , OnSize );
#endif
    ADD_MSG_HANDLER(WM_CLOSE, OnClose);
    ADD_MSG_HANDLER(WM_PAINT, OnPaint);

    ADD_MSG_HANDLER(UWM_oTuner_NewPitch_wpfPitch_lpfVolume, OnUpdateUI);
    ADD_MSG_HANDLER(UWM_oTuner_DroppedBuffers_wpNBufsDropped_lpNULL, OnDroppedBufs);
}
//--------------------------------------------------------------------------------------------------


COpenTunerDlg::~COpenTunerDlg()
{
    DestroyIcon  ((HICON)m_hIcon); m_hIcon        = NULL;

    SelectObject(m_hdcBmpTitleLHS, m_hOldObjBmpTitleLHS); DeleteDC(m_hdcBmpTitleLHS); m_hdcBmpTitleLHS = 0; DeleteObject (m_hBmpTitleLHS); m_hBmpTitleLHS = NULL;
    SelectObject(m_hdcBmpTitleRHS, m_hOldObjBmpTitleRHS); DeleteDC(m_hdcBmpTitleRHS); m_hdcBmpTitleRHS = 0; DeleteObject (m_hBmpTitleRHS); m_hBmpTitleRHS = NULL;

    SelectObject(m_hdcBmpVolume, m_hOldObjBmpVolume); DeleteDC(m_hdcBmpVolume); m_hdcBmpVolume = 0; DeleteObject (m_hBmpVolume); m_hBmpVolume = NULL;

    SelectObject(m_hdcBmpArrowEmpLHS, m_hOldObjBmpArrowEmpLHS); DeleteDC(m_hdcBmpArrowEmpLHS); m_hdcBmpArrowEmpLHS = 0; DeleteObject (m_hBmpArrowEmpLHS); m_hBmpArrowEmpLHS = NULL;
    SelectObject(m_hdcBmpArrowEmpRHS, m_hOldObjBmpArrowEmpRHS); DeleteDC(m_hdcBmpArrowEmpRHS); m_hdcBmpArrowEmpRHS = 0; DeleteObject (m_hBmpArrowEmpRHS); m_hBmpArrowEmpRHS = NULL;
    SelectObject(m_hdcBmpArrowFulLHS, m_hOldObjBmpArrowFulLHS); DeleteDC(m_hdcBmpArrowFulLHS); m_hdcBmpArrowFulLHS = 0; DeleteObject (m_hBmpArrowFulLHS); m_hBmpArrowFulLHS = NULL;
    SelectObject(m_hdcBmpArrowFulRHS, m_hOldObjBmpArrowFulRHS); DeleteDC(m_hdcBmpArrowFulRHS); m_hdcBmpArrowFulRHS = 0; DeleteObject (m_hBmpArrowFulRHS); m_hBmpArrowFulRHS = NULL;
    SelectObject(m_hdcBmpCentreEmp, m_hOldObjBmpCentreEmp); DeleteDC(m_hdcBmpCentreEmp); m_hdcBmpCentreEmp = 0; DeleteObject (m_hBmpCentreEmp); m_hBmpCentreEmp = NULL;
    SelectObject(m_hdcBmpCentreFul, m_hOldObjBmpCentreFul); DeleteDC(m_hdcBmpCentreFul); m_hdcBmpCentreFul = 0; DeleteObject (m_hBmpCentreFul); m_hBmpCentreFul = NULL;


    SelectObject(m_ahdcBmpNumbers[0], m_ahOldObjBmpNumbers[0]); DeleteDC(m_ahdcBmpNumbers[0]); m_ahdcBmpNumbers[0] = 0; DeleteObject (m_ahBmpNumbers[0]); m_ahBmpNumbers[0] = NULL;
    SelectObject(m_ahdcBmpNumbers[1], m_ahOldObjBmpNumbers[1]); DeleteDC(m_ahdcBmpNumbers[1]); m_ahdcBmpNumbers[1] = 0; DeleteObject (m_ahBmpNumbers[1]); m_ahBmpNumbers[1] = NULL;
    SelectObject(m_ahdcBmpNumbers[2], m_ahOldObjBmpNumbers[2]); DeleteDC(m_ahdcBmpNumbers[2]); m_ahdcBmpNumbers[2] = 0; DeleteObject (m_ahBmpNumbers[2]); m_ahBmpNumbers[2] = NULL;
    SelectObject(m_ahdcBmpNumbers[3], m_ahOldObjBmpNumbers[3]); DeleteDC(m_ahdcBmpNumbers[3]); m_ahdcBmpNumbers[3] = 0; DeleteObject (m_ahBmpNumbers[3]); m_ahBmpNumbers[3] = NULL;
    SelectObject(m_ahdcBmpNumbers[4], m_ahOldObjBmpNumbers[4]); DeleteDC(m_ahdcBmpNumbers[4]); m_ahdcBmpNumbers[4] = 0; DeleteObject (m_ahBmpNumbers[4]); m_ahBmpNumbers[4] = NULL;
    SelectObject(m_ahdcBmpNumbers[5], m_ahOldObjBmpNumbers[5]); DeleteDC(m_ahdcBmpNumbers[5]); m_ahdcBmpNumbers[5] = 0; DeleteObject (m_ahBmpNumbers[5]); m_ahBmpNumbers[5] = NULL;
    SelectObject(m_ahdcBmpNumbers[6], m_ahOldObjBmpNumbers[6]); DeleteDC(m_ahdcBmpNumbers[6]); m_ahdcBmpNumbers[6] = 0; DeleteObject (m_ahBmpNumbers[6]); m_ahBmpNumbers[6] = NULL;
    SelectObject(m_ahdcBmpNumbers[7], m_ahOldObjBmpNumbers[7]); DeleteDC(m_ahdcBmpNumbers[7]); m_ahdcBmpNumbers[7] = 0; DeleteObject (m_ahBmpNumbers[7]); m_ahBmpNumbers[7] = NULL;
    SelectObject(m_ahdcBmpNumbers[8], m_ahOldObjBmpNumbers[8]); DeleteDC(m_ahdcBmpNumbers[8]); m_ahdcBmpNumbers[8] = 0; DeleteObject (m_ahBmpNumbers[8]); m_ahBmpNumbers[8] = NULL;
    SelectObject(m_ahdcBmpNumbers[9], m_ahOldObjBmpNumbers[9]); DeleteDC(m_ahdcBmpNumbers[9]); m_ahdcBmpNumbers[9] = 0; DeleteObject (m_ahBmpNumbers[9]); m_ahBmpNumbers[9] = NULL;

    SelectObject(m_ahdcBmpLetters[ 0], m_ahOldObjBmpLetters[ 0]); DeleteDC(m_ahdcBmpLetters[ 0]); m_ahdcBmpLetters[ 0] = 0; DeleteObject (m_ahBmpLetters[ 0]); m_ahBmpLetters[ 0] = NULL;
    SelectObject(m_ahdcBmpLetters[ 1], m_ahOldObjBmpLetters[ 1]); DeleteDC(m_ahdcBmpLetters[ 1]); m_ahdcBmpLetters[ 1] = 0; DeleteObject (m_ahBmpLetters[ 1]); m_ahBmpLetters[ 1] = NULL;
    SelectObject(m_ahdcBmpLetters[ 2], m_ahOldObjBmpLetters[ 2]); DeleteDC(m_ahdcBmpLetters[ 2]); m_ahdcBmpLetters[ 2] = 0; DeleteObject (m_ahBmpLetters[ 2]); m_ahBmpLetters[ 2] = NULL;
    SelectObject(m_ahdcBmpLetters[ 3], m_ahOldObjBmpLetters[ 3]); DeleteDC(m_ahdcBmpLetters[ 3]); m_ahdcBmpLetters[ 3] = 0; DeleteObject (m_ahBmpLetters[ 3]); m_ahBmpLetters[ 3] = NULL;
    SelectObject(m_ahdcBmpLetters[ 4], m_ahOldObjBmpLetters[ 4]); DeleteDC(m_ahdcBmpLetters[ 4]); m_ahdcBmpLetters[ 4] = 0; DeleteObject (m_ahBmpLetters[ 4]); m_ahBmpLetters[ 4] = NULL;
    SelectObject(m_ahdcBmpLetters[ 5], m_ahOldObjBmpLetters[ 5]); DeleteDC(m_ahdcBmpLetters[ 5]); m_ahdcBmpLetters[ 5] = 0; DeleteObject (m_ahBmpLetters[ 5]); m_ahBmpLetters[ 5] = NULL;
    SelectObject(m_ahdcBmpLetters[ 6], m_ahOldObjBmpLetters[ 6]); DeleteDC(m_ahdcBmpLetters[ 6]); m_ahdcBmpLetters[ 6] = 0; DeleteObject (m_ahBmpLetters[ 6]); m_ahBmpLetters[ 6] = NULL;
    SelectObject(m_ahdcBmpLetters[ 7], m_ahOldObjBmpLetters[ 7]); DeleteDC(m_ahdcBmpLetters[ 7]); m_ahdcBmpLetters[ 7] = 0; DeleteObject (m_ahBmpLetters[ 7]); m_ahBmpLetters[ 7] = NULL;
    SelectObject(m_ahdcBmpLetters[ 8], m_ahOldObjBmpLetters[ 8]); DeleteDC(m_ahdcBmpLetters[ 8]); m_ahdcBmpLetters[ 8] = 0; DeleteObject (m_ahBmpLetters[ 8]); m_ahBmpLetters[ 8] = NULL;
    SelectObject(m_ahdcBmpLetters[ 9], m_ahOldObjBmpLetters[ 9]); DeleteDC(m_ahdcBmpLetters[ 9]); m_ahdcBmpLetters[ 9] = 0; DeleteObject (m_ahBmpLetters[ 9]); m_ahBmpLetters[ 9] = NULL;
    SelectObject(m_ahdcBmpLetters[10], m_ahOldObjBmpLetters[10]); DeleteDC(m_ahdcBmpLetters[10]); m_ahdcBmpLetters[10] = 0; DeleteObject (m_ahBmpLetters[10]); m_ahBmpLetters[10] = NULL;
    SelectObject(m_ahdcBmpLetters[11], m_ahOldObjBmpLetters[11]); DeleteDC(m_ahdcBmpLetters[11]); m_ahdcBmpLetters[11] = 0; DeleteObject (m_ahBmpLetters[11]); m_ahBmpLetters[11] = NULL;
    SelectObject(m_ahdcBmpLetters[12], m_ahOldObjBmpLetters[12]); DeleteDC(m_ahdcBmpLetters[12]); m_ahdcBmpLetters[12] = 0; DeleteObject (m_ahBmpLetters[12]); m_ahBmpLetters[12] = NULL;

    DeleteObject (m_fntFixed    ); m_fntFixed     = NULL;
}
//--------------------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                  Message Handlers
//--------------------------------------------------------------------------------------------------


long COpenTunerDlg::OnInitDialog()
{
#ifndef UNDER_CE //Ce seems to require ONLY a maximize box, but we'll want a min box for regular windows
	SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_OVERLAPPED |WS_MINIMIZEBOX | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_BORDER);
    SetWindowPos(m_hWnd, 0, 0, 0, 246, m_BmpLettersHeight + 2*GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYCAPTION), SWP_NOMOVE|SWP_NOZORDER);
#endif
    CBscBblDlg::OnInitDialog();

    SortOutMenuBarEtcIfRunningWinCE();

    //Set icon for this dialog.
    SendMessage(m_hWnd, WM_SETICON, ICON_BIG  , (LPARAM)m_hIcon);
    SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)m_hIcon);

    //Set title to include version number..
    std::basic_string<TCHAR> buf(WMET_APP_TITLE _T("              "));
    cBscFileVersion::Get((TCHAR*)(buf.c_str()+_tcslen(WMET_APP_TITLE)));
    m_strAppTitle = buf.c_str();
    SetWindowText(m_hWnd, m_strAppTitle.c_str());

    //Use a fixed-width font so that the pitch-pointer appears at the appropriate horizontal point
    HFONT hFont = (HFONT)SendMessage(*this, WM_GETFONT, 0, 0);
    LOGFONT theFontInfo;
    GetObject(hFont, sizeof(theFontInfo), &theFontInfo);
    _tcscpy(theFontInfo.lfFaceName, _T("Courier New"));
    theFontInfo.lfHeight /= 1.2;
    m_fntFixed = CreateFontIndirect(&theFontInfo);
    m_hgtFont  = (unsigned short)abs(theFontInfo.lfHeight)+2; //Not sure why +2 is required, but it is!

    //We're only bothered about repainting the first four lines of text in the dialog
    GetClientRect(m_hWnd, &m_rcUpdateRgn);
    m_rcUpdateRgn.bottom = m_BmpLettersHeight;

    InvalidateRect(m_hWnd, NULL, TRUE);

    m_autopCore = (std::auto_ptr<CoTunerCore>) new CoTunerCore (m_hWnd);

    return 1;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------


#ifdef UNDER_CE //Catch the top-left "x" press, which in CE doesn't shut the window
long COpenTunerDlg::OnSize(unsigned long const nWhatIsGoingOn, long const nIgnore2) 
{
    if (nWhatIsGoingOn == SIZE_MINIMIZED)
    {
        PostMessage(m_hWnd, WM_CLOSE, 0, 0);
        PostMessage(m_hWnd, WM_QUIT , 0, 0);
        return 1;
    }
    return 0;
}
//--------------------------------------------------------------------------------------------------
#endif

//#include "_kiss_fft_guts.h"
//#include "kiss_fft.h"
//#include "kiss_fftr.h"
//extern kiss_fft_scalar s_MaxSamples;
//extern kiss_fft_scalar s_MaxTim2FFT;
//extern kiss_fft_scalar s_MaxFFT2Tim;


long COpenTunerDlg::OnCancel(                                   ) {EndDialog(m_hWnd, IDCANCEL); return 1;}
long COpenTunerDlg::OnOK    (                                   ) {EndDialog(m_hWnd, IDOK    ); return 1;}
long COpenTunerDlg::OnClose (unsigned long wIgnore, long lIgnore) 
{
    EndDialog(m_hWnd, IDOK    ); 
//    printf("\r\n\r\nMaxes:\r\n%e,%e,%e\n", s_MaxSamples, s_MaxTim2FFT, s_MaxFFT2Tim);
    return 1;
}
//--------------------------------------------------------------------------------------------------


long COpenTunerDlg::OnPaint(unsigned long pfPitch, long pfVolume)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(m_hWnd, &ps); //!!!How necessary is this? Help says it erases the old window

    if (m_hdcBmpTitleLHS == NULL)
    {
        m_hdcBmpTitleLHS    = CreateCompatibleDC(hdc); m_hOldObjBmpTitleLHS    = (HBITMAP) SelectObject(m_hdcBmpTitleLHS   , m_hBmpTitleLHS   );
        m_hdcBmpTitleRHS    = CreateCompatibleDC(hdc); m_hOldObjBmpTitleRHS    = (HBITMAP) SelectObject(m_hdcBmpTitleRHS   , m_hBmpTitleRHS   );

        m_hdcBmpVolume    = CreateCompatibleDC(hdc); m_hOldObjBmpVolume    = (HBITMAP) SelectObject(m_hdcBmpVolume   , m_hBmpVolume   );

        m_hdcBmpArrowEmpLHS    = CreateCompatibleDC(hdc); m_hOldObjBmpArrowEmpLHS    = (HBITMAP) SelectObject(m_hdcBmpArrowEmpLHS   , m_hBmpArrowEmpLHS   );
        m_hdcBmpArrowEmpRHS    = CreateCompatibleDC(hdc); m_hOldObjBmpArrowEmpRHS    = (HBITMAP) SelectObject(m_hdcBmpArrowEmpRHS   , m_hBmpArrowEmpRHS   );
        m_hdcBmpArrowFulLHS    = CreateCompatibleDC(hdc); m_hOldObjBmpArrowFulLHS    = (HBITMAP) SelectObject(m_hdcBmpArrowFulLHS   , m_hBmpArrowFulLHS   );
        m_hdcBmpArrowFulRHS    = CreateCompatibleDC(hdc); m_hOldObjBmpArrowFulRHS    = (HBITMAP) SelectObject(m_hdcBmpArrowFulRHS   , m_hBmpArrowFulRHS   );
        m_hdcBmpCentreEmp    = CreateCompatibleDC(hdc); m_hOldObjBmpCentreEmp    = (HBITMAP) SelectObject(m_hdcBmpCentreEmp   , m_hBmpCentreEmp   );
        m_hdcBmpCentreFul    = CreateCompatibleDC(hdc); m_hOldObjBmpCentreFul    = (HBITMAP) SelectObject(m_hdcBmpCentreFul   , m_hBmpCentreFul   );
        m_ahdcBmpNumbers[0] = CreateCompatibleDC(hdc); m_ahOldObjBmpNumbers[0] = (HBITMAP) SelectObject(m_ahdcBmpNumbers[0], m_ahBmpNumbers[0]);
        m_ahdcBmpNumbers[1] = CreateCompatibleDC(hdc); m_ahOldObjBmpNumbers[1] = (HBITMAP) SelectObject(m_ahdcBmpNumbers[1], m_ahBmpNumbers[1]);
        m_ahdcBmpNumbers[2] = CreateCompatibleDC(hdc); m_ahOldObjBmpNumbers[2] = (HBITMAP) SelectObject(m_ahdcBmpNumbers[2], m_ahBmpNumbers[2]);
        m_ahdcBmpNumbers[3] = CreateCompatibleDC(hdc); m_ahOldObjBmpNumbers[3] = (HBITMAP) SelectObject(m_ahdcBmpNumbers[3], m_ahBmpNumbers[3]);
        m_ahdcBmpNumbers[4] = CreateCompatibleDC(hdc); m_ahOldObjBmpNumbers[4] = (HBITMAP) SelectObject(m_ahdcBmpNumbers[4], m_ahBmpNumbers[4]);
        m_ahdcBmpNumbers[5] = CreateCompatibleDC(hdc); m_ahOldObjBmpNumbers[5] = (HBITMAP) SelectObject(m_ahdcBmpNumbers[5], m_ahBmpNumbers[5]);
        m_ahdcBmpNumbers[6] = CreateCompatibleDC(hdc); m_ahOldObjBmpNumbers[6] = (HBITMAP) SelectObject(m_ahdcBmpNumbers[6], m_ahBmpNumbers[6]);
        m_ahdcBmpNumbers[7] = CreateCompatibleDC(hdc); m_ahOldObjBmpNumbers[7] = (HBITMAP) SelectObject(m_ahdcBmpNumbers[7], m_ahBmpNumbers[7]);
        m_ahdcBmpNumbers[8] = CreateCompatibleDC(hdc); m_ahOldObjBmpNumbers[8] = (HBITMAP) SelectObject(m_ahdcBmpNumbers[8], m_ahBmpNumbers[8]);
        m_ahdcBmpNumbers[9] = CreateCompatibleDC(hdc); m_ahOldObjBmpNumbers[9] = (HBITMAP) SelectObject(m_ahdcBmpNumbers[9], m_ahBmpNumbers[9]);
        
        m_ahdcBmpLetters[ 0] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[ 0] = (HBITMAP) SelectObject(m_ahdcBmpLetters[ 0], m_ahBmpLetters[ 0]);
        m_ahdcBmpLetters[ 1] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[ 1] = (HBITMAP) SelectObject(m_ahdcBmpLetters[ 1], m_ahBmpLetters[ 1]);
        m_ahdcBmpLetters[ 2] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[ 2] = (HBITMAP) SelectObject(m_ahdcBmpLetters[ 2], m_ahBmpLetters[ 2]);
        m_ahdcBmpLetters[ 3] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[ 3] = (HBITMAP) SelectObject(m_ahdcBmpLetters[ 3], m_ahBmpLetters[ 3]);
        m_ahdcBmpLetters[ 4] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[ 4] = (HBITMAP) SelectObject(m_ahdcBmpLetters[ 4], m_ahBmpLetters[ 4]);
        m_ahdcBmpLetters[ 5] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[ 5] = (HBITMAP) SelectObject(m_ahdcBmpLetters[ 5], m_ahBmpLetters[ 5]);
        m_ahdcBmpLetters[ 6] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[ 6] = (HBITMAP) SelectObject(m_ahdcBmpLetters[ 6], m_ahBmpLetters[ 6]);
        m_ahdcBmpLetters[ 7] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[ 7] = (HBITMAP) SelectObject(m_ahdcBmpLetters[ 7], m_ahBmpLetters[ 7]);
        m_ahdcBmpLetters[ 8] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[ 8] = (HBITMAP) SelectObject(m_ahdcBmpLetters[ 8], m_ahBmpLetters[ 8]);
        m_ahdcBmpLetters[ 9] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[ 9] = (HBITMAP) SelectObject(m_ahdcBmpLetters[ 9], m_ahBmpLetters[ 9]);
        m_ahdcBmpLetters[10] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[10] = (HBITMAP) SelectObject(m_ahdcBmpLetters[10], m_ahBmpLetters[10]);
        m_ahdcBmpLetters[11] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[11] = (HBITMAP) SelectObject(m_ahdcBmpLetters[11], m_ahBmpLetters[11]);
        m_ahdcBmpLetters[12] = CreateCompatibleDC(hdc); m_ahOldObjBmpLetters[12] = (HBITMAP) SelectObject(m_ahdcBmpLetters[12], m_ahBmpLetters[12]);
    }

    //Do these conditionally based on psRect####
    BitBlt(hdc, 0										  , 0, m_BmpTitleLHSWidth   , m_BmpTitleLHSHeight   , m_hdcBmpTitleLHS, 0, 0, SRCCOPY);

    BitBlt(hdc, m_BmpTitleLHSWidth                        , 0, m_BmpTitleNumberWidth, m_BmpTitleNumberHeight, m_ahdcBmpNumbers[m_ReferenceAFrequencyHundreds], 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpTitleLHSWidth+m_BmpTitleNumberWidth  , 0, m_BmpTitleNumberWidth, m_BmpTitleNumberHeight, m_ahdcBmpNumbers[m_ReferenceAFrequencyTens    ], 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpTitleLHSWidth+2*m_BmpTitleNumberWidth, 0, m_BmpTitleNumberWidth, m_BmpTitleNumberHeight, m_ahdcBmpNumbers[m_ReferenceAFrequencyOnes    ], 0, 0, SRCCOPY);

    BitBlt(hdc, m_BmpTitleLHSWidth+3*m_BmpTitleNumberWidth, 0, m_BmpTitleRHSWidth   , m_BmpTitleRHSHeight   , m_hdcBmpTitleRHS, 0, 0, SRCCOPY);


    //Better to build a big array of all possibilities and do as a single BLT?
    BitBlt(hdc, 0                        , m_BmpTitleLHSHeight, m_BmpArrowEmpLHSWidth   , m_BmpArrowEmpLHSHeight   ,  (m_iPitchIndicator8IsCentre==0)?m_hdcBmpArrowFulLHS:m_hdcBmpArrowEmpLHS, 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpLHSWidth    , m_BmpTitleLHSHeight, m_BmpArrowEmpLHSWidth   , m_BmpArrowEmpLHSHeight   , ((m_iPitchIndicator8IsCentre==0)||(m_iPitchIndicator8IsCentre==1)||(m_iPitchIndicator8IsCentre==2))?m_hdcBmpArrowFulLHS:m_hdcBmpArrowEmpLHS, 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpLHSWidth * 2, m_BmpTitleLHSHeight, m_BmpArrowEmpLHSWidth   , m_BmpArrowEmpLHSHeight   , ((m_iPitchIndicator8IsCentre==2)||(m_iPitchIndicator8IsCentre==3)||(m_iPitchIndicator8IsCentre==4))?m_hdcBmpArrowFulLHS:m_hdcBmpArrowEmpLHS, 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpLHSWidth * 3, m_BmpTitleLHSHeight, m_BmpArrowEmpLHSWidth   , m_BmpArrowEmpLHSHeight   , ((m_iPitchIndicator8IsCentre==4)||(m_iPitchIndicator8IsCentre==5)||(m_iPitchIndicator8IsCentre==6))?m_hdcBmpArrowFulLHS:m_hdcBmpArrowEmpLHS, 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpLHSWidth * 4, m_BmpTitleLHSHeight, m_BmpArrowEmpLHSWidth   , m_BmpArrowEmpLHSHeight   , ((m_iPitchIndicator8IsCentre==6)||(m_iPitchIndicator8IsCentre==7)||(m_iPitchIndicator8IsCentre==8))?m_hdcBmpArrowFulLHS:m_hdcBmpArrowEmpLHS, 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpLHSWidth * 5, m_BmpTitleLHSHeight, m_BmpCentreEmpWidth     , m_BmpArrowEmpLHSHeight   ,  (m_iPitchIndicator8IsCentre==8)?m_hdcBmpCentreFul:m_hdcBmpCentreEmp  , 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpRHSWidth * 5 + m_BmpCentreEmpWidth, m_BmpTitleRHSHeight, m_BmpArrowEmpRHSWidth   , m_BmpArrowEmpRHSHeight   , ((m_iPitchIndicator8IsCentre==8 )||(m_iPitchIndicator8IsCentre== 9)||(m_iPitchIndicator8IsCentre==10))?m_hdcBmpArrowFulRHS:m_hdcBmpArrowEmpRHS, 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpRHSWidth * 6 + m_BmpCentreEmpWidth, m_BmpTitleRHSHeight, m_BmpArrowEmpRHSWidth   , m_BmpArrowEmpRHSHeight   , ((m_iPitchIndicator8IsCentre==10)||(m_iPitchIndicator8IsCentre==11)||(m_iPitchIndicator8IsCentre==12))?m_hdcBmpArrowFulRHS:m_hdcBmpArrowEmpRHS, 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpRHSWidth * 7 + m_BmpCentreEmpWidth, m_BmpTitleRHSHeight, m_BmpArrowEmpRHSWidth   , m_BmpArrowEmpRHSHeight   , ((m_iPitchIndicator8IsCentre==12)||(m_iPitchIndicator8IsCentre==13)||(m_iPitchIndicator8IsCentre==14))?m_hdcBmpArrowFulRHS:m_hdcBmpArrowEmpRHS, 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpRHSWidth * 8 + m_BmpCentreEmpWidth, m_BmpTitleRHSHeight, m_BmpArrowEmpRHSWidth   , m_BmpArrowEmpRHSHeight   , ((m_iPitchIndicator8IsCentre==14)||(m_iPitchIndicator8IsCentre==15)||(m_iPitchIndicator8IsCentre==16))?m_hdcBmpArrowFulRHS:m_hdcBmpArrowEmpRHS, 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpRHSWidth * 9 + m_BmpCentreEmpWidth, m_BmpTitleRHSHeight, m_BmpArrowEmpRHSWidth   , m_BmpArrowEmpRHSHeight   ,  (m_iPitchIndicator8IsCentre==16)?m_hdcBmpArrowFulRHS:m_hdcBmpArrowEmpRHS, 0, 0, SRCCOPY);

    if (m_iNoteName >= 0)
        BitBlt(hdc, m_BmpArrowEmpRHSWidth *11, 0, m_BmpLettersWidth   , m_BmpLettersHeight   , m_ahdcBmpLetters[m_iNoteName], 0, 0, SRCCOPY);
    else
        BitBlt(hdc, m_BmpArrowEmpRHSWidth *11, 0, m_BmpLettersWidth   , m_BmpLettersHeight   , m_ahdcBmpLetters[12         ], 0, 0, SRCCOPY);

    float fVol = m_volume;
    if (_isnan(fVol))
        fVol = 0;
    BitBlt(hdc, m_BmpArrowEmpRHSWidth *11 + m_BmpLettersWidth, 0                                           , m_BmpVolumeWidth   , m_BmpVolumeHeight-fVol*m_BmpVolumeHeight, m_ahdcBmpLetters[12         ], 0, 0, SRCCOPY);
    BitBlt(hdc, m_BmpArrowEmpRHSWidth *11 + m_BmpLettersWidth, m_BmpVolumeHeight-fVol*m_BmpVolumeHeight, m_BmpVolumeWidth   , fVol*m_BmpVolumeHeight   , m_hdcBmpVolume, 0, m_BmpVolumeHeight-fVol*m_BmpVolumeHeight, SRCCOPY);


    /*###
    m_hFontOld = (HFONT)SelectObject(hdc, m_fntFixed); //!!!Do I need to do this each time?

    if (m_DroppedBufs > 0) //Overwrite tail end of Volume Bar with number of dropped buffers
        _stprintf(&m_strVol[(sizeof(m_strTune )/(2*sizeof(TCHAR))) - 4], _T("%5d"), m_DroppedBufs);

    ExtTextOut(hdc, 0, 0           , 0, NULL, &m_strTune[1]                                       , (sizeof(m_strTune)/sizeof(TCHAR))-1, NULL);
    //ExtTextOut(hdc, 0, 0           , 0, NULL, m_strTune                                       , 1+((sizeof(m_strTune)/sizeof(TCHAR))/2), NULL);
//    ExtTextOut(hdc, 0, m_hgtFont   , 0, NULL,&m_strTune[((sizeof(m_strTune)/sizeof(TCHAR))/2)], 1+((sizeof(m_strTune)/sizeof(TCHAR))/2), NULL);
    ExtTextOut(hdc, 0, m_hgtFont, 0, NULL, m_strVol                                        ,    (sizeof(m_strVol )/sizeof(TCHAR))-1   , NULL);

    SelectObject(hdc, m_hFontOld);
    */

	EndPaint(m_hWnd, &ps);

    return 0; //MSDN: "An application returns zero if it processes this message"
}
//--------------------------------------------------------------------------------------------------


long COpenTunerDlg::OnDroppedBufs(unsigned long nDroppedBufs, long lIgnore)
{
    if (nDroppedBufs)
    {
        if (m_strAppTitle[0] != '!')
        {
            m_strAppTitle[0] = '!';
            SetWindowText(m_hWnd, m_strAppTitle.c_str());
        }
    }
    else
    {
        if (m_strAppTitle[0] != 'o')
        {
            m_strAppTitle[0] = 'o';
            SetWindowText(m_hWnd, m_strAppTitle.c_str());
        }
    }
    return 1;
}


//--------------------------------------------------------------------------------------------------
//std::deque<double> pitchHist; //WinCE is quite crap at push_back
#define s_pitchHistSize 4
float s_PitchHist[s_pitchHistSize] = {0.0f, 0.0f, 0.0f, 0.0f};
unsigned long s_nPitchHist = 0;
unsigned long s_iPitchHist = 0;
//!!!Original function
/*
void print_tuning(double pitch, double volume, bool valid) {
	static double prevPitch = 0.0;
	static double avgPitch = 0.0;
	
	if(valid) {
		if(!pitchHist.empty() && fabs(pitchHist.back() - pitch) > 0.2) pitchHist.clear();
		pitchHist.push_back(pitch);
		if(pitchHist.size() > 4) pitchHist.pop_front();
		avgPitch = std::accumulate(pitchHist.begin(), pitchHist.end(), 0.0) / pitchHist.size();
		pitch = avgPitch;
	} else {
		pitchHist.clear();
	}
	
  str_temp[0] = '\0';
  char *name = noteName(toInt(pitch));
  char name1[2] = { name[0], '\0' };
  char name2[2] = { name[1], '\0' };
	  
	if(valid) {
		double pitch_err = pitch +0.5 - floor(pitch+0.5);
		double pitch_err2 = prevPitch +0.5 - floor(prevPitch+0.5);
		int pos = int(floor(pitch_err * 49.0));
		int pos2 = int(floor(pitch_err2 * 49.0));
		int minPos = (pos < pos2) ? pos : pos2;
		int maxPos = (pos < pos2) ? pos2 : pos;
		if(fabs(pitch_err - pitch_err2) > 0.5) { minPos = maxPos = pos; }
		//if(fabs(pitch_err - pitch_err2) < 0.1) drawType = "*"; else drawType = "'";
		strcat(str_temp, " [");
		for(int j=0; j<49; j++) {
			if(j == pos) { strcat(str_temp, "X"); }
			else if(j >= minPos && j <= maxPos) { strcat(str_temp, "-"); }
			else if(j == 24) { strcat(str_temp, name1); }
			else if(j == 25) { strcat(str_temp, name2); }
			else { strcat(str_temp, " "); }
		}
		strcat(str_temp, "]");
  } else {
    //sprintf(str_temp, " [------------------------|------------------------]");
    sprintf(str_temp, " [                        |                        ]");
  }
  int dist = toInt(volume * 30.0);
	for(int j=0; j<30; j++) { strcat(str_temp, (j<dist)?"=":" "); }
	printf("%s%c", str_temp, line_char); fflush(stdout);
	prevPitch = pitch;
}
*/


TCHAR const *noteNamesT[12] = { _T("C "), _T("C#"), _T("D "), _T("D#"), _T("E "), _T("F "), _T("F#"), _T("G "), _T("G#"), _T("A "), _T("A#"), _T("B ") };
long COpenTunerDlg::OnUpdateUI(unsigned long pfPitch, long pfVolume)
{
#ifdef UNDER_CE                 //(#include <Winuser.h> for SystemIdleTimerReset())
    SystemIdleTimerReset(); //Don't let it turn the screen off etc to save power.
#endif

    float       pitch  = *((float*)&pfPitch );
    m_volume = *((float*)&pfVolume);
    bool  const valid  = !_isnan(pitch);
    
    if(valid) 
    {
        //if (!pitchHist.empty() && fabs(pitchHist.back() - pitch) > 0.2)
        //    pitchHist.clear();

        if (s_nPitchHist != 0) //If we have some history available
        {
            if (fabs(s_PitchHist[s_iPitchHist==0?(s_pitchHistSize-1):s_iPitchHist-1] - pitch) > 0.2) //And current pitch is quite different from last pitch..
                s_nPitchHist = 0; //Clear history
        }

        //pitchHist.push_back(pitch); //!!!aha! push_back is slow! So (& see below): use circular buffer (& what does accumulate do?)

        s_PitchHist[s_iPitchHist++] = pitch; ++s_nPitchHist;
        if (s_iPitchHist >= s_pitchHistSize)
            s_iPitchHist = 0;

        //if(pitchHist.size() > 4) 
        //    pitchHist.pop_front();

        m_fAvPitch = 0;
        long iPitchHist = s_iPitchHist;
        for (unsigned long i = 0; i < s_nPitchHist; ++i)
        {
            --iPitchHist; if (iPitchHist < 0) iPitchHist = 3;
            m_fAvPitch += s_PitchHist[iPitchHist];
        }
        m_fAvPitch/= s_nPitchHist;

        //m_fAvPitch = std::accumulate(pitchHist.begin(), pitchHist.end(), 0.0) / pitchHist.size();

        pitch = (float)m_fAvPitch;
    } 
    else 
        s_nPitchHist = 0; //Clear history
//        pitchHist.clear();
    
    if(valid) 
    {
        m_iNoteName = cycle(toInt(pitch), 12);

        //TCHAR const *name     = noteNamesT[cycle(toInt(pitch), 12)];
    
        double pitch_err = pitch +0.5 - floor(pitch+0.5);
        double pitch_err2 = m_fPrevPitch +0.5 - floor(m_fPrevPitch+0.5);
        int pos = int(floor(pitch_err * 49.0));
        int pos2 = int(floor(pitch_err2 * 49.0));
        int minPos = (pos < pos2) ? pos : pos2;
        int maxPos = (pos < pos2) ? pos2 : pos;
        if(fabs(pitch_err - pitch_err2) > 0.5) { minPos = maxPos = pos; }


        // /2 -12 halves resolution (do we care?)
        m_iPitchIndicator8IsCentre = 8 + pos/2 - 12; //###Is this correct?! Is resolution ok, or too fernickity?
        if (m_iPitchIndicator8IsCentre < 0)
            m_iPitchIndicator8IsCentre = 0;
        if (m_iPitchIndicator8IsCentre > 16)
            m_iPitchIndicator8IsCentre = 16;

        /*
        //if(fabs(pitch_err - pitch_err2) < 0.1) drawType = "*"; else drawType = "'";

        for(int j=1; j<50; ++j)
        {
            if     (j-1 == pos                  ) { m_strTune[j] = _T('X'); }
            else if(j-1 >= minPos && j <= maxPos) { m_strTune[j] = _T('-'); } //###what was the purpose of this? ah, is pitch error like an error-bar? It looks more like distance from previous pitch (so a measure of wibble)
            else if(j-1 == 24                   ) { m_strTune[j] = name[0]; }
            else if(j-1 == 25                   ) { m_strTune[j] = name[1]; }
            else { m_strTune[j] = _T(' '); }
        }
        if (pos > 47)
            m_strTune[47] = '\\'; //###fudge if it falls off the screen!
        */
    } 
    else //!!!I can probably just un-draw the last char and draw in the new char?
    {
        //_stprintf(m_strTune, _T("[                        |                        ]"));
        m_iNoteName = -1;
        m_iPitchIndicator8IsCentre = -1;
    }

    //###volume! And dropped buffers!
    /*
    int const dist = toInt(volume * 30.0f);
    for(int j=0; j<(sizeof(m_strVol)/sizeof(TCHAR)); ++j)
        m_strVol[j] = ((j<dist)?_T('='):_T(' '));
    m_strVol[sizeof(m_strVol)/sizeof(m_strVol[0])-1] = 0;
    
    m_fPrevPitch = pitch;

#ifdef _DEBUG
    TCHAR rn[] = _T("\n");
    printf("%s%s%s%s", _T(" "), m_strTune, m_strVol, rn);
#endif
    */

    //!!!New "filter" code
    //To display pitch, I've decided I want three or more consecutive "close" frequencies
    //This seems to work well as a flicker-elimination method
    //When tuning an instrument, you'd expect stability like that (I hope!)!
    //So: if I have a stable frequency, I display it
    //If I get several consecutive unstable frequencies, I erase the last displayed frequency (but not otherwise)
    //This means that the stable readings persist on the display for a little while, which is what you want.
    if (s_nPitchHist <= 2)
    {   
        m_nUnstableCount++;

        if (m_nUnstableCount > 32)
        {
            m_iNoteName = -1;
            m_iPitchIndicator8IsCentre = -1;
            InvalidateRect(m_hWnd, &m_rcUpdateRgn, FALSE);
        }
    }
    else
    {
        m_nUnstableCount = 0;
        InvalidateRect(m_hWnd, &m_rcUpdateRgn, FALSE);
    }

//    char lpszVol[256]; sprintf(lpszVol, _T("%f\r\n"), m_volume);
//    OutputDebugString(&lpszVol[0]);

    return 1;
}
//--------------------------------------------------------------------------------------------------
