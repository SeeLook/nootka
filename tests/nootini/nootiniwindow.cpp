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
#include <tinitcorelib.h>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QtWidgets>

bool m_nootInd = false;
qreal m_minVolToSplit = 0.0;
NootiniWindow::NootiniWindow(const QString& audioFile, QWidget* parent) :
  QMainWindow(parent),
  m_chart(0)
{
  readConfig();
  QFont nf("nootka", 256);
  setWindowIcon(QIcon(pixFromString("m", nf)));
  resize(800, 600);
  QWidget* innerWidget = new QWidget(this);

  QVBoxLayout* lay = new QVBoxLayout;
  innerWidget->setLayout(lay);
  setCentralWidget(innerWidget);

  QMenu *fileMenu = menuBar()->addMenu(tr("file"));
  m_openAct = fileMenu->addAction(QIcon(style()->standardIcon(QStyle::SP_DirOpenIcon)), tr("open audio file"),
                                  this, SLOT(openFileSlot()), QKeySequence::Open);
  m_settAct = fileMenu->addAction(QIcon(style()->standardIcon(QStyle::QStyle::SP_DialogApplyButton)), tr("settings"),
                                  this, SLOT(settingsSlot()), QKeySequence("Ctrl+E"));
  m_againAct = fileMenu->addAction(QIcon(style()->standardIcon(QStyle::SP_MediaPlay)), tr("process again"),
                      this, SLOT(processAgain()), QKeySequence("Ctrl+space"));

//   QMenu *chartMenu = menuBar()->addMenu(tr("chart"));
//   chartMenu->addAction(tr("process again"));

  m_loader = new NaudioLoader();

  if (audioFile.isEmpty()) {
    m_chart = new Nchart();
    lay->addWidget(m_chart);
  } else
    processAudioFile(audioFile);
}


NootiniWindow::~NootiniWindow()
{
  delete m_loader;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void NootiniWindow::openFileSlot() {
  QString wavFileName = QFileDialog::getOpenFileName(this, "", QDir::homePath(), tr("WAV file (*.wav)"));
  if (!wavFileName.isEmpty())
    processAudioFile(wavFileName);
}


void NootiniWindow::settingsSlot() {
  NootiniSettings sett(&m_tartiniParams, this);
  sett.setNootkaIndexing(m_nootInd);
  sett.setMinVolToSplit(m_minVolToSplit);
  if (sett.exec() == QDialog::Accepted) {
    m_loader->fillTartiniParams(&m_tartiniParams);
    m_nootInd = sett.nootkaIndexing();
    m_minVolToSplit = sett.minVolToSplit();
  }
}


void NootiniWindow::processAudioFile(const QString& fileName) {
  if (m_loader->setAudioFile(fileName)) {
    delete m_chart;
    m_chart = new Nchart();
    centralWidget()->layout()->addWidget(m_chart);
    setWindowTitle("Nootini - " + fileName);
    startProcessing();
  }
}


void NootiniWindow::processAgain() {
  QString fileName = m_loader->fileName();
  delete m_loader;
  m_loader = new NaudioLoader();
  if (m_loader->setAudioFile(fileName))
    startProcessing();
}


void NootiniWindow::startProcessing() {
  m_loader->fillTartiniParams(&m_tartiniParams);
  m_chart->setAudioLoader(m_loader);
  m_chart->setNootkaIndexing(m_nootInd);
  m_chart->setMinVolToSplit(m_minVolToSplit);
  m_loader->startLoading();
  m_chart->drawChunk();
}


void NootiniWindow::closeEvent(QCloseEvent* e) {
  writeConfig();
  QWidget::closeEvent(e);
}


void NootiniWindow::resizeEvent(QResizeEvent* e) {
  if (e->oldSize().height() > 0) {
    double coef = ((double)e->size().height() / (double)e->oldSize().height());
    m_chart->scale(coef, coef);
  }
  QWidget::resizeEvent(e);
}


void NootiniWindow::readConfig() {
  Tcore::gl()->config->beginGroup("Tartini");
    m_tartiniParams.threshold = Tcore::gl()->config->value("threshold", 93).toInt();
    m_tartiniParams.equalLoudness = Tcore::gl()->config->value("equalLoudness", true).toBool();
    m_tartiniParams.doingHarmonicAnalysis = Tcore::gl()->config->value("doingHarmonicAnalysis", false).toBool();
    m_tartiniParams.dBFloor = Tcore::gl()->config->value("dBFloor", -150).toDouble();
    m_nootInd = Tcore::gl()->config->value("nootkaIndexing", false).toBool();
    m_minVolToSplit = Tcore::gl()->config->value("minVolumeToSplit", 0.0).toReal();
  Tcore::gl()->config->endGroup();
}


void NootiniWindow::writeConfig() {
  Tcore::gl()->config->beginGroup("Tartini");
    Tcore::gl()->config->setValue("threshold", m_tartiniParams.threshold);
    Tcore::gl()->config->setValue("equalLoudness", m_tartiniParams.equalLoudness);
    Tcore::gl()->config->setValue("doingHarmonicAnalysis", m_tartiniParams.doingHarmonicAnalysis);
    Tcore::gl()->config->setValue("nootkaIndexing", m_nootInd);
    Tcore::gl()->config->setValue("minVolumeToSplit", m_minVolToSplit);
    Tcore::gl()->config->setValue("dBFloor", m_tartiniParams.dBFloor);
  Tcore::gl()->config->endGroup();
}


