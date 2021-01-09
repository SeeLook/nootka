/***************************************************************************
 *   Copyright (C) 2015-2021 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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

#include "tqtaudioin.h"
#include "tpitchfinder.h"
#include "taudiobuffer.h"
#include <taudioparams.h>
#include <tglobals.h>

#include <QtMultimedia/qaudioinput.h>
#include <QtCore/qiodevice.h>
#include <QtCore/qthread.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qevent.h>

#include <QtCore/qdebug.h>


#define TOUCH_PAUSE (1000)


/*static */
QStringList TaudioIN::getAudioDevicesList() {
  QStringList devNamesList;
  QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  for (int i = 0; i < devList.size(); ++i)
    devNamesList << devList[i].deviceName();
  return devNamesList;
}


TaudioIN*              TaudioIN::m_instance = nullptr;
QString                TaudioIN::m_deviceName = QStringLiteral("anything");


TaudioIN::TaudioIN(TaudioParams* params, QObject *parent) :
  TcommonListener(params, parent),
  m_audioParams(params),
  m_audioIN(nullptr),
  m_inBuffer(nullptr)
{
  if (m_instance) {
    qDebug() << "Nothing of this kind... TaudioIN already exist!";
    return;
  }
  m_instance = this;
  m_inBuffer = new TaudioBuffer(this);

  m_touchHandler = new TtouchHandler(this);

  createInputDevice();
}


TaudioIN::~TaudioIN() {
  stopListening();
  delete m_touchHandler;
  m_audioIN->blockSignals(true); // otherwise stop() will activate it again
  m_audioIN->stop();

  m_audioParams->INdevName = m_deviceName; // store device name at the app exit
  m_deviceName = QStringLiteral("anything");

  m_instance = 0;

  if (m_audioIN)
    delete m_audioIN;
}


void TaudioIN::updateAudioParams() {
  if (m_audioIN && m_audioParams->INdevName != m_deviceName) // device changed
    createInputDevice();
  TcommonListener::setAudioInParams();
}


void TaudioIN::startListening() {
  m_touchHandler->skip();
  if (detectingState() == e_stopped) {
    resetVolume();
    resetChunkPitch();
  }
  if (!stoppedByUser() && detectingState() != e_detecting) {
//     qDebug() << "[TaudioIN] start listening";
    setState(e_detecting);
  }
}


void TaudioIN::stopListening() {
  m_touchHandler->skip();
  if (detectingState() != e_stopped) {
//     qDebug() << "[TaudioIN] Stop listening";
    setState(e_stopped);
    finder()->stop(true);
  }
}

//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TaudioIN::stateChangedSlot(QAudio::State s) {
  if (s != QAudio::ActiveState) {
    qDebug() << "[TaudioIN] input device is not active, trying to activate";
    if (m_audioIN->error() != QAudio::NoError)
      qDebug() << "[TaudioIN] error occurred" << m_audioIN->error();
    m_audioIN->blockSignals(true);
    QTimer::singleShot(100, [=]{
      if (m_audioIN->state() != QAudio::StoppedState)
        m_audioIN->stop();
      m_audioIN->start(m_inBuffer);
      m_audioIN->blockSignals(false);
      qDebug() << "[TaudioIN] input device started again";
    });
  }
}


void TaudioIN::createInputDevice() {
  m_deviceInfo = QAudioDeviceInfo::defaultInputDevice();
  QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  for (int i = 0; i < devList.size(); ++i) { // find device with name or keep default one
    if (devList[i].deviceName() == m_audioParams->INdevName) {
      m_deviceInfo = devList[i];
      break;
    }
  }
  m_deviceName = m_deviceInfo.deviceName();
  qDebug() << "IN:" << m_deviceName;
  QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(48000);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
  if (!m_deviceInfo.isFormatSupported(format)) {
    qDebug() << "[TaudioIN] Format 48000/16 mono is not supported";
    format = m_deviceInfo.nearestFormat(format);
    qDebug() << "[TaudioIN] Format is" << format.sampleRate() << format.channelCount() << format.sampleSize();
  }

  if (m_audioIN) {
    delete m_audioIN;
    m_inBuffer->close();
  }
  m_audioIN = new QAudioInput(m_deviceInfo, format, this);
  finder()->setSampleRate(m_audioIN->format().sampleRate()); // framesPerChunk is determined here
  m_audioIN->setBufferSize(1024); // about 11ms

  m_inBuffer->open(QIODevice::WriteOnly);
  m_inBuffer->setBufferSize(0); // respect amount of data send by input device, otherwise it will be overwritten
  connect(m_inBuffer, SIGNAL(readAudio(const char*, qint64&)), this, SLOT(bufferReady(const char*, qint64&)));

  m_audioIN->start(m_inBuffer);
  connect(m_audioIN, &QAudioInput::stateChanged, this, &TaudioIN::stateChangedSlot);
//   qDebug() << "setAudioInParams" << "\nrate:" << finder()->aGl()->rate << m_audioIN->format().sampleRate() << "\nmethod:"
//           << m_audioParams->detectMethod
//           << "\nmin duration" << m_audioParams->minDuration << "\nmin volume" << m_audioParams->minimalVol
//           << "\nsplit volume" << (finder()->isSplitByVolume() ? finder()->minVolumeToSplit() * 100.0 : 0.0)
//           << "\nskip volume" << finder()->skipStillerValue() * 100.0
//           << "\nnoise filter:" << finder()->aGl()->equalLoudness << "\ndetection range:" << detectionRange();
}


void TaudioIN::bufferReady(const char* data, qint64& dataLenght) {
  if (detectingState() == e_detecting)
    finder()->copyToBuffer((void*)data, (unsigned int)dataLenght / 2); // convert data lenght to frames amount
}


//#################################################################################################
//###################        class TtouchHandler       ############################################
//#################################################################################################

TtouchHandler::TtouchHandler(TcommonListener* sniffer) :
  m_sniffer(sniffer)
{
  m_touchTimer = new QTimer(this);
  connect(m_touchTimer, &QTimer::timeout, this, &TtouchHandler::touchTimerSlot);
  m_installed = GLOB->touchStopsSniff();
  if (GLOB->touchStopsSniff())
    qApp->installEventFilter(this);
}


void TtouchHandler::stopHandling() {
  if (m_installed) {
    qApp->removeEventFilter(this);
    m_touchTimer->stop();
    m_touched = false;
    m_installed = false;
    qDebug() << "[TtouchHandler] Handling touch events disabled";
  }
}


void TtouchHandler::startHandling() {
  if (!m_installed) {
    qApp->installEventFilter(this);
    m_installed = true;
    qDebug() << "[TtouchHandler] Handling touch events enabled";
  }
}


bool TtouchHandler::eventFilter(QObject* obj, QEvent* event) {
  if (event->type() == QEvent::TouchBegin) {
      if (!m_touched) {
        m_touched = true;
        if (m_sniffer->detectingState() == TaudioIN::e_detecting) {
          m_sniffer->stopListening();
          m_touchTimer->start(TOUCH_PAUSE);
        }
      }
  } else if (event->type() == QEvent::TouchEnd) {
      if (m_touched) {
        m_touched = false;
        if (m_touchTimer->isActive()) {
          m_touchTimer->stop();
          m_touchTimer->start(TOUCH_PAUSE);
        }
      }
  }
  return QObject::eventFilter(obj, event);
}


void TtouchHandler::touchTimerSlot() {
  m_touchTimer->stop();
  if (!m_touched)
    m_sniffer->startListening();
  else
    m_touchTimer->start(TOUCH_PAUSE);
}


