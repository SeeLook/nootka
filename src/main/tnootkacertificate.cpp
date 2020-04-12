/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
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
#include "texamexecutor.h"
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <help/texamhelp.h>
#include <tpath.h>
#include "texamview.h"
#include <Android/tfiledialog.h>

#if defined (Q_OS_ANDROID)
  #include <Android/tandroid.h>
#endif

#include <QtWidgets/QtWidgets>
#include <QtPrintSupport/qprinter.h>


#define MARGIN (40.0) // margin of Certificate paper
#define SPACER (10.0) // line space


void alignCenter(QGraphicsTextItem* tip) {
  tip->setTextWidth(tip->boundingRect().width() * tip->scale());
  QTextBlockFormat format;
  format.setAlignment(Qt::AlignCenter);
  QTextCursor cursor = tip->textCursor();
  cursor.select(QTextCursor::Document);
  cursor.mergeBlockFormat(format);
  cursor.clearSelection();
  tip->setTextCursor(cursor);
}


TnootkaCertificate::TnootkaCertificate(QQuickItem* parent) :
  QQuickPaintedItem(parent),
  m_exam(EXECUTOR->exam())
{
  setWidth(EXECUTOR->width() * 0.5);
  setHeight(EXECUTOR->height() * 0.98);
  m_scene = new QGraphicsScene(this);
  m_cert = new QGraphicsRectItem(0.0, 0.0, width(), height());
  m_scene->addItem(m_cert);
  m_cert->setPen(Qt::NoPen);
  m_cert->setBrush(Qt::NoBrush);
//-MARGIN--Nootka Academy--------------------------28 December 2013
//-MARGIN-----------------------------------------------date
  m_academyI = createCertItem(fillCert(tr("<h3>Nootka Academy Of Music</h3>", "top-left corner")));
  m_dateI = createCertItem(fillCert(tr("[DATE]<br><i>date<i>",
      "All those entries assembly a certificate. You can adjust translation to your imagination of it look by manipulating where to put an entry, using some 'HTML tags: http://qt-project.org/doc/qt-4.8/richtext-html-subset.html' and using exam data tags like: [DATE], [STUDENT], [LEVELNAME], [TOTALTIME], [SCORE] and [QUESTNR]. Single entry can't be adjusted (center, left, right) like in pure HTML - this is why it is divided. TO OBTAIN CERT PREVIEW IN ANY TIME OF AN EXAM JUST PRESS: [Shift+Alt+middle-mouse-button] (and don't tell this secret anybody). Feel free to translate it directly as well. This entry is displayed at top-right corner of a cert")));
  alignCenter(m_dateI);
  m_certW = 2 * MARGIN + m_academyI->boundingRect().width() + 2 * m_dateI->boundingRect().width();
  m_academyI->setPos(MARGIN, SPACER);
  m_dateI->setPos(MARGIN + m_academyI->boundingRect().width() + m_dateI->boundingRect().width(), SPACER);
  m_certH = qMax(m_academyI->boundingRect().height(), m_dateI->boundingRect().height()) + 5.0 * SPACER;
//-MARGIN-MARGIN- Student HELMUT has been awarded the
  m_studentI = createCertItem(fillCert(tr("Student <big><b>[STUDENT]</b></big> has been awarded the", "2nd line, single indent")));
  m_studentI->setPos(2 * MARGIN, m_certH);
  m_certH += m_studentI->boundingRect().height() + 4.0 * SPACER;
// -----Certificate Of Exam Completion-------- (middle)
  QString Fake;
  if (!m_exam->isFinished())
    Fake = QStringLiteral("<h3>Translators preview of</h3>");
  m_certHeadI = createCertItem(Fake + tr("<h1>Certificate Of Exam Completion</h1>", "Main header - centered"));
  alignCenter(m_certHeadI);
  if (m_certHeadI->boundingRect().width() > m_certW - 2.0 * SPACER)
    m_certHeadI->setScale((m_certW - 2.0 * SPACER) / m_certHeadI->boundingRect().width());
  m_certHeadI->setPos((m_certW - m_certHeadI->boundingRect().width() * m_certHeadI->scale()) / 2, m_certH);
  m_certHeadI->setPos((m_certW - m_certHeadI->boundingRect().width()) / 2, m_certH);
  m_certH += m_certHeadI->boundingRect().height() + 2.0 * SPACER;
//-MARGIN-MARGIN-Exam results-----------------
  m_resultsI = createCertItem(fillCert(tr("Passing the exam on the level <big><b>[LEVELNAME]</b></big>,<br>having answered the required [QUESTNR] questions<br>in time <big><b>[TOTALTIME]</b></big><br>and achieving the score <big><b>[SCORE]</b></big>", "Exam results - double indented, left aligned")));
  m_resultsI->setPos(2 * MARGIN, m_certH);
  m_certH += m_resultsI->boundingRect().height() + SPACER * 5.0;
//-MARGIN--As a witness to this accomplishment
  m_witnesI = createCertItem(fillCert(tr("As a witness to this accomplishment,<br>we hereby award this certificate on <b>[DATE]</b>.", "Under results - single indent")));
  m_witnesI->setPos(MARGIN, m_certH);
  m_certH += m_witnesI->boundingRect().height() + SPACER;
  //----------------------------------------------- examining board -MARGIN-
  m_boardI = createCertItem(fillCert(tr("<small><i>examining board:</i><br><i>president:</i><b> Nootka itself</b><br><b>professor Processor</b> &amp;<br><b>Mrs RAM</b> his assistant<br><i>secretary:</i><b> Mr Disk</b></small>", "Right aligned and centered")));
  alignCenter(m_boardI);
  m_boardI->setPos(m_certW - MARGIN - m_boardI->boundingRect().width(), m_certH);
  m_certH += m_boardI->boundingRect().height() + SPACER;
// -------------------------(stamp)--------------------- (middle)
  m_stampPixmap = new QGraphicsPixmapItem(QPixmap(Tpath::img("stamp")));
  m_stampPixmap->setParentItem(m_cert);
  m_stampPixmap->setZValue(100);
  m_stampPixmap->setScale((width() / 3.0) / m_stampPixmap->pixmap().size().width());
  qreal stampYpos = m_boardI->pos().y() + m_boardI->boundingRect().height() - 2.0 * SPACER;

  m_stampI = createCertItem(QLatin1String(".......................<br>") + tr("<i>stamp</i>", "bottom, centered"));
  alignCenter(m_stampI);
  m_stampI->setPos((m_certW - m_stampI->boundingRect().width()) / 2, stampYpos + m_stampPixmap->boundingRect().height() * m_stampPixmap->scale() - SPACER);
  m_certH = m_stampI->pos().y() + m_stampI->boundingRect().height() + 2.0 * SPACER;

  m_stampPixmap->setPos((m_certW - m_stampPixmap->boundingRect().width() * m_stampPixmap->scale()) / 2.0,
                        m_stampI->y() + m_stampI->boundingRect().height() / 2 - m_stampPixmap->boundingRect().height() * m_stampPixmap->scale());

  QPixmap bgPix = QPixmap(Tpath::img("certBg")).scaled(m_certW, m_certH);
  auto paper = new QGraphicsPixmapItem(bgPix);
  paper->setParentItem(m_cert);
  paper->setZValue(2);
  auto waterMark = new QGraphicsSimpleTextItem;
  QString bgSymbol;
  if (m_exam->level()->instrument != Tinstrument::NoInstrument)
    bgSymbol = Tinstrument(m_exam->level()->instrument).glyph();
  else
    bgSymbol = QStringLiteral("n");
  QFont nf = QFont(QStringLiteral("nootka"), 20, QFont::Normal);
  QFontMetricsF fm = QFontMetricsF(nf);
  nf.setPointSize(nf.pointSize() * (boundingRect().height() / fm.boundingRect(bgSymbol).height()));
  waterMark->setFont(nf);
  QColor penTrans = QColor(0xE1, 0xE1, 0xE1);
  penTrans.setAlpha(80);
  waterMark->setBrush(QBrush(penTrans));
  waterMark->setParentItem(m_cert);
  waterMark->setZValue(3);
  waterMark->setText(bgSymbol);
  waterMark->setGraphicsEffect(new QGraphicsBlurEffect);
  waterMark->setPos((m_certW - waterMark->boundingRect().width()) / 2, (m_certH - waterMark->boundingRect().height()) / 2 );
}


TnootkaCertificate::~TnootkaCertificate() {
  if (m_sceneImage)
    delete m_sceneImage;
}


void TnootkaCertificate::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  Q_UNUSED(oldGeometry)
  if (newGeometry.width() > 0.0 && newGeometry.height() > 0.0 && m_cert)
    update();
}


void TnootkaCertificate::update() {
  if (m_renderState != e_renderInProgress) {
    m_renderState = e_renderInProgress;
    m_cert->setScale(width() / m_certW);
    m_scene->setSceneRect(0.0, 0.0, width(), height());
    if (m_sceneImage)
      delete m_sceneImage;
    m_sceneImage = new QImage(m_scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
    m_sceneImage->fill(Qt::transparent);
    QPainter painter;
    painter.begin(m_sceneImage);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    m_scene->render(&painter);
    painter.end();
    m_renderState = e_renderFinished;
    QQuickPaintedItem::update();
  }
}


void TnootkaCertificate::paint(QPainter* painter) {
  if (!painter->paintEngine() || boundingRect().width() < 1.0)
    return;

//   m_cert->setScale(height() / m_certH);
//   m_scene->setSceneRect(0.0, 0.0, width(), height());
//   m_scene->render(painter);
  if (m_renderState == e_renderFinished)
    painter->drawImage(0, 0, *m_sceneImage);
}


void TnootkaCertificate::save() {
#if defined (Q_OS_ANDROID)
  QString fileName = TfileDialog::getSaveFileName(Tandroid::getExternalPath() + QLatin1String("/")
                                                  + m_exam->userName() + QLatin1String("-") + m_exam->level()->name,
                                                  QStringLiteral("pdf"));
#else
  QString fileName = TfileDialog::getSaveFileName(tr("Save certificate"),
          QDir::toNativeSeparators(QDir::homePath() + QLatin1String("/") + m_exam->userName() + QLatin1String("-") + m_exam->level()->name),
                                                  QStringLiteral(" (*.pdf)"));
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
  m_scene->render(&painter);
  painter.end();
}


void TnootkaCertificate::stopExam() {
  EXECUTOR->tipLink(QStringLiteral("certClosing"));
  EXECUTOR->tipLink(QStringLiteral("stopExam"));
}


void TnootkaCertificate::continueExam() {
  EXECUTOR->tipLink(QStringLiteral("certClosing"));
  EXECUTOR->tipLink(QStringLiteral("nextQuest"));
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
  auto item = new QGraphicsTextItem;
  item->setParentItem(m_cert);
  item->setZValue(100);
  item->setDefaultTextColor(Qt::black);
  item->setHtml(htmlText);
  return item;
}




