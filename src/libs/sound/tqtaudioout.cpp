/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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


#include "tqtaudioout.h"
#include <taudioparams.h>
#include <tpath.h>
#include <QtMultimedia/QMediaPlayer>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDir>



TaudioOUT::TaudioOUT(TaudioParams* _params, QObject* parent) :
  TabstractPlayer(parent),
  m_audioParams(_params)
{
  setType(e_audio);
  setAudioOutParams();

  m_player = new QMediaPlayer(parent, QMediaPlayer::LowLatency);
  QFile::copy(Tpath::sound("classical-guitar", ".wav").replace(":", "assets:"), QDir::tempPath() + "/nootkaPlayback.wav");
  m_player->setMedia(QUrl::fromLocalFile(QDir::tempPath() + "/nootkaPlayback.wav"));
  m_player->setVolume(95);
  qDebug() << m_player->error() << m_player->errorString();
  playable = true;
  offTimer = new QTimer(this);
  offTimer->setTimerType(Qt::PreciseTimer);
  connect(offTimer, &QTimer::timeout, this, &TaudioOUT::playingFinishedSlot);
//   connect(ao(), &TaudioObject::playingFinished, this, &TaudioOUT::playingFinishedSlot);
}


TaudioOUT::~TaudioOUT() 
{
  offTimer->stop();
  m_player->stop();
  QFile::remove(QDir::tempPath() + "/nootkaPlayback.ogg");
}


void TaudioOUT::setAudioOutParams() {
}



bool TaudioOUT::play(int noteNr) {
  if (!playable)
      return false;

//   qDebug() << "play" << noteNr << m_player->state() << m_player->mediaStatus();
  noteNr = noteNr + qRound(m_audioParams->a440diff);
  if (noteNr < -11 || noteNr > 41)
    return false;

  offTimer->stop();
//   if (m_player->state() == QMediaPlayer::PlayingState)
//     m_player->pause();
  m_player->setPosition(qMax<int>((noteNr + 11) * 2000 - 10, 0)); // every note takes 2000 ms in recording, but start it 10 ms before
//   if (m_player->state() == QMediaPlayer::PlayingState)
//     m_player->pause();
  m_player->play();

  offTimer->start(1600);

  doEmit = true;

  return true;
}


void TaudioOUT::playingFinishedSlot() {
  offTimer->stop();
  m_player->pause();
  if (doEmit)
    emit noteFinished();
}


void TaudioOUT::stop() {
  doEmit = false;
	playingFinishedSlot();
}



