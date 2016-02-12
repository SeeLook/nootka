/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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


#include "tquestiontip.h"
#include <exam/tqaunit.h>
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/textrans.h>
#include <graphics/tnotepixmap.h>
#include <widgets/tquestionaswdg.h>
#include <animations/tfadeanim.h>
#include <tcolor.h>
#include <tinitcorelib.h>
#include <tnoofont.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qgraphicssceneevent.h>


/* static */
QString TquestionTip::getTextHowAccid(Tnote::Ealter accid) {
    QString S = QString("<br><span style=\"color: %1\">").arg(Tcore::gl()->GfingerColor.name());
    if (accid) S += tr("Use %1").arg(QString::fromStdString(signsAcid[accid + 2]));
    else S += tr(" Don't use accidentals!");
    S +=  QLatin1String("</span>");
    return S;
}


QString TquestionTip::onStringTxt(quint8 strNr) {
    return QLatin1String("<b>") + tr("on %1 string.").arg(QString("</b><span style=\"font-family: nootka;\">%1</span><b>").arg(strNr))
          + QLatin1String("</b>");
}


QString TquestionTip::playOrSing(int instr) {
	if (Einstrument(instr) == e_noInstrument)
		return tr("Play or sing");
	else
		return tr("Play");
}


QString TquestionTip::m_questText;

//##########################################################################################
//#################################### CONSTRUCTOR #########################################
//##########################################################################################

TquestionTip::TquestionTip(Texam* exam, double scale) :
  TgraphicsTextTip(getQuestion(exam->question(exam->count()-1), exam->count(), exam->level(), scale)),
  m_markCorner(false),
  m_minimized(false)
{
  setOpacity(0.0);
  m_fadeInAnim = new TfadeAnim(this);
  m_fadeInAnim->setEasingCurveType(QEasingCurve::OutCubic);
  m_fadeInAnim->startFade(1.0);
	setAcceptHoverEvents(true);
// gradient a'la staff lines
	QColor startColor = Tcore::gl()->EquestionColor;
	startColor.setAlpha(20);
	QColor endColor = startColor;
	endColor.setAlpha(40);
	m_staffGradient.setCoordinateMode(QGradient::ObjectBoundingMode);
	m_staffGradient.setStart(0.5, 0.0);
	m_staffGradient.setFinalStop(0.5, 1.0);
	for (int i = 0; i < 5; ++i) {
		m_staffGradient.setColorAt(0.32 + i * 0.1, startColor);
		m_staffGradient.setColorAt(0.34 + i * 0.1, endColor);
	  m_staffGradient.setColorAt(0.36 + i * 0.1, endColor);
		m_staffGradient.setColorAt(0.38 + i * 0.1, startColor);
	}
	setTipMovable(true);
}

TquestionTip::~TquestionTip() 
{
	delete m_fadeInAnim;
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

QString TquestionTip::getNiceNoteName(Tnote& note, Tnote::EnameStyle style) {
	return QString("<b><span style=\"%1\">&nbsp;").arg(Tcolor::bgTag(Tcore::gl()->EquestionColor)) +
            note.toRichText(style) + QLatin1String(" </span></b>");
}


QString TquestionTip::getQuestion(TQAunit* question, int questNr, Tlevel* level, double scale) {
  QString br = QStringLiteral("<br>");
  QString sp = QStringLiteral(" ");
  m_scoreFree = true;
  m_nameFree = !(bool)question->melody(); // no name widget when level uses melodies
  m_guitarFree = true;
  m_questText.clear();
  double sc = 4.0;
  if (scale) {
        m_questText = QString("<p style=\"font-size: %1px;\">").arg(qRound(scale * 22.0));
    sc = 4.0 * scale;
  }
  QString attemptText;
	if (question->attemptsCount() > 1)
		attemptText = QLatin1String(" <small><i>") + TexTrans::attemptTxt() + QString(" %1").arg(question->attemptsCount()) + "</i></small>";
  m_questText += QString("<b><u>&nbsp;%1.&nbsp;</u></b>").arg(questNr) + attemptText + br;
  QString apendix;
  QString noteStr;
  switch (question->questionAs) {
      case TQAtype::e_asNote: {
        m_scoreFree = false;
        if (question->answerAsNote()) {
            if (question->qa.note.alter != question->qa_2.note.alter)
                m_questText += tr("Change enharmonically and show on the staff");
            else
                m_questText += tr("Given note show on the staff");
						if (level->useKeySign && level->manualKey)
								apendix = tr("<br><b>in %1 key.</b>", "in key signature").arg(question->key.getName());
						      m_questText += getTextHowAccid((Tnote::Ealter)question->qa_2.note.alter);
        } else if (question->answerAsName()) {
            m_nameFree = false;
            m_questText += tr("Give name of");
				} else if (question->answerAsFret()) {
              m_guitarFree = false;
              m_questText += tr("Show on the guitar");
				} else if (question->answerAsSound()) {
									if (question->melody())
										      m_questText += tr("Play or sing a melody.");
									else
										      m_questText += playOrSing(int(level->instrument));
				}
        if (question->answerAsFret() || question->answerAsSound()) {
					if (level->instrument != e_noInstrument && !level->canBeMelody() && level->showStrNr && !level->onlyLowPos) {
						apendix = br + sp + onStringTxt(question->qa.pos.str());
					}
        }
        if (!question->melody()) {
					if (level->useKeySign && level->manualKey && question->answerAsNote()) // hide key signature
                m_questText += br + wrapPixToHtml(question->qa.note, true, TkeySignature(0), sc);
					else
                m_questText += br + wrapPixToHtml(question->qa.note, true, question->key, sc);
				}
        if (!apendix.isEmpty())
            m_questText += apendix;
				break;
			}

      case TQAtype::e_asName:
        m_nameFree = false;
        noteStr = br + getNiceNoteName(question->qa.note, question->styleOfQuestion());
        if (question->answerAsNote()) {
						m_scoreFree = false;
						      m_questText += tr("Show on the staff") + noteStr;
						if (level->useKeySign && level->manualKey) {
							         m_questText += tr("<br><b>in %1 key.</b>", "in key signature").arg(question->key.getName());
						}
        } else if (question->answerAsName()) {
            m_nameFree = false;
            noteStr = br + getNiceNoteName(question->qa.note, question->styleOfQuestion());
            if (question->qa.note.alter != question->qa_2.note.alter) {
                m_questText += tr("Change enharmonically and give name of");
                m_questText += noteStr + getTextHowAccid((Tnote::Ealter)question->qa_2.note.alter);
            } else
                m_questText += tr("Use another style to give name of") + noteStr;
				} else if (question->answerAsFret()) {
						m_guitarFree = false;
						      m_questText += tr("Show on the guitar") + noteStr;
				} else if (question->answerAsSound()) {
						      m_questText += playOrSing(int(level->instrument)) + noteStr;
				}
				if (question->answerAsFret() || question->answerAsSound()) {
						if (level->instrument != e_noInstrument && level->showStrNr && !level->onlyLowPos)
								        m_questText += br + sp + onStringTxt(question->qa.pos.str());
				}
			break;

      case TQAtype::e_asFretPos:
        m_guitarFree = false;
        if (question->answerAsNote()) {
						m_scoreFree = false;
						      m_questText += tr("Show on the staff note played on");
						if (level->useKeySign && level->manualKey) {
							apendix = tr("<b>in %1 key.</b>", "in key signature").arg(question->key.getName());
						}
        } else if (question->answerAsName()) {
            m_nameFree = false;
            m_questText += tr("Give name of");
				} else if (question->answerAsFret()) {
              m_questText += tr("Show sound from position:", "... and string + fret numbers folowing");
              apendix = br + sp + onStringTxt(question->qa_2.pos.str());
				} else if (question->answerAsSound()) {
							     m_questText += playOrSing(int(level->instrument));
				}
        m_questText += QString("<br><span style=\"font-size: xx-large; %1\">&nbsp;").arg(Tcolor::bgTag(Tcore::gl()->EquestionColor)) +
                    question->qa.pos.toHtml() + QLatin1String(" </span>");
        if (!apendix.isEmpty())
            m_questText += br + apendix;
        if (question->answerAsNote() || question->answerAsName())
          if (level->forceAccids)
                m_questText += br + getTextHowAccid((Tnote::Ealter)question->qa.note.alter);
      break;

      case TQAtype::e_asSound:
        if (question->answerAsNote()) {
						m_scoreFree = false;
						if (question->melody()) {
								        m_questText += TexTrans::writeDescTxt();
								if (level->useKeySign && level->manualKey && level->onlyCurrKey)
									           m_questText += br + tr("Guess a key signature");
						} else {
								        m_questText += tr("Listened sound show on the staff");
								if (level->useKeySign && level->manualKey)
									           m_questText += tr("<br><b>in %1 key.</b>", "in key signature").arg(question->key.getName());
								if (level->forceAccids)
									           m_questText += getTextHowAccid((Tnote::Ealter)question->qa.note.alter);
						}
        } else if (question->answerAsName()) {
            m_nameFree = false;
            m_questText += tr("Give name of listened sound");
            if (level->forceAccids)
                m_questText += getTextHowAccid((Tnote::Ealter)question->qa.note.alter);
				} else if (question->answerAsFret()) {
              m_guitarFree = false;
              m_questText += tr("Listened sound show on the guitar");
              if (level->showStrNr)
                m_questText += br + sp + onStringTxt(question->qa.pos.str());
				} else if (question->answerAsSound()) {
                m_questText += tr("Play or sing listened sound");
				}
      break;
    }
  if (scale)
      m_questText += QLatin1String("</p>");
  return m_questText;
}


void TquestionTip::setMinimized(bool min) {
	if (min != m_minimized) {
		m_minimized = min;
		if (m_minimized) {
        QString titleText = m_questText.mid(0, m_questText.indexOf(QLatin1String("<br>")) - 1);
        setHtml(titleText);
		} else {
        setHtml(m_questText);
		}
		setFixPos(pos());
	}
}


void TquestionTip::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	QRectF rect = boundingRect();
// background rectangle with question color border
	painter->setPen(QPen(QColor(Tcore::gl()->EquestionColor.name()), 2));
	painter->setBrush(QBrush(qApp->palette().base().color()));
	painter->drawRoundedRect(rect, 5, 5);
// colored rectangle over
	painter->setBrush(QBrush(m_staffGradient));
	painter->drawRoundedRect(rect, 5, 5);
// question mark
	QColor mC = Tcore::gl()->EquestionColor;
	mC.setAlpha(40);
	painter->setPen(mC);
	TnooFont nf;
	QFontMetrics fm = QFontMetrics(nf);
	nf.setPointSize(nf.pixelSize() * ((rect.height() / (qreal)fm.boundingRect(QLatin1String("?")).height())) * 0.6);
	painter->setFont(nf);
	painter->drawText(rect, Qt::AlignRight | Qt::AlignVCenter, m_minimized ? QStringLiteral("? ") : QStringLiteral("?"));
	QGraphicsTextItem::paint(painter, option, widget);
// corner line for minimizing
	QColor lc = m_markCorner ? qApp->palette().highlight().color() : qApp->palette().text().color();
	painter->setPen(QPen(lc, 4, Qt::SolidLine, Qt::RoundCap));
	painter->drawLine(rect.width() - 20, 10, rect.width() - 10, 10);
}


QRectF TquestionTip::boundingRect() const {
	return QGraphicsTextItem::boundingRect();
}

//##########################################################################################
//################     PROTECTED MOUSE EVENTS    ###########################################
//##########################################################################################

void TquestionTip::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  TgraphicsTextTip::hoverLeaveEvent(event);
	m_markCorner = false;
	update(boundingRect().width() - 30, 0, 30, 30);
}


void TquestionTip::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
	bool markState = m_markCorner;
	if (event->pos().x() > boundingRect().width() - 25 && event->pos().y() < 20) {
		m_markCorner = true;
		setCursor(Qt::ArrowCursor);
	} else {
		m_markCorner = false;
		setCursor(Qt::SizeAllCursor);
	}
	if (markState != m_markCorner)
		update(boundingRect().width() - 30, 0, 30, 30);
}


void TquestionTip::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	if (event->button() == Qt::LeftButton) {
		if (m_markCorner) {
			setMinimized(!m_minimized);
			emit minimizeChanged();
		}
	}
	TgraphicsTextTip::mousePressEvent(event);
}







