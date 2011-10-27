/***************************************************************************
                          myscrollbar.h  -  description
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

#include <qwidget.h>
//Added by qt3to4:
#include <QResizeEvent>
#include "useful.h"

class QScrollBar;

class MyScrollBar : public QWidget
{
  Q_OBJECT

  double _minValue;
  double _maxValue;
  double _value;
  double _lineStep;
  double _pageStep;
  double _step;
  
  QScrollBar *bar;
  
public:
  MyScrollBar(Qt::Orientation orientation, QWidget * parent, const char * name = 0);
    //QScrollBar(orientation, parent, name) { }
    MyScrollBar(double minValue_, double maxValue_, double linesPerPage_, double pageStep_, double value_, double step_, Qt::Orientation orientation, QWidget * parent, const char * name = 0);
      //QScrollBar(minValue, maxValue, MAX(toInt(double(pageStep)/double(linesPerPage)), 1), MAX(pageStep, 1), value, orientation, parent, name) { _linesPerPage = linesPerPage; }
  virtual ~MyScrollBar();

  QSize sizeHint() const;
  void resizeEvent(QResizeEvent *q);

  double minValue() { return _minValue; }
  double maxValue() { return _maxValue; }
  double lineStep() { return _lineStep; }
  double pageStep() { return _pageStep; }
  double value() { return _value; }
  double step() { return _step; }

public slots:
  void setMinValue(double minValue_);
  void setMaxValue(double maxValue_);
  void setRange(double minValue_, double maxValue_);
  void setLineStep(double lineStep_);
  void setPageStep(double pageStep_);
  void setValue(double value_);
  void setIntValue(int value);
  void sliderMoving(int value);
  void sliderMoving();
  void setStep(double step_);
  void actionTriggering(int action);

signals:
  void valueChanged(double value_);
  //void valueChanged();
  void sliderMoved(double value_);
  //void sliderMoved();
};
