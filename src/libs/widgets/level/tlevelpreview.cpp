/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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

#include "tlevelpreview.h"
#include <exam/tlevel.h>
#include <graphics/tnotepixmap.h>
#include "widgets/tquestionaswdg.h"
#include "tinitcorelib.h"
#include <tnoofont.h>
#include <tscoreparams.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qtextbrowser.h>
#include <QtGui/qpainter.h>
#include <QtWidgets/qscroller.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qdesktopwidget.h>
#include <QtCore/qdebug.h>



QString tdAlign() {
	return QStringLiteral("<td valign=\"middle\" align=\"center\">");
}


TlevelPreview::TlevelPreview(QWidget* parent) :
  QWidget(parent),
  m_instrText(QString()),
  m_enableFixing(false)
{
		setMouseTracking(true);
    QLabel *headLab = new QLabel(tr("Level summary:"), this);
		m_summaryEdit = new QTextBrowser(this);
		m_summaryEdit->setReadOnly(true);
#if defined (Q_OS_ANDROID)
    m_summaryEdit->setMinimumWidth(fontMetrics().boundingRect("W").width() * 22);
    m_summaryEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_summaryEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);;
#else
    m_summaryEdit->setMinimumWidth(fontMetrics().boundingRect("W").width() * 28);
#endif
    m_summaryEdit->setMaximumWidth(qApp->desktop()->availableGeometry().width() * 0.45);
    m_summaryEdit->viewport()->setStyleSheet("background-color: transparent;");
		m_summaryEdit->setOpenLinks(false);
		QVBoxLayout *mainLay = new QVBoxLayout;
      mainLay->setContentsMargins(0, 0, 0, 0); // Others layouts have enough margins
			mainLay->addWidget(headLab);
			mainLay->addWidget(m_summaryEdit);
    setLayout(mainLay);
		setLevel();
		m_summaryEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    QScroller::grabGesture(m_summaryEdit->viewport(), QScroller::LeftMouseButtonGesture);
}


TlevelPreview::~TlevelPreview() {}


void TlevelPreview::setLevel() {
		Tlevel empty;
		empty.name = tr("no level selected");
		empty.desc = "";
		empty.loNote = Tnote();
		empty.hiNote = Tnote();
		empty.hiFret = 0;
		empty.questionAs.setAsNote(false);
		empty.questionAs.setAsName(false);
		empty.questionAs.setAsFret(false);
		empty.questionAs.setAsSound(false);
		empty.useKeySign = false;
		empty.withDblAcc = false;
		empty.withFlats = false;
		empty.withSharps = false;
		empty.instrument = e_noInstrument;
		setLevel(empty);
}


void TlevelPreview::setLevel(Tlevel& tl) {
	QString instrName;
	if (tl.hasInstrToFix) {
		m_instrText.clear(); // don't display background instrument symbol
#if !defined (Q_OS_ANDROID)
		if (m_enableFixing) {
				instrName = QLatin1String("<a href=\"fixInstrument\">") + tr("fix an instrument") + QLatin1String("</a>");
				m_summaryEdit->setTextInteractionFlags(Qt::TextBrowserInteraction);
		}
#endif
	} else {
		m_instrText = instrumentToGlyph(tl.instrument);
		instrName = instrumentToText(tl.instrument);
		m_summaryEdit->setTextInteractionFlags(Qt::NoTextInteraction);
	}
	const QString spTDEnd = QStringLiteral(" </td>");
  const QString nbsp = QStringLiteral("&nbsp;");
  const QString TRTD = QStringLiteral("<tr><td>");
  const QString TDTREnd = QStringLiteral("</td></tr>");
  QString S;
    S = QLatin1String("<center><b>") + tl.name + QLatin1String("</b>");
    S += QLatin1String("<table border=\"1\" cellpadding=\"3\">");
		S += QLatin1String("<tr><td colspan=\"2\" align=\"center\">") + instrName + spTDEnd;
    QString clefString(tr("Clef") + QLatin1String(":") + nbsp + nbsp); // "Clef:  "
    if (tl.loNote.isValid() && tl.hiNote.isValid())
      clefString += QString("<br><br>%1</td></tr>").
        arg(wrapPixToHtml(Tnote(0, 0, 0), tl.clef.type(), TkeySignature(0), (tl.clef.type() == Tclef::e_pianoStaff) ? 3.0 : 5.0)).
                          replace(QLatin1String("<img"), QLatin1String("<img width=\"70px\""));
    S += QLatin1String("<td rowspan=\"_ROW_SPAN_\"><br>&nbsp;&nbsp;") + clefString;
    S += TRTD + notesRangeTxt() + spTDEnd;
		if (tl.loNote.note && tl.hiNote.note)
			S += tdAlign() + tl.loNote.toRichText() + QLatin1String(" - ") + tl.hiNote.toRichText() + TDTREnd;
    if (tl.canBeGuitar()) { // level uses guitar
        S += TRTD + fretsRangeTxt() + spTDEnd;
        S += tdAlign() + QString("%1 - %2").arg(int(tl.loFret)).arg(int(tl.hiFret)) + TDTREnd;
    }
    if (tl.useKeySign) {
        S += TRTD + tr("key signature:") + spTDEnd + tdAlign();
        S += tl.loKey.getMajorName().remove(QLatin1String("-") + Tcore::gl()->S->majKeyNameSufix);
        S += QLatin1String(" (") + tl.loKey.accidNumber(true) + QLatin1String(")");
        if (!tl.isSingleKey) {
            S += QLatin1String(" - ") + tl.hiKey.getMajorName().remove(QLatin1String("-") + Tcore::gl()->S->majKeyNameSufix);
            S += QLatin1String(" (") + tl.hiKey.accidNumber(true) + QLatin1String(")");
        }
        S += TDTREnd;
    }
    S += TRTD + tr("accidentals:") + spTDEnd + tdAlign();
    if (!tl.withSharps && !tl.withFlats && !tl.withDblAcc)
        S += tr("none");
    else {
        if (tl.withSharps) S += TnooFont::span(QLatin1String(" #"));
        if (tl.withFlats) S += TnooFont::span(QLatin1String(" b"));
        if (tl.withDblAcc) S += TnooFont::span(QLatin1String(" x B"));
    }
    S += TDTREnd;
    S += TRTD + TquestionAsWdg::questionsTxt() + QLatin1String(": </td>") + tdAlign(); // QUESTIONS
    QString tmp;
    const QString space = QStringLiteral(" ");
    if (tl.questionAs.isNote())
      tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asNote) + space;
    if (tl.questionAs.isName())
      tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asName) + space;
    if (tl.questionAs.isFret())
      tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asFretPos) + space;
    if (tl.questionAs.isSound()) {
			if (tl.canBeMelody())
				tmp += QLatin1String("m");
			else
				tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asSound);
		}
    int fontSize = fontMetrics().height() * 1.3;
    S += TnooFont::span(tmp, fontSize);
    S += TDTREnd;
    tmp.clear();
    S += TRTD + TquestionAsWdg::answersTxt() + QLatin1String(": </td><td align=\"center\">"); // ANSWERS
    if (tl.answerIsNote())
            tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asNote) + space;
    if (tl.answerIsName())
            tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asName) + space;
    if (tl.answerIsGuitar())
            tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asFretPos) + space;
    if (tl.answerIsSound()) {
			if (tl.canBeMelody())
				tmp += QLatin1String("m");
			else
				tmp += TquestionAsWdg::qaTypeSymbol(TQAtype::e_asSound);
		}
    S += TnooFont::span(tmp, fontSize);
    S += TDTREnd;
    if (tl.canBeName() || tl.canBeScore() || tl.canBeSound()) {
      S += QLatin1String("<tr><td colspan=\"2\" align=\"center\">");
      if (tl.requireOctave)
          S += tr("proper octave is required");
      else
          S += tr("octave does no matter");
      S += TDTREnd;
    }
    S += QLatin1String("</table></center>");
    S.replace(QLatin1String("_ROW_SPAN_"), QString("%1").arg(S.count(QLatin1String("<tr>"))));
		S += QLatin1String("<br><br>") + tl.desc;
		m_summaryEdit->setHtml(S);
}


void TlevelPreview::adjustToHeight() {
	m_summaryEdit->setFixedHeight((m_summaryEdit->document()->toHtml().count(QLatin1String("<tr>")) + 3) * (fontMetrics().height() + 7));
}

//##########################################################################################################
//########################################## PROTECTED #####################################################
//##########################################################################################################

void TlevelPreview::setFixInstrEnabled(bool enabled) {
	m_enableFixing = enabled;
	if (enabled) {
		connect(m_summaryEdit, SIGNAL(anchorClicked(QUrl)), this, SLOT(linkToFixLevel(QUrl)));
	}
}


void TlevelPreview::paintEvent(QPaintEvent* ) {
	QPainter painter(this);
	painter.setBrush(palette().base());
	painter.setPen(Qt::NoPen);
	painter.drawRect(m_summaryEdit->geometry());
	if (m_instrText != "") {
			TnooFont nFont;
			QFontMetrics fm = QFontMetrics(nFont);
			nFont.setPixelSize(nFont.pixelSize() * ((qreal)m_summaryEdit->height() / (qreal)fm.boundingRect(m_instrText).height()));
			painter.setFont(nFont);
			QColor bg = palette().highlight().color();
			bg.setAlpha(60);
			painter.setPen(QPen(bg));
			painter.drawText(QRect(m_summaryEdit->geometry().x(), 15, m_summaryEdit->width(), m_summaryEdit->height()),
											 Qt::AlignCenter, m_instrText);
	}
}


void TlevelPreview::linkToFixLevel(QUrl url) {
	if (url.toString() == QLatin1String("fixInstrument"))
		emit instrumentLevelToFix();
}





