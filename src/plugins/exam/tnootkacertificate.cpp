/***************************************************************************
 *   Copyright (C) 2013-2018 by Tomasz Bojczuk                             *
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


#include "tnootkacertificate.h"
#include <exam/texam.h>
#include <exam/texamview.h>
#include <exam/tlevel.h>
#include <graphics/tgraphicstexttip.h>
#include <graphics/tnotepixmap.h>
#include <help/texamhelp.h>
#include <animations/tcombinedanim.h>
#include <tpath.h>
#include <QtWidgets/QtWidgets>
#include <QtPrintSupport/qprinter.h>
#if defined (Q_OS_ANDROID)
  #include <Android/tandroid.h>
  #include <tfiledialog.h>
  #include <tmtr.h>
#endif


#define MARGIN (40.0) // margin of Certificate paper
#define SPACER (10.0) // line space


TnootkaCertificate::TnootkaCertificate(QGraphicsView* view, Texam* exam) : 
  QGraphicsObject(),
  m_exam(exam),
  m_saveHint(nullptr),
  m_view(view)
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
  QString Fake;
  if (!m_exam->isFinished())
    Fake = QStringLiteral("<h3>Translators preview of</h3>");
  m_certHeadI = createCertItem(Fake + tr("<h1>Certificate Of Exam Completion</h1>", "Main header - centered"));
  TgraphicsTextTip::alignCenter(m_certHeadI);
  if (m_certHeadI->boundingRect().width() > width() - 2 * SPACER)
      m_certHeadI->setScale((width() - 2 * SPACER) / m_certHeadI->boundingRect().width());
  m_certHeadI->setPos((width() - m_certHeadI->boundingRect().width() * m_certHeadI->scale()) / 2, height());
  m_certHeadI->setPos((width() - m_certHeadI->boundingRect().width()) / 2, height());
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
  m_stampPixmap->setZValue(100);
  qreal stampYpos = m_boardI->pos().y() + m_boardI->boundingRect().height() - 2 * SPACER;

  m_stampI = createCertItem(QStringLiteral(".......................<br>") + tr("<i>stamp</i>", "bottom, centered"));
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
      bgSymbol = QStringLiteral("n");
  QFont nf = QFont(QStringLiteral("nootka"), 20, QFont::Normal);
  QFontMetricsF fm = QFontMetricsF(nf);
  nf.setPointSize(nf.pointSize() * (boundingRect().height() / fm.boundingRect(bgSymbol).height()));
  waterMark->setFont(nf);
  QColor penTrans = QColor(QStringLiteral("#E1E1E1"));
  penTrans.setAlpha(80);
  waterMark->setBrush(QBrush(penTrans));
  waterMark->setParentItem(m_cert);
  waterMark->setZValue(3);
  waterMark->setText(bgSymbol);
  waterMark->setGraphicsEffect(new QGraphicsBlurEffect);
  waterMark->setPos((m_width - waterMark->boundingRect().width()) / 2, (m_height - waterMark->boundingRect().height()) / 2 );

  m_stampPixmap->setScale(3.0);
  TcombinedAnim *flyingStamp = new TcombinedAnim(m_stampPixmap, this);
    flyingStamp->setDuration(800);
    flyingStamp->setMoving(QPointF(width() + 50.0, stampYpos - 100.0), QPointF((width() - m_stampPixmap->boundingRect().width()) / 2, stampYpos));
    flyingStamp->setScaling(1.0);
    flyingStamp->startAnimations();
  connect(flyingStamp, SIGNAL(finished()), scene(), SLOT(update()));

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
}


TnootkaCertificate::~TnootkaCertificate() {
	removeHints();
	delete m_bgRect;
}


void TnootkaCertificate::createHints() {
  if (!m_saveHint) {
    QIcon ic = QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton);
    QString br = QStringLiteral("<br>");
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
#if defined (Q_OS_ANDROID)
    QPixmap scaledPix = ic.pixmap(Tmtr::fingerPixels() * 0.7, Tmtr::fingerPixels() * 0.7);
#else
    QPixmap scaledPix = ic.pixmap(32, 32);
#endif
    scaledPix.save(&buffer, "PNG");
#if defined (Q_OS_ANDROID)
    m_saveHint = new ThackedTouchTip(QLatin1String("<big>")
#else
    m_saveHint = new TgraphicsTextTip(QLatin1String("<big>")
#endif
              + tr("CONGRATULATIONS!<br>You have just passed the exam!") + br
#if !defined (Q_OS_ANDROID) // one line break more on desktop
              + br
#endif
              + QLatin1String("<img src=\"data:image/png;base64,")
              + byteArray.toBase64() + QLatin1String("\"/>") + br + tr("Save this certificate to file in remembrance.") + QLatin1String("</big>"),
          qApp->palette().highlight().color());
    m_saveHint->setTextWidth(qMin(pos().x() - 10, m_view->width() / 2.0));
    m_view->scene()->addItem(m_saveHint);
    if (m_saveHint->realH() > m_view->height() * 0.3)
      m_saveHint->setScale((m_view->height() * 0.3) / m_saveHint->boundingRect().height());
    m_saveHint->setPos((pos().x() - m_saveHint->realW()) / 2.0, m_view->height() * 0.03);
    m_saveHint->setZValue(3);
    connect(m_saveHint, &TgraphicsTextTip::clicked, this, &TnootkaCertificate::hintClicked);
#if defined (Q_OS_ANDROID)
    m_nextHint = new ThackedTouchTip(tr("You can still play with it and improve effectiveness.")
#else
    m_nextHint = new TgraphicsTextTip(tr("You can still play with it and improve effectiveness.")
#endif
        + br + QLatin1String("<big>") + TexamHelp::toGetQuestTxt()
#if defined (Q_OS_ANDROID)
        + br + TexamHelp::tapIconTxt(pixToHtml(Tpath::img("nextQuest"), Tmtr::fingerPixels() * 0.7))
#else
        + QLatin1String(":") + br +  TexamHelp::clickSomeButtonTxt(pixToHtml(Tpath::img("nextQuest"), 32))
        + QLatin1String(",") + br + TexamHelp::pressSpaceKey() + QLatin1String(" ") + TexamHelp::orRightButtTxt()
#endif
        + QLatin1String("</big>"), m_view->palette().window().color());
    m_view->scene()->addItem(m_nextHint);
    m_nextHint->setTextWidth(qMin(pos().x() - 10, m_view->width() / 2.0));
    if (m_nextHint->realH() > m_view->height() * 0.3)
      m_nextHint->setScale((m_view->height() * 0.3) / m_nextHint->boundingRect().height());
    m_nextHint->setPos((pos().x() - m_nextHint->realW()) / 2, m_view->height() * 0.4);
    m_nextHint->setZValue(3);
    connect(m_nextHint, &TgraphicsTextTip::clicked, this, &TnootkaCertificate::hintClicked);

    ic = QIcon(Tpath::img("stopExam"));
#if defined (Q_OS_ANDROID)
    m_closeHint = new ThackedTouchTip(QLatin1String("<big>")
#else
    m_closeHint = new TgraphicsTextTip(QLatin1String("<big>")
#endif
        + TexamHelp::toStopExamTxt(pixToHtml(Tpath::img("stopExam"), 32).replace(QLatin1String("<img"), QLatin1String("<br><img"))
        + QLatin1String("</big>")), Qt::red);
    m_view->scene()->addItem(m_closeHint);
    m_closeHint->setTextWidth(qMin(pos().x() - 10, m_view->width() / 2.0));
    if (m_closeHint->realH() > m_view->height() * 0.25)
      m_closeHint->setScale((m_view->height() * 0.24) / m_closeHint->boundingRect().height());
    m_closeHint->setPos((pos().x() - m_closeHint->realW()) / 2.0, m_view->height() * 0.74);
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
#if defined (Q_OS_ANDROID)
  QString fileName = TfileDialog::getSaveFileName(0,
                        Tandroid::getExternalPath() + QLatin1String("/") + m_exam->userName() + QLatin1String("-") + m_exam->level()->name,
                        QStringLiteral("pdf"));
#else
	QString fileName = QFileDialog::getSaveFileName(0, tr("Save certificate"),
          QDir::toNativeSeparators(QDir::homePath() + QLatin1String("/") + m_exam->userName() + QLatin1String("-") + m_exam->level()->name),
                                                  QStringLiteral("*.pdf"));
#endif
	if (fileName.isEmpty())
		return;
  if (fileName.right(4) != QLatin1String(".pdf"))
    fileName += QLatin1String(".pdf");
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
    emit userAction(QStringLiteral("certClosing")); // to inform executor
    if (sender() == m_nextHint)
        emit userAction(QStringLiteral("nextQuest"));
    else 
        emit userAction(QStringLiteral("stopExam"));
  }
}


QString TnootkaCertificate::fillCert(QString entry) {
  entry.replace(QLatin1String("[DATE]"), QDate::currentDate().toString(QStringLiteral("d MMMM yyyy")));
  entry.replace(QLatin1String("[STUDENT]"), m_exam->userName().toUpper());
  entry.replace(QLatin1String("[LEVELNAME]"), m_exam->level()->name);
  entry.replace(QLatin1String("[TOTALTIME]"), TexamView::formatedTotalTime(m_exam->workTime() * 1000));
  entry.replace(QLatin1String("[SCORE]"), QString("%1 %").arg(m_exam->effectiveness(), 0, 'f', 1, '0'));
  entry.replace(QLatin1String("[QUESTNR]"), QString("<b>%1</b>").arg(m_exam->count()));
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




