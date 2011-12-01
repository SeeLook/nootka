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
// oTunerCore.cpp : implementation file
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#ifdef _DEBUG
#include <sstream>
#endif
#include "resource.h"
#include "OpenTunerCore.h"
#include "cBscFileVersion.h"

#ifdef _DEBUG
#ifdef _AFX
#define new DEBUG_NEW
#endif
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/*
!!!Thoughts:
Acquiring __int32's might be faster, what with it being an arm processor and all.
After that, losing the / 32768 is probably going to help...
*/

//#define cbscTEST_USING_WAV_FILE 1

#ifdef cbscTEST_USING_WAV_FILE
#include "Testing\WaveFile.h"
CWaveFile s_WAV(_T("example.wav"));
unsigned long s_WAVSeek = 0;
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                         Constants
//--------------------------------------------------------------------------------------------------


//For whatever reason including the appropriate #include causes various defninition conflicts
//#defining it here seems to work ok, however
#ifndef WAVE_FORMAT_IEEE_FLOAT
#define  WAVE_FORMAT_IEEE_FLOAT 0x0003  /*  Microsoft Corporation  */
#endif
static float const s_SampleFreq_Hz = 44100;
static float const          s_qNaN = (float)sqrt(-1.0);

////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                         Construction/ Destruction
//--------------------------------------------------------------------------------------------------


static unsigned long const s_NumBufs = 1024;
CoTunerCore::CoTunerCore(HWND const hWndNotify) : m_hWnd(hWndNotify), m_bQuit(false), 
    m_Step_size(1024),
    m_pTartini(0),
    m_NextBuffer(s_NumBufs-1),
    m_hKickProc(CreateEvent(NULL, FALSE, FALSE, NULL)),
    m_hThread(0),
    m_BuffersQueued(0),
    m_BuffersHandled(0),
    m_LastBuffersDropped(0),
    m_nWr(1), //We start writing at +1 cos 1st ever read should read an all-zero "pre" buffer
    m_nRd(0)
{
    WAVEHDR tmp0 = {0,m_Step_size*sizeof(TRawSampleFormat),0,(unsigned long)this,0,0,0,0};
    m_aWHs.resize(s_NumBufs, tmp0);

    std::vector<TRawSampleFormat> tmpBuf; tmpBuf.resize(m_Step_size, 0);
    m_aBufBuf.resize(s_NumBufs, tmpBuf);

    memset(&m_hwi,0,sizeof(m_hwi));
    memset(&m_fmt, 0, sizeof(m_fmt));

    m_pTartini = new_tartini(m_Step_size*2, s_SampleFreq_Hz);
                    //1+<step_size>*3

	unsigned long const BufSize = m_Step_size*(s_NumBufs+1); //1+ to handle special case of wraparound when we have do do a double-copy
    m_aSamples.resize(BufSize, 0); 

    m_fmt.cbSize          = sizeof(m_fmt);
    m_fmt.nChannels       = 1;
    m_fmt.nSamplesPerSec  = (DWORD)s_SampleFreq_Hz;
    m_fmt.wBitsPerSample  = sizeof(TRawSampleFormat)*8;
    m_fmt.wFormatTag      = sizeof(TRawSampleFormat)==2?WAVE_FORMAT_PCM:WAVE_FORMAT_IEEE_FLOAT;
    m_fmt.nBlockAlign     = m_fmt.wBitsPerSample/8;
    m_fmt.nAvgBytesPerSec = m_fmt.nSamplesPerSec * m_fmt.nChannels * (m_fmt.wBitsPerSample/8);


    DWORD dwThreadID = 0;
    m_hThread = CreateThread(NULL, NULL, &SampleProcessor_stub, this, NULL, &dwThreadID);

    unsigned long const NumWAVDevices = waveInGetNumDevs();
    ErrorCheck((NumWAVDevices>0), _T("No WAV Input Device Found! Cannot Acquire Samples"), true);

    MMRESULT mmres = MMSYSERR_NODRIVER; unsigned long const cbkFN = CALLBACK_FUNCTION;
    {for (unsigned long devId = 0; ((devId < NumWAVDevices)&&(mmres != MMSYSERR_NOERROR)); ++devId)
        mmres = waveInOpen(&m_hwi, devId, &m_fmt, (DWORD)cbkWAVBuf_stub,(unsigned long)this,cbkFN);}

    for (unsigned long i = 0; i < m_aWHs.size(); ++i)
    {
        m_aWHs[i].lpData = (char*)&*m_aBufBuf[i].begin();

        if (i < m_aWHs.size() -1)
        {
	        mmres = waveInPrepareHeader(m_hwi, &m_aWHs[i], sizeof(WAVEHDR));	
            ErrorCheck((mmres == MMSYSERR_NOERROR), _T("Failed to prep WAV, cannot Acquire Samples"), true);

            waveInAddBuffer(m_hwi, &m_aWHs[i], sizeof(m_aWHs[i]));
        }
        //else the very last one will be prepped on receipt by the thread
    }

	mmres = waveInStart(m_hwi);	
    ErrorCheck((mmres == MMSYSERR_NOERROR), _T("Failed to start acquiring samples"), true);
}
//--------------------------------------------------------------------------------------------------


CoTunerCore::~CoTunerCore()
{
    m_bQuit = true;
    SetEvent(m_hKickProc);
    WaitForSingleObject(m_hThread, INFINITE);
    CloseHandle(m_hKickProc);
    CloseHandle(m_hThread);
    if (m_hwi)
    {
        ErrorCheck((MMSYSERR_NOERROR == waveInStop(m_hwi)), _T("Failed to stop recording!"), false);
        ErrorCheck((MMSYSERR_NOERROR == waveInReset(m_hwi)),_T("Failed to reset recording!"),false);

        for (unsigned long i = 0; i < m_aWHs.size(); ++i)
        {
            if (m_aWHs[i].dwFlags & WHDR_PREPARED)
            {
                ErrorCheck((MMSYSERR_NOERROR == waveInUnprepareHeader(m_hwi, &m_aWHs[i], sizeof(m_aWHs[i]))), 
                    _T("Failed to unprepare WAV header!"), false);
            }
        }
        ErrorCheck((MMSYSERR_NOERROR == waveInClose(m_hwi)),_T("Failed to close playback!"), false);
        m_hwi = 0;
    }

    delete_tartini(m_pTartini); m_pTartini = 0;
}
//--------------------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                          Sample-handling callback
//--------------------------------------------------------------------------------------------------


void CoTunerCore::cbkWAVBuf(HWAVEIN hwi, UINT uMsg, DWORD dwParam1, DWORD dwParam2)
{
    if (m_bQuit)
        return;

    switch (uMsg)
    {
    case WIM_CLOSE :{OutputDebugString(_T("Bouffe (Close)!\r\n"));break;}
    case WIM_DATA  :
        {
            //OutputDebugString(_T("Bouffe (Data)!\r\n"));
            WAVEHDR * pHdr = (WAVEHDR *)dwParam1;

            TRawSampleFormat  * pSamples   = (TRawSampleFormat*) pHdr->lpData;
            unsigned long const NumSamples = pHdr->dwBytesRecorded/sizeof(TRawSampleFormat);

            if (m_LastBuffersDropped < 10)
                BufferNewData(pSamples, NumSamples);
            //###else just throw this data away
            SetEvent(m_hKickProc);

            if (m_aWHs[m_NextBuffer].dwFlags & WHDR_PREPARED)
                waveInUnprepareHeader(m_hwi, &m_aWHs[m_NextBuffer], sizeof(WAVEHDR));
            memset(&m_aWHs[m_NextBuffer], 0, sizeof(m_aWHs[m_NextBuffer]));
            m_aWHs[m_NextBuffer].lpData = (char*)&*m_aBufBuf[m_NextBuffer].begin();
            m_aWHs[m_NextBuffer].dwBufferLength = m_aBufBuf[m_NextBuffer].size() * sizeof(m_aBufBuf[m_NextBuffer][0]);
            m_aWHs[m_NextBuffer].dwUser = (unsigned long)this;
            waveInPrepareHeader(m_hwi, &m_aWHs[m_NextBuffer], sizeof(WAVEHDR));
            waveInAddBuffer(hwi, &m_aWHs[m_NextBuffer], sizeof(m_aWHs[m_NextBuffer]));
            ++m_NextBuffer;

            if (m_NextBuffer >= m_aWHs.size())
                m_NextBuffer = 0;

            break;
        }
    case WIM_OPEN  :{OutputDebugString(_T("Bouffe (Open)!\r\n"));break;}
    default:        {OutputDebugString(_T("Bouffe!Bouffe!\r\n"));break;}
    }
    
}
//--------------------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                         Process new sample buffer
//--------------------------------------------------------------------------------------------------


//###Inefficient:/ Doubly so, as tartini just memcpy's these into another buf I believe!
//Triply so if I'm going to do integer FFT!
//From inspection, only the inverse FFT would populate the last third of autocorrTime, and it doesn't seem to be: 
//Hmm, maybe it does in non-debug mode; however, tartini only seems to want the frist 1024 elements from the inverse anyway!
void CoTunerCore::BufferNewData(TRawSampleFormat * pSamples , unsigned long const NumSamples)
{
    //Copy last sample buffer to first half of samples to analyse
    //Copy new sample buffer to last half of samples to analyse

    /*
    std::vector<TRawSampleFormat> const & Buf = (m_NextBuffer==3?m_aBuf3:(m_NextBuffer==2?m_aBuf2:m_aBuf1));

    {for (unsigned long i = 0; i < NumSamples; ++i)
    {   
        m_pTartini->autocorrTime[i] = Buf[i] / 32768.0f;
        m_pTartini->autocorrTime[i+m_Step_size] = pSamples[i] / 32768.0f;
        m_pTartini->autocorrTime[i+2*m_Step_size] = 0;

        //^^^Hmm, commenting it out causes buffer drops! Weird!
    }}
    */

    /*
    {for (unsigned long i = 0; i < NumSamples; ++i)
    {   //...Except it doesn't work on arm:/###
        //Whee, magic scaling by fudging the exponent:) == /=32768.0 - hopefully quicker than a float-divide!
        
        m_pTartini->autocorrTime[i] = Buf[i];
        *((unsigned __int32*)((float*)&m_pTartini->autocorrTime[i])) -= (unsigned __int32)0x7800000;
        m_pTartini->autocorrTime[i+m_Step_size] = pSamples[i];
        *((unsigned __int32*)((float*)&m_pTartini->autocorrTime[i+m_Step_size])) -= (unsigned __int32)0x7800000;
        m_pTartini->autocorrTime[i+2*m_Step_size] = 0; //<<<Hmm, commenting it out causes buffer drops! Weird!        
    }}
    */

    //[0] is all 0's
    //1st write will be to [1] out of [3]; 1st analysis is of 0,1
    //2nd will be to [2]              [3]; 2nd analysis is of 1,2
    //3rd will be to [3]              [3]; 3rd analysis is of 2,3
    //At which point I copy 3 into 0 also
    //4th will be to [1]                 ; 4th analysis is of 0,1 == 3,4

#ifdef cbscTEST_USING_WAV_FILE
    unsigned long WAVLength = s_WAV.GetLength();
    if (s_WAV.GetLength())
    {
        if (s_WAVSeek > s_WAV.GetLength())
        {
            s_WAVSeek = 0;
            m_bQuit = true;
        }

        s_WAV.ReadSamples(pSamples, NumSamples*2, s_WAVSeek); //###hardcoded to __int32
        s_WAVSeek+=NumSamples*2;
    }
#endif

    unsigned long nBuff = m_Step_size * m_nWr++;

    if (m_nWr > s_NumBufs) //Then, before the ++, we were writing to the last buffer; need to dupe this into the
    {                      //0th buffer for the next analysis.
        {for (unsigned long i = 0; i < NumSamples; ++i)
        {
            //m_aSamples[i] = m_aSamples[i+m_Step_size];
            //m_aSamples[i+m_Step_size] = pSamples[i]/32768.0f;
            m_aSamples[i] = pSamples[i]; //###Hmm: I have bufbuf of 16 bit values, then into this, then into fft buf:(
        }}

		//###Will next analysis begin at 0? What is going on here, I understand setting to 0 1st time round, but not subsequently,..
        m_nWr = 1;// Wrap back to the start; next analysis will begin at 0, and take in the data newly copied into 1, plus the old data in 0;
    }

    {for (unsigned long i = 0; i < NumSamples; ++i)
    {
        //m_aSamples[i] = m_aSamples[i+m_Step_size];
        //m_aSamples[i+m_Step_size] = pSamples[i]/32768.0f;
        m_aSamples[nBuff++] = pSamples[i];
    }}
    m_BuffersQueued++; 
}
//--------------------------------------------------------------------------------------------------


void CoTunerCore::GetPitchFromNewBuffer()
{
    float fPitch = s_qNaN;
    float fVol   = s_qNaN;

    double     freq, logPower;
    unsigned long nBuff =  m_Step_size * m_nRd++; //0(&1), 1(2), 2(&3), so if we're now at 3 (postinc), need to reset

    if (m_nRd >= s_NumBufs)
        m_nRd = 0;

//    int nOldPriority = GetThreadPriority(m_hThread);
//    SetThreadPriority(m_hThread, THREAD_PRIORITY_ABOVE_NORMAL); //!!!Nope, redlines it (I just cant get my PDA to process the buffers fast enough otherwise)

	bool valid  = find_pitch(m_pTartini, /*m_pTartini->autocorrTime*/&m_aSamples[nBuff], &freq, &logPower); 
//    SetThreadPriority(m_hThread, nOldPriority); //!!!Nope, redlines it (I just cant get my PDA to process the buffers fast enough otherwise)
    //valid = true; //###
    ++m_BuffersHandled;

    if (valid)
    {
	    fPitch = (float)freq2pitch(freq); //^^^ == MyTransforms::calculateAnalysisData!!!
        fVol   = (float)(1.0 - (logPower / m_pTartini->noiseFloor));
    }

#ifdef _DEBUG
    std::stringstream strStr; strStr << "Pit:" << fPitch << " @ " << fVol << "\r\n"; 
    //###OutputDebugString(strStr.str().c_str());
#endif


    long BuffersDropped = m_BuffersQueued-m_BuffersHandled;

    if (BuffersDropped > 65535) m_BuffersHandled = m_BuffersQueued = 0;

    if ((BuffersDropped != m_LastBuffersDropped) && (!m_bQuit))
        PostMessage(m_hWnd, UWM_oTuner_DroppedBuffers_wpNBufsDropped_lpNULL, BuffersDropped, 0);
    m_LastBuffersDropped = BuffersDropped;

    if (!m_bQuit)
        PostMessage(m_hWnd, UWM_oTuner_NewPitch_wpfPitch_lpfVolume, *((WPARAM *)&fPitch), *((LPARAM *)&fVol));
}
//--------------------------------------------------------------------------------------------------


void CoTunerCore::SampleProcessor()
{
    while (!m_bQuit)
    {
        WaitForSingleObject(m_hKickProc, INFINITE);

        while (  (!m_bQuit) && 
                    ((m_nRd+1) != (m_nWr)) //We have not caught up with m_nWr (no need to special case for wraparound, it just works!
              )//(0+1) < 2; (1+1) < 3; (2+1) < 1; 0+1 < 2  
            GetPitchFromNewBuffer();
        //Sleep(0);
    }
}
//--------------------------------------------------------------------------------------------------
