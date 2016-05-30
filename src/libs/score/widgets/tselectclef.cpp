/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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


#include "tselectclef.h"
#include <graphics/tnotepixmap.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qbuttongroup.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qdesktopwidget.h>
#include <QtCore/qtimer.h>
#if defined (Q_OS_ANDROID)
  #include "touch/ttoucharea.h"
  #include "tmtr.h"
#endif



//######### TselectClefPrivate #######################################################################

TselectClefPrivate::TselectClefPrivate(bool isMenu, QWidget* parent) :
  QWidget(parent)
{
  treble = new TradioClef(Tclef(Tclef::e_treble_G), parent, isMenu);
  treble_8 = new TradioClef(Tclef(Tclef::e_treble_G_8down), parent, isMenu);
  bass = new TradioClef(Tclef(Tclef::e_bass_F), parent, isMenu);
  bass_8 = new TradioClef(Tclef(Tclef::e_bass_F_8down), parent, isMenu);
  alto = new TradioClef(Tclef(Tclef::e_alto_C), parent, isMenu);
  tenor = new TradioClef(Tclef(Tclef::e_tenor_C), parent, isMenu);
  piano = new TradioClef(Tclef(Tclef::e_pianoStaff), parent, isMenu);

  QButtonGroup *clefsGroup = new QButtonGroup(this);
    clefsGroup->addButton(treble->radio());
    clefsGroup->addButton(treble_8->radio());
    clefsGroup->addButton(bass->radio());
    clefsGroup->addButton(bass_8->radio());
    clefsGroup->addButton(alto->radio());
    clefsGroup->addButton(tenor->radio());
    clefsGroup->addButton(piano->radio());

  if (isMenu) {
      QVBoxLayout *leftLay = new QVBoxLayout;
      QVBoxLayout *rightLay = new QVBoxLayout;
      leftLay->addWidget(treble);
      leftLay->addWidget(treble_8);
      leftLay->addWidget(bass);
      leftLay->addWidget(bass_8);
      rightLay->addWidget(alto);
      rightLay->addWidget(tenor);
      rightLay->addWidget(piano);
      QHBoxLayout *lay = new QHBoxLayout;
      lay->addLayout(leftLay);
      lay->addLayout(rightLay);
      setLayout(lay);
  } else {
#if defined (Q_OS_ANDROID)
      QVBoxLayout *upLay = new QVBoxLayout;
      QVBoxLayout *lowLay = new QVBoxLayout;
#else
      QHBoxLayout *upLay = new QHBoxLayout;
      QHBoxLayout *lowLay = new QHBoxLayout;
#endif
      upLay->addWidget(treble);
#if defined (Q_OS_ANDROID)
      static_cast<QHBoxLayout*>(treble->layout())->insertWidget(2, getLabelFromStatus(treble, false));
#endif
      lowLay->addWidget(treble_8);
#if defined (Q_OS_ANDROID)
      static_cast<QHBoxLayout*>(treble_8->layout())->insertWidget(2, getLabelFromStatus(treble_8, false));
#endif
      upLay->addWidget(bass);
#if defined (Q_OS_ANDROID)
      static_cast<QHBoxLayout*>(bass->layout())->insertWidget(2, getLabelFromStatus(bass, false, true));
#endif
      lowLay->addWidget(bass_8);
#if defined (Q_OS_ANDROID)
      static_cast<QHBoxLayout*>(bass_8->layout())->insertWidget(2, getLabelFromStatus(bass_8, false));
#endif
      upLay->addWidget(alto);
#if defined (Q_OS_ANDROID)
      static_cast<QHBoxLayout*>(alto->layout())->insertWidget(2, getLabelFromStatus(alto, false));
#endif
      lowLay->addWidget(tenor);
#if defined (Q_OS_ANDROID)
      static_cast<QHBoxLayout*>(tenor->layout())->insertWidget(2, getLabelFromStatus(tenor, false));
#endif
      QVBoxLayout *upLowLay = new QVBoxLayout;
      upLowLay->addLayout(upLay);
      upLowLay->addLayout(lowLay);
#if defined (Q_OS_ANDROID)
      QVBoxLayout *lay = new QVBoxLayout;
#else
      QHBoxLayout *lay = new QHBoxLayout;
#endif
      lay->addLayout(upLowLay);
      lay->addWidget(piano);
#if defined (Q_OS_ANDROID)
      static_cast<QHBoxLayout*>(piano->layout())->insertWidget(2, getLabelFromStatus(piano, false));
#endif

      setLayout(lay);
  }
  connect(treble, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
  connect(treble_8, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
  connect(bass, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
  connect(bass_8, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
  connect(alto, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
  connect(tenor, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));
  connect(piano, SIGNAL(selectedClef(Tclef)), this, SLOT(clefWasSelected(Tclef)));

  connect(treble, SIGNAL(statusTipWanted(QString)), this, SLOT(onStatusTip(QString)));
  connect(treble_8, SIGNAL(statusTipWanted(QString)), this, SLOT(onStatusTip(QString)));
  connect(bass, SIGNAL(statusTipWanted(QString)), this, SLOT(onStatusTip(QString)));
  connect(bass_8, SIGNAL(statusTipWanted(QString)), this, SLOT(onStatusTip(QString)));
  connect(alto, SIGNAL(statusTipWanted(QString)), this, SLOT(onStatusTip(QString)));
  connect(tenor, SIGNAL(statusTipWanted(QString)), this, SLOT(onStatusTip(QString)));
  connect(piano, SIGNAL(statusTipWanted(QString)), this, SLOT(onStatusTip(QString)));
}


void TselectClefPrivate::selectClef(Tclef clef) {
  switch(clef.type()) {
    case Tclef::e_treble_G:
      treble->setChecked(true); break;
    case Tclef::e_treble_G_8down:
      treble_8->setChecked(true); break;
    case Tclef::e_bass_F:
      bass->setChecked(true); break;
    case Tclef::e_bass_F_8down:
      bass_8->setChecked(true); break;
    case Tclef::e_alto_C:
      alto->setChecked(true); break;
    case Tclef::e_tenor_C:
      tenor->setChecked(true); break;
    case Tclef::e_pianoStaff:
      piano->setChecked(true); break;
    default: break;
  }
}


//########## TradioClef ##############################################################################


TradioClef::TradioClef(Tclef clef, QWidget* parent, bool isMenu) :
  QWidget(parent),
  m_clef(clef),
  m_hasMouseOver(false)
{
  QHBoxLayout *lay = new QHBoxLayout;
  m_radio = new QRadioButton(this);
  lay->addWidget(m_radio);
  setMouseTracking(true);

#if defined (Q_OS_ANDROID)
  QLabel *pixLabel = new QLabel(wrapPixToHtml(Tnote(0, 0, 0), m_clef.type(), 0,
      qMin(Tmtr::fingerPixels() * 1.5,
           qMin(qApp->desktop()->availableGeometry().width(), qApp->desktop()->availableGeometry().height()) / 110.0)),
      this);
#else
  QLabel *pixLabel = new QLabel(wrapPixToHtml(Tnote(0, 0, 0), m_clef.type(), 0,
    qMin(qApp->desktop()->availableGeometry().width(), qApp->desktop()->availableGeometry().height()) / 219.4285714285), this);
#endif
  lay->addWidget(pixLabel);
  if (isMenu) {
      QLabel *textLabel = new QLabel(m_clef.name().replace(" ", "<br>"), this);
      lay->addWidget(textLabel);
  }
  lay->addStretch();
  setLayout(lay);
  QString clefUsage;
  switch (clef.type()) {
    case Tclef::e_treble_G:
      clefUsage = tr("Common used clef (for violin, flute, saxophones, etc.)");
      break;
    case Tclef::e_treble_G_8down:
      clefUsage = tr("Clef for guitars (classical, electric and so)");
      break;
    case Tclef::e_bass_F:
      clefUsage = tr(" Bottom clef on the grand staff but also used for cello, trombone, etc.");
      break;
    case Tclef::e_bass_F_8down:
      clefUsage = tr(" Clef for bass guitar and double bass.");
      break;
    case Tclef::e_alto_C:
      clefUsage = tr("Sometimes it is called clef for viola and mostly used for this instrument.");
      break;
    default:
      break;
  }
  setStatusTip(QLatin1String("<b>") + m_clef.name() + QLatin1String("</b>  (") + m_clef.desc() + QLatin1String(")<br>") + clefUsage);
  m_radio->setStatusTip(statusTip());
  pixLabel->setStatusTip(statusTip());
  connect(m_radio, SIGNAL(pressed()), this, SLOT(clefClickedSlot()));

  m_signalTimer = new QTimer(this);
  connect(m_signalTimer, &QTimer::timeout, [=]{
    m_signalTimer->stop();
    emit selectedClef(m_clef);
  });
}


TradioClef::~TradioClef() {
  m_signalTimer->stop();
}

void TradioClef::setChecked(bool checked) {
    m_radio->setChecked(checked);
}


void TradioClef::clefClickedSlot() {
  if (!m_radio->isChecked())
    m_radio->setChecked(true);
  m_signalTimer->start(5);
}


bool TradioClef::event(QEvent* event) {
#if !defined (Q_OS_ANDROID)
  if (event->type() == QEvent::Leave || event->type() == QEvent::Hide) {
      m_hasMouseOver = false;
      update();
      if (event->type() == QEvent::Leave)
        emit statusTipWanted(QString());
  } else if (event->type() == QEvent::MouseMove && !m_hasMouseOver) {
      m_hasMouseOver = true;
      update();
      emit statusTipWanted(statusTip());
  } else if (event->type() == QEvent::Enter) {
      m_hasMouseOver = true;
      update();
      emit statusTipWanted(statusTip());
  }  else
#endif
    if (event->type() == QEvent::MouseButtonPress)
      clefClickedSlot();
  return QWidget::event(event);
}


void TradioClef::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event)
  if (m_hasMouseOver) {
    QPainter painter(this);
    QLinearGradient lg(width() / 2, 0, width() / 2, height());
    QColor bg = palette().highlight().color();
    bg.setAlpha(100);
    lg.setColorAt(0.0, bg.darker());
    lg.setColorAt(0.9, palette().highlight().color());
    painter.setPen(Qt::NoPen);
    painter.setBrush(lg);
    painter.drawRoundedRect(0, 0, width(), height(), 6, 6);
  }
}



//########### TselectClef #################################################################################

TselectClef::TselectClef(QWidget* parent) :
  TselectClefPrivate(false, parent)
{}


void TselectClef::clefWasSelected(Tclef clef) {
  emit clefSelected(clef);
}


Tclef TselectClef::selectedClef(){
  if (treble->radio()->isChecked())
    return Tclef(Tclef::e_treble_G);
  if (treble_8->radio()->isChecked())
    return Tclef(Tclef::e_treble_G_8down);
  if (bass->radio()->isChecked())
    return Tclef(Tclef::e_bass_F);
  if (bass_8->radio()->isChecked())
    return Tclef(Tclef::e_bass_F_8down);
  if (alto->radio()->isChecked())
    return Tclef(Tclef::e_alto_C);
  if (tenor->radio()->isChecked())
    return Tclef(Tclef::e_tenor_C);
  if (piano->radio()->isChecked())
    return Tclef(Tclef::e_pianoStaff);
  else
    return Tclef(Tclef::e_none);
}


//########### TclefMenu ###########################################################################
TclefMenu::TclefMenu(QMenu* parent) :
  TselectClefPrivate(true, new QWidget(parent)),
  m_menu(parent)
{
  m_menu->setLayout(TselectClefPrivate::layout());
  m_menu->installEventFilter(this);
  m_curentClef = Tclef(Tclef::e_none);
}


void TclefMenu::setMenu(QMenu* menuParent) {
  if (m_menu) // free a layout from current menu
    TselectClefPrivate::setLayout(m_menu->layout());
  m_menu = menuParent;
  setParent(menuParent);
  if (m_menu) { // When new menu - add a layout to it
    m_menu->setLayout(TselectClefPrivate::layout());
    m_menu->installEventFilter(this); // grab leave event
  }
}


Tclef TclefMenu::exec(QPoint pos) {
  if (!m_menu)
    return Tclef(Tclef::e_none);
  m_menu->move(pos.x(), qMin(pos.y(), qRound(qApp->desktop()->availableGeometry().height() * 0.55))); // It works everywhere (Qt style)
  m_menu->show(); // since Qt 5.6 it is necessary
  m_menu->exec(); // in contrary to exec(pos)
  return m_curentClef;
}



void TclefMenu::clefWasSelected(Tclef clef) {
  m_curentClef = clef;
  if (m_menu)
    m_menu->close();
}


bool TclefMenu::eventFilter(QObject* ob, QEvent* e) {
  if (ob == m_menu && e->type() == QEvent::Leave) {
    m_curentClef = Tclef(Tclef::e_none);
    m_menu->close();
  }
  return QObject::eventFilter(ob, e);
}






