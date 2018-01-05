/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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

#include "tabout.h"
#include "tnootkalabel.h"
#include <nootkaconfig.h>
#include <widgets/troundedlabel.h>
#include <tpath.h>
#include <QtWidgets/qboxlayout.h>
#include <QtCore/qdatetime.h>
#include <QtWidgets/qgraphicseffect.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qdesktopwidget.h>
#if defined (Q_OS_ANDROID)
  #include <touch/ttoucharea.h>
  #include <tmtr.h>
#endif


// TODO: merge it with commonly used TnootaLabel class
class TheadLabel : public QWidget {

public:

  TheadLabel(QWidget* parent) :
    QWidget(parent)
  {
    qsrand(QDateTime::currentDateTime().toTime_t());
    m_color = QColor(qrand() % 220, qrand() % 220, qrand() % 220);
    QPixmap nootkaLogo(Tpath::img("logo"));
    QPixmap pixmap(nootkaLogo.size());
    pixmap.fill(m_color);
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawPixmap(0, 0, nootkaLogo);
    p.setPen(QPen(Qt::white));
    p.end();
    auto *label = new QLabel(this);
    label->setContentsMargins(0, 0, 0, 0);
#if defined (Q_OS_ANDROID)
    label->setPixmap(pixmap.scaledToHeight(qRound(Tmtr::fingerPixels() * 1.4), Qt::SmoothTransformation));
#else
    label->setPixmap(pixmap.scaledToWidth(qApp->desktop()->width() / 6, Qt::SmoothTransformation));
#endif
    auto colorEffect = new QGraphicsColorizeEffect;
    colorEffect->setColor(m_color);
    label->setGraphicsEffect(colorEffect);
    auto lay = new QHBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(label, 0, Qt::AlignCenter);
    setLayout(lay);
  }

protected:
  void paintEvent(QPaintEvent*) override {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_color));
    p.drawRect(contentsRect());
    p.end();
  }

private:
  QColor                  m_color;
};



Tabout::Tabout(QWidget* parent) :
  QWidget(parent)
{
//  auto nootkaLab = new TnootkaLabel(Tpath::img("logo"), this, palette().highlight().color());
  auto nootkaLab = new TheadLabel(this);
  QString info = tr("Welcome on board.<br>Nootka is an open source application to help you in learning (and in teaching) classical score notation.<br>It is specially designed for guitarists but others will find many usable features as well.");
  QString betaInfo = tr("This is a beta version and may contain bugs or behave in unexpected ways. Also, it has unfinished features.<br>In spite of that, you are welcome to try it!");
  QString moreInfo = tr("See a <a href=\"https://nootka.sourceforge.io\">program site</a> for more details and further releases.</p><p>Any bugs, suggestions, translations and so on, report to: <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><p/><p>with respects<br>Author");
   betaInfo.clear(); // comment it to show info in beta releases
  m_aboutLab = new TroundedLabel(QLatin1String("<center><p style=\"background-color: palette(Base); border: 1px solid palette(Text); border-radius: 10px; font-size: xx-large;\"><b>Nootka ") + QLatin1String(NOOTKA_VERSION) + QLatin1String("</b></p></center><p><br>") + info + QLatin1String("</p><p>") + betaInfo + QLatin1String("</p><p>") + moreInfo + QLatin1String("</p><br>")
#if defined(Q_OS_WIN32)
  + tr("<br>To support this project and also to avoid infection of your PC<br><b>PLEASE, use the official project site to download updates, new releases and to see news.</b>")
#endif
            , this);
  m_aboutLab->setContentsMargins(10, 10, 10, 10);
  m_aboutLab->setWordWrap(true);
  m_aboutLab->setOpenExternalLinks(true);
  m_aboutLab->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

  auto lay = new QVBoxLayout;
    lay->addWidget(nootkaLab);
    lay->addStretch();
    lay->addWidget(m_aboutLab);
    lay->addStretch();
#if defined (Q_OS_ANDROID)
    lay->setContentsMargins(0, 0, 0, 0);
  auto touchArea = new TtouchArea(this);
    touchArea->setLayout(lay);
  auto touchLay = new QVBoxLayout;
    touchLay->addWidget(touchArea);
    touchLay->setContentsMargins(0, 0, 0, 0);
  setLayout(touchLay);
#else
    lay->addStretch();
  setLayout(lay);
#endif
}
