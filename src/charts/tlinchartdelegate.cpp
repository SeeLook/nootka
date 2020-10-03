/***************************************************************************
 *   Copyright (C) 2019-2020 by Tomasz Bojczuk                             *
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

#include "tlinchartdelegate.h"
#include "tchartitem.h"
#include "sorting.h"
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/textrans.h>
#include <exam/tattempt.h>
#include <music/tnamestylefilter.h>
#include <tglobals.h>
#include <tnoofont.h>

#include <QtGui/qpalette.h>
#include <QtCore/qdebug.h>


/**
 *    (0.1)            Y                 |
 *    |                |                 |
 *    |                |                 |
 *    | h (0.75)       |                 |
 *    |                |                 | total delegate height
 *    |                |                 |
 *    (0.8333)         ---------- X      |
 *                                       |
 */

#define HH ((0.8333333333333333 - 0.1))


TlinChartDelegate::TlinChartDelegate(QQuickItem* parent) :
  QQuickPaintedItem(parent)
{
//   setRenderTarget(QQuickPaintedItem::FramebufferObject);
  setAntialiasing(true);
  setAcceptHoverEvents(true);

  m_qInf = new TtipInfo();

  connect(qApp, &QGuiApplication::paletteChanged, this, [=]{ update(); });
  connect(this, &QQuickItem::heightChanged, this, [=]{
    calcProgressRangeY();
    emit pointYChanged();
  });
}


TlinChartDelegate::~TlinChartDelegate()
{
  delete m_qInf;
}


qreal TlinChartDelegate::pointY() const {
  if (m_qInf->qaUnit()) {
    qreal h = height() * HH;
    if (m_chart->settings()->yValue == Tchart::e_YquestionTime)
      return height() * 0.1 + h - ((static_cast<qreal>(m_qInf->qaUnit()->time) / 10.0) / m_chart->mainChart()->maxValue()) * h;
    if (m_chart->settings()->yValue == Tchart::e_Yeffectiveness)
      return height() * 0.1 + h - (m_qInf->qaUnit()->effectiveness() / m_chart->mainChart()->maxValue()) * h;
    if (m_chart->settings()->yValue == Tchart::e_YprepareTime && m_chart->exam()->melodies())
      return height() * 0.1 + h - ((static_cast<qreal>(m_qInf->qaUnit()->attempt(0)->prepareTime()) / 10.0) / m_chart->mainChart()->maxValue()) * h;
    if (m_chart->settings()->yValue == Tchart::e_YattemptsCount)
      return height() * 0.1 + h - (static_cast<qreal>(m_qInf->qaUnit()->attemptsCount()) / m_chart->mainChart()->maxValue()) * h;
    if (m_chart->settings()->yValue == Tchart::e_YplayedCount)
      return height() * 0.1 + h - (static_cast<qreal>(m_qInf->qaUnit()->totalPlayBacks()) / m_chart->mainChart()->maxValue()) * h;
  }

  return 0.0;
}


void TlinChartDelegate::setChart(TchartItem* ch) {
  if (!m_chart) {
    m_chart = ch;
    emit examChanged();
    connect(m_chart, &TchartItem::examChanged, this, [=]{
        if (m_chart->ignoreSignalExamChanged())
          return;
        if (m_nrInchart > -1) { // reset @p qaPtr associated with this delegate
          int tmpNr = m_nrInchart;
          m_nrInchart = -1;
          setChartNr(tmpNr);
        }
        emit examChanged();
        emit pointYChanged();
        update();
    });
  }
}


void TlinChartDelegate::setChartNr(int n) {
  if (m_nrInchart != n) {
    if (n > -1 && n < m_chart->exam()->count()) {
        m_nrInchart = n;
        if (m_chart->settings()->order == Tchart::e_byNumber) {
            m_qInf->setQAUnit(getUnit(n));
            m_qInf->setNr(n + 1);
            calcProgressRangeY();
        } else {
            auto qaPtr = m_chart->mainChart()->getSortedPtr(n);
            if (qaPtr) {
              m_qInf->setQAUnit(qaPtr->qaPtr);
              m_qInf->setNr(qaPtr->nr + 1);
              m_qInf->setColor(qaPtr->color);
              m_qInf->setGr(qaPtr->grNr);
            }
        }
        if (m_qInf->qaUnit()->isCorrect())
          m_qInf->setColor(GLOB->correctColor());
        else if (m_qInf->qaUnit()->isNotSoBad())
          m_qInf->setColor(GLOB->notBadColor());
        else
          m_qInf->setColor(GLOB->wrongColor());
        emit nrChanged();
    }
  }
}


int TlinChartDelegate::groupNr() const { return m_qInf->grNr(); }

QColor TlinChartDelegate::pointColor() const { return m_qInf->color(); }


/**
 * Nootka glyphs:
 * 'm' - for melodies, 'M' - sixteenth upside down (wrongs), 'g' - just sixteenth
 */
QString TlinChartDelegate::pointSymbol() const {
  return m_qInf->qaUnit() && m_qInf->qaUnit()->melody() ? QStringLiteral("m") :
                                        (m_qInf->qaUnit() && m_qInf->qaUnit()->isWrong() ? QStringLiteral("M") : QStringLiteral("G"));
}


qreal TlinChartDelegate::averageY() const {
  if (m_chart->settings()->yValue == Tchart::e_YquestionTime) {
    if (m_chart->settings()->order == Tchart::e_byNumber)
      return m_chart->averageTime();
    else if (m_qInf->grNr() > -1) {
      auto gr = m_chart->mainChart()->group(m_qInf->grNr());
      if (gr && gr->size() > 1 && !gr->description().isEmpty()) // empty description means unrelated - average makes no sense
        return gr->averTime() / 10.0;
    }
  }
  return 0.0;
}


QString TlinChartDelegate::nrText() const {
  QString att;
  if (m_qInf->qaUnit() && m_qInf->qaUnit()->melody())
    att = QLatin1String("<br><font size=\"1\">(")
        + QGuiApplication::translate("Texam", "%n attempt(s)", "", m_qInf->qaUnit()->attemptsCount()) + QLatin1String(")</font>");
  return m_chart->settings()->order == Tchart::e_byNumber ? QString("<big><b>%1</b></big>").arg(m_nrInchart + 1) + att : QString();
}


QString TlinChartDelegate::noteText() const {
  if (!m_qInf->qaUnit() || m_qInf->qaUnit()->melody())
    return QString();

  QString altStyleText;
  auto l = m_chart->exam()->level();
  if (l->requireStyle || (l->questionAs.isName() && l->answersAs[TQAtype::e_asName].isName())) {
    /** Display alternate to user pref names but only for levels where different styles can occur */
    Tnote::EnameStyle altStyle;
    if (Tnote::defaultStyle == Tnote::e_italiano_Si || Tnote::defaultStyle == Tnote::e_russian_Ci)
      altStyle = TnameStyleFilter::get(Tnote::e_english_Bb);
    else
      altStyle = TnameStyleFilter::get(Tnote::e_italiano_Si);
    Tnote::EnameStyle defStyle = Tnote::defaultStyle;
    Tnote::defaultStyle = altStyle;
    altStyleText = QString(" <font size=\"1\">(%1)</font>").arg(m_qInf->qaUnit()->qa.note.styledName(false));
    Tnote::defaultStyle = defStyle;
  }
  return m_qInf->qaUnit()->qa.note.styledName() + altStyleText;
}


QString TlinChartDelegate::posText() const {
  if (m_qInf->qaUnit() && m_qInf->qaUnit()->exam()
      && Tinstrument(m_qInf->qaUnit()->exam()->level()->instrument).isGuitar()
      && !m_qInf->qaUnit()->melody()) {
    if (m_qInf->qaUnit()->questionAs == TQAtype::e_onInstr
      || m_qInf->qaUnit()->answerAs == TQAtype::e_onInstr
      || m_qInf->qaUnit()->answerAs == TQAtype::e_asSound)
        return QLatin1String("<span style=\"font-family: 'Nootka';\">")
              + QString::number(static_cast<int>(m_qInf->qaUnit()->qa.pos().str())) + QLatin1String("</span>")
              + TfingerPos::romanFret(m_qInf->qaUnit()->qa.pos().fret());
  }
  return QString();
}


QString TlinChartDelegate::keyText() const {
  if (m_qInf->qaUnit()) {
    if (m_chart->exam()->level()->useKeySign && (m_qInf->qaUnit()->questionAs == TQAtype::e_onScore || m_qInf->qaUnit()->answerAs == TQAtype::e_onScore))
      return m_qInf->qaUnit()->key.getName();
  }
  return QString();
}


void TlinChartDelegate::pointEntered() {
  m_qInf->setCursorPos(QPointF(parentItem()->x(), pointY())); // delegate is wrapped by QML Loader, so just this item has proper x coordinate towards view area
  m_chart->tipEntered(m_qInf);
}


void TlinChartDelegate::pointExited() {
  m_chart->tipExited();
}


void TlinChartDelegate::lineEntered(qreal posX, qreal posY) {
  m_chart->lineTip()->setCursorPos(QPointF(parentItem()->x() + posX, posY));
  if (m_chart->settings()->order == Tchart::e_byNumber)
    m_chart->lineTip()->tipText = TexTrans::averAnsverTimeTxt()
                + QString("<br><big><b>%1</b></big>").arg(Texam::formatReactTime(m_chart->exam()->averageReactonTime(), true));
  else {
    auto gr = m_chart->mainChart()->group(m_qInf->grNr());
    m_chart->lineTip()->tipText = gr->fullDescription()
                + QLatin1String("<br><big><b>") + Texam::formatReactTime(qRound(gr->averTime()), true) + QLatin1String("</b></big>");
  }
  m_chart->lineTip()->setColor(QColor(0, 192, 192));
  m_chart->tipEntered(m_chart->lineTip());
  m_chart->setAverLineGroup(m_qInf->grNr());
}


void TlinChartDelegate::lineExited() {
  m_chart->tipExited();
  m_chart->setAverLineGroup(-100);
}


QString TlinChartDelegate::getHintText() {
  if (m_chart->settings()->order != Tchart::e_byNumber) {
    auto gr = m_chart->mainChart()->group(m_qInf->grNr());
    int nrInGr = -1; // not found
    for (int g = 0; g < gr->size(); ++g) {
      if (gr->operator[](g).qaPtr == m_qInf->qaUnit()) {
        nrInGr = g;
        break;
      }
    }
    if (nrInGr > -1 && gr->size() / 2 == nrInGr) {
      if (gr->description().isEmpty() && gr->size() > 1) // for single question groups it makes no sense
        return QGuiApplication::translate("TlinearChart", "questions unrelated<br>with chart type");
      if (m_chart->settings()->order == Tchart::e_byFret)
        return QString("<span style=\"font-size: %1px;\">%2</span>").arg(qApp->font().pointSize() * 4).arg(TfingerPos::romanFret(gr->first()->qa.pos().fret()));
      if (m_chart->settings()->order == Tchart::e_byAccid) {
        auto acc = gr->first()->qa.note.alter();
        if (acc)
          return QString("%1").arg(accidToNotka(acc, 40));
        else
          return QGuiApplication::translate("TlinearChart", "without accidentals");
      }
      if (m_chart->settings()->order == Tchart::e_byKey)
        return QString("<p style=\"font-size: %1px; text-align: center\">").arg(qApp->font().pointSize() * 2) + gr->first()->key.getName()
              + QLatin1String("<br>") + getWasInAnswOrQuest(TQAtype::e_onScore, gr->first()) + QLatin1String("</p>");
    }
  }
  return QString();
}


void TlinChartDelegate::paint(QPainter* painter) {
  if (m_nrInchart < 1)
    return;
  
  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  if (m_qInf->grNr() > 0 && m_qInf->grNr() % 2) {
    auto gr = m_chart->mainChart()->group(m_qInf->grNr());
    if (gr && m_qInf->qaUnit() != gr->first()) {
      painter->setPen(Qt::NoPen);
      auto hiC = qApp->palette().highlight().color();
      hiC.setAlpha(40);
      painter->setBrush(hiC);
      painter->drawRect(QRectF(0.0, 0.0, width(), height() * 0.85));
    }
  }

  qreal h = height() * HH;
  if ((m_nrInchart + 1) % 5 == 0) {
    painter->setPen(QPen(qApp->palette().text().color(), 1.0, Qt::DashLine));
    painter->drawLine(QPointF(width() - height() / 300.0, height() * 0.1), QPointF(width() - height() / 300.0, height() * 0.8333333333333333));
  }
  painter->setPen(QPen(qApp->palette().text().color(), width() / 200.0));
  qreal y1 = 0.0;
  if (m_chart->settings()->yValue == Tchart::e_YquestionTime)
    y1 = height() * 0.1 + h - (static_cast<qreal>(getUnit(m_nrInchart - 1)->time) / 10.0 / m_chart->mainChart()->maxValue()) * h;
  else if (m_chart->settings()->yValue == Tchart::e_Yeffectiveness)
    y1 = height() * 0.1 + h - (getUnit(m_nrInchart - 1)->effectiveness() / m_chart->mainChart()->maxValue()) * h;
  else if (m_chart->settings()->yValue == Tchart::e_YprepareTime && m_chart->exam()->melodies())
    y1 = height() * 0.1 + h - (static_cast<qreal>(getUnit(m_nrInchart - 1)->attempt(0)->prepareTime()) / 10.0 / m_chart->mainChart()->maxValue()) * h;
  if (m_chart->settings()->yValue == Tchart::e_YattemptsCount)
    y1 = height() * 0.1 + h - (static_cast<qreal>(getUnit(m_nrInchart - 1)->attemptsCount()) / m_chart->mainChart()->maxValue()) * h;
  if (m_chart->settings()->yValue == Tchart::e_YplayedCount)
    y1 = height() * 0.1 + h - (static_cast<qreal>(getUnit(m_nrInchart - 1)->totalPlayBacks()) / m_chart->mainChart()->maxValue()) * h;
  qreal d = h * 0.0075;
  for (qreal i = -3.0; i < 2.0; i++)
    painter->drawLine(QPointF(0.0, y1 + i * d), QPointF(width(), pointY() + i * d));

  if (m_prevAverY) {
//     if (!m_qInf->qaUnit()->isWrong() || (m_qInf->qaUnit()->isWrong() && m_chart->settings()->inclWrongAnsw)) {
        painter->setPen(QPen(Qt::magenta, height() / 200.0, Qt::DashDotLine));
        painter->drawLine(QPointF(0.0, m_prevAverY), QPointF(width(), m_thisAverY));
//     }
  }
}


//#################################################################################################
//###################                PROTECTED         ############################################
//#################################################################################################

TQAunit *TlinChartDelegate::getUnit(int qNr) const {
  if (m_chart->settings()->order == Tchart::e_byNumber)
    return m_chart->exam()->question(qNr);
  else
    return m_chart->mainChart()->getSorted(qNr);
}


void TlinChartDelegate::setProgressHoverred(bool ph) {
  if (ph != m_progressHoverred) {
    m_progressHoverred = ph;
    if (m_progressHoverred) {
        m_chart->lineTip()->setCursorPos(QPointF(parentItem()->x(), qMin(m_prevAverY, m_thisAverY)));
        m_chart->lineTip()->tipText = QLatin1String("<b>") + QGuiApplication::translate("TgraphicsLine", "progress line").toUpper()
            + QLatin1String("</b><br><span style=\"color: #ff00ff; font-size: x-large; font-family: 'Courier New', Courier, monospace;\"><b> \\ </b></span>")
            + QGuiApplication::translate("TgraphicsLine", "descending - you are progressing better")
            + QLatin1String("<br><span style=\"color: #ff00ff; font-size: x-large; font-family: 'Courier New', Courier, monospace;\"><b> / </b></span>")
            + QGuiApplication::translate("TgraphicsLine", "ascending - you are thinking too much");
        m_chart->lineTip()->setColor(Qt::magenta);
        m_chart->tipEntered(m_chart->lineTip());
    } else {
        m_chart->tipExited();
    }
  }
}


void TlinChartDelegate::hoverEnterEvent(QHoverEvent* e) {
  if ((e->posF().y() > m_prevAverY && e->posF().y() < m_thisAverY) || (e->posF().y() < m_prevAverY && e->posF().y() > m_thisAverY))
    setProgressHoverred(true);
}


void TlinChartDelegate::hoverMoveEvent(QHoverEvent* e) {
  if (m_prevAverY) {
    if ((e->posF().y() > m_prevAverY && e->posF().y() < m_thisAverY) || (e->posF().y() < m_prevAverY && e->posF().y() > m_thisAverY))
      setProgressHoverred(true);
    else
      setProgressHoverred(false);
  }
}


void TlinChartDelegate::hoverLeaveEvent(QHoverEvent*) {
  setProgressHoverred(false);
}


void TlinChartDelegate::calcProgressRangeY() {
  if (m_chart->settings()->order == Tchart::e_byNumber && (!m_chart->isMelody() || m_chart->settings()->yValue == Tchart::e_YquestionTime)
      && m_nrInchart > 0 && m_chart->mainChart()->averChunk(m_nrInchart - 1) > 0.0) {
      qreal h = height() * HH;
      m_prevAverY = height() * 0.1 + h - (m_chart->mainChart()->averChunk(m_nrInchart - 1) / 10.0 / m_chart->mainChart()->maxValue()) * h;
      m_thisAverY = height() * 0.1 + h - (m_chart->mainChart()->averChunk(m_nrInchart) / 10.0 / m_chart->mainChart()->maxValue()) * h;
  } else {
      m_prevAverY = 0.0;
      m_thisAverY = 0.0;
  }
}
