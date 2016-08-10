/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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
#include <exam/texam.h>
#include "texamview.h"
#include <exam/tlevel.h>
#include <graphics/tgraphicstexttip.h>
#include <graphics/tnotepixmap.h>
#include <help/texamhelp.h>
#include <animations/tcombinedanim.h>
#include <tpath.h>
#include <QDate>
#include <QBuffer>
#include <QApplication>
#include <QStyle>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QtPrintSupport/QPrinter>
#include <QPainter>
#include <QGraphicsView>
#include <QGraphicsEffect>
#include <QDebug>

#define MARGIN (40.0) // margin of Certificate paper
#define SPACER (10.0) // line space


TnootkaCertificate::TnootkaCertificate(QGraphicsView* view, Texam* exam) : 
	QGraphicsObject(),
	m_view(view),
  m_exam(exam),
  m_saveHint(0)
{
		setFlag(ItemHasNoContents);
		m_view->scene()->addItem(this);
		setZValue(105); // higher than result tips
    m_cert = new QGraphicsRectItem;
      m_cert->setParentItem(this);
      m_cert->setPen(Qt::NoPen);
      m_cert->setBrush(Qt::NoBrush);
  //-MARGIN--Nootka Academy--------------------------28 December 2013
  //-MARGIN-----------------------------------------------date
    m_academyI = createCertItem(fillCert(tr("<h3>Nootka Academy Of Music</h3>", "top-left corner")));
    m_dateI = createCertItem(fillCert(tr("[DATE]<br><i>date<i>", 
				"All those entries assembly a certificate. You can adjust translation to your imagination of it look by manipulating where to put an entry, using some 'HTML tags: http://qt-project.org/doc/qt-4.8/richtext-html-subset.html' and using exam data tags like: [DATE], [STUDENT], [LEVELNAME], [TOTALTIME], [SCORE] and [QUESTNR]. Single entry can't be adjusted (center, left, right) like in pure HTML - this is why it is divided. TO OBTAIN CERT PREVIEW IN ANY TIME OF AN EXAM JUST PRESS: [Shift+Alt+middle-mouse-button] (and don't tell this secret anybody). Feel free to translate it directly as well. This entry is displayed at top-right corner of a cert")));
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
    QString Fake = "";
    if (!m_exam->isFinished())
      Fake = "<h3>Translators preview of</h3>";
    m_certHeadI = createCertItem(Fake + tr("<h1>Certificate Of Exam Completion</h1>", "Main header - centered"));
		TgraphicsTextTip::alignCenter(m_certHeadI);
    if (m_certHeadI->boundingRect().width() > width() - 2 * SPACER)
        m_certHeadI->setScale((width() - 2 * SPACER) / m_certHeadI->boundingRect().width());
    m_certHeadI->setPos((width() - m_certHeadI->boundingRect().width() * m_certHeadI->scale()) / 2, height());
    m_height += m_certHeadI->boundingRect().height() + 2 * SPACER;
  //-MARGIN-MARGIN-Exam results-----------------
  //-MARGIN-MARGIN-Exam results-----------------
  //-MARGIN-MARGIN-Exam results-----------------
    m_resultsI = createCertItem(fillCert(tr("Passing the exam on the level <big><b>[LEVELNAME]</b></big>,<br>having answered the required [QUESTNR] questions<br>in time <big><b>[TOTALTIME]</b></big><br>and achieving the score <big><b>[SCORE]</b></big>", "Exam results - double indented, left aligned")));
    m_resultsI->setPos(2 * MARGIN, height());
    m_height += m_resultsI->boundingRect().height() + SPACER * 5;
  //-MARGIN--As a witness to this accomplishment
    m_witnesI = createCertItem(fillCert(tr("As a witness to this accomplishment,<br>we hereby award this certificate on <b>[DATE]</b>.", "Under results - single indent")));
    m_witnesI->setPos(MARGIN, height());
    m_height += m_witnesI->boundingRect().height() + SPACER;
    //----------------------------------------------- examining board -MARGIN-
    m_boardI = createCertItem(fillCert(tr("<small><i>examining board:</i><br><i>president:</i><b> Nootka itself</b><br><b>professor Processor</b> &amp;<br><b>Mrs RAM</b> his assistant<br><i>secretary:</i><b> Mr Disk</b></small>", "Right aligned and centered")));
    TgraphicsTextTip::alignCenter(m_boardI);
    m_boardI->setPos(width() - MARGIN - m_boardI->boundingRect().width(), height());
    m_height += m_boardI->boundingRect().height() + SPACER;
  // -------------------------(stamp)--------------------- (middle)
    m_stampPixmap = new QGraphicsPixmapItem(QPixmap(Tpath::img("stamp")));
    m_stampPixmap->setParentItem(m_cert);
    m_stampPixmap->setScale(3.0);
    m_stampPixmap->setZValue(100);
    qreal stampYpos = m_boardI->pos().y() + m_boardI->boundingRect().height() - 2 * SPACER;
    
    m_stampI = createCertItem(".......................<br>" + tr("<i>stamp</i>", "bottom, centered"));
    TgraphicsTextTip::alignCenter(m_stampI);
    m_stampI->setPos((width() - m_stampI->boundingRect().width()) / 2, stampYpos + m_stampPixmap->boundingRect().height() - SPACER);
    m_height = m_stampI->pos().y() + m_stampI->boundingRect().height() + 2 * SPACER;
    
    QPixmap bgPix = QPixmap(Tpath::img("certBg")).scaled(m_width, m_height);
    QGraphicsPixmapItem *paper = new QGraphicsPixmapItem(bgPix);
			paper->setParentItem(m_cert);
			paper->setZValue(2);
		QGraphicsSimpleTextItem *waterMark = new QGraphicsSimpleTextItem;
		QString bgSymbol;
		if (m_exam->level()->instrument != e_noInstrument)
				bgSymbol = instrumentToGlyph(m_exam->level()->instrument);
		else
				bgSymbol = "n";
		QFont nf = QFont("nootka", 20, QFont::Normal);
		QFontMetricsF fm = QFontMetricsF(nf);
		nf.setPointSize(nf.pointSize() * (boundingRect().height() / fm.boundingRect(bgSymbol).height()));
		waterMark->setFont(nf);
		QColor penTrans = QColor("#E1E1E1");
		penTrans.setAlpha(80);
		waterMark->setBrush(QBrush(penTrans));
		waterMark->setParentItem(m_cert);
		waterMark->setZValue(3);
		waterMark->setText(bgSymbol);
		waterMark->setGraphicsEffect(new QGraphicsBlurEffect);
		waterMark->setPos((m_width - waterMark->boundingRect().width()) / 2, (m_height - waterMark->boundingRect().height()) / 2 );
    
    TcombinedAnim *flyingStamp = new TcombinedAnim(m_stampPixmap, this);
      flyingStamp->setDuration(750);
      flyingStamp->setMoving(QPointF(width() + 50.0, stampYpos - 100.0), QPointF((width() - m_stampPixmap->boundingRect().width()) / 2, stampYpos));
      flyingStamp->setScaling(1.0);
      flyingStamp->startAnimations();
    
    if (height() != m_view->sceneRect().height() * 0.96) {
      m_cert->setScale((m_view->sceneRect().height() * 0.96) / height());
    }
    setPos((m_view->width() - m_cert->scale() * width()) - MARGIN, (m_view->height() - m_cert->scale() * height()) / 2);
    
    QPixmap viewPix(m_view->size());
    QColor pixBgColor = m_view->palette().text().color();
    pixBgColor.setAlpha(210);
    viewPix.fill(pixBgColor);
    QPainter painter(&viewPix);
    m_view->render(&painter);
		m_bgRect = new QGraphicsPixmapItem(viewPix);
      QGraphicsBlurEffect *viewBlur = new QGraphicsBlurEffect();
      viewBlur->setBlurRadius(10);
    m_bgRect->setGraphicsEffect(viewBlur);
			m_view->scene()->addItem(m_bgRect);
			m_bgRect->setZValue(1);
      m_bgRect->setAcceptHoverEvents(true);
		setAcceptHoverEvents(true);
		createHints();
    
		connect(flyingStamp, SIGNAL(finished()), scene(), SLOT(update()));
}


TnootkaCertificate::~TnootkaCertificate() {
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
      m_saveHint->setZValue(3);
      connect(m_saveHint, &TgraphicsTextTip::clicked, this, &TnootkaCertificate::hintClicked);
			
		m_nextHint = new TgraphicsTextTip(tr("You can still play with it and improve effectiveness.") + "<br><big>" + TexamHelp::toGetQuestTxt() + ":<br>" +  TexamHelp::clickSomeButtonTxt(pixToHtml(Tpath::img("nextQuest"), 32)) + 
    ",<br>" + TexamHelp::pressSpaceKey() + " " + TexamHelp::orRightButtTxt() + "</big>", m_view->palette().window().color());
			m_view->scene()->addItem(m_nextHint);
      m_nextHint->setTextWidth((m_view->width() -10 - boundingRect().width()) / 2);
			m_nextHint->setPos((pos().x() - m_nextHint->boundingRect().width()) / 2, m_view->height() / 2);
      m_nextHint->setZValue(3);
      connect(m_nextHint, &TgraphicsTextTip::clicked, this, &TnootkaCertificate::hintClicked);
		
		ic = QIcon(Tpath::img("stopExam"));
		m_closeHint = new TgraphicsTextTip("<big>" + TexamHelp::toStopExamTxt(pixToHtml(Tpath::img("stopExam"), 32).replace("<img", "<br><img") + "</big>"), Qt::red);
			m_view->scene()->addItem(m_closeHint);
      m_closeHint->setTextWidth((m_view->width() -10 - boundingRect().width()) / 2);
			m_closeHint->setPos((pos().x() - m_closeHint->boundingRect().width()) / 2,
                          m_view->height() - m_closeHint->boundingRect().height() * m_closeHint->scale() - 20.0);
      m_closeHint->setZValue(3);
      connect(m_closeHint, &TgraphicsTextTip::clicked, this, &TnootkaCertificate::hintClicked);
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
}


void TnootkaCertificate::saveSlot() {
	QString fileName = QFileDialog::getSaveFileName(0, tr("Save certificate"),
          QDir::toNativeSeparators(QDir::homePath() + "/" + m_exam->userName() + "-" + m_exam->level()->name), "*.pdf");
	if (fileName == "")
		return;
  if (fileName.right(4) != ".pdf")
    fileName += ".pdf";
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
  if (sender() == m_saveHint)
    saveSlot();
  else if (sender() == m_nextHint || sender() == m_closeHint) {
    emit userAction("certClosing"); // to inform executor 
    if (sender() == m_nextHint)
        emit userAction("nextQuest");
    else 
        emit userAction("stopExam");
  }
}


QString TnootkaCertificate::fillCert(QString entry) {
  entry.replace("[DATE]", QDate::currentDate().toString("d MMMM yyyy"));
  entry.replace("[STUDENT]", m_exam->userName().toUpper());
  entry.replace("[LEVELNAME]", m_exam->level()->name);
  entry.replace("[TOTALTIME]", TexamView::formatedTotalTime(m_exam->workTime() * 1000));
  entry.replace("[SCORE]", QString("%1 %").arg(m_exam->effectiveness(), 0, 'f', 1, '0'));
  entry.replace("[QUESTNR]", QString("<b>%1</b>").arg(m_exam->count()));
  return entry;
}


QGraphicsTextItem* TnootkaCertificate::createCertItem(const QString& htmlText) {
  QGraphicsTextItem* item = new QGraphicsTextItem;
    item->setParentItem(m_cert);
    item->setZValue(100);
    item->setDefaultTextColor(Qt::black);
    item->setHtml(htmlText);  
  return item;
}




