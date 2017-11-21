/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#include "tlevelselector.h"
#include "tlevelsdefs.h"
#include "tlevelpreviewitem.h"
#include <texamparams.h>
#include <music/ttune.h>
#include <tglobals.h>

#include <QtCore/qsettings.h>
#if defined (Q_OS_ANDROID)
  #include "Android/tfiledialog.h"
#else
  #include <QtWidgets/qfiledialog.h>
#endif





/*static*/

QString TlevelSelector::checkLevel(Tlevel& l) {
  QString warringText;
  if (GLOB->instrument().type() == Tinstrument::NoInstrument && l.instrument != Tinstrument::NoInstrument)
          warringText = tr("Level is not suitable for current instrument type");
  else if (l.canBeGuitar() || (l.instrument != Tinstrument::NoInstrument && l.canBeSound())) {
    if (l.hiFret > GLOB->GfretsNumber || GLOB->Gtune()->stringNr() < 3 ||
        l.loNote.chromatic() < GLOB->loString().chromatic() ||
        l.hiNote.chromatic() > GLOB->hiNote().chromatic())
          warringText = tr("Level is not suitable for current tuning and/or fret number");
  }
  return warringText;
}

/*end static*/


//##########################################################################################################
//########################################## CONSTRUCTOR ###################################################
//##########################################################################################################
TlevelSelector::TlevelSelector(QQuickItem* parent) :
  QQuickItem(parent)
{
  m_fakeLevel.desc.clear();
  m_fakeLevel.name = tr("no level selected");
  m_fakeLevel.loNote = Tnote();
  m_fakeLevel.hiNote = Tnote();
  m_fakeLevel.hiFret = 0;
  m_fakeLevel.questionAs.setAsNote(false);
  m_fakeLevel.questionAs.setAsName(false);
  m_fakeLevel.questionAs.setAsFret(false);
  m_fakeLevel.questionAs.setAsSound(false);
  m_fakeLevel.useKeySign = false;
  m_fakeLevel.withDblAcc = false;
  m_fakeLevel.withFlats = false;
  m_fakeLevel.withSharps = false;
  m_fakeLevel.instrument = Tinstrument::NoInstrument;
  findLevels();
}

TlevelSelector::~TlevelSelector() {
  updateRecentLevels();
}


//##########################################################################################################
//########################################## PUBLIC #######################################################
//##########################################################################################################

void TlevelSelector::setLevelPreview(TlevelPreviewItem* lpi) {
  m_levelPreview = lpi;
  if (m_levelPreview)
    m_levelPreview->setLevel(&m_fakeLevel);
}


void TlevelSelector::findLevels() {
  Tlevel lev = Tlevel();
// from predefined list
  QList<Tlevel> llist;
  getExampleLevels(llist);
  for (int i = 0; i < llist.size(); i++) {
      addLevel(llist[i]);
      m_levels.last().suitable = isSuitable(llist[i]);
  }
// from constructor (Master of Masters)
  addLevel(lev);
  m_levels.last().suitable = true;
// from setting file - recent load/saved levels
  QStringList recentLevels = GLOB->config->value("recentLevels").toStringList();
  for (int i = recentLevels.size() - 1; i >= 0; i--) {
      QFile file(recentLevels[i]);
      if (file.exists()) {
          Tlevel level = getLevelFromFile(file);
          if (!level.name.isEmpty()) {
              addLevel(level, file.fileName());
              m_levels.last().suitable = isSuitable(level);
          } else
              recentLevels.removeAt(i);
      } else
          recentLevels.removeAt(i);
  }
  GLOB->config->setValue("recentLevels", recentLevels);
}


void TlevelSelector::addLevel(const Tlevel& lev, QString levelFile, bool check) {
  if (check && !levelFile.isEmpty()) {
    int pos = -1;
    for (int i = 0; i < m_levels.size(); i++)
      if (m_levels[i].file == levelFile) // file and level exist
          pos = i;
    m_levels.removeAt(pos);
  }
  SlevelContener l;
  l.level = lev;
  l.file = levelFile;
  l.itemId = m_levels.count();
  l.suitable = true;
  m_levels << l;
  m_levelsModel << l.level.name;
}


bool TlevelSelector::isSuitable(Tlevel &l) {
  QString warringText = checkLevel(l);
//   if (!warringText.isEmpty()) {
//       m_levels.last().item->setStatusTip("<span style=\"color: red;\">" + warringText + "</span>");
//       m_levels.last().item->setForeground(QBrush(Qt::red));
//       return false;
//   }
  return true;
}


void TlevelSelector::disableNotSuitable() {
  for (int i = 0; i < m_levels.size(); i++)
    if (!m_levels[i].suitable) {
//       m_levels[i].item->setFlags(Qt::NoItemFlags);
//       m_levels[i].item->setForeground(QBrush(palette().color(QPalette::Disabled, QPalette::Text)));
    }
}


bool TlevelSelector::isSuitable() const {
//   if (idOfSelected() > -1 )
//       return m_levels[idOfSelected()].suitable;
//   else
//       return false;

  return true;
}


bool TlevelSelector::isRemovable(int id) const {
  return id >= 0 && id < m_levels.count() ? !m_levels[id].file.isEmpty() : false;
}


void TlevelSelector::showLevel(int id) {
  if (id >= 0 && id < m_levels.count() && m_levelPreview)
    m_levelPreview->setLevel(&m_levels[id].level);
}


QString TlevelSelector::desc(int id) {
  return id >= 0 && id < m_levels.count() ? m_levels[id].level.desc : QString();
}


bool TlevelSelector::isMelody(int id) {
  return id >= 0 && id < m_levels.count() ? m_levels[id].level.canBeMelody() : false;
}


QString TlevelSelector::levelName(int id) const {
  return id >= 0 && id < m_levels.count() ? m_levels[id].level.name : QString();
}


QString TlevelSelector::levelFile(int id) const {
  return id >= 0 && id < m_levels.count() ? m_levels[id].file : QString();
}


void TlevelSelector::loadFromFile(QString levelFile) {
  if (levelFile.isEmpty())
#if defined (Q_OS_ANDROID)
    levelFile = TfileDialog::getOpenFileName(this, GLOB->E->levelsDir, QStringLiteral("nel"));
#else
    levelFile = QFileDialog::getOpenFileName(nullptr, tr("Load exam level"), GLOB->E->levelsDir, levelFilterTxt() + QLatin1String(" (*.nel)"));
#endif
  QFile file(levelFile);
  Tlevel level = getLevelFromFile(file);
  if (!level.name.isEmpty()) {
      GLOB->E->levelsDir = QFileInfo(levelFile).absoluteDir().absolutePath();
      addLevel(level, levelFile, true);
//       if (isSuitable(level)) // TODO
//           selectLevel(); // select the last
      updateRecentLevels();
      emit levelsModelChanged();
  }
}


bool TlevelSelector::removeLevel(int id, bool removeFile) {

  if (id >= 0 && id < m_levels.count()) {
    if (removeFile) {
      QFile levF(m_levels[id].file);
      if (!levF.remove())
        qDebug() << "Can't remove level file" << levF.fileName() ;
    }
    m_levels.removeAt(id);
    m_levelsModel.removeAt(id);
    emit levelsModelChanged();
    if (m_levelPreview)
      m_levelPreview->setLevel(&m_fakeLevel);
    updateRecentLevels();
    return true;
  }
  return false;
}


Tlevel& TlevelSelector::getSelectedLevel() {
//     if (m_levelsListWdg->currentRow() == -1 ) {
//         return m_fakeLevel;
//     } else
//         return m_levels[m_levelsListWdg->currentRow()].level;
}


void TlevelSelector::updateRecentLevels() {
  QStringList recentLevels;
  for (int i = m_levels.size() - 1; i > 1; i--) {
    if (!m_levels[i].file.isEmpty())
      recentLevels << m_levels[i].file;
  }
  GLOB->config->setValue(QStringLiteral("recentLevels"), recentLevels);
}


//##########################################################################################################
//########################################## PRIVATE #######################################################
//##########################################################################################################

Tlevel TlevelSelector::getLevelFromFile(QFile &file) {
  Tlevel level;
  level.name.clear();;
  if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in.setVersion(QDataStream::Qt_5_2);
        quint32 lv; // level version identifier
        in >> lv;
        bool wasLevelValid = true;
        bool wasLevelFile = true;
        if (Tlevel::levelVersionNr(lv) == 1 || Tlevel::levelVersionNr(lv) == 2) // *.nel with binary data
          wasLevelValid = getLevelFromStream(in, level, lv); // *.nel in XML
        else if (Tlevel::levelVersionNr(lv) == 3) {
          Tlevel::EerrorType er;
          QXmlStreamReader xml(in.device());
          if (!xml.readNextStartElement()) // open first XML node
            er = Tlevel::e_noLevelInXml;
          else 
            er = level.loadFromXml(xml);
          switch (er) {
            case Tlevel::e_levelFixed:
                wasLevelValid = false; break;
            case Tlevel::e_noLevelInXml:
            case Tlevel::e_otherError:
              wasLevelFile = false; break;
            default:
              break;
          }
        } else
            wasLevelFile = false;
        file.close();
//          if (!wasLevelFile) {
//             QMessageBox::critical(this, QString(), tr("File: %1 \n is not Nootka level file!").arg(file.fileName()));
//             level.name.clear();
//             return level;
//          } else if (!wasLevelValid)
//              QMessageBox::warning(0, QString(), tr("Level file\n %1 \n was corrupted and repaired!\n Check please, if its parameters are as expected.").arg(file.fileName()));
  } else {
      if (!file.fileName().isEmpty()) // skip empty file names (ignored by user)
        Tlevel::fileIOerrorMsg(file, nullptr);
  }
  return level;
}





