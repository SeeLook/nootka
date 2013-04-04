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


#include "trtaudioabstract.h"
#include <QDebug>
#include <QFileInfo>

TrtAudioAbstract::TrtAudioAbstract(TaudioParams* params) :
  deviceName("anything"),
  rtDevice(0),
  audioParams(params),
  streamOptions(0)
{

}

/*static*/
RtAudio* TrtAudioAbstract::getRtAudio() {
    RtAudio *rta = new RtAudio();
#if defined(__LINUX_PULSE__)
    if (rta->getCurrentApi() != RtAudio::UNIX_JACK) {
      QFileInfo pulseBin("/usr/bin/pulseaudio");
      if (pulseBin.exists()) {
      RtAudio *rtPulse = new RtAudio(RtAudio::LINUX_PULSE);
        if (rtPulse->getCurrentApi() == RtAudio::LINUX_PULSE) {
          if (rtPulse->getDeviceCount() > 0) {
              delete rta;
              rta = rtPulse;
          }
        }
      }
    }
#endif
    rta->showWarnings(false);
    return rta;
}


void TrtAudioAbstract::showSupportedFormats(RtAudio::DeviceInfo& devInfo) {
  QString fmt;
  if (devInfo.nativeFormats & 0x1)
    fmt += " RTAUDIO_SINT8";
  if (devInfo.nativeFormats & 0x2)
    fmt += " RTAUDIO_SINT16";
  if (devInfo.nativeFormats & 0x4)
    fmt += " RTAUDIO_SINT24";
  if (devInfo.nativeFormats & 0x8)
    fmt += " RTAUDIO_SINT32";
  if (devInfo.nativeFormats & 0x10)
    fmt += " RTAUDIO_FLOAT32";
  if (devInfo.nativeFormats & 0x20)
    fmt += " RTAUDIO_FLOAT32";
  qDebug() << "suported formats:" << fmt;
}



bool TrtAudioAbstract::getDeviceInfo(RtAudio::DeviceInfo& devInfo, int id) {
  try {
          devInfo = rtDevice->getDeviceInfo(id);
  }
  catch (RtError& e) {
    qDebug() << "error when probing input device" << id;
    return false;
  }
  return true;
}



bool TrtAudioAbstract::openStream(RtAudio::StreamParameters* outParams, RtAudio::StreamParameters* inParams,
                                  RtAudioFormat frm, unsigned int rate, unsigned int* buffFrames, RtAudioCallback callBack,
                                  void* userData, RtAudio::StreamOptions* options) {
  try {
    if (rtDevice && !rtDevice->isStreamOpen())
      rtDevice->openStream(outParams, inParams, frm, rate, buffFrames, callBack, userData, options);
  }
  catch (RtError& e) {
    qDebug() << "can't open stream";
    return false;
  }
  return true;
}


bool TrtAudioAbstract::startStream() {
  try {
    if (rtDevice && !rtDevice->isStreamRunning())
      rtDevice->startStream();
  }
  catch (RtError& e) {
    qDebug() << "can't start stream";
    return false;
  }
  return true;
}


void TrtAudioAbstract::stopStream() {
  try {
    if (rtDevice && rtDevice->isStreamRunning())
      rtDevice->stopStream();
  }
  catch (RtError& e) {
    qDebug() << "can't stop stream";
  }
}


void TrtAudioAbstract::closeStram() {
  try {
    stopStream();
    if (rtDevice && rtDevice->isStreamOpen())
      rtDevice->closeStream();
  }
  catch (RtError& e) {
    qDebug() << "can't close stream";
  }
}




