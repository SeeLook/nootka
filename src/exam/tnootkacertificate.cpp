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
#include <animations/tcombinedanim.h>
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

#include <QDebug>

#define MARGIN (40.0) // margin of Certificate paper
#define SPACER (10.0) // line space


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
    m_cert = new QGraphicsRectItem;
      m_cert->setParentItem(this);
      m_cert->setPen(Qt::NoPen);
      m_cert->setBrush(Qt::NoBrush);
  //-MARGIN--Nootka Akademy--------------------------28 December 2013
  //-MARGIN-----------------------------------------------date
    m_academyI = createCertItem(tr("<h3>Nootka Akademy Of Music</h3>", "top-left corner"));
    m_dateI = createCertItem(fillCert(tr("[DATE]<br><i>date<i>", "top-right corner")));
    TgraphicsTextTip::alignCenter(m_dateI);
    m_width = 2 * MARGIN + m_academyI->boundingRect().width() + 2 * m_dateI->boundingRect().width();
    m_academyI->setPos(MARGIN, SPACER);
    m_dateI->setPos(MARGIN + m_academyI->boundingRect().width() + m_dateI->boundingRect().width(), SPACER);
    m_height = qMax(m_academyI->boundingRect().height(), m_dateI->boundingRect().height()) + 5 * SPACER;
  //-MARGIN-MARGIN- Student HELMUT has been awarded the
    m_studentI = createCertItem(fillCert(tr("Student <big><b>[STUDENT]</b></big> has been awarded the", "2nd line, single indent")));
    m_studentI->setPos(2 * MARGIN, height());
    m_height += m_studentI->boundingRect().height() + 4 * SPACER;
  // -----Certificate Of Exam Completion-------- (middle)
    m_certHeadI = createCertItem(tr("<h1>Certificate Of Exam Completion</h1>", "Main header - centered"));
//     m_certHeadI->setGraphicsEffect(new QGraphicsDropShadowEffect);
    m_certHeadI->setPos((width() - m_certHeadI->boundingRect().width()) / 2, height());
    m_height += m_certHeadI->boundingRect().height() + 2 * SPACER;
  //-MARGIN-MARGIN-Exam results-----------------
  //-MARGIN-MARGIN-Exam results-----------------
  //-MARGIN-MARGIN-Exam results-----------------
    m_resultsI = createCertItem(fillCert(tr("Passing the exam on the level <big><b>[LEVELNAME]</b></big>,<br>having answered the required [NR_QUESTIONS]<br>in time <big><b>[TOTALTIME]</b></big><br>and achieving the score <big><b>[SCORE]</b></big>", "Exam results - double indented, [NR_QUESTIONS] is fe.: 23 questions but only number is available by [JUSTNR]")));
    m_resultsI->setPos(2 * MARGIN, height());
    m_height += m_resultsI->boundingRect().height() + SPACER * 5;
  //-MARGIN--As a witness to this accomplishment
    m_witnesI = createCertItem(fillCert(tr("As a witness to this accomplishment,<br>we hereby award this certificate on <b>[DATE]</b>.", "Under results - single indent")));
    m_witnesI->setPos(MARGIN, height());
    m_height += m_witnesI->boundingRect().height() + SPACER;
    //----------------------------------------------- examining board -MARGIN-
    m_boardI = createCertItem(tr("<small><i>examining board:</i><br><i>president:</i><b> Nootka itself</b><br><b>professor Processor</b> &amp;<br><b>Mrs RAM</b> his assistant<br><i>secretary:</i><b> Mr Disk</b></small>", "Right aligned and centered"));
    TgraphicsTextTip::alignCenter(m_boardI);
    m_boardI->setPos(width() - MARGIN - m_boardI->boundingRect().width(), height());
    m_height += m_boardI->boundingRect().height() + SPACER;
  // -------------------------(stamp)--------------------- (middle)
    m_stampPixmap = new QGraphicsPixmapItem(QPixmap(m_path + "picts/stamp.png"));
//     m_stampPixmap->hide();
    m_stampPixmap->setParentItem(m_cert);
    qreal stampYpos = m_boardI->pos().y() + m_boardI->boundingRect().height() - 2 * SPACER;
    
    m_stampI = createCertItem(".......................<br>" + tr("<i>stamp</i>", "bottom, centered"));
    TgraphicsTextTip::alignCenter(m_stampI);
    m_stampI->setPos((width() - m_stampI->boundingRect().width()) / 2, stampYpos + m_stampPixmap->boundingRect().height() - SPACER);
    m_height = m_stampI->pos().y() + m_stampI->boundingRect().height() + 2 * SPACER;
    
    m_stampPixmap->setScale(3.0);
    TcombinedAnim *flyingStamp = new TcombinedAnim(m_stampPixmap, this);
      flyingStamp->setDuration(750);
      flyingStamp->setMoving(QPointF(width() + 50.0, stampYpos - 100.0), QPointF((width() - m_stampPixmap->boundingRect().width()) / 2, stampYpos));
//       flyingStamp->moving()->setEasingCurveType(QEasingCurve::InOutQuint);
      flyingStamp->setScaling(1.0);
//       flyingStamp->scaling()->setEasingCurveType(QEasingCurve::InOutQuint);
      flyingStamp->startAnimations();
    
    
    
    if (height() != m_view->sceneRect().height() * 0.96) {
      m_cert->setScale((m_view->sceneRect().height() * 0.96) / height());
      qDebug() << "Certificate scaled of" << m_cert->scale();
    }
    
    setPos((m_view->width() - m_cert->scale() * width()) - MARGIN, (m_view->height() - m_cert->scale() * height()) / 2);
    
		m_bgRect = new QGraphicsRectItem(m_view->sceneRect());
    m_bgRect->setPen(Qt::NoPen);
		QColor bg = m_view->palette().text().color();
			bg.setAlpha(170);
			m_bgRect->setBrush(QBrush(bg));
			m_view->scene()->addItem(m_bgRect);
			m_bgRect->setZValue(0);
		setAcceptHoverEvents(true);
		createHints();
    
    connect(scene(), SIGNAL(selectionChanged()), this, SLOT(hintClicked()));
    
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
		m_saveHint = new TgraphicsTextTip("<big>" + tr("CONGRATULATIONS!<br>You have just passed the exam!") + "<br><br><img src=\"data:image/png;base64," + byteArray.toBase64() + "\"/><br>" + tr("Save this certificate to file in remembrance.") + "</big>",
																				QApplication::palette().highlight().color());
			m_saveHint->setTextWidth((pos().x() - 10));
			m_view->scene()->addItem(m_saveHint);
			m_saveHint->setPos((pos().x() - m_saveHint->boundingRect().width()) / 2, 20.0);
      m_saveHint->setFlag(QGraphicsItem::ItemIsSelectable);
			
		m_nextHint = new TgraphicsTextTip(tr("You can still play with it and improve effectiveness.") + "<br><big>" + TexamHelp::toGetQuestTxt() + ":<br>" +  TexamHelp::clickSomeButtonTxt(pixToHtml(m_path + "picts/nextQuest.png", 32)) + 
    ",<br>" + TexamHelp::pressSpaceKey() + " " + TexamHelp::orRightButtTxt() + "</big>", m_view->palette().window().color());
			m_view->scene()->addItem(m_nextHint);
      m_nextHint->setTextWidth((m_view->width() -10 - boundingRect().width()) / 2);
			m_nextHint->setPos((pos().x() - m_nextHint->boundingRect().width()) / 2, m_view->height() / 2);
      m_nextHint->setFlag(QGraphicsItem::ItemIsSelectable);
		
		ic = QIcon(m_path + "picts/stopExam.png");
		m_closeHint = new TgraphicsTextTip("<big>" + TexamHelp::toStopExamTxt(pixToHtml(m_path + "picts/stopExam.png", 32).replace("<img", "<br><img") + "</big>"), Qt::red);
			m_view->scene()->addItem(m_closeHint);
      m_closeHint->setTextWidth((m_view->width() -10 - boundingRect().width()) / 2);
			m_closeHint->setPos((pos().x() - m_closeHint->boundingRect().width()) / 2,
                          m_view->height() - m_closeHint->boundingRect().height() * m_closeHint->scale() - 20.0);
      m_closeHint->setFlag(QGraphicsItem::ItemIsSelectable);
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
	return QRect(0, 0, m_width * m_cert->scale(), m_height  * m_cert->scale());
}


void TnootkaCertificate::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
	painter->setPen(m_view->palette().text().color());
  QImage bgImage(m_path + "picts/certBg.png");
  painter->drawImage(boundingRect(), bgImage, bgImage.rect());
  QString bgSymbol;
  if (m_exam->level()->instrument != e_noInstrument)
      bgSymbol = instrumentToGlyph(m_exam->level()->instrument);
  else
      bgSymbol = "n";
  QFont nf = QFont("nootka", 20, QFont::Normal);
  QFontMetricsF fm = QFontMetricsF(nf);
  nf.setPointSize(nf.pointSize() * (boundingRect().height() / fm.boundingRect(bgSymbol).height()));
  painter->setFont(nf);
  QColor penTrans = QColor("#E1E1E1");
  penTrans.setAlpha(100);
  painter->setPen(QPen(penTrans));
  painter->drawText(boundingRect(), Qt::AlignCenter, bgSymbol);
}


void TnootkaCertificate::saveSlot() {
	QString fileName = QFileDialog::getSaveFileName(0, tr("Save certificate"),
										QDir::toNativeSeparators(QDir::homePath() + "/" +
												m_exam->userName() + "-" + m_exam->level()->name + ".pdf"), "*.pdf", 0 , QFileDialog::DontUseNativeDialog);
	if (fileName == "")
		return;
	QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setColorMode(QPrinter::Color);
    printer.setPaperSize(boundingRect().size(), QPrinter::Point);
    printer.setFullPage(true);
    printer.setOutputFileName(fileName);
	QPainter painter;
	painter.begin(&printer);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	scene()->render(&painter, painter.viewport(), QRectF(pos(), boundingRect().size()));
	painter.end();
}


void TnootkaCertificate::hintClicked() {
  if (m_saveHint->isSelected())
    saveSlot();
  else if (m_nextHint->isSelected()) {
    emit userAction("nextQuest");
    disconnect(scene(), SIGNAL(selectionChanged()), this, SLOT(hintClicked()));
  } else if (m_closeHint->isSelected())
    emit userAction("stopExam");
}


QString TnootkaCertificate::fillCert(QString entry) {
  entry.replace("[DATE]", QDate::currentDate().toString("d MMMM yyyy"));
  entry.replace("[STUDENT]", m_exam->userName().toUpper());
  entry.replace("[LEVELNAME]", m_exam->level()->name);
  entry.replace("[NR_QUESTIONS]", tr("<big><b>%n</b></big> question", "", m_exam->count()));
  entry.replace("[TOTALTIME]", TexamView::formatedTotalTime(m_exam->workTime() * 1000));
  entry.replace("[SCORE]", QString("%1 %").arg(m_exam->effectiveness(), 0, 'f', 1, '0'));
  entry.replace("[JUSTNR]", QString("%1").arg(m_exam->count()));
  return entry;
}


QGraphicsTextItem* TnootkaCertificate::createCertItem(const QString& htmlText) {
  QGraphicsTextItem* item = new QGraphicsTextItem;
    item->setParentItem(m_cert);
    item->setDefaultTextColor(Qt::black);
    item->setHtml(htmlText);  
  return item;
}




