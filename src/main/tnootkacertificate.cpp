/***************************************************************************
 *   Copyright (C) 2013-2021 by Tomasz Bojczuk                             *
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
#include <tpath.h>
#include <nootkaconfig.h>
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
  m_scene = new QGraphicsScene(this);
  m_cert = new QGraphicsRectItem(0.0, 0.0, 200.0, 600.0); // dummy size for a while
  m_scene->addItem(m_cert);
  m_cert->setPen(Qt::NoPen);
  m_cert->setBrush(Qt::NoBrush);
//-MARGIN--Nootka Academy--------------------------28 December 2013
//-MARGIN-----------------------------------------------date
  auto academyIt = createCertItem(fillCert(tr("<h3>Nootka Academy Of Music</h3>", "top-left corner")));
  auto dateIt = createCertItem(fillCert(tr("[DATE]<br><i>date<i>",
      "All those entries assembly a certificate. You can adjust translation to your imagination of it look by manipulating where to put an entry, using some 'HTML tags: http://qt-project.org/doc/qt-4.8/richtext-html-subset.html' and using exam data tags like: [DATE], [STUDENT], [LEVELNAME], [TOTALTIME], [SCORE] and [QUESTNR]. Single entry can't be adjusted (center, left, right) like in pure HTML - this is why it is divided. TO OBTAIN CERT PREVIEW IN ANY TIME OF AN EXAM JUST PRESS: [Shift+Alt+middle-mouse-button] (and don't tell this secret anybody). Feel free to translate it directly as well. This entry is displayed at top-right corner of a cert")));
  alignCenter(dateIt);
  m_certW = 2 * MARGIN + academyIt->boundingRect().width() + 2 * dateIt->boundingRect().width();
  academyIt->setPos(MARGIN, SPACER);
  dateIt->setPos(MARGIN + academyIt->boundingRect().width() + dateIt->boundingRect().width(), SPACER);
  m_certH = qMax(academyIt->boundingRect().height(), dateIt->boundingRect().height()) + 5.0 * SPACER;
//-MARGIN-MARGIN- Student HELMUT has been awarded the
  auto studentIt = createCertItem(fillCert(tr("Student <big><b>[STUDENT]</b></big> has been awarded the", "2nd line, single indent")));
  studentIt->setPos(2 * MARGIN, m_certH);
  m_certH += studentIt->boundingRect().height() + 4.0 * SPACER;
// -----Certificate Of Exam Completion-------- (middle)
  QString Fake;
  if (!m_exam->isFinished())
    Fake = QStringLiteral("<h3>Translators preview of</h3>");
  auto certHeadIt = createCertItem(Fake + tr("<h1>Certificate Of Exam Completion</h1>", "Main header - centered"));
  alignCenter(certHeadIt);
  if (certHeadIt->boundingRect().width() > m_certW - 2.0 * SPACER)
    certHeadIt->setScale((m_certW - 2.0 * SPACER) / certHeadIt->boundingRect().width());
  certHeadIt->setPos((m_certW - certHeadIt->boundingRect().width() * certHeadIt->scale()) / 2, m_certH);
  certHeadIt->setPos((m_certW - certHeadIt->boundingRect().width()) / 2, m_certH);
  m_certH += certHeadIt->boundingRect().height() + 2.0 * SPACER;
//-MARGIN-MARGIN-Exam results-----------------
  auto resultsIt = createCertItem(fillCert(tr("Passing the exam on the level <big><b>[LEVELNAME]</b></big>,<br>having answered the required [QUESTNR] questions<br>in time <big><b>[TOTALTIME]</b></big><br>and achieving the score <big><b>[SCORE]</b></big>", "Exam results - double indented, left aligned")));
  resultsIt->setPos(2 * MARGIN, m_certH);
  m_certH += resultsIt->boundingRect().height() + SPACER * 5.0;
//-MARGIN--As a witness to this accomplishment
  auto witnesIt = createCertItem(fillCert(tr("As a witness to this accomplishment,<br>we hereby award this certificate on <b>[DATE]</b>.", "Under results - single indent")));
  witnesIt->setPos(MARGIN, m_certH);
  m_certH += witnesIt->boundingRect().height() + SPACER;
  //----------------------------------------------- examining board -MARGIN-
  auto boardIt = createCertItem(fillCert(tr("<small><i>examining board:</i><br><i>president:</i><b> Nootka itself</b><br><b>professor Processor</b> &amp;<br><b>Mrs RAM</b> his assistant<br><i>secretary:</i><b> Mr Disk</b></small>", "Right aligned and centered")));
  alignCenter(boardIt);
  boardIt->setPos(m_certW - MARGIN - boardIt->boundingRect().width(), m_certH);
  m_certH += boardIt->boundingRect().height() + SPACER;
// -------------------------(stamp)--------------------- (middle)
  m_stampPixmap = new QGraphicsPixmapItem(QPixmap(Tpath::img("stamp")));
  m_stampPixmap->setParentItem(m_cert);
  m_stampPixmap->setZValue(100);
  m_stampPixmap->setScale((m_certW / 3.0) / m_stampPixmap->pixmap().size().width());
  qreal stampYpos = boardIt->pos().y() + boardIt->boundingRect().height() - 2.0 * SPACER;

  auto stampIt = createCertItem(QLatin1String(".......................<br>") + tr("<i>stamp</i>", "bottom, centered"));
  alignCenter(stampIt);
  stampIt->setPos((m_certW - stampIt->boundingRect().width()) / 2, stampYpos + m_stampPixmap->boundingRect().height() * m_stampPixmap->scale() - SPACER);
  m_certH = stampIt->pos().y() + stampIt->boundingRect().height() + 2.0 * SPACER;

  m_stampPixmap->setPos((m_certW - m_stampPixmap->boundingRect().width() * m_stampPixmap->scale()) / 2.0,
                        stampIt->y() + stampIt->boundingRect().height() / 2 - m_stampPixmap->boundingRect().height() * m_stampPixmap->scale());

  auto bgPix = QPixmap(Tpath::img("certBg")).scaled(m_certW, m_certH);
  auto paper = new QGraphicsPixmapItem(bgPix);
  paper->setParentItem(m_cert);
  paper->setZValue(2);
  auto waterMark = new QGraphicsSimpleTextItem;
  QString bgSymbol;
  if (m_exam->level()->instrument != Tinstrument::NoInstrument)
    bgSymbol = Tinstrument(m_exam->level()->instrument).glyph();
  else
    bgSymbol = QStringLiteral("n");
  QFont nf(QStringLiteral("nootka"), 20, QFont::Normal);
  QFontMetricsF fm(nf);
  nf.setPointSize(nf.pointSize() * (m_certH / fm.boundingRect(bgSymbol).height()));
  fm = QFontMetricsF(nf);
  if (fm.boundingRect(bgSymbol).width() > m_certW * 0.9) // fit wider glyphs to width as well
    nf.setPointSize(nf.pointSize() * (m_certW * 0.9 / fm.boundingRect(bgSymbol).width()));

  waterMark->setFont(nf);
  QColor penTrans = QColor(0xE1, 0xE1, 0xE1);
  penTrans.setAlpha(80);
  waterMark->setBrush(QBrush(penTrans));
  waterMark->setParentItem(m_cert);
  waterMark->setZValue(3);
  waterMark->setText(bgSymbol);
  waterMark->setGraphicsEffect(new QGraphicsBlurEffect);
  waterMark->setPos((m_certW - waterMark->boundingRect().width()) / 2, (m_certH - waterMark->boundingRect().height()) / 2 );
  m_cert->setRect(0.0, 0.0, m_certW, m_certH);
}


TnootkaCertificate::~TnootkaCertificate() {
  if (m_sceneImage)
    delete m_sceneImage;
}


void TnootkaCertificate::setParentHeight(qreal ph) {
  if (ph != m_parentHeight) {
    m_parentHeight = ph;
    qreal scaleWillBe = m_parentHeight / m_cert->boundingRect().height();
    if (parentItem()) {
      if (m_cert->boundingRect().width() * scaleWillBe > parentItem()->width() / 2.0)
        scaleWillBe *= (parentItem()->width() / 2.0) / (m_cert->boundingRect().width() * scaleWillBe);
    }
    m_cert->setScale(scaleWillBe);
    setSize(m_cert->boundingRect().size() * scaleWillBe);
    m_scene->setSceneRect(0.0, 0.0, width(), height());
    emit widthChanged();
    emit heightChanged();
    emit parentHeightChanged();
    update();
  }
}


void TnootkaCertificate::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  Q_UNUSED(oldGeometry)
  if (newGeometry.width() > 0.0 && newGeometry.height() > 0.0 && m_cert) {
    update();
  }
}


void TnootkaCertificate::update() {
  if (m_renderState != e_renderInProgress) {
    m_renderState = e_renderInProgress;
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
  auto userLevel = m_exam->userName() + QLatin1String("-") + m_exam->level()->name;
  userLevel = userLevel.replace(QLatin1String("."), QLatin1String("")); //HACK: file dialogues don't like dots in the names
#if defined (Q_OS_ANDROID)
  QString fileName = TfileDialog::getSaveFileName(Tandroid::getExternalPath() + QLatin1String("/") + userLevel, QStringLiteral("pdf"));
#else
  QString fileName = TfileDialog::getSaveFileName(tr("Save certificate"),
          QDir::toNativeSeparators(QDir::homePath() + QLatin1String("/") + userLevel), QStringLiteral(" (*.pdf)"));
#endif
  if (fileName.isEmpty())
    return;

  if (fileName.right(4) != QLatin1String(".pdf"))
    fileName += QLatin1String(".pdf");
  QPrinter printer;
  printer.setOutputFormat(QPrinter::PdfFormat);
  printer.setColorMode(QPrinter::Color);
  printer.setPageSize(QPageSize(boundingRect().size(), QPageSize::Point, QString(), QPageSize::ExactMatch));
  printer.setFullPage(true);
  printer.setOutputFileName(fileName);
#if defined (Q_OS_LINUX) && !defined(Q_OS_ANDROID)
  printer.setCreator(QLatin1String("Nootka-") + NOOTKA_VERSION);
#endif
  printer.setDocName(m_exam->userName() + QLatin1String("-") + m_exam->level()->name + QLatin1String(" Nootka-") + NOOTKA_VERSION);
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




