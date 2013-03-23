/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "rt/RtAudio.h"
#include <QString>

class TaudioParams;


/** Abstract class for RtAudio input/outpus classes.
* It doesn't provide destructor, so inherit classes have to
* delete rtDevice, streamOptions and audioParams themself. */
class TrtAudioAbstract
{

public:
    TrtAudioAbstract(TaudioParams *params);
    QString devName() { return deviceName; }
    static RtAudio* getRtAudio();
    
protected:
    RtAudio* rtDevice;
    RtAudio::StreamOptions *streamOptions;
    TaudioParams *audioParams;
    QString deviceName;
    
    bool openStream(RtAudio::StreamParameters *outParams,  RtAudio::StreamParameters *inParams, 
                    RtAudioFormat frm, unsigned int rate, unsigned int *buffFrames,
                    RtAudioCallback callBack, void *userData = 0 , RtAudio::StreamOptions *options = 0);
    bool startStream();
    void stopStream();
    void closeStram();
    
    bool getDeviceInfo(RtAudio::DeviceInfo &devInfo, int id);
        
};

#endif // TRTAUDIOABSTRACT_H
