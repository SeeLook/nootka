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
#if !defined(AFX_oTunerCore_H__E8300326_0843_11D6_91F5_004033D19F0F__INCLUDED_)
#define AFX_oTunerCore_H__E8300326_0843_11D6_91F5_004033D19F0F__INCLUDED_
// oTunerCore.h : header file
////////////////////////////////////////////////////////////////////////////////////////////////////

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include <vector>
#include <string>
#include "kiss_fftr.h"
#include "mmsystem.h"
#include "tartini_core\tartinilib\tartini_core.h"

static unsigned long const UWM_oTuner_ERROR_OCCURRED_wpNULL_lpNULL           = (WM_APP + 2); //WPARAM and LPARAM both always NULL
static unsigned long const UWM_oTuner_NewPitch_wpfPitch_lpfVolume            = (WM_APP + 3); //wParam (a float *) == pitch, lParam (a float * ) == Volume
static unsigned long const UWM_oTuner_DroppedBuffers_wpNBufsDropped_lpNULL   = (WM_APP + 4); //wParam is number of dropped buffers, LPARAM is always NULL


#define ErrorCheck(cmd, ErrorString, ReturnOnError) if (!cmd) {m_strLastError = ErrorString; ::PostMessage(m_hWnd, UWM_oTuner_ERROR_OCCURRED_wpNULL_lpNULL, NULL, NULL); if (ReturnOnError) return;}


class CoTunerCore
{
public:
	CoTunerCore(HWND const hWndNotify);
    virtual ~CoTunerCore();
    
    
    virtual std::basic_string<TCHAR> GetLastErr() const {return m_strLastError;};


private:
    std::basic_string<TCHAR> m_strLastError;

    HWND const m_hWnd;

    bool m_bQuit;


private:
	HWAVEIN         m_hwi; //Used for actual audio Input
    WAVEFORMATEX    m_fmt; //Format of default WAV

    typedef __int16 TRawSampleFormat; //!!!
    //typedef float TRawSampleFormat; //!!!

    std::vector<WAVEHDR> m_aWHs;
    std::vector<std::vector<TRawSampleFormat> > m_aBufBuf;

    unsigned long m_NextBuffer;


    unsigned long m_BuffersQueued;
    unsigned long m_BuffersHandled;


    static void CALLBACK cbkWAVBuf_stub(HWAVEIN hwi, UINT uMsg, DWORD pvThis, DWORD dw1, DWORD dw2)
        {((CoTunerCore*)pvThis)->cbkWAVBuf(hwi, uMsg, dw1, dw2);};

    void cbkWAVBuf(HWAVEIN hwi, UINT uMsg, DWORD dwParam1, DWORD dwParam2);


    void BufferNewData(TRawSampleFormat * pSamples , unsigned long const NumSamples);
    void GetPitchFromNewBuffer();


private:
    HANDLE m_hKickProc;
    HANDLE m_hThread;
    static DWORD WINAPI SampleProcessor_stub(void* pvThis)
        {((CoTunerCore*)pvThis)->SampleProcessor();return 0;};
    void SampleProcessor();

private:
    int       const m_Step_size;
    Tartini *       m_pTartini ;
    std::vector<kiss_fft_scalar> m_aSamples; //size 2048; all inbound buffers to cbk are 1024...
    unsigned long m_nWr;
    unsigned long m_nRd;
    long m_LastBuffersDropped;
};
#endif // !defined(AFX_oTunerCore_H__E8300326_0843_11D6_91F5_004033D19F0F__INCLUDED_)
