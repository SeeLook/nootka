/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#include "scorekwindow.h"
#include "pianokeybd.h"
#include <tpath.h>
#include <tmultiscore.h>
#include <tscorescene.h>
#include <music/tmeter.h>
#include <../../src/libs/main/exam/trandmelody.h>
#include <music/tmelody.h>
#include <widgets/tpushbutton.h>
#include <QtWidgets/QtWidgets>


ScorekWindow::ScorekWindow(QWidget* parent) :
  QMainWindow(parent)
{
  setWindowIcon(QIcon(Tpath::img("score")));
  setGeometry(500, 300, 1400, 768);
  m_score = new TmultiScore(this, this);
  auto statusLabel = new QLabel(this);
    statusLabel->setFixedHeight(fontMetrics().boundingRect("A").height() * 3);
    statusLabel->setWordWrap(true);
    statusLabel->setAlignment(Qt::AlignCenter);

  m_piano = new PianoKeybd(this);

  auto lay = new QVBoxLayout;
    lay->addWidget(statusLabel);
    lay->addWidget(m_score);
    lay->addWidget(m_piano);

  auto centralW = new QWidget(this);
    centralW->setLayout(lay);

  setCentralWidget(centralW);

  dblAccidsSlot(true);
  keySignatureSlot(true);

  m_score->scoreScene()->enableAccidsAnimation(true);
  m_score->setMeter(Tmeter(Tmeter::e_2_4));

  auto scoreMenu = new QMenu("Score");
    auto keyAct = scoreMenu->addAction("Key signature");
      keyAct->setCheckable(true);
      keyAct->setChecked(true);
      connect(keyAct, &QAction::toggled, this, &ScorekWindow::keySignatureSlot);
    auto meterAct = scoreMenu->addAction("Time signature");
      meterAct->setCheckable(true);
      meterAct->setChecked(true);
      connect(meterAct, &QAction::toggled, this, &ScorekWindow::meterSlot);
    auto dblAccidsAct = scoreMenu->addAction("Double accidentals");
      dblAccidsAct->setCheckable(true);
      dblAccidsAct->setChecked(true);
      connect(dblAccidsAct, &QAction::toggled, this, &ScorekWindow::dblAccidsSlot);
    auto randMelAct = scoreMenu->addAction(QStringLiteral("Random melody"));
      randMelAct->setShortcut(QKeySequence(QStringLiteral("Ctrl+r")));
      connect(randMelAct, &QAction::triggered, this, &ScorekWindow::randomSlot);

  auto bar = menuBar();
  bar->addMenu(scoreMenu);

  connect(m_score, &TmultiScore::statusTip, [=](const QString& s){ statusLabel->setText(s); });
  TpushButton::setCheckColor(palette().highlight().color(), palette().highlightedText().color());
}


void ScorekWindow::keySignatureSlot(bool keyEnabled) {
  m_score->setEnableKeySign(keyEnabled);
}


void ScorekWindow::meterSlot(bool meterEnabled) {
  if (meterEnabled)
    m_score->setMeter(Tmeter(Tmeter::e_3_4));
  else
    m_score->setMeter(Tmeter(Tmeter::e_none));
}


void ScorekWindow::dblAccidsSlot(bool dblAccidsEnabled) {
  m_score->setEnabledDblAccid(dblAccidsEnabled);
}


void ScorekWindow::randomSlot() {
  bool ok = false;
  int len = QInputDialog::getInt(this, QStringLiteral("Generate random melody"), QStringLiteral("Melody length"), 10, 2, 100, 1, &ok);
  if (ok) {
    QList<TQAgroup> ql;
    int ambit = 25; //highestNote().chromatic() - lowestNote().chromatic();
    for (int i = 0; i < ambit; i++) {
      TQAgroup qa;
      qa.note = Tnote(1 + i);
      ql << qa;
    }
    Tmelody *mel = new Tmelody(QString(), m_score->keySignature());
    mel->setClef(m_score->clef().type());
    getRandomMelody(ql, mel, len, true, true);
//     Trhythm r(m_score->meter()->lower() == 4 ? Trhythm::e_quarter : Trhythm::e_eighth);
    qsrand(QDateTime::currentDateTime().toTime_t());
    for (int n = 0; n < len; ++n) {
      int val = 3 + (qrand() % 3);
      bool rest = static_cast<bool>(qrand() % 2);
      if (rest)
        mel->note(n)->p().note = 0;
      bool dot = false;
      if (val < 5)
        dot = static_cast<bool>(qrand() % 2);
      mel->note(n)->p().setRhythm(Trhythm(static_cast<Trhythm::Erhythm>(val), rest, dot));
      m_score->lastStaff()->addNote(mel->note(n)->p());
    }
    delete mel;
  }
}


void ScorekWindow::resizeEvent(QResizeEvent* event) {
  m_piano->setFixedHeight(height() / 5);
  QMainWindow::resizeEvent(event);
}

