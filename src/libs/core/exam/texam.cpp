/***************************************************************************
 *   Copyright (C) 2011-2020 by Tomasz Bojczuk                             *
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

#include "texam.h"
#include "tlevel.h"
#include "tattempt.h"
#include "tglobals.h"
#include <QtCore/qfile.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qdir.h>
#include <QtCore/qsettings.h>
#include <QtWidgets/qmessagebox.h>

#include <QtCore/qdebug.h>



/*static*/
/** Versions history:
 * 1. 0x95121702;
 *
 * 2. 0x95121704; (2012.07)
 *     - exam stores penalties in the list
 *
 * 3. 0x95121706 (2013.12.02)
 *     - new level version
 *
 * 4. 0x95121708 (2014.09.01)
 *    - encoded XML structure
 *
 * 5. 0x9512170A (05.02.2018)
 *    - new instruments, melodies in Music XML, new mistake types - it can not be compatible with older versions
 */

const qint32 Texam::examVersion = 0x95121702;
const qint32 Texam::examVersion2 = 0x95121704;
const qint32 Texam::currentVersion = 0x9512170A; // version 5

const quint16 Texam::maxAnswerTime = 65500;

int Texam::examVersionNr(qint32 ver) {
  if ((ver - examVersion) % 2)
      return -1; // invalid when rest of division is 1
  return ((ver - examVersion) / 2) + 1 ;
}


bool Texam::couldBeExam(qint32 ver) {
  int givenVersion = examVersionNr(ver);
  if (givenVersion >= 1 && givenVersion <= 127)
    return true;
  else
    return false;
}


bool Texam::isExamVersion(qint32 ver) {
  if (examVersionNr(ver) <= examVersionNr(currentVersion))
    return true;
  else
    return false;
}

/** Obsolete! It has no meaning in XML versions (above 3) */
qint32 Texam::examVersionToLevel(qint32 examVer) {
  if (examVersionNr(examVer) <= 2)
    return Tlevel::getVersionId(1); // level version 1 for exam versions 1 and 2
  else
    return Tlevel::getVersionId(2); // level version 2 for exam versions 3 and so
}


bool Texam::areQuestTheSame(TQAunit* q1, TQAunit* q2) {
  if (q1->questionAs == q2->questionAs && // the same questions
      q1->answerAs == q2->answerAs && // the same answers
      q1->qa.note == q2->qa.note && // the same notes
      q1->qa.pos() == q2->qa.pos() // the same frets
     )
      return true;
  else
      return false;
}


QString Texam::formatReactTime(quint16 timeX10, bool withUnit) {
  QString hh, mm, ss;
  int dig = 0;
  if (timeX10 / 36000) {
      hh = QString("%1").arg(timeX10 / 36000);
      dig = 2;
  }
  int dig2 = 0;
  if ((timeX10 % 36000) / 600) {
      mm = QString("%1").arg((timeX10 % 36000) / 600, dig, 'i', 0, '0');
      dig2 = 2;
  }
  ss = QString("%1").arg(((timeX10 % 36000) % 600) / 10, dig2, 'i', 0, '0' );
  QString res;
  if (!hh.isEmpty())
      res = hh + QLatin1String(":");
  if (!mm.isEmpty())
      res += mm + QLatin1String(":");
  QString unitS;
  if (withUnit && timeX10 < 600)
      unitS = QLatin1String(" s");
  return res + ss + QString(".%1").arg(timeX10 % 10) + unitS;
}

/*end of static*/

//#################################################################################################
//###################             CONSTRUCTOR          ############################################
//#################################################################################################
Texam::Texam(Tlevel* l, const QString& userName):
  m_fileName(QString()),
  m_userName(userName),
  m_totalTime(0),  m_attempts(0),
  m_mistNr(0), m_tmpMist(0),
  m_averReactTime(0), m_workTime(0),
  m_halfMistNr(0), m_tmpHalf(0),
  m_isFinished(false), m_melody(false), m_isExercise(false),
  m_penaltysNr(0),
  m_blackCount(0),
  m_okTime(0),
  m_effectivenes(0.0),
  m_skippedUnit(nullptr)
{
  setLevel(l);
}


Texam::~Texam()
{
  clearAnswList();
  m_blackList.clear();
  m_blackNumbers.clear();
  if (m_skippedUnit)
    delete m_skippedUnit;
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################
void Texam::setExercise() {
  if (count()) {
    qDebug() << "[Texam] Exam has got questions already. Can't set it as an exercise!";
    return;
  }
  setFileName(QDir::toNativeSeparators(QFileInfo(GLOB->config->fileName()).absolutePath() + QLatin1String("/exercise2.noo")));
  m_isExercise = true;
}


void Texam::setLevel(Tlevel* l) {
  m_level = l;
  m_melody = l->canBeMelody();
}


void Texam::setFileName(const QString& fileName) {
  if (isExercise()) {
    qDebug() << "[Texam] Can not set a file name for exercise";
    return;
  }
  m_fileName = fileName;
}


void Texam::skipLast(bool skip) {
  if (skip != (bool)m_skippedUnit) {
    if (skip) {
        if (m_skippedUnit)
          qDebug() << "[Texam] Previously skipped question unit will be overridden by newly skipped.";
        delete m_skippedUnit;
        m_skippedUnit = m_answList.takeLast();
    } else {
        if (!m_skippedUnit)
          qDebug() << "[Texam] There is no skipped unit to revert it back!";
        else {
          m_answList << m_skippedUnit;
          m_skippedUnit = 0;
        }
    }
  }
}


Texam::EerrorType Texam::loadFromFile(const QString& fileName) {
  m_okTime = 0;
  m_tmpMist = 0;
  m_tmpHalf = 0;
  m_fileName = fileName;
  QFile file(fileName);
  m_workTime = 0;
  m_mistNr = 0;
  m_blackCount = 0;
  m_attempts = 0;
  m_isExercise = false;
  m_blackList.clear();
  clearAnswList();
  EerrorType result = e_file_OK;
  quint32 ev; //exam template version
  if (file.open(QIODevice::ReadOnly)) {
      QDataStream in(&file);
      in >> ev;
      if (couldBeExam(ev)) {
        if (!isExamVersion(ev))
            return e_newerVersion;
      }  else
          return e_file_not_valid;

      bool isExamFileOk = true;
      if (examVersionNr(ev) > 3) {
          in.setVersion(QDataStream::Qt_5_2);
          QByteArray arrayXML = file.readAll();
          arrayXML.remove(0, 4);
          QByteArray unZipXml = qUncompress(arrayXML);
          if (!unZipXml.isEmpty()) {
              QXmlStreamReader xml(unZipXml);
              isExamFileOk = loadFromXml(xml);
          } else {
              qDebug() << "[Texam] Problems with decompressing exam file";
              return e_file_not_valid;
          }
      } else {
          in.setVersion(QDataStream::Qt_4_7);
          isExamFileOk = loadFromBin(in, ev);
      }

      m_melody = m_level->canBeMelody();
      updateEffectiveness();
      updateAverageReactTime(true);

      if (!isExamFileOk)
          result = e_file_corrupted;
      file.close();
  } else {
      Tlevel::fileIOerrorMsg(file);
      result = e_cant_open;
  }
  updateBlackCount();

  if (m_level->clef.type() == Tclef::Bass_F_8down) {
    qDebug() << "[Texam] OBSOLETE bass dropped clef detected. Converting exam to ordinary bass clef.";
    transposeAfterBassDropped();
  }

  return result;
}


bool Texam::loadFromBin(QDataStream& in, quint32 ev) {
  quint16 questNr;
  bool isExamFileOk = true;
  in >> m_userName;
  getLevelFromStream(in, *(m_level), examVersionToLevel(ev));
  in >> m_tune;
  in >> m_totalTime;
  in >> questNr >> m_averReactTime >> m_mistNr;
  if (examVersionNr(ev) >= 2) {
    in >> m_halfMistNr >> m_penaltysNr >> m_isFinished;
  } else { // exam version 1
      m_halfMistNr = 0;
      m_penaltysNr = 0;
      m_isFinished = false;
  }
  while (!in.atEnd()) {
    TQAunit qaUnit;
    if (!getTQAunitFromStream(in, qaUnit))
        isExamFileOk = false;
    if ((qaUnit.questionAs == TQAtype::e_asName || qaUnit.answerAs == TQAtype::e_asName)
      && qaUnit.styleOfQuestion() < 0) {
        qaUnit.setStyle(static_cast<Tnote::EnameStyle>(GLOB->noteNameStyle()), qaUnit.styleOfAnswer());
    } /** In old versions, style was set to 0 so now it gives styleOfQuestion = -1
      * Also in transition Nootka versions it was left unchanged.
      * Unfixed it invokes stupid names in charts.
      * We are fixing it by insert user preferred style of naming */
    if (qaUnit.time <= maxAnswerTime || ev == examVersion) { // add to m_answList
        m_answList << new TQAunit(qaUnit);
        grabFromLastUnit();
    } else { // add to m_blackList
        m_blackList << qaUnit;
    }
  }
  if (!checkQuestionNumber(questNr)) {
    isExamFileOk = false;
  }
  if (examVersionNr(ev) >= 2 && (m_tmpMist != m_mistNr || m_tmpHalf != m_halfMistNr)) {
      m_mistNr = m_tmpMist; // we try to fix exam file to give proper number of mistakes
      m_halfMistNr = m_tmpHalf;
      isExamFileOk = false;
  } else {
      m_mistNr = m_tmpMist; // transition to exam version 2
  }
  if (ev == examVersion) {
      convertToVersion2();
      m_halfMistNr = m_tmpHalf;
  }
  return isExamFileOk;
}



bool Texam::loadFromXml(QXmlStreamReader& xml) {
  bool ok = true;
  int questNr = 0;
  if (xml.readNextStartElement()) {
    if (xml.name() != QLatin1String("exam")) {
      qDebug() << "[Texam] There is no 'exam' key in that XML";
      return false;
    }
    m_userName = xml.attributes().value(QStringLiteral("user")).toString();
    if (m_userName.isEmpty() || m_userName.size() > 30) {
      qDebug() << "[Texam] Exam has wrong user name";
      return false;
    }
  }
  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("head")) {
        while (xml.readNextStartElement()) {
          if (xml.name() == QLatin1String("level")) {
              auto err = m_level->loadFromXml(xml);
              if (err != Tlevel::e_level_OK) {
                qDebug() << "[Texam] Exam has wrong level definition" << static_cast<int>(err);
                ok = false;
              }
          } else if (xml.name() == QLatin1String("tuning")) {
              if (!m_tune.fromXml(xml, true)) {
                qDebug() << "[Texam] Exam has wrong tuning";
                ok = false;
              }
          } else if (xml.name() == QLatin1String("totalTime"))
              m_totalTime = xml.readElementText().toInt();
          else if (xml.name() == QLatin1String("questNr"))
              questNr = xml.readElementText().toInt();
          else if (xml.name() == QLatin1String("averReactTime"))
              m_averReactTime = xml.readElementText().toInt();
          else if (xml.name() == QLatin1String("mistNr"))
              m_mistNr = xml.readElementText().toInt();
          else if (xml.name() == QLatin1String("halfMistNr"))
              m_halfMistNr = xml.readElementText().toInt();
          else if (xml.name() == QLatin1String("penaltysNr"))
              m_penaltysNr = xml.readElementText().toInt();
          else if (xml.name() == QLatin1String("finished"))
              m_isFinished = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("exercise")) {
              m_isExercise = true;
              xml.skipCurrentElement();
          } else
              Tlevel::skipCurrentXmlKey(xml);
        }
    } else if (xml.name() == QLatin1String("answers")) {
        if (!readAnswerFromXml(m_answList, xml))
          ok = false;
    } else if (xml.name() == QLatin1String("penalties")) {
        if (!readPenaltyFromXml(m_blackList, xml))
            ok = false;
    } else if (xml.name() == QLatin1String("black")) {
        m_blackNumbers.clear();
        while (xml.readNextStartElement()) {
          if (xml.name() == "n")
            m_blackNumbers << xml.readElementText().toInt();
          else
            Tlevel::skipCurrentXmlKey(xml);
        }
    } else
        Tlevel::skipCurrentXmlKey(xml);
  }
  if (!checkQuestionNumber(questNr)) {
    ok = false;
  }
  if (m_tmpMist != m_mistNr || m_tmpHalf != m_halfMistNr) {
    if (m_tmpMist != m_mistNr)
      qDebug() << "[Texam] Mistakes number do not match. Exam file corrupted!" << m_tmpMist << m_mistNr;
    else if (m_tmpHalf != m_halfMistNr)
      qDebug() << "[Texam] 'Not bad' number do not match. Exam file corrupted!" << m_tmpHalf << m_halfMistNr;
    m_mistNr = m_tmpMist; // we try to fix exam file to give proper number of mistakes
    m_halfMistNr = m_tmpHalf;
    ok = false;
  }
  return ok;
}


Texam::EerrorType Texam::saveToFile(const QString& fileName) {
  if (!fileName.isEmpty())
    setFileName(fileName); // m_fileName becomes fileName
  if (m_fileName.isEmpty())
    return e_noFileName;

  QFile file(m_fileName);
  if (file.open(QIODevice::WriteOnly)) {
      QDataStream out(&file);
      out.setVersion(QDataStream::Qt_5_2);
      out << currentVersion;
      QByteArray arrayXML;
      QXmlStreamWriter xml(&arrayXML);
  //     xml.setAutoFormatting(true);
      xml.writeStartDocument();
      xml.writeComment("\nXML file of Nootka exam data.\nhttps://nootka.sf.net\nThis file should never be opened in other software then Nootka.\nProbably you are doing something illegal!");
      writeToXml(xml);
      xml.writeEndDocument();

      out << qCompress(arrayXML);
  //     out << arrayXML;

      file.close();
  } else {
      QMessageBox::critical(nullptr, QString(), QObject::tr("Cannot save exam file:\n%1").arg(QString::fromLocal8Bit(qPrintable(file.errorString()))));
      return e_cant_open;
  }
  qDebug() << "[Texam] Exam saved to:" << m_fileName;
  return e_file_OK;
}


void Texam::writeToXml(QXmlStreamWriter& xml) {
  xml.writeStartElement(QStringLiteral("exam"));
    xml.writeAttribute(QStringLiteral("user"), m_userName);
    xml.writeStartElement(QStringLiteral("head"));
      m_level->writeToXml(xml);
      m_tune.toXml(xml, true);
      xml.writeTextElement(QStringLiteral("totalTime"), QVariant(m_totalTime).toString());
      xml.writeTextElement(QStringLiteral("questNr"), QVariant(count()).toString());
      xml.writeTextElement(QStringLiteral("averReactTime"), QVariant(m_averReactTime).toString());
      xml.writeTextElement(QStringLiteral("mistNr"), QVariant(m_mistNr).toString());
      xml.writeTextElement(QStringLiteral("halfMistNr"), QVariant(m_halfMistNr).toString());
      xml.writeTextElement(QStringLiteral("penaltysNr"), QVariant(m_penaltysNr).toString());
      xml.writeTextElement(QStringLiteral("finished"), QVariant(m_isFinished).toString());
      if (isExercise())
        xml.writeEmptyElement(QStringLiteral("exercise"));
    xml.writeEndElement(); // head
    xml.writeStartElement(QStringLiteral("answers"));
    for (int i = 0; i < count(); ++i)
      m_answList[i]->toXml(xml);
    xml.writeEndElement(); // answers
    if (m_blackList.size()) {
        xml.writeStartElement(QStringLiteral("penalties"));
        for (int i = 0; i < m_blackList.size(); ++i)
          m_blackList[i].toXml(xml);
        xml.writeEndElement(); // penalties
    } else if (m_blackNumbers.size()) {
        xml.writeStartElement(QStringLiteral("black"));
        for (int i = 0; i < m_blackNumbers.size(); ++i)
          xml.writeTextElement(QStringLiteral("n"), QString::number(m_blackNumbers[i]));
        xml.writeEndElement(); // penalties
    }
  xml.writeEndElement(); // exam
}


void Texam::newAttempt() {
  curQ()->newAttempt();
  if (curQ()->attemptsCount() > 1) { // unset answered and revert mistakes - user tries once more
    if (curQ()->isNotSoBad())
      m_halfMistNr--;
    else if (curQ()->isWrong())
      m_mistNr--;
    else
      qDebug() << "[Texam] A new attempt called for correct answer!";
    curQ()->unsetAnswered();
  }
}


void Texam::sumarizeAnswer() {
  curQ()->updateEffectiveness();
  curQ()->time = qMin(maxAnswerTime, curQ()->time); // when user think too much
  if (melodies()) {
    m_workTime += curQ()->lastAttempt()->totalTime();
    if (!curQ()->isWrong()) {
        if (curQ()->effectiveness() < 50)
          curQ()->setMistake(TQAunit::e_veryPoor);
        else if (curQ()->effectiveness() < 70)
          curQ()->setMistake(TQAunit::e_poorEffect);
    }
    m_attempts++;
  }
  updateAverageReactTime(true);
  if (melodies()) {
    if (curQ()->isNotSoBad())
      m_halfMistNr++;
    else if (curQ()->isWrong())
      m_mistNr++;
  } else {
    addPenalties(); // for melodies it should be invoked after ensuring that answer was finished
    if (!isExercise())
      updateBlackCount();
    m_workTime += curQ()->time;
  }
  updateEffectiveness();
}


void Texam::addPenalties() {
  if (!curQ()->isCorrect()) {
    if (melodies()) // for any kind of mistake add one more random melody or mistaken one if not random and melody set
      m_blackNumbers.append(m_level->isMelodySet() && !m_level->randOrderInSet ? count() - 1 : -1);
    if (curQ()->isNotSoBad()) {
        if (!isExercise() && !isFinished())
          m_penaltysNr++;
        if (!melodies())
          m_halfMistNr++;
    } else {
        if (melodies())
          m_blackNumbers.append(count() - 1); // repeat current melody in some further question
        if (!isExercise() && !isFinished())
          m_penaltysNr += 2;
        if (!melodies())
          m_mistNr++;
    }
  }
}


void Texam::updateEffectiveness() {
  qreal sum = 0.0;
  for (int i = 0; i < count(); ++i)
    sum += answList()->at(i)->effectiveness();
  m_effectivenes = sum / (qreal)count();
}


void Texam::updateAverageReactTime(bool skipWrong) {
  int totalTime = 0;
  int cnt = 0;
  for (int i = 0; i < count(); ++i) {
    if (!skipWrong || (skipWrong && !m_answList[i]->isWrong())) {
      totalTime += m_answList[i]->time;
      cnt++;
    }
  }
  if (cnt)
    m_averReactTime = totalTime / cnt;
  else
    m_averReactTime = 0;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void Texam::updateBlackCount() {
  m_blackCount = 0;
  if (m_blackList.size()) {
    for (int i = 0; i < m_blackList.size(); i++)
      m_blackCount += (m_blackList[i].time - maxAnswerTime);
  }
}


bool Texam::readPenaltyFromXml(QList<TQAunit>& blackList, QXmlStreamReader& xml) {
  bool ok = true;
  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("u")) {
        blackList << TQAunit(this);
        if (!blackList.last().fromXml(xml)) {
          qDebug() << "[Texam] Exam has wrong unit" << blackList.size();
          blackList.removeLast();
          ok = false;
        }
    } else
        Tlevel::skipCurrentXmlKey(xml);
  }
  return ok;
}


bool Texam::readAnswerFromXml(QList<TQAunit*>& list, QXmlStreamReader& xml) {
  bool ok = true;
  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("u")) {
      list << new TQAunit(this);
      if (list.last()->fromXml(xml)) {
          grabFromLastUnit();
          if (melodies())
            m_attempts += curQ()->attemptsCount();
      } else {
          qDebug() << "[Texam] Exam has wrong unit" << list.size();
          list.removeLast();
          ok = false;
      }
    } else
        Tlevel::skipCurrentXmlKey(xml);
  }
  return ok;
}


void Texam::grabFromLastUnit() {
  m_workTime += curQ()->time;
  if (!curQ()->isCorrect()) {
    if (curQ()->isWrong())
      m_tmpMist++;
    else
      m_tmpHalf++; // not so bad answer
    }
  if (!curQ()->isWrong())
    m_okTime += curQ()->time;
}


bool Texam::checkQuestionNumber(int questNr) {
  bool ok = true;
  if (questNr != m_answList.size()) {
    qDebug() << "[Texam] Exam questions number read from file" << questNr
             << "and those calculated" << m_answList.size() << "do not match. Exam file corrupted.";
    ok = false;
  }
  return ok;
}


void Texam::clearAnswList() {
  for (int i = 0; i < m_answList.size(); ++i)
    delete m_answList[i];
  m_answList.clear();
}


void Texam::transposeAfterBassDropped() {
  if (m_tune.type() == Ttune::Custom)
    m_tune.riseOctaveUp();
  m_level->convFromDropedBass();
  for (int a = 0; a < m_answList.size(); ++a)
    m_answList[a]->riseOctaveUp();
  for (int b = 0; b < m_blackList.size(); ++b)
    m_blackList[b].riseOctaveUp();
}


/**
 * This method exist for backward compatibility but is has very rare usage in 'modern' Nootka times
 */
void Texam::convertToVersion2() {
  bool hasStyle = false;
  Tnote::EnameStyle randStyles[3];
  if (m_level->canBeName()) {
    // version 1 didn't put proper Tnote::EnameStyle to file - we fixing it
    hasStyle = true;
    qDebug() << "[Texam] Fixing styles of note names in file";
    qsrand(QDateTime::currentDateTimeUtc().toTime_t());
   if (m_level->requireStyle) { // prepare styles array to imitate switching
      randStyles[0] = Tnote::e_italiano_Si;
      if (GLOB->seventhIsB()) {
          randStyles[1] = Tnote::e_english_Bb;
          randStyles[2] = Tnote::e_nederl_Bis;
      } else {
          randStyles[1] = Tnote::e_norsk_Hb;
          randStyles[2] = Tnote::e_deutsch_His;
      }
   }
  }

  for (int i = 0; i < m_answList.size(); i++) {
    if (m_answList[i]->time > maxAnswerTime) // fix too long times from version 1 if any
        m_answList[i]->time = maxAnswerTime;
  // version 1 didn't put proper Tnote::EnameStyle to file - we fixing it
    if (hasStyle) {
      if (m_level->requireStyle) {
          if (m_answList[i]->questionAs == TQAtype::e_asName && m_answList[i]->answerAs == TQAtype::e_asName) {
            Tnote::EnameStyle qSt = randStyles[qrand() % 3];
            Tnote::EnameStyle aSt;
            if (qSt == Tnote::e_italiano_Si)
              aSt = randStyles[(qrand() % 2) +1];
            else
              aSt = Tnote::e_italiano_Si;
            m_answList[i]->setStyle(qSt, aSt);
          } else
            if (m_answList[i]->questionAs == TQAtype::e_asName) {
                m_answList[i]->setStyle(randStyles[qrand() % 3], static_cast<Tnote::EnameStyle>(GLOB->noteNameStyle()));
            } else
                if (m_answList[i]->questionAs == TQAtype::e_asName) {
                  m_answList[i]->setStyle(static_cast<Tnote::EnameStyle>(GLOB->noteNameStyle()), randStyles[qrand() % 3]);
                }
      } else // fixed style - we changing to user preferred
          m_answList[i]->setStyle(static_cast<Tnote::EnameStyle>(GLOB->noteNameStyle()), static_cast<Tnote::EnameStyle>(GLOB->noteNameStyle()));
    }

    if (!m_answList[i]->isCorrect()) {
      quint16 penCnt = 0; // counts of penalties
      if (m_answList[i]->isWrong()) {
          if (i < (m_answList.size() -1) && areQuestTheSame(m_answList[i], m_answList[i+1])) {
            // there was next question repeated
            if (m_answList[i+1]->isCorrect()) // and was correct
              penCnt = 65501; // so add one penalty
            else // when again wrong
              penCnt = 65502; // add two
            // The next loop will add next two penalties !!
          } else // question was not repeated
              penCnt = 65502;
      } else { // not so bad
          if (i < (m_answList.size() -1) && areQuestTheSame(m_answList[i], m_answList[i+1])) {
            // there was next question repeated
            if (m_answList[i+1]->isCorrect()) // and was correct
              penCnt = 0;
            else
              penCnt = 65501;
          }
      }
      if (penCnt) {
        m_blackList << *m_answList[i];
        m_blackList.last().time = penCnt;
        m_penaltysNr += (penCnt - 65500);
      }
    }
  }
}



