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
#include <tlevel.h>
#include <tgraphicstexttip.h>
#include <tpixmaker.h>
#include <texamhelp.h>
#include <QDate>
#include <QBuffer>
#include <QApplication>
#include <QStyle>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QPrinter>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsEffect>
#include <QGraphicsProxyWidget>

#include <QDebug>


QString spanPx = "<span style=\"font-size: 20px;\">";

QString finishExamText(Texam* exam, QString path) {
		QString txt = "<p align=\"right\" style=\"margin-right: 10px;\">" +
		QString("date: %1").arg(QDate::currentDate().toString("d MMMM yyyy")) + ""
    "<h3>" + "Nootka Akademy Of Music" + "</h3></p>" +
    QString("Student %1 ").arg(spanPx + "<b>" + exam->userName().toUpper() + "</span></b> ") +
    "Has been awarded the" + "<h1 align=\"center\">" + 
		"Certificate Of Exam Completion" + "</h1><p style=\"margin-left: 10px;\">" +
    "Passing the exam on the level" + spanPx + " <b>" + exam->level()->name + "</span></b>,<br>" +
    "having answered the required" + spanPx + QString("<b> %1 </span></b>").arg(exam->count()) + "answers<br>" +
    "in time" + spanPx + " <b>" + TexamView::formatedTotalTime(exam->workTime() * 1000) + "</b></span><br>" +
    "and achieving the score" + ": " + spanPx +"<b>" + 
		QString("<b>%1%</b>").arg(exam->effectiveness(), 0, 'f', 1, '0')+ "</span></b><br></p>" +
    "As a witness to this accomplishment,<br>we hereby award this certificate on " + "<b>" +
		QDate::currentDate().toString("d MMMM yyyy") +	"</b><br><br>" +
    "<p align=\"right\" style=\"margin-right: 10px;\">" + "examining board:" + "<br>" +
    "<i>president</i>: Nootka itself" + "<br>" +
    "professor Processor &amp; Mrs RAM his assistant" + "<br>" +
    "<i>secretary</i>: Mr Disk" + "</p><p align=\"center\">" + 
    QString("<img src=\"%1\" /><br>").arg(path + "picts/stamp.png") +
    "................<br>" +
		"stamp" + "</p>";
    return txt;
}

TnootkaCertificate::TnootkaCertificate(QGraphicsView* view, const QString& path, Texam* exam) : 
	QGraphicsObject(),
	m_view(view),
  m_exam(exam),
  m_path(path),
  m_saveHint(0)
{
		m_view->setAttribute(Qt::WA_TransparentForMouseEvents, false); // unlock mouse
		m_view->scene()->addItem(this);
		setZValue(100);
	 	m_cert = new QGraphicsProxyWidget(this);
			m_cert->setParentItem(this);
			QTextEdit *te = new QTextEdit();
      te->setStyleSheet(QString("background-image: url(%1);").arg(path + "picts/noise.png"));
			te->setLineWrapMode(QTextEdit::NoWrap);
			te->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			te->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
			te->setFrameShape(QFrame::NoFrame);
			te->setHtml(finishExamText(m_exam, m_path));
// 			te->setLineWrapMode(QTextEdit::NoWrap);
			te->setFixedSize(QSize(m_view->width() * 0.5, m_view->height() * 0.9));
			m_cert->setWidget(te);
// 			m_cert->setScale((m_view->height() * 0.9) / boundingRect().height());
			setPos((m_view->width() - m_cert->scale() * boundingRect().width()) / 2,
                      (m_view->height() - m_cert->scale() * boundingRect().height()) / 2);
		
		m_bgRect = new QGraphicsRectItem(m_view->sceneRect());
			m_bgRect->setPen(Qt::NoPen);
		QColor bg = m_view->palette().text().color();
			bg.setAlpha(170);
			m_bgRect->setBrush(QBrush(bg));
			m_view->scene()->addItem(m_bgRect);
			m_bgRect->setZValue(0);
		QGraphicsBlurEffect *bgBlur = new QGraphicsBlurEffect();
			bgBlur->setBlurRadius(20.0);
			m_bgRect->setGraphicsEffect(bgBlur);
// 		m_cert->setTextWidth(boundingRect().width());
// 		qDebug() << te->toHtml();
		setAcceptHoverEvents(true);
		createHints();
}


TnootkaCertificate::~TnootkaCertificate() {
	m_view->setAttribute(Qt::WA_TransparentForMouseEvents, true);
	removeHints();
	delete m_bgRect;
}


void TnootkaCertificate::createHints() {
	if (!m_saveHint) {
		QIcon ic = QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton);
		QByteArray byteArray;
		QBuffer buffer(&byteArray);
    QPixmap scaledPix = ic.pixmap(32, 32);
    scaledPix.save(&buffer, "PNG");
		m_saveHint = new TgraphicsTextTip(QString("<a href=\"saveCert\"><img src=\"data:image/png;base64,") + byteArray.toBase64() + "\"/>" + "<br><big>" + tr("Save this certificate to file in remembrance.") + "</a></big>",
																				QApplication::palette().highlight().color());
			m_saveHint->setTextWidth((m_view->width() -10 - boundingRect().width() * m_cert->scale()) / 2);
			m_view->scene()->addItem(m_saveHint);
			m_saveHint->setPos(1, m_view->height() / 5);
			m_saveHint->setTextInteractionFlags(Qt::TextBrowserInteraction);
			connect(m_saveHint, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
			
		m_nextHint= new TgraphicsTextTip("<big><a href=\"nextQuest\">" + TexamHelp::toGetQuestTxt() + ":<br>" + 
    TexamHelp::clickSomeButtonTxt(pixToHtml(m_path + "picts/nextQuest.png", 32)) + ",<br>" + TexamHelp::pressSpaceKey() + " " + TexamHelp::orRightButtTxt() + "</a></big>", m_view->palette().highlight().color());
			m_view->scene()->addItem(m_nextHint);
			m_nextHint->setPos(1, m_view->height() / 2);
			m_nextHint->setScale(((m_view->width() - 10 - boundingRect().width() * m_cert->scale()) / 2) / m_nextHint->boundingRect().width());
			m_nextHint->setTextInteractionFlags(Qt::TextBrowserInteraction);
			connect(m_nextHint, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
		
		ic = QIcon(m_path + "picts/stopExam.png");
		m_closeHint = new TgraphicsTextTip("<big><a href=\"stopExam\">" + 
					TexamHelp::toStopExamTxt(pixToHtml(m_path + "picts/stopExam.png", 32) + "</a></big>"), Qt::red);
			m_view->scene()->addItem(m_closeHint);
			m_closeHint->setScale(((m_view->width() - 10 - boundingRect().width() * m_cert->scale()) / 2) / m_closeHint->boundingRect().width());
			m_closeHint->setPos(5 + pos().x() + boundingRect().width() * m_cert->scale(), m_view->height() / 5);
			m_closeHint->setTextInteractionFlags(Qt::TextBrowserInteraction);
			connect(m_closeHint, SIGNAL(linkActivated(QString)), this, SLOT(linkActivatedSlot(QString)));
	}
}


void TnootkaCertificate::removeHints() {
		if (m_closeHint) {
			delete m_closeHint;
			delete m_saveHint;
			delete m_nextHint;
			m_closeHint = 0;
			m_saveHint = 0;
			m_nextHint = 0;
		}
}


QRectF TnootkaCertificate::boundingRect() const {
	return m_cert->boundingRect();
}


void TnootkaCertificate::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
// 	painter->setPen(m_view->palette().text().color());
// 	painter->setBrush(QBrush(QColor("#FFFF00")));
// 	painter->drawRoundedRect(boundingRect().adjusted(0, 0, 10, 10), 10, 10);
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


void TnootkaCertificate::linkActivatedSlot(QString link) {
	if (link == "saveCert")
		saveSlot();
	else 
		emit userAction(link);
}






