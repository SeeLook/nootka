/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "ttipchart.h"
#include "tqaunit.h"
#include "texamview.h"
#include "tquestionaswdg.h"
#include "tquestionpoint.h"
#include "tnotename.h"
#include "tnotepixmap.h"
#include <QPainter>
#include <qstyleoption.h>
#include <QApplication>
#include <QGraphicsEffect>
#include <QTextBlockFormat>
#include <QTextCursor>
#include <QBuffer>
#include <QDebug>


/* static */
QString TtipChart::qaTypeText(TQAtype::Etype &type) {
  QString txt;
  switch (type) {
    case TQAtype::e_asNote : txt = TquestionAsWdg::asNoteTxt(); break;
    case TQAtype::e_asName : txt = TquestionAsWdg::asNameTxt(); break;
    case TQAtype::e_asFretPos : txt = TquestionAsWdg::asFretPosTxt(); break;
    case TQAtype::e_asSound : txt = TquestionAsWdg::asSoundTxt(); break;    
  }
  return txt;
}

QString TtipChart::romanFret(quint8 fret) {
  if (fret >= 0 && fret < 25)
      return fretsList[fret];
  else
      return "";
}


const QString TtipChart::fretsList[25] = { "0",
  "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X",
  "XI", "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "XIX", "XX",
  "XI", "XII", "XIII", "XIV"
};

QString TtipChart::insertQMark() {
    return QString("<span style=\"color: red; font-family: nootka; font-size: 45px;\">?</span>");
}

QString TtipChart::wrapPosToHtml(TfingerPos pos) {
    return QString("<span style=\"font-size: 25px; font-family: nootka\">%1%2</span>").arg(pos.str()).arg(romanFret(pos.fret()));
}

QString TtipChart::wrapPixToHtml(Tnote note, bool clef, TkeySignature key, double factor) {
    QPixmap pixmap = getNotePixmap(note, clef, key, factor);
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    pixmap.save(&buffer, "PNG");
    return QString("<img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>";
}

TtipChart::TtipChart(TquestionPoint *point) :
    QGraphicsTextItem(),
    m_point(point)
{
  QString txt = TquestionAsWdg::questionTxt() + " " + qaTypeText(point->question()->questionAs) + "<br>" +
          TquestionAsWdg::answerTxt() + " " + qaTypeText(point->question()->answerAs) + "<br>";
  QString qS = "", aS = "";
  switch (point->question()->questionAs) {
    case TQAtype::e_asNote :
      qS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
      if (point->question()->answerAs == TQAtype::e_asNote) {
          qS = wrapPixToHtml(point->question()->qa.note, true, TkeySignature(0)); // no key in question
          aS = wrapPixToHtml(point->question()->qa_2.note, true, point->question()->key);
      }
      break;
    case TQAtype::e_asName:
      qS = "<span style=\"font-size: 25px;\">" + TnoteName::noteToRichText(point->question()->qa.note) + "</span>";
      if (point->question()->answerAs == TQAtype::e_asName)
          aS = "<span style=\"font-size: 25px;\">" + TnoteName::noteToRichText(point->question()->qa_2.note) + "</span>";
      break;
    case TQAtype::e_asFretPos:
//         qS = QString("<span style=\"font-size: 20px;\">(%1) %2</span>").arg(point->question()->qa.pos.str()).arg(point->question()->qa.pos.fret());
        qS = wrapPosToHtml(point->question()->qa.pos);
      break;
    case TQAtype::e_asSound:
        qS = QString("<span style=\"font-family: nootka; font-size: 45px;\">n</span>");
        if (point->question()->answerAs == TQAtype::e_asSound)
            aS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
      break;
  }
  if (aS == "") {
      switch (point->question()->answerAs) {
        case TQAtype::e_asNote :
          aS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
          break;
        case TQAtype::e_asName:
          aS = "<span style=\"font-size: 25px;\">" + TnoteName::noteToRichText(point->question()->qa.note) + "</span>";
          break;
        case TQAtype::e_asFretPos:
//             aS = QString("<span style=\"font-size: 20px;\">(%1) %2</span>").arg(point->question()->qa.pos.str()).arg(point->question()->qa.pos.fret());
           aS = wrapPosToHtml(point->question()->qa.pos);
          break;
        case TQAtype::e_asSound:
          if (point->question()->questionAs == TQAtype::e_asNote)
              aS = QString("<span style=\"font-family: nootka; font-size: 45px;\">n</span>");
          else
              aS = wrapPixToHtml(point->question()->qa.note, true, point->question()->key);
          break;

      }
  }
  txt += "<table valign=\"middle\" align=\"center\"><tr><td> " + qS + " </td><td>" + insertQMark() + " </td><td> " + aS + " </td></tr></table>";
  txt += QString("<span style=\"color: %1;\">").arg(point->color().name());
  if (point->question()->correct()) {
    txt += QApplication::translate("TtipChart", "It was good!");
  } else
      if (point->question()->wrongNote() || point->question()->wrongPos())
          txt += QApplication::translate("TtipChart", "Wrong answer!");
            else {
                txt += QApplication::translate("TtipChart", "Not so bad.") + "<br>";
                QString misMes = ""; // Message with mistakes
                if (point->question()->wrongAccid())
                    misMes = "wrong accidental";
                if (point->question()->wrongKey()) {
                    if (misMes != "")
                        misMes += ", ";
                    misMes += "wrong key signature";
                }
                if (point->question()->wrongOctave()) {
                    if (misMes != "")
                        misMes += ", ";
                    if (misMes.length() > 25)
                        misMes += "<br>";
                    misMes += "wrong octave";
                }
                txt += misMes;
            }
            
  txt += "</span><br>";
  txt += TexamView::reactTimeTxt() +
        QString("<span style=\"font-size: 20px\">  %1s</span>").arg((double)point->question()->time / 10.0);
  
  setHtml(txt);
  
  setTextWidth(boundingRect().width());
  QTextBlockFormat format;
  format.setAlignment(Qt::AlignCenter);
  QTextCursor cursor = textCursor();
  cursor.select(QTextCursor::Document);
  cursor.mergeBlockFormat(format);
  cursor.clearSelection();
  setTextCursor(cursor);
  
  setZValue(75);

  
  setFlag(QGraphicsItem::ItemIgnoresTransformations);
  
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
  shadow->setBlurRadius(5);
  shadow->setOffset(5, 5);
  setGraphicsEffect(shadow);
}

TtipChart::~TtipChart() {
}



void TtipChart::setPos(QPointF p) {
  QGraphicsItem::setPos(p);
}



void TtipChart::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

    QRectF rect = boundingRect();
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(option->palette.light()));
    painter->drawRoundedRect(rect, 5, 5);
    QColor startColor = m_point->color();
    startColor.setAlpha(25);
    QColor endColor = startColor;
    endColor.setAlpha(75);
    QLinearGradient grad(rect.topLeft(), rect.bottomRight());
    grad.setColorAt(0.1, startColor);
    grad.setColorAt(0.9, endColor);
    painter->setBrush(QBrush(grad));
    painter->drawRoundedRect(rect, 5, 5);

    QGraphicsTextItem::paint(painter, option, widget);

}

QRectF TtipChart::boundingRect() const {
    return QGraphicsTextItem::boundingRect();
}



