/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#include "tcolordialog.h"
#include "color_wheel.hpp"
#include <touch/ttoucharea.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qspinbox.h>
#include <QtWidgets/qdialogbuttonbox.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qframe.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qscreen.h>
#include <QtWidgets/qstyle.h>
#include <QtGui/qpainter.h>



class TcolorPreview : public QFrame {

public:
  explicit TcolorPreview(QWidget* parent = 0) : QFrame(parent) {}
  void setColor(const QColor& c) { m_color = c; update(); }
  QColor color() const { return m_color; }

protected:
  virtual void paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(m_color));
    p.drawRect(contentsRect());
  }

private:
  QColor      m_color;
};

TcolorDialog::TcolorDialog(const QColor& initial, QWidget* parent) :
  QDialog(parent)
{
  showMaximized();
  m_wheel = new color_widgets::ColorWheel(this);
  m_wheel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_wheel->setWheelWidth(qApp->screens()[0]->geometry().width() / (qApp->screens()[0]->physicalSize().width() / 7.0));

  m_redSpin = new QSpinBox(this);
  m_redSpin->setRange(0, 255);
  m_redSpin->setStyleSheet(m_redSpin->styleSheet() + QStringLiteral("color: #ff0000;"));

  m_greenSpin = new QSpinBox(this);
  m_greenSpin->setRange(0, 255);
  m_greenSpin->setStyleSheet(m_greenSpin->styleSheet() + QStringLiteral("color: #00ff00;"));

  m_blueSpin = new QSpinBox(this);
  m_blueSpin->setRange(0, 255);
  m_blueSpin->setStyleSheet(m_blueSpin->styleSheet() + QStringLiteral("color: #0000ff;"));

  QDialogButtonBox box;
  auto save = box.addButton(QDialogButtonBox::Save);
  auto discard = box.addButton(QDialogButtonBox::Discard);

  m_acceptBut = new QPushButton(QIcon(style()->standardIcon(QStyle::SP_DialogOkButton)), save->text(), this);
  m_cancelButt = new QPushButton(QIcon(style()->standardIcon(QStyle::SP_DialogCancelButton)), discard->text(), this);

  m_colorPreview = new TcolorPreview(this);
  m_colorPreview->setMinimumHeight(5);
  m_colorPreview->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  m_colorPreview->setFrameShape(QFrame::StyledPanel);
  m_colorPreview->setFrameShadow(QFrame::Sunken);

  auto lay = new QHBoxLayout;
    auto wheelLay = new QHBoxLayout;
    wheelLay->addWidget(m_wheel);
//     wheelLay->addStretch();
  lay->addLayout(wheelLay);
//   lay->addStretch();
  auto spinLay = new QVBoxLayout;
    spinLay->addWidget(m_cancelButt);
    spinLay->addWidget(m_redSpin);
    spinLay->addWidget(m_greenSpin);
    spinLay->addWidget(m_blueSpin);
    spinLay->addWidget(m_colorPreview);
    spinLay->addWidget(m_acceptBut);
  auto ta = new TtouchArea(this);
  ta->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  ta->setLayout(spinLay);
  lay->addWidget(ta);
  setLayout(lay);

  connect(m_wheel, &color_widgets::ColorWheel::colorSelected, this, &TcolorDialog::colorChanged);
  connect(m_acceptBut, &QPushButton::clicked, this, &QDialog::accept);
  connect(m_cancelButt, &QPushButton::clicked, this, &QDialog::close);
  connect(m_redSpin, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
  connect(m_greenSpin, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
  connect(m_blueSpin, SIGNAL(valueChanged(int)), this, SLOT(spinValueChanged()));
  setColor(initial);
}


void TcolorDialog::setColor(const QColor& c) {
  m_wheel->setColor(c);
  colorChanged(c);
  m_colorPreview->setColor(c);
}


QColor TcolorDialog::selectedColor() {
  return m_wheel->color();
}


void TcolorDialog::colorChanged(const QColor& c) {
  m_redSpin->setValue(c.red());
  m_blueSpin->setValue(c.blue());
  m_greenSpin->setValue(c.green());
  m_colorPreview->setColor(m_wheel->color());
}


void TcolorDialog::spinValueChanged() {
  m_wheel->setColor(QColor(m_redSpin->value(), m_greenSpin->value(), m_blueSpin->value()));
  m_colorPreview->setColor(m_wheel->color());
}




