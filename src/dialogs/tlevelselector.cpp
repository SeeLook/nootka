/***************************************************************************
 *   Copyright (C) 2011-2021 by Tomasz Bojczuk                             *
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
#include <tpath.h>
#include <Android/tfiledialog.h>
#if defined (Q_OS_ANDROID)
  #include <Android/tandroid.h>
#endif

#include <QtCore/qsettings.h>
#include <QtCore/qdiriterator.h>


/*static*/

QString TlevelSelector::checkLevel(const Tlevel& l) {
  QString warningText;
  if (GLOB->instrument().none()) {
      if (l.instrument != Tinstrument::NoInstrument)
        warningText = tr("Level is not suitable for current instrument type");
  } else if (GLOB->instrument().isGuitar()) {
      if (l.canBeInstr() || (l.instrument != Tinstrument::NoInstrument && l.canBeSound())) {
        if (l.hiFret > GLOB->GfretsNumber || GLOB->Gtune()->stringNr() < 3 ||
          l.loNote.chromatic() < GLOB->loString().chromatic() ||
          l.hiNote.chromatic() > GLOB->hiNote().chromatic())
            warningText = tr("Level is not suitable for current tuning and/or fret number");
      }
    } else {
      if (GLOB->instrument().type() != l.instrument)
        warningText = tr("Level is not suitable for current instrument type");
  }
  return warningText;
}

/*end static*/


TlevelSelector::TlevelSelector(QQuickItem* parent) :
  QQuickItem(parent)
{
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
    m_levelPreview->setLevel(nullptr);
}


void TlevelSelector::setCurrentIndex(int ci) {
  m_currentLevelId = ci;
}


void TlevelSelector::findLevels() {
  Tlevel lev;
// from predefined list
  QList<Tlevel> levels;
  getExampleLevels(levels);
  for (int i = 0; i < levels.size(); i++) {
    addLevel(levels[i]);
    // NOTE: Do not check built-in levels
  }

// from files shipped with Nootka installation (levels directory)
  addLevelsFormInstallDir(Tpath::levels()); // general purpose levels
  addLevelsFormInstallDir(Tpath::levels() + QLatin1String("/") + GLOB->instrument().levelsDir()); // instrument specific levels

// from constructor (Master of Masters)
  addLevel(lev);
  m_levels.last().suitable = true; // always suitable

// from setting file - recent load/saved levels
  QStringList recentLevels = GLOB->config->value(QLatin1String("recentLevels")).toStringList();
  for (int i = recentLevels.size() - 1; i >= 0; i--) {
    QFile file(recentLevels[i]);
    if (file.exists()) {
        auto level = getLevelFromFile(file);
        if (!level.name.isEmpty()) {
            addLevel(level, file.fileName());
            checkLast();
        } else
            recentLevels.removeAt(i);
    } else
        recentLevels.removeAt(i);
  }
  GLOB->config->setValue(QLatin1String("recentLevels"), recentLevels);
}


void TlevelSelector::addLevel(const Tlevel& lev, const QString& levelFile, bool check) {
  if (check && !levelFile.isEmpty()) { // check for duplicates
    int pos = -1;
    for (int i = 0; i < m_levels.size(); i++) {
      if (m_levels[i].file == levelFile) // file and level exist
        pos = i;
    }
    if (pos > -1) {
      m_levels.removeAt(pos);
      m_levelsModel.removeAt(pos);
      qDebug() << "[TlevelSelector] Level file" << levelFile << "was already on the list. Previous entry removed!";
    }
  }
  SlevelContener l;
  l.level = lev;
  l.file = levelFile;
  l.suitable = true;
  m_levels << l;
  m_levelsModel << l.level.name;
}


Tlevel* TlevelSelector::currentLevel() {
  return m_currentLevelId >= 0 && m_currentLevelId < m_levels.count() ? &m_levels[m_currentLevelId].level : nullptr;
}


QVariant TlevelSelector::currentLevelVar() {
  return QVariant::fromValue(currentLevel());
}



bool TlevelSelector::isRemovable(int id) const {
  return id >= 0 && id < m_levels.count() ? !m_levels[id].file.isEmpty() : false;
}


void TlevelSelector::showLevel(int id) {
  if (id >= 0 && id < m_levels.count() && m_levelPreview)
    m_levelPreview->setLevel(&m_levels[id].level);
  emit levelChanged();
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
  if (GLOB->E->levelsDir.isEmpty())
    GLOB->E->levelsDir = Tandroid::getExternalPath();
  levelFile = TfileDialog::getOpenFileName(GLOB->E->levelsDir, QStringLiteral("nel"));
#else
    levelFile = TfileDialog::getOpenFileName(tr("Load exam level"), GLOB->examParams->levelsDir, levelFilterTxt() + QLatin1String(" (*.nel)"));
#endif
  QFile file(levelFile);
  Tlevel level = getLevelFromFile(file);
  if (!level.name.isEmpty()) {
    GLOB->examParams->levelsDir = QFileInfo(levelFile).absoluteDir().absolutePath();
    addLevel(level, levelFile, true);
    checkLast();
    updateRecentLevels();
    emit levelsModelChanged();
//     if (m_levels.last().suitable) {
      emit selectLast();
      emit levelChanged();
      m_levelPreview->setLevel(&level);
//     }
  }
}


bool TlevelSelector::removeLevel(int id, bool removeFile) {
  if (id >= 0 && id < m_levels.count()) {
    if (removeFile) {
      QFile levF(m_levels[id].file);
      if (!levF.remove())
        qDebug() << "[TlevelSelector] Can't remove level file" << levF.fileName() ;
    }
    m_levels.removeAt(id);
    m_levelsModel.removeAt(id);
    emit levelsModelChanged();
    if (m_levelPreview)
      m_levelPreview->setLevel(nullptr);
    updateRecentLevels();
    return true;
  }
  return false;
}


void TlevelSelector::updateRecentLevels() {
  QStringList recentLevels;
  for (int i = m_levels.size() - 1; i > 1; i--) {
    if (!m_levels[i].file.isEmpty())
      recentLevels << m_levels[i].file;
  }
  GLOB->config->setValue(QLatin1String("recentLevels"), recentLevels);
}

//##########################################################################################################
//########################################## PRIVATE #######################################################
//##########################################################################################################

Tlevel TlevelSelector::getLevelFromFile(QFile &file) {
  Tlevel level;
  level.name.clear();;
  if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        quint32 lv; // level version identifier
        in >> lv;
        bool wasLevelValid = true;
        bool wasLevelFile = true;
        auto levelVer = Tlevel::levelVersionNr(lv);
        if (levelVer == 1 || levelVer == 2) { // *.nel with binary data
            in.setVersion(QDataStream::Qt_4_7);
            wasLevelValid = getLevelFromStream(in, level, lv);
        } else if (levelVer > 2 && levelVer <= 5) { // *.nel in XML, from version 5 compressed
            Tlevel::EerrorType er;
            QXmlStreamReader xml;
            if (levelVer >= 5) { // compressed level
                in.setVersion(QDataStream::Qt_5_9);
                auto arrayXML = file.readAll();
                arrayXML.remove(0, 4);
                auto unZipXml = qUncompress(arrayXML);
                xml.addData(unZipXml);
            } else { // bare text (XML)
                in.setVersion(QDataStream::Qt_5_2);
                xml.setDevice(in.device());
            }
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
        } else {
            qDebug() << "[TlevelSelector] Level was created with newer Nootka version!";
            GLOB->warnAboutNewerVersion(file.fileName());
            level.name.clear();
            return level;
        }
        file.close();

        if (!wasLevelFile) {
            auto m = QLatin1String("<br><font size=\"4\"><b>")
                + tr("File: %1 \n is not Nootka level file!").arg(file.fileName()).replace(QLatin1String("\n"), QLatin1String("<br>"))
                + QLatin1String("</b></font>");
            emit warnMessage(m, Qt::red);
            level.name.clear();
            return level;
        } else if (!wasLevelValid) {
            auto m = QLatin1String("<br><font size=\"4\"><b>")
            + tr("Level file\n %1 \n was corrupted and repaired!\n Check please, if its parameters are as expected.").arg(file.fileName())
                .replace(QLatin1String("\n"), QLatin1String("<br>")) + QLatin1String("</font>");
            emit warnMessage(m, Qt::yellow);
        }

        if (wasLevelFile) {
          if (level.clef.type() == Tclef::Bass_F_8down) {
            qDebug() << "[TlevelSelector] OBSOLETE bass dropped clef detected. Converting level to ordinary bass clef.";
            level.convFromDropedBass();
          }
        }
  } else {
      if (!file.fileName().isEmpty()) // skip empty file names (ignored by user)
        Tlevel::fileIOerrorMsg(file);
  }
  return level;
}


void TlevelSelector::checkLast() {
  SlevelContener& last = m_levels.last();
  QString notSuitableText = checkLevel(last.level);
  if (notSuitableText.isEmpty())
      m_levels.last().suitable = true;
  else {
      m_levels.last().suitable = false;
      m_levels.last().level.desc = QLatin1String("<font color=\"red\">") + notSuitableText + QLatin1String("</font>");
  }
}


void TlevelSelector::addLevelsFormInstallDir(const QString& dirPath) {
  QDir instrDIrPath(dirPath);
  if (!instrDIrPath.exists())
    return;

  QDirIterator nelFiles(dirPath, QStringList() << QStringLiteral("*.nel"), QDir::Files);
  while (nelFiles.hasNext()) {
    QFile file(nelFiles.next());
    auto level = getLevelFromFile(file);
    if (!level.name.isEmpty()) {
        addLevel(level);
        checkLast();
    } else
        qDebug() << "[TlevelSelector] built-in level is corrupted:\n  =>" << file.fileName();
  }
}





