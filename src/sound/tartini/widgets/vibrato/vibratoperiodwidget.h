/***************************************************************************
                          vibratoperiodwidget.h  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef VIBRATOPERIODWIDGET_H
#define VIBRATOPERIODWIDGET_H

#include <QGLWidget>

class VibratoPeriodWidget : public QGLWidget {
  Q_OBJECT

  public:
    VibratoPeriodWidget(QWidget *parent);
    virtual ~VibratoPeriodWidget();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    QSize minimumSizeHint() const { return QSize(100, 75); }

  private:
    int prevLeftMinimumTime;
    bool smoothedPeriods;
    bool drawSineReference;
    bool sineStyle;
    bool drawPrevPeriods;
    bool periodScaling;
    bool drawComparison;

    GLuint sineReference;
    GLuint previousPoly[5];
    GLuint currentPeriod;
    GLuint comparisonPoly;
    GLuint comparisonReference;

    int lastPeriodToDraw;

  public slots:
    void doUpdate();
    void setSmoothedPeriods(bool value);
    void setDrawSineReference(bool value);
    void setSineStyle(bool value);
    void setDrawPrevPeriods(bool value);
    void setPeriodScaling(bool value);
    void setDrawComparison(bool value);
};

#endif
