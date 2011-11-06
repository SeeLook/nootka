/***************************************************************************
                          vibratowidget.h  -  description
                             -------------------
    begin                : May 18 2005
    copyright            : (C) 2005-2007 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef VIBRATOWIDGET_H
#define VIBRATOWIDGET_H

#include <QGLWidget>

class VibratoWidget : public QGLWidget {
  Q_OBJECT

  public:
    VibratoWidget(QWidget *parent, int nls);
    virtual ~VibratoWidget();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    QSize sizeHint() const { return QSize(300, 100); }

  private:
    int noteLabelOffset; // The horizontal space in pixels a note label requires
    double zoomFactorX;
    double zoomFactorY;
    int offsetY;

    QFont vibratoFont;

    GLuint verticalPeriodBars;
    GLuint verticalSeparatorLines;
    GLuint referenceLines;
    GLuint pronyWidthBand;
    GLuint pronyAveragePitch;
    GLuint vibratoPolyline;
    GLuint currentWindowBand;
    GLuint currentTimeLine;
    GLuint maximaMinimaPoints;

    int noteLabelCounter;
    struct noteLabelStruct {
      QString label;
      float y;
    };
    noteLabelStruct noteLabels[100];

  public slots:
    void doUpdate();
    void setZoomFactorX(double x);
    void setZoomFactorY(double y);
    void setOffsetY(int value);
};

#endif
