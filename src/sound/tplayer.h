/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TPLAYER_H
#define TPLAYER_H

#include "portaudio.h"
#include <QString>

class Tnote;

class Tplayer
{
public:
    Tplayer();
    ~Tplayer();

    void play(Tnote note);



private:
    void getAudioData();
    static int paCallBack( const void *inBuffer, void *outBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void *userData );
//    int getValueFromChunk(char *chunk, int len);



    PaStreamParameters m_paParam;
    PaError m_paErr;
    PaStream *m_outStream;

    unsigned short m_chanels;
    quint32 m_sampleRate;
    char *m_audioArr;
    short *m_wavDataPtr;
    static int m_samplesCnt;


};

#endif // TPLAYER_H
