/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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


#include "tmaterialmenu.h"
#include <tmtr.h>
#include <tpath.h>
#include <tnootkalabel.h>
#include <touch/ttouchmenu.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qscrollbar.h>
#include <QtWidgets/qscroller.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qstyle.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtCore/qtimer.h>

#include <QtCore/qdebug.h>

TmaterialMenu::TmaterialMenu(QWidget* parent) :
  QWidget(parent),
  m_selectedAction(0),
  m_isMoving(false)
{
  setFixedHeight(Tmtr::shortScreenSide());
  auto w = new QWidget(this);
  m_scrollArea = new QScrollArea(this);
  m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setFrameShape(QFrame::NoFrame);
  m_scrollArea->setWidgetResizable(true);
  m_scrollArea->setWidget(w);
// HACK: to allow paintEvent painting dummy scroll bar over this widget all proxies widgets have to be transparent
  m_scrollArea->viewport()->setObjectName(QStringLiteral("menuView"));
  m_scrollArea->viewport()->setStyleSheet(QStringLiteral("QWidget#menuView { background: transparent; }"));
  w->setObjectName(QStringLiteral("menuWidget"));
  w->setStyleSheet(QStringLiteral("QWidget#menuWidget { background: transparent; }"));
  m_scrollArea->setObjectName(QStringLiteral("menuArea"));
  m_scrollArea->setStyleSheet(QStringLiteral("QScrollArea#menuArea { background: transparent; }"));

  m_nootkaLabel = new TnootkaLabel(Tpath::img("logo"), this, Qt::darkGray, QStringLiteral("1.3.0-alpha"));

  m_lay = new QVBoxLayout;
  m_lay->setContentsMargins(2, 5, 10, 0);
  m_lay->addWidget(m_nootkaLabel);
  m_lay->addStretch(); // Add stretch in case all actions (buttons) don't fill whole visible space (screen height)
  w->setContentsMargins(0, 0, 0, 0);
  w->setLayout(m_lay);

  auto mainLay = new QVBoxLayout;
  mainLay->setContentsMargins(0, 0, 0, 0);
  mainLay->addWidget(m_scrollArea);
  setLayout(mainLay);

  m_hideScrollTimer = new QTimer(this);
  m_hideScrollTimer->setSingleShot(true);
  connect(m_hideScrollTimer, &QTimer::timeout, this, [this]{
      m_isMoving = false;
      m_hideScrollTimer->stop();
      update();
  });

  QScroller::grabGesture(m_scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
  auto scroller = QScroller::scroller(m_scrollArea->viewport());
  QScrollerProperties prop = scroller->scrollerProperties();
  QVariant overshootPolicy = QVariant::fromValue<QScrollerProperties::OvershootPolicy>(QScrollerProperties::OvershootAlwaysOff);
  prop.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, overshootPolicy);
  scroller->setScrollerProperties(prop);
  connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this]{
      m_isMoving = true;
      m_hideScrollTimer->start(500);
      update();
  });
}


void TmaterialMenu::addAction(QAction* a) {
  auto butt = new QPushButton(a->icon(), a->text(), this);
  butt->setFlat(true);
  butt->setIconSize(QSize(style()->pixelMetric(QStyle::PM_SmallIconSize), style()->pixelMetric(QStyle::PM_SmallIconSize)));
  butt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  butt->setStyleSheet(QStringLiteral("text-align: left;"));
//   butt->setFont(QFont(font().family(), 20));
  if (m_lay->count() > 1) // some button has been already added
    m_lay->addWidget(new TlineSpacer(1, this));
  if (a->isCheckable()) {
      auto chB = new QCheckBox(this);
      auto lay = new QHBoxLayout;
      lay->addWidget(butt);
      lay->addWidget(chB);
      chB->setChecked(a->isChecked());
      connect(chB, &QCheckBox::clicked, butt, &QPushButton::click);
      m_lay->insertLayout(m_lay->count(), lay); // Squeeze it before last element which is a stretch.
  } else
      m_lay->insertWidget(m_lay->count(), butt);
  butt->insertAction(0, a);
  connect(butt, &QPushButton::clicked, this, [this]{
      m_selectedAction = qobject_cast<QWidget*>(sender())->actions().first();
      if (m_menu)
        m_menu->close();
  });
}


QAction* TmaterialMenu::exec() {
  m_selectedAction = 0;
  setFixedWidth(m_scrollArea->sizeHint().width());
  auto menuLay = new QVBoxLayout;
  menuLay->setContentsMargins(0, 0, 0, 0);
  menuLay->addWidget(this);
  m_menu = new TtouchMenu;
  m_menu->setLayout(menuLay);
  m_menu->setFixedSize(sizeHint().width(), Tmtr::shortScreenSide());
  m_menu->setContentsMargins(0, 0, 0, 0);
  m_menu->exec(QPoint(), QPoint(-width(), 0));
  menuLay->removeWidget(this);
  setParent(0);
  delete m_menu;
  return m_selectedAction;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmaterialMenu::paintEvent(QPaintEvent* event) {
  if (m_isMoving) {
    QPainter p(this);
    p.setPen(Qt::NoPen);
    QColor bg(palette().text().color());
    bg.setAlpha(50);
    p.setBrush(bg);
    p.drawRect(width() - 8, 2, 6, height() - 4);
    p.setBrush(palette().highlight());
    p.drawRect(width() - 8,
               ((height() - 54) /
                qAbs<int>(m_scrollArea->verticalScrollBar()->maximum() - m_scrollArea->verticalScrollBar()->minimum())) *
                m_scrollArea->verticalScrollBar()->value(),
               6, 100);
  }
}

//#################################################################################################
//###################               TlineSpacer        ############################################
//#################################################################################################

TlineSpacer::TlineSpacer(int lineWidth, QWidget* parent) :
  m_lineWidth(lineWidth),
  QWidget(parent)
{
  setFixedHeight(m_lineWidth + 2);
  setContentsMargins(0, 0, 0, 0);
}


void TlineSpacer::paintEvent(QPaintEvent* e) {
  QPainter p(this);
  QColor lc(palette().text().color());
  lc.setAlpha(100);
  p.setPen(QPen(lc, m_lineWidth, Qt::DashLine));
  p.drawLine(10, 1 + m_lineWidth / 2, width() - 10, 1 + m_lineWidth / 2);
}




