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
#include "taudiobuffer.h"
#include "tpitchfinder.h"
#include <taudioparams.h>
#include <tglobals.h>

#include <QtCore/qiodevice.h>
#include <QtCore/qthread.h>
#include <QtGui/qevent.h>
#include <QtMultimedia/qaudioinput.h>
#include <QtMultimedia/qmediadevices.h>
#include <QtWidgets/qapplication.h>

#include <QtCore/qdebug.h>

#define TOUCH_PAUSE (1000)

/*static */
QStringList TaudioIN::getAudioDevicesList()
{
    QStringList devNamesList;
    auto devList = QMediaDevices::audioInputs();
    for (auto const &d : devList)
        devNamesList << d.description();
    return devNamesList;
}

TaudioIN *TaudioIN::m_instance = nullptr;
QString TaudioIN::m_deviceName = QStringLiteral("anything");

TaudioIN::TaudioIN(TaudioParams *params, QObject *parent)
    : TcommonListener(params, parent)
    , m_audioParams(params)
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

TaudioIN::~TaudioIN()
{
    stopListening();
    delete m_touchHandler;
    if (m_mediaIN) {
        m_mediaIN->blockSignals(true); // otherwise stop() will activate it again
        m_mediaIN->stop();
    }

    m_audioParams->INdevName = m_deviceName; // store device name at the app exit
    m_deviceName = QStringLiteral("anything");

    m_instance = nullptr;

    if (m_audioIN)
        delete m_audioIN;
}

void TaudioIN::updateAudioParams()
{
    if (m_audioIN && m_audioParams->INdevName != m_deviceName) // device changed
        createInputDevice();
    TcommonListener::setAudioInParams();
}

void TaudioIN::stopDevice()
{
    if (m_mediaIN) {
        disconnect(m_mediaIN, &QMediaRecorder::recorderStateChanged, this, &TaudioIN::stateChangedSlot);
        m_mediaIN->stop();
    } else
        qDebug() << "[tqtaudioin] Input device doesn't exist! Nothing to stop!";
}

void TaudioIN::startDevice()
{
    if (m_mediaIN) {
        m_mediaIN->record();
        connect(m_mediaIN, &QMediaRecorder::recorderStateChanged, this, &TaudioIN::stateChangedSlot, Qt::UniqueConnection);
    } else
        qDebug() << "[tqtaudioin] Input device doesn't exist! Nothing to start!";
}

void TaudioIN::startListening()
{
    m_touchHandler->skip();
    if (m_mediaIN && m_mediaIN->recorderState() == QMediaRecorder::RecordingState)
        startDevice();
    if (detectingState() == e_stopped) {
        resetVolume();
        resetChunkPitch();
    }
    if (!stoppedByUser() && detectingState() != e_detecting) {
        //     qDebug() << "[TaudioIN] start listening";
        setState(e_detecting);
    }
}

void TaudioIN::stopListening()
{
    m_touchHandler->skip();
    if (detectingState() != e_stopped) {
        //     qDebug() << "[TaudioIN] Stop listening";
        setState(e_stopped);
        finder()->stop(true);
    }
}

// #################################################################################################
// ###################              PRIVATE             ############################################
// #################################################################################################

void TaudioIN::stateChangedSlot(QMediaRecorder::RecorderState s)
{
    if (s != QMediaRecorder::RecordingState) {
        qDebug() << "[TaudioIN] input device is not active, trying to activate";
        if (m_mediaIN->error() != QMediaRecorder::NoError)
            qDebug() << "[TaudioIN] error occurred" << m_mediaIN->error();
        m_audioIN->blockSignals(true);
        QTimer::singleShot(100, this, [=] {
            if (m_mediaIN->recorderState() != QMediaRecorder::StoppedState)
                m_mediaIN->stop();
            m_mediaIN->record();
            m_mediaIN->blockSignals(false);
            qDebug() << "[TaudioIN] input device started again";
        });
    }
}

void TaudioIN::createInputDevice()
{
    m_deviceInfo = QMediaDevices::defaultAudioInput();
    // QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    // for (int i = 0; i < devList.size(); ++i) { // find device with name or keep default one
    //     if (devList[i].deviceName() == m_audioParams->INdevName) {
    //         m_deviceInfo = devList[i];
    //         break;
    //     }
    // }
    m_deviceName = m_deviceInfo.description();
    qDebug() << "IN:" << m_deviceName;
    QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(48000);
    // format.setSampleType(QAudioFormat::SignedInt);
    // format.setSampleSize(16);
    // format.setCodec("audio/pcm");
    // format.setByteOrder(QAudioFormat::LittleEndian);
    // if (!m_deviceInfo.isFormatSupported(format)) {
    //     qDebug() << "[TaudioIN] Format 48000/16 mono is not supported";
    //     format = m_deviceInfo.nearestFormat(format);
    //     qDebug() << "[TaudioIN] Format is" << format.sampleRate() << format.channelCount() << format.sampleSize();
    // }

    if (m_audioIN) {
        delete m_audioIN;
        m_inBuffer->close();
    }
    m_audioIN = new QAudioInput(this);
    finder()->setSampleRate(48000); // framesPerChunk is determined here
    // m_audioIN->setBufferSize(1024); // about 11ms

    m_inBuffer->open(QIODevice::WriteOnly);
    m_inBuffer->setBufferSize(0); // respect amount of data send by input device, otherwise it will be overwritten
    connect(m_inBuffer, SIGNAL(readAudio(const char *, qint64 &)), this, SLOT(bufferReady(const char *, qint64 &)));

    startDevice();
    //   qDebug() << "setAudioInParams" << "\nrate:" << finder()->aGl()->rate << m_audioIN->format().sampleRate() << "\nmethod:"
    //           << m_audioParams->detectMethod
    //           << "\nmin duration" << m_audioParams->minDuration << "\nmin volume" << m_audioParams->minimalVol
    //           << "\nnoise filter:" << finder()->aGl()->equalLoudness << "\ndetection range:" << detectionRange();
}

void TaudioIN::bufferReady(const char *data, qint64 &dataLenght)
{
    if (detectingState() == e_detecting)
        finder()->copyToBuffer((void *)data, (unsigned int)dataLenght / 2); // convert data lenght to frames amount
}

// #################################################################################################
// ###################        class TtouchHandler       ############################################
// #################################################################################################

TtouchHandler::TtouchHandler(TcommonListener *sniffer)
    : m_sniffer(sniffer)
{
    m_touchTimer = new QTimer(this);
    connect(m_touchTimer, &QTimer::timeout, this, &TtouchHandler::touchTimerSlot);
    m_installed = GLOB->touchStopsSniff();
    if (GLOB->touchStopsSniff())
        qApp->installEventFilter(this);
}

void TtouchHandler::stopHandling()
{
    if (m_installed) {
        qApp->removeEventFilter(this);
        m_touchTimer->stop();
        m_touched = false;
        m_installed = false;
        qDebug() << "[TtouchHandler] Handling touch events disabled";
    }
}

void TtouchHandler::startHandling()
{
    if (!m_installed) {
        qApp->installEventFilter(this);
        m_installed = true;
        qDebug() << "[TtouchHandler] Handling touch events enabled";
    }
}

bool TtouchHandler::eventFilter(QObject *obj, QEvent *event)
{
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

void TtouchHandler::touchTimerSlot()
{
    m_touchTimer->stop();
    if (!m_touched)
        m_sniffer->startListening();
    else
        m_touchTimer->start(TOUCH_PAUSE);
}
