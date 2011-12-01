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
#if !defined(AFX_OpenTunerDLG_H__E8300326_0843_11D6_91F5_004033D19F0F__INCLUDED_)
#define AFX_OpenTunerDLG_H__E8300326_0843_11D6_91F5_004033D19F0F__INCLUDED_
// OpenTunerDlg.h : header file
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <memory>
#include "BscBblDlg.h"
#include "OpenTunerCore.h"


//Implements "Back-end" of IDD_OpenTuner_DIALOG (see project's .rc file)
//Usage: See CBscBblDlg
class COpenTunerDlg : public CBscBblDlg
{
public:    
	COpenTunerDlg(HINSTANCE hInstance = 0, 
                  HWND      hParent   = 0); //Construction: See base-class documentation

    virtual ~COpenTunerDlg();


protected:
	virtual long OnCancel();
    virtual long OnOK();
    virtual long OnInitDialog();
    virtual void SortOutMenuBarEtcIfRunningWinCE();


protected: //Message handlers: see also base-class documentation
#ifdef UNDER_CE
    DECLARE_MSG_HANDLER(OnSize         , COpenTunerDlg);
#endif
    DECLARE_MSG_HANDLER(OnClose        , COpenTunerDlg); 
    DECLARE_MSG_HANDLER(OnPaint        , COpenTunerDlg); 
    DECLARE_MSG_HANDLER(OnUpdateUI     , COpenTunerDlg); 
    DECLARE_MSG_HANDLER(OnDroppedBufs  , COpenTunerDlg); 


private:
    HICON m_hIcon;

    HDC m_ahdcBmpNumbers[10]; HBITMAP m_ahBmpNumbers[10]; HBITMAP m_ahOldObjBmpNumbers[10];
    HDC m_ahdcBmpLetters[13]; HBITMAP m_ahBmpLetters[13]; HBITMAP m_ahOldObjBmpLetters[13];
    HDC m_hdcBmpTitleLHS    ; HBITMAP m_hBmpTitleLHS    ; HBITMAP m_hOldObjBmpTitleLHS    ;
    unsigned long const m_BmpTitleLHSWidth;
    unsigned long const m_BmpTitleLHSHeight;
    HDC m_hdcBmpVolume    ; HBITMAP m_hBmpVolume    ; HBITMAP m_hOldObjBmpVolume    ;
    unsigned long const m_BmpVolumeWidth;
    unsigned long const m_BmpVolumeHeight;
    HDC m_hdcBmpTitleRHS    ; HBITMAP m_hBmpTitleRHS    ; HBITMAP m_hOldObjBmpTitleRHS    ;
    unsigned long const m_BmpTitleRHSWidth;
    unsigned long const m_BmpTitleRHSHeight;
    unsigned long const m_BmpTitleNumberWidth ;
    unsigned long const m_BmpTitleNumberHeight;
    unsigned long const m_BmpLettersWidth ;
    unsigned long const m_BmpLettersHeight;

    HDC m_hdcBmpArrowEmpLHS    ; HBITMAP m_hBmpArrowEmpLHS    ; HBITMAP m_hOldObjBmpArrowEmpLHS    ;
    unsigned long const m_BmpArrowEmpLHSWidth;
    unsigned long const m_BmpArrowEmpLHSHeight;
    HDC m_hdcBmpArrowEmpRHS    ; HBITMAP m_hBmpArrowEmpRHS    ; HBITMAP m_hOldObjBmpArrowEmpRHS    ;
    unsigned long const m_BmpArrowEmpRHSWidth;
    unsigned long const m_BmpArrowEmpRHSHeight;

    HDC m_hdcBmpArrowFulLHS    ; HBITMAP m_hBmpArrowFulLHS    ; HBITMAP m_hOldObjBmpArrowFulLHS    ;
    unsigned long const m_BmpArrowFulLHSWidth;
    unsigned long const m_BmpArrowFulLHSHeight;
    HDC m_hdcBmpArrowFulRHS    ; HBITMAP m_hBmpArrowFulRHS    ; HBITMAP m_hOldObjBmpArrowFulRHS    ;
    unsigned long const m_BmpArrowFulRHSWidth;
    unsigned long const m_BmpArrowFulRHSHeight;

    HDC m_hdcBmpCentreEmp    ; HBITMAP m_hBmpCentreEmp    ; HBITMAP m_hOldObjBmpCentreEmp    ;
    unsigned long const m_BmpCentreEmpWidth;
    unsigned long const m_BmpCentreEmpHeight;
    HDC m_hdcBmpCentreFul    ; HBITMAP m_hBmpCentreFul    ; HBITMAP m_hOldObjBmpCentreFul    ;
    unsigned long const m_BmpCentreFulWidth;
    unsigned long const m_BmpCentreFulHeight;


    HFONT m_fntFixed;
    HFONT m_hFontOld;
    RECT  m_rcUpdateRgn;

    unsigned short m_hgtFont    ;

    short m_DroppedBufs;
    unsigned short m_nUnstableCount;
    float          m_fPrevPitch ;
    float          m_fAvPitch   ;


    std::basic_string<TCHAR> m_strAppTitle;

    TCHAR m_strVol [30];
    TCHAR m_strTune[52];


    unsigned char const m_ReferenceAFrequencyHundreds;
    unsigned char const m_ReferenceAFrequencyTens    ;
    unsigned char const m_ReferenceAFrequencyOnes    ;

	float m_volume;
    int m_iNoteName;
    int m_iPitchIndicator8IsCentre;

private:
    std::auto_ptr<CoTunerCore> m_autopCore;
};
#endif // !defined(AFX_OpenTunerDLG_H__E8300326_0843_11D6_91F5_004033D19F0F__INCLUDED_)
