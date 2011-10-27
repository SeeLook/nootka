/***************************************************************************
                          myscrollbar.cpp  -  description
                             -------------------
    begin                : Tue Jul 20 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#include <qscrollbar.h>
//Added by qt3to4:
#include <QResizeEvent>
#include "myscrollbar.h"
#include <qlayout.h>

MyScrollBar::MyScrollBar(Qt::Orientation orientation, QWidget *parent, const char *name) :
  QWidget(parent, name)
{
  bar = new QScrollBar(orientation, this);
  bar->show();
}

MyScrollBar::MyScrollBar(double minValue_, double maxValue_, double lineStep_, double pageStep_, double value_, double step_, Qt::Orientation orientation, QWidget *parent, const char *name) :
  QWidget(parent, name)
{
  _minValue = minValue_;
  _maxValue = maxValue_;
  _lineStep = lineStep_;
  _pageStep = pageStep_;
  _value = value_;
  _step = step_;
  //QBoxLayout *layout = new QHBoxLayout(this);
  
  //bar = new QScrollBar(toInt(_minValue*_step), toInt(_maxValue*_step), MAX(toInt(_lineStep*_step), 1), MAX(toInt(_pageStep*_step), 1), toInt(_value*_step), orientation, this);
  bar = new QScrollBar(orientation, this);
  bar->setMinimum(toInt(_minValue*_step));
  bar->setMaximum(toInt(_maxValue*_step));
  bar->setSingleStep(MAX(toInt(_lineStep*_step), 1));
  bar->setPageStep(MAX(toInt(_pageStep*_step), 1));
  bar->setValue(toInt(_value*_step));
  //layout->addWidget(bar);
  //bar->setMinimumSize(100, 40);
  //bar->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  setSizePolicy(bar->sizePolicy());
  //bar->show();
/*
  connect(bar, SIGNAL(valueChanged(int)), this, SLOT(setIntValue(int)));
  connect(bar, SIGNAL(sliderMoved(int)), this, SLOT(sliderMoving(int)));
  connect(bar, SIGNAL(nextPage()), this, SLOT(sliderMoving()));
  connect(bar, SIGNAL(prevPage()), this, SLOT(sliderMoving()));
  connect(bar, SIGNAL(nextLine()), this, SLOT(sliderMoving()));
  connect(bar, SIGNAL(prevLine()), this, SLOT(sliderMoving()));
*/
  connect(bar, SIGNAL(actionTriggered(int)), this, SLOT(actionTriggering(int)));
}

MyScrollBar::~MyScrollBar()
{
  delete bar;
}

QSize MyScrollBar::sizeHint() const
{
  return bar->sizeHint();
}

void MyScrollBar::resizeEvent(QResizeEvent *q)
{
  bar->resize(q->size());
}

void MyScrollBar::setMinValue(double minValue_)
{
  _minValue = minValue_;
  //bar->setMinValue(toInt(_minValue * _step));
  bar->setMinimum(toInt(_minValue * _step));
}

void MyScrollBar::setMaxValue(double maxValue_)
{
  _maxValue = maxValue_;
  //bar->setMaxValue(toInt(_maxValue * _step));
  bar->setMaximum(toInt(_maxValue * _step));
}

void MyScrollBar::setRange(double minValue_, double maxValue_)
{
  setMinValue(minValue_);
  setMaxValue(maxValue_);
}

void MyScrollBar::setLineStep(double lineStep_)
{
  _lineStep = lineStep_;
  //bar->setLineStep(toInt(_lineStep * _step));
  bar->setSingleStep(toInt(_lineStep * _step));
}

void MyScrollBar::setPageStep(double pageStep_)
{
  _pageStep = pageStep_;
  bar->setPageStep(toInt(_pageStep * _step));
}

void MyScrollBar::setValue(double value_)
{
  _value = value_;
  bar->setValue(toInt(_value * _step));
}

void MyScrollBar::setIntValue(int value_)
{
  _value = double(value_) / _step;
  emit valueChanged(_value);
}

void MyScrollBar::setStep(double step_)
{
  _step = step_;
}

void MyScrollBar::sliderMoving(int value_)
{
  _value = double(value_) / _step;
  emit sliderMoved(_value);
  //emit sliderMoved();
}

void MyScrollBar::sliderMoving()
{
  _value = double(bar->value()) / _step;
  emit sliderMoved(_value);
  //emit sliderMoved();
}

void MyScrollBar::actionTriggering(int /*action*/)
{
  setIntValue(bar->sliderPosition());
  emit sliderMoved(_value);
  //emit sliderMoved();
}

