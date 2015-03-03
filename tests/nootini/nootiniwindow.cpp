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

#include "nootiniwindow.h"
#include "nchart.h"
#include "naudioloader.h"
#include "nootinisettings.h"
#include <graphics/tnotepixmap.h>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QtWidgets>


NootiniWindow::NootiniWindow(const QString& audioFile, QWidget* parent) :
  QMainWindow(parent)
{
  setWindowIcon(QIcon(glyphToPixmap("n", 64)));
  resize(800, 600);
  QWidget* innerWidget = new QWidget(this);

  m_chart = new Nchart();

  QVBoxLayout* lay = new QVBoxLayout;
  lay->addWidget(m_chart);
  innerWidget->setLayout(lay);
  setCentralWidget(innerWidget);

  QMenu *fileMenu = menuBar()->addMenu(tr("file"));
  m_openAct = fileMenu->addAction(QIcon(style()->standardIcon(QStyle::SP_DirOpenIcon)), tr("open audio file"),
                                  this, SLOT(openFileSlot()), QKeySequence::Open);
  m_settAct = fileMenu->addAction(QIcon(style()->standardIcon(QStyle::QStyle::SP_DialogApplyButton)), tr("settings"),
                                  this, SLOT(settingsSlot()));

  m_loader = new NaudioLoader();

  if (!audioFile.isEmpty())
    processAudioFile(audioFile);
}


NootiniWindow::~NootiniWindow()
{
  delete m_loader;
}


void NootiniWindow::openFileSlot() {
  QString wavFileName = QFileDialog::getOpenFileName(this, "", QDir::homePath(), tr("WAV file (*.wav)"));
  if (!wavFileName.isEmpty())
    processAudioFile(wavFileName);
}


void NootiniWindow::settingsSlot() {
  NootiniSettings sett(this);
  if (sett.exec() == QDialog::Accepted) {
    qDebug() << "Accepted";
  }
}



void NootiniWindow::processAudioFile(const QString& fileName) {
  if (m_loader->setAudioFile(fileName)) {
    m_chart->setPitchFinder(m_loader->finder());
    m_chart->setXnumber(m_loader->totalChunks() + 1);
//     connect(m_loader, &NaudioLoader::processingFinished, m_chart, &Nchart::allDataLoaded);
    m_loader->startLoading();
  }
}





