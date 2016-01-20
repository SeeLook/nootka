/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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
#include <tpath.h>
#include <nootkaconfig.h>
#include <touch/ttouchmenu.h>
#include <QtWidgets/QtWidgets>
#include <QtGui/QtGui>
#include <QtCore/qtimer.h>
#include <QtCore/qdatetime.h>
#include <tmtr.h>
#include <QtWidgets/qlabel.h>

#define ITEM_WIDTH (Tmtr::fingerPixels() * 5) // also max width of entire menu (plus margins)

//=================================================================================
//                                 class TmaterialMenu
//=================================================================================
TmaterialMenu::TmaterialMenu(QWidget* parent) :
  QWidget(parent),
  m_selectedAction(0),
  m_isMoving(false),
  m_aboutAction(0)
{
  setFixedHeight(qApp->desktop()->availableGeometry().height());
  auto w = new QWidget(this);
  m_scrollArea = new QScrollArea(this);
  m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setFrameShape(QFrame::NoFrame);
  m_scrollArea->setWidgetResizable(true);
  m_scrollArea->setWidget(w);
// HACK: to allow paintEvent painting dummy scroll bar over this widget all proxy widgets have to be transparent
  m_scrollArea->viewport()->setObjectName(QStringLiteral("menuView"));
  m_scrollArea->viewport()->setStyleSheet(QStringLiteral("QWidget#menuView { background: transparent; }"));
  w->setObjectName(QStringLiteral("menuWidget"));
  w->setStyleSheet(QStringLiteral("QWidget#menuWidget { background: transparent; }"));
  m_scrollArea->setObjectName(QStringLiteral("menuArea"));
  m_scrollArea->setStyleSheet(QStringLiteral("QScrollArea#menuArea { background: transparent; }"));

  m_nootkaLabel = new TlabelWidget(this);

  m_lay = new QVBoxLayout;
  m_lay->setContentsMargins(2, 0, 10, 0);
  m_lay->addWidget(m_nootkaLabel);
  m_lay->setSpacing(0);
  auto spaceWidget = new QWidget(this);
  spaceWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  m_lay->addWidget(spaceWidget);
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
  prop.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, overshootPolicy);
  scroller->setScrollerProperties(prop);
  connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this]{
      m_isMoving = true;
      m_hideScrollTimer->start(500);
      update();
  });
}


void TmaterialMenu::addAction(QAction* a) {
  auto it = new TmenuItem(a, this);
  m_lay->insertWidget(m_lay->count() - 1, it); // the last is stretch to keep items up
  connect(it, &TmenuItem::clicked, this, &TmaterialMenu::menuItemClicked);
  it->installEventFilter(this);
}


void TmaterialMenu::setAboutAction(QAction* a) {
  m_aboutAction = a;
  m_nootkaLabel->addAction(a);
  connect(m_nootkaLabel, &TlabelWidget::clicked, this, &TmaterialMenu::menuItemClicked, Qt::UniqueConnection);
}


QAction* TmaterialMenu::exec() {
  m_selectedAction = 0;
  setFixedWidth(m_scrollArea->sizeHint().width());
  auto menuLay = new QVBoxLayout;
  menuLay->setContentsMargins(0, 0, 0, 0);
  menuLay->addWidget(this);
  m_menu = new TtouchMenu;
  m_menu->setLayout(menuLay);
  m_menu->setFixedSize(sizeHint().width(), qApp->desktop()->availableGeometry().height());
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
  if (m_nootkaLabel || m_isMoving) {
    QPainter p(this);
    p.setPen(Qt::NoPen);
    if (m_nootkaLabel) {
      p.setBrush(m_nootkaLabel->color());
      p.drawRect(0, 0, width(), m_nootkaLabel->height() + 1 - m_scrollArea->verticalScrollBar()->value());
    }
    if (m_isMoving) {
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
}


void TmaterialMenu::menuItemClicked() {
  m_selectedAction = qobject_cast<QWidget*>(sender())->actions().first();
  if (m_menu)
    m_menu->close();
}


bool TmaterialMenu::eventFilter(QObject* ob, QEvent* e) {
  if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseMove || e->type() == QEvent::MouseButtonRelease) {
//     if (m_menu)
//       qApp->notify(m_menu, e);
  }
  return QObject::eventFilter(ob, e);
}


//=================================================================================
//                            class TlabelWidget
//=================================================================================
TlabelWidget::TlabelWidget(QWidget* parent) :
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
    QFont f(font());
    f.setPixelSize(20);
    p.setFont(f);
    p.drawText(QRect(0, pixmap.height() - p.fontMetrics().height(), pixmap.width(), p.fontMetrics().height()),
             Qt::AlignCenter, QStringLiteral(NOOTKA_VERSION) + QLatin1String("   "));
    p.end();
  auto *label = new QLabel(this);
    label->setContentsMargins(0, 0, 0, 0);
    label->setPixmap(pixmap.scaledToWidth(ITEM_WIDTH, Qt::SmoothTransformation));
    auto colorEffect = new QGraphicsColorizeEffect;
      colorEffect->setColor(m_color);
    label->setGraphicsEffect(colorEffect);
  auto lay = new QHBoxLayout;
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(label);
  setLayout(lay);
}


void TlabelWidget::paintEvent(QPaintEvent* e) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);
  p.setPen(Qt::NoPen);
  p.setBrush(QBrush(m_color));
  p.drawRect(contentsRect());
  p.end();
}


void TlabelWidget::mouseReleaseEvent(QMouseEvent* e) {
  QWidget::mouseReleaseEvent(e);
  emit clicked();
}


//=================================================================================
//                                 class TmenuItem
//=================================================================================
#define SPACING (5)
TmenuItem::TmenuItem(QAction* action, QWidget* parent) :
  QWidget(parent),
  m_radio(0),
  m_pressed(false)
{
  addAction(action);
  m_sizeHint.setHeight(style()->pixelMetric(QStyle::PM_SmallIconSize) * 1.3);
  m_sizeHint.setWidth(ITEM_WIDTH);
  setFixedSize(m_sizeHint);

  m_iconLabel = new QLabel(this);
  if (!action->icon().isNull())
    m_iconLabel->setPixmap(action->icon().pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize)));
  m_iconLabel->setFixedWidth(style()->pixelMetric(QStyle::PM_SmallIconSize));

  m_textLabel = new QLabel(fontMetrics().elidedText(action->text(), Qt::ElideMiddle,
          ITEM_WIDTH - style()->pixelMetric(QStyle::PM_SmallIconSize) -
          (m_radio ? style()->pixelMetric(QStyle::PM_SmallIconSize) : 0) - 3 * SPACING), this);

  auto lay = new QHBoxLayout;
    lay->setContentsMargins(SPACING, 0, SPACING, 0);
    lay->addWidget(m_iconLabel);
    lay->addWidget(m_textLabel);
    lay->addStretch();
  if (action->isCheckable()) {
    m_radio = new QRadioButton(this);
    m_radio->setChecked(action->isChecked());
    m_radio->setFixedHeight(m_sizeHint.height());
    connect(m_radio, &QRadioButton::toggled, [=]{ emit clicked(); });
    lay->addWidget(m_radio);
  }

  setLayout(lay);
}


void TmenuItem::paintEvent(QPaintEvent*) {
  QPainter p(this);
  if (m_pressed) {
    p.setBrush(palette().highlight());
    p.setPen(Qt::NoPen);
    p.drawRect(0, 0, m_sizeHint.width(), m_sizeHint.height());
  }
  QColor lc(palette().text().color());
  lc.setAlpha(100);
  p.setPen(QPen(lc, 1, Qt::DashLine));
  p.drawLine(10, height() - 1, width() - 10, height() - 1);
}


void TmenuItem::mousePressEvent(QMouseEvent* e) {
  m_pressed = true;
  update();
  m_startPos = e->pos();
//   QWidget::mousePressEvent(e);
}


void TmenuItem::mouseMoveEvent(QMouseEvent* e) {
  QLine move(m_startPos, e->pos());
  if (qAbs<int>(move.dx()) > 10 || qAbs<int>(move.dy()) > 10) {
    if (m_pressed) {
      m_pressed = false;
      update();
    }
  }
//   QWidget::mouseMoveEvent(e);
}


void TmenuItem::mouseReleaseEvent(QMouseEvent* e) {
  if (m_pressed) {
      m_pressed = false;
      update();
      emit clicked();
  } else
      QWidget::mouseReleaseEvent(e);
}






