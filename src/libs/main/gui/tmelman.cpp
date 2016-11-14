/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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


#include "tmelman.h"
#include "tmenu.h"
#include "score/tmainscore.h"
#if defined (Q_OS_ANDROID)
  #include <widgets/tmelodyitem.h>
#endif
#include <exam/tqagroup.h>
#include <music/tmelody.h>
#include <tpath.h>
#include "exam/trandmelody.h"
#include <QtWidgets/QtWidgets>


TmelMan::TmelMan(TmainScore* score) :
	QObject(score),
	m_score(score),
	m_audioMark(0)
{
	m_menu = new Tmenu();
  m_playMelAct = createAction(tr("Play melody"), SLOT(playMelodySlot()), QKeySequence(Qt::Key_Space),
               QIcon(Tpath::img("playMelody")));
  m_playMelAct->setCheckable(true);
	m_recMelAct = createAction(tr("Note by note"), SLOT(recordMelodySlot()), QKeySequence("Ctrl+Space"),
							 QIcon(Tpath::img("record")));
#if defined (Q_OS_MAC) // Ctrl is CMD under Mac and CMD+space is system wide shortcut - use Ctrl (Meta there) instead
  m_recMelAct->setShortcut(QKeySequence(QStringLiteral("Meta+Space")));
#endif
	m_recMelAct->setStatusTip(tr("Notes are written on the score one by one. Either playing, selecting fret or note name adds a new note to the staff automatically."));
  m_recMelAct->setCheckable(true);
	m_genMelodyAct = createAction(tr("Generate melody"), SLOT(randomizeMelodySlot()), QKeySequence(), QIcon(Tpath::img("melody")));
	m_genMelodyAct->setStatusTip(tr("Generate a melody with random notes."));

// 	m_menu->addSeparator();
// 	QDialogButtonBox box;
// 	QPushButton *saveBut = box.addButton(QDialogButtonBox::Save);
// 	QPushButton *openBut = box.addButton(QDialogButtonBox::Open);
// 	createAction(openBut->text(), SLOT(loadMelodySlot()), QKeySequence::Open,
// 							 QIcon(score->style()->standardIcon(QStyle::SP_DialogOpenButton)));
// 	createAction(saveBut->text(), SLOT(saveMelodySlot()), QKeySequence::Save,
// 							 QIcon(score->style()->standardIcon(QStyle::SP_DialogSaveButton)));
	
	
	m_button = new QToolButton(score);
	m_button->setIcon(QIcon(Tpath::img("melody")));
	m_button->setText(tr("Melody"));
  QString ssss = tr("Open, save, generate and play a melody.");
// 	m_button->setStatusTip(tr("Open, save, generate and play a melody."));
	m_button->setMenu(m_menu);
	m_button->setPopupMode(QToolButton::InstantPopup);
	m_button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	m_melAct = new QWidgetAction(score);
	m_melAct->setDefaultWidget(m_button);
	connect(m_score, SIGNAL(playbackFinished()), this, SLOT(playMelodySlot()));

  m_audioMark = m_score->scene()->addPixmap(QPixmap(Tpath::img("melody-rec")));
  m_audioMark->hide();
  m_audioMark->setScale(0.25);
  m_audioMark->setFlag(QGraphicsItem::ItemIgnoresTransformations);
}


TmelMan::~TmelMan()
{
  delete m_menu;
}

//####################################################################################################
//########################################## PROTECTED ###############################################
//####################################################################################################

void TmelMan::playMelodySlot() {
	if (m_score->isScorePlayed()) {
		m_score->playScore(); // It will be stopped
		m_recMelAct->setDisabled(false);
    m_playMelAct->setIcon(QIcon(Tpath::img("playMelody")));
    m_playMelAct->setChecked(false);
		if (m_score->insertMode() == TmultiScore::e_record)
      showAudioMark(e_recording);
    else
      showAudioMark(e_none);
#if defined (Q_OS_ANDROID)
    if (TmelodyItem::instance())
        TmelodyItem::instance()->playingStopped();
#endif
	} else {
		if (m_score->currentIndex() < 0) {
      if (m_score->notesCount() > 0)
        m_score->selectNote(0); // start playing from the first note
      else
        return;
    }
		m_recMelAct->setDisabled(true);
    m_playMelAct->setIcon(QIcon(Tpath::img("stopMelody")));
    m_playMelAct->setChecked(true);
		m_score->playScore();
    showAudioMark(e_playing);
	}
}


void TmelMan::recordMelodySlot() {
	if (m_score->insertMode() == TmainScore::e_multi) {
		m_recMelAct->setIcon(QIcon(Tpath::img("stopMelody")));
    m_recMelAct->setChecked(true);
		m_score->setInsertMode(TmainScore::e_record);
    showAudioMark(e_recording);
	} else {
		m_recMelAct->setIcon(QIcon(Tpath::img("record")));
    m_recMelAct->setChecked(false);
		m_score->setInsertMode(TmainScore::e_multi);
    if (!m_score->isScorePlayed())
      showAudioMark(e_none);
#if defined (Q_OS_ANDROID)
    if (TmelodyItem::instance())
        TmelodyItem::instance()->recordingStopped();
#endif
	}
}


void TmelMan::randomizeMelodySlot() {
	QList<TQAgroup> ql;
	int ambit = 25; //highestNote().chromatic() - lowestNote().chromatic();
	for (int i = 0; i < ambit; i++) {
		TQAgroup qa;
		qa.note = Tnote(1 + i);
		ql << qa;
	}
	Tmelody *mel = new Tmelody(QString(), m_score->keySignature());
  mel->setClef(m_score->clef().type());
	getRandomMelodyNG(ql, mel, 14, true, true);
	m_score->setMelody(mel);
	delete mel;
}


void TmelMan::loadMelodySlot() {
	QString melodyFile = QFileDialog::getOpenFileName(0, tr("Open melody file"), QString(), tr("MusicXML file") + QStringLiteral(" (*.xml)"));
	if (!melodyFile.isEmpty()) {
		Tmelody *mel = new Tmelody();
		if (mel->grabFromMusicXml(melodyFile)) {
			m_score->setMelody(mel);
		}
		delete mel;
	}
}


void TmelMan::saveMelodySlot() {
	QString melodyFile = QFileDialog::getSaveFileName(0, tr("Save melody as:"), QString(), tr("MusicXML file") + QStringLiteral(" (*.xml)"));
	if (!melodyFile.isEmpty()) {
		if (melodyFile.right(4) != QLatin1String(".xml"))
        melodyFile += QStringLiteral(".xml");
		Tmelody *mel = new Tmelody();
		m_score->getMelody(mel, QStringLiteral("some melody"));
		mel->saveToMusicXml(melodyFile);
		delete mel;
	}
}

//####################################################################################################
//########################################## PRIVATE #################################################
//####################################################################################################
QAction* TmelMan::createAction(const QString& t, const char* slot, const QKeySequence& k, const QIcon& i) {
	QAction *a = new QAction(this);
	if (!i.isNull())
		a->setIcon(i);
#if !defined (Q_OS_ANDROID)
	if (!k.isEmpty())
		a->setShortcut(k);
#endif
	a->setText(t);
	connect(a, SIGNAL(triggered()), this, slot);
	m_menu->addAction(a);
	return a;
}


void TmelMan::showAudioMark(EscoreState ss) {
  switch (ss) {
    case e_playing:
      m_button->setIcon(QIcon(Tpath::img("melody-play")));
#if defined (Q_OS_ANDROID)
      TmelodyItem::instance()->playingStarted();
#endif
//       m_audioMark->setPixmap(QPixmap(Tpath::img("melody-play")));
//       m_audioMark->show();
      break;
    case e_recording:
      m_button->setIcon(QIcon(Tpath::img("melody-rec")));
#if defined (Q_OS_ANDROID)
      TmelodyItem::instance()->recordingStarted();
#endif
//       m_audioMark->setPixmap(QPixmap(Tpath::img("melody-rec")));
//       m_audioMark->show();
      break;
    default:
//       m_audioMark->hide();
      m_button->setIcon(QIcon(Tpath::img("melody")));
      break;
  }
  if (ss != e_none) // workaround to refresh button icon when tool bar is auto hiding
    m_button->resize(m_button->size());
}







