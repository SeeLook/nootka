/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TRTAUDIOABSTRACT_H
#define TRTAUDIOABSTRACT_H


#include "nootkasoundglobal.h"
#include "rt/RtAudio.h"
#include <QString>

class TaudioParams;


/** Abstract class for RtAudio input/outputs classes.
* It doesn't provide destructor, so inherit classes have to
* delete rtDevice, streamOptions and audioParams them-self. */
class NOOTKASOUND_EXPORT TrtAudio
{

public:
		enum EdevType {	e_input, e_output	};
		typedef bool (*callBackType)(void*, unsigned int, const RtAudioStreamStatus&);
	
    TrtAudio(TaudioParams *audioP, TrtAudio::EdevType type);
		~TrtAudio();
		
    QString deviceName() { if (m_type == e_input) return m_inDevName; else return m_outDevName; }
    
    void open() { openStream(); }
    void startAudio() { startStream(); }
		void stopAudio() { stopStream(); }
		void terminate() { closeStram(); }
		
    
    static void createRtAudio(); /** Creates RtAudio instance. Once for whole application */
		
		void updateAudioParams();
		TaudioParams* audioParams() { return m_audioParams; }
    
        /** Prints in console supported audio formats. */
    static void printSupportedFormats(RtAudio::DeviceInfo &devInfo);
    static void printSupportedSampleRates(RtAudio::DeviceInfo &devInfo);
		static unsigned int bufferFrames() { return m_bufferFrames; }
		
				/** Returns instance of RtAudio device. 
				 * It is single instance for entire application */
		static RtAudio* rtDevice() { return m_rtAduio; }
		
		static quint32 sampleRate() { return m_sampleRate; }
    
protected:
// 		virtual bool inCallBack(void* inBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& status) {
// 				Q_UNUSED(inBuff)
// 				Q_UNUSED(nBufferFrames)
// 				Q_UNUSED(status)
// 				return false; 
// 		}
// 		virtual bool outCallBack(void* outBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& status) {
// 				Q_UNUSED(outBuff)
// 				Q_UNUSED(nBufferFrames)
// 				Q_UNUSED(status)
// 				return false;
// 		}
		void setOutCallBack(callBackType cbOut) { m_cbOut = cbOut; }
		void setInCallBack(callBackType cbIn) { m_cbIn = cbIn; }
		
		void deleteOutParams() { delete m_outParams; m_outParams = 0; }
		void deleteInParams() { delete m_inParams; m_inParams = 0; }
		
protected:
    static RtAudio::StreamOptions *streamOptions;
		RtAudio::StreamParameters* streamParams() { if (m_type == e_input) return m_inParams; else return m_outParams; }
		
        /** Examines available sample rates to check more appropriate. 
         * 44100 48000 88200 ... 192000. If not the latest from the list is taken. 
         * @param sampleRate is setting. */
    quint32 determineSampleRate(RtAudio::DeviceInfo& devInfo);
    
		bool openStream();
    bool openStream(RtAudio::StreamParameters *outParams,  RtAudio::StreamParameters *inParams, 
                    RtAudioFormat frm, unsigned int rate, unsigned int *buffFrames,
                    RtAudioCallback callBack, void *userData = 0 , RtAudio::StreamOptions *options = 0);
    bool startStream();
    void stopStream();
    void closeStram();
    
    bool getDeviceInfo(RtAudio::DeviceInfo &devInfo, int id);
		
private:
		static int duplexCallBack(void *outBuffer, void *inBuffer, unsigned int nBufferFrames, double streamTime,
                         RtAudioStreamStatus status, void *userData);
    
private:
		TaudioParams													*m_audioParams;
		static RtAudio					 							*m_rtAduio;
		EdevType															 m_type;
		static QString  											 m_inDevName, m_outDevName;
		static RtAudio::StreamParameters 			*m_inParams;
		static RtAudio::StreamParameters 			*m_outParams;
		static quint32 												 m_sampleRate;
		static unsigned int										 m_bufferFrames;
		static bool														 m_isAlsaDefault;
		bool																	 m_isOpened;
		static callBackType										 m_cbIn, m_cbOut;
        
};

#endif // TRTAUDIOABSTRACT_H
