/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "tnootkacertificate.h"
#include "texam.h"
#include "texamview.h"
#include <texamlevel.h>
#include <tgraphicstexttip.h>
#include <QDate>
#include <QApplication>
#include <QStyle>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QPrinter>
#include <QPainter>

#include <QDebug>


QString spanPx = "<span style=\"font-size: 20px;\">";

QString finishExamText(Texam* exam) {
		QString txt = "<div><p style=\"text-align: right;\">" +
		QString("date: %1").arg(QDate::currentDate().toString("d MMMM yyyy")) + "</p>" +
    "<h4 style=\"text-align: right; margin-right:30px;\">" + "Nootka<br>Akademy Of Music" + "</h4>" +
    QString("Student %1 ").arg(spanPx + "<b>" + exam->userName().toUpper() + "</span></b><br>") +
    "Has been awarded the" + "<h1 style=\"text-align: center;\">" + 
		"Certificate Of Exam Completion" + "</h1>" +
    "Passing the exam on the level" + spanPx + " <b>" + exam->level()->name + "</span></b>,<br>" +
    "having answered the required" + spanPx + QString("<b> %1 </span></b>").arg(exam->count()) + "answers<br>" +
    "in time" + spanPx + " <b>" + TexamView::formatedTotalTime(exam->workTime() * 1000) + "</b></span><br>" +
    "and achieving the score" + ": " + spanPx +"<b>" + 
		QString("<b>%1%</b>").arg(exam->effectiveness(), 0, 'f', 1, '0')+ "</span></b><br><br>" +
    "As a witness to this accomplishment,<br>we hereby award this certificate on " + "<b>" +
		QDate::currentDate().toString("d MMMM yyyy") +	"</b><br><br>" +
    "<p style=\"text-align: right;\">" + "examining board:" + "<br>" +
    "<i>president</i>: Nootka itself" + "<br>" +
    "professor Processor &amp; Mrs RAM his assistant" + "<br>" +
    "<i>secretary</i>: Mr Disk" + "<br><br><br><br>" + "................<br>" + "stamp" +
//     <img src="http://nootka.googlecode.com/hg/unused-picts/stamp.png">
    "</p></div>";
    return txt;
}


TnootkaCertificate::TnootkaCertificate(Texam* exam, QGraphicsObject* parent) :
	QGraphicsObject(parent),
  m_exam(exam),
  m_saveHint(0)
{
	 	m_cert = new TgraphicsTextTip(finishExamText(exam), QColor("#FFFF00"));
		m_cert->setParentItem(this);
		m_cert->setTextWidth(boundingRect().width());
// 		qDebug() << m_cert->toHtml();
		setAcceptHoverEvents(true);
		createHints();
		hideHints();
}


void TnootkaCertificate::createHints() {
	if (!m_saveHint) {
		QIcon ic = QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton);
    m_saveIcon = new QGraphicsPixmapItem(QPixmap(ic.pixmap(32, 32)), this);
			m_saveIcon->setPos(10, 5);
			m_saveIcon->setFlag(QGraphicsItem::ItemIsFocusable);
		ic = QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton);
		m_closeIcon = new QGraphicsPixmapItem(QPixmap(ic.pixmap(32, 32)), this);
			m_closeIcon->setPos(15 + m_saveIcon->boundingRect().width(), 5);
			m_closeIcon->setFlag(QGraphicsItem::ItemIsFocusable);
			m_saveHint = new TgraphicsTextTip("<big>" + tr("Save this certificate to file in remembrance.") + "</big>",
																				QApplication::palette().highlight().color());
			m_saveHint->setParentItem(this);
			m_saveHint->setPos((boundingRect().width() - m_saveHint->boundingRect().width()) / 2, 
					boundingRect().height() - m_saveHint->boundingRect().height() - 5	);
	}
}


void TnootkaCertificate::removeHints() {
		if (m_closeIcon) {
			delete m_closeIcon;
			delete m_saveHint;
			delete m_saveIcon;
			m_closeIcon = 0;
			m_saveHint = 0;
			m_saveIcon = 0;
		}
}


QRectF TnootkaCertificate::boundingRect() const {
	return m_cert->boundingRect();
}


void TnootkaCertificate::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
	if (m_saveIcon) {
		m_saveIcon->show();
		m_closeIcon->show();
		m_saveHint->show();
	}
}


void TnootkaCertificate::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
		hideHints();
}


void TnootkaCertificate::hideHints() {
	if (m_saveIcon) {
		m_saveIcon->hide();
		m_closeIcon->hide();
		m_saveHint->hide();
	}
}



void TnootkaCertificate::saveSlot() {
	QString fileName = QFileDialog::getSaveFileName(0, tr("Save certificate"),
										QDir::toNativeSeparators(QDir::homePath() + "/" +
												m_exam->userName() + "-" + m_exam->level()->name + ".pdf"), "*.pdf", 0 , QFileDialog::DontUseNativeDialog);
	if (fileName == "")
		return;
	QPrinter printer;
	printer.setOutputFormat(QPrinter::PdfFormat);
	printer.setPaperSize(boundingRect().size() * scale(), QPrinter::Point);
	printer.setFullPage(true);
	printer.setOutputFileName(fileName);
	QPainter painter;
	painter.begin(&printer);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	scene()->render(&painter, printer.paperRect(QPrinter::Point), QRectF(pos(), boundingRect().size() * scale()));
	painter.end();
}


void TnootkaCertificate::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	if (m_saveIcon->hasFocus()) {
			removeHints();
			saveSlot();
			createHints();
		}
	if (m_closeIcon->hasFocus())
		deleteLater();
}






