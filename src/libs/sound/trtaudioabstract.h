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
class NOOTKASOUND_EXPORT TrtAudioAbstract
{

public:
    TrtAudioAbstract(TaudioParams *params);
    QString devName() { return deviceName; }
    
        /** Creates RtAudio instance. */
    static RtAudio* getRtAudio();
    
        /** Prints in console supported audio formats. */
    static void printSupportedFormats(RtAudio::DeviceInfo &devInfo);
    static void printSupportedSampleRates(RtAudio::DeviceInfo &devInfo);
    
protected:
    RtAudio* rtDevice;
    RtAudio::StreamOptions *streamOptions;
    RtAudio::StreamParameters streamParams;
    TaudioParams *audioParams;
    QString deviceName;
    quint32 sampleRate;
		
		void setToFloat32() { m_isFloat = true; }
		void setToInt16() { m_isFloat = false; }
		bool isFloat() { return m_isFloat; } /** True when device is opened in RTAUDIO_FLOAT32 mode */
		
				/** Grabs supported formats list and sets m_isFloat according to this. 
				 * Returns data format preferred or RTAUDIO_SINT8 if none Nootka capable was found. */
		RtAudioFormat determineDataFormat(RtAudio::DeviceInfo &devInf);
		
				/** Checks is buffer size supported by Nootka.
				 * Returns false when it is less than 256 - which cases crashes. 
				 * Prints warnings on std out. */
		bool checkBufferSize(unsigned int bufferFrames);
		
        /** Examines available sample rates to check more appropriate. 
         * 44100 48000 88200 ... 192000. If not the latest from the list is taken. 
         * @param sampleRate is setting. */
    void determineSampleRate(RtAudio::DeviceInfo &devInfo);
    
    bool openStream(RtAudio::StreamParameters *outParams,  RtAudio::StreamParameters *inParams, 
                    RtAudioFormat frm, unsigned int rate, unsigned int *buffFrames,
                    RtAudioCallback callBack, void *userData = 0 , RtAudio::StreamOptions *options = 0);
    bool startStream();
    void stopStream();
    void closeStram();
    
    bool getDeviceInfo(RtAudio::DeviceInfo &devInfo, int id);
    
private:
		bool m_isFloat; /** True when device is opened in RTAUDIO_FLOAT32 mode */
        
};

#endif // TRTAUDIOABSTRACT_H
