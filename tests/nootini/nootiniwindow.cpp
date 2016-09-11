/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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

#include "nootiniwindow.h"
#include "nchart.h"
#include "naudioloader.h"
#include "nootinisettings.h"
#include <graphics/tnotepixmap.h>
#include <tinitcorelib.h>
#include <tpath.h>
#include <tpitchfinder.h>
#include <QtWidgets/QtWidgets>
#include <QtSvg/qsvggenerator.h>


static bool    m_drawVolume = true;
static QString m_lastAudioDir;


NootiniWindow::NootiniWindow(const QString& audioFile, QWidget* parent) :
  QMainWindow(parent),
  m_chart(0)
{
  readConfig();
  QFont nf("nootka", 256);
  setWindowIcon(QIcon(pixFromString("m", nf, Qt::darkMagenta)));
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
  m_againAct->setDisabled(true);
  m_toSvgAct = fileMenu->addAction(QIcon(style()->standardIcon(QStyle::SP_DialogSaveButton)), tr("save to SVG"),
                      this, SLOT(saveAsSvg()), QKeySequence::Save);
  m_toSvgAct->setDisabled(true);

  QMenu *chartMenu = menuBar()->addMenu(tr("chart"));
  m_zoomInAct = chartMenu->addAction(QIcon(Tpath::img("zoom-in")), tr("zoom in"),
                      this, SLOT(zoom()), QKeySequence::ZoomIn);
  m_zoomOutAct = chartMenu->addAction(QIcon(Tpath::img("zoom-out")), tr("zoom out"),
                      this, SLOT(zoom()), QKeySequence::ZoomOut);
  m_zoomFitAct = chartMenu->addAction(QIcon(style()->standardIcon(QStyle::SP_ArrowUp)), tr("fit to height"),
                      this, SLOT(fitHeight()), QKeySequence("Ctrl+0"));

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
  QString audioFileName = QFileDialog::getOpenFileName(this, QString(), m_lastAudioDir, tr("WAV file or raw audio (*.wav *.pcm *.raw)"));
  if (!audioFileName.isEmpty()) {
    m_lastAudioDir = QFileInfo(audioFileName).absoluteDir().absolutePath();
    processAudioFile(audioFileName);
  }
}


void NootiniWindow::settingsSlot() {
  NootiniSettings sett(&m_tartiniParams, this);
  sett.setDrawVolumeChart(m_drawVolume);
  sett.setRange(m_loader->pitchRange());
  if (sett.exec() == QDialog::Accepted) {
    m_loader->fillTartiniParams(&m_tartiniParams);
    m_drawVolume = sett.drawVolumeChart();
    NaudioLoader::setPitchRange(sett.range());
  }
}


void NootiniWindow::processAudioFile(const QString& fileName) {
  if (m_loader->setAudioFile(fileName)) {
    delete m_chart;
    m_chart = new Nchart();
    centralWidget()->layout()->addWidget(m_chart);
    setWindowTitle("Nootini - " + QFileInfo(fileName).baseName());
    m_againAct->setDisabled(false);
    m_toSvgAct->setDisabled(false);
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
  m_chart->setDrawVolume(m_drawVolume);
  m_chart->setAudioLoader(m_loader);
  m_loader->startLoading();
}


void NootiniWindow::zoom() {
  qreal coef = sender() == m_zoomOutAct ? 0.9375 : 1.0625;
  m_chart->scale(coef, coef);
}


void NootiniWindow::fitHeight() {
  qreal coef = m_chart->viewport()->height() / (m_chart->scene->sceneRect().height() * m_chart->transform().m11());
  m_chart->scale(coef, coef);
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
    m_tartiniParams.doingHarmonicAnalysis = Tcore::gl()->config->value("doingHarmonicAnalysis", false).toBool();
    m_tartiniParams.doingAutoNoiseFloor = Tcore::gl()->config->value("doingAutoNoiseFloor", true).toBool();
    m_tartiniParams.dBFloor = Tcore::gl()->config->value("dBFloor", -150).toDouble();
    m_drawVolume = Tcore::gl()->config->value("drawVolumeChart", true).toBool();
    NaudioLoader::setPitchRange(Tcore::gl()->config->value("pitchRange", 1).toInt());
    m_lastAudioDir = Tcore::gl()->config->value("lastWavDir", QDir::homePath()).toString();
  Tcore::gl()->config->endGroup();
}


void NootiniWindow::writeConfig() {
  Tcore::gl()->config->beginGroup("Tartini");
    Tcore::gl()->config->setValue("threshold", m_tartiniParams.threshold);
    Tcore::gl()->config->setValue("doingHarmonicAnalysis", m_tartiniParams.doingHarmonicAnalysis);
    Tcore::gl()->config->setValue("doingAutoNoiseFloor", m_tartiniParams.doingAutoNoiseFloor);
    Tcore::gl()->config->setValue("dBFloor", m_tartiniParams.dBFloor);
    Tcore::gl()->config->setValue("drawVolumeChart", m_drawVolume);
    Tcore::gl()->config->setValue("pitchRange", NaudioLoader::pitchRange());
    Tcore::gl()->config->setValue("lastWavDir", m_lastAudioDir);
  Tcore::gl()->config->endGroup();
}


void NootiniWindow::saveAsSvg() {
  QString svgFileName = QFileDialog::getSaveFileName(this, tr("Save chart as SVG image"),
                                                     m_loader->fileName().replace(".wav", "", Qt::CaseInsensitive), tr("SVG image (*.svg)"));
  if (!svgFileName.isEmpty()) {
    QSvgGenerator svgGen;
    svgGen.setFileName(svgFileName);
    svgGen.setSize(m_chart->scene->sceneRect().size().toSize());
    svgGen.setViewBox(m_chart->scene->sceneRect().toRect());
    svgGen.setTitle(tr("Pitch detection chart of") + " " + m_loader->fileName());
    svgGen.setDescription(tr("Analyze of musical pitch changes in PCM data."));
    QPainter painter( &svgGen );
    m_chart->scene->render(&painter);
  }
}



