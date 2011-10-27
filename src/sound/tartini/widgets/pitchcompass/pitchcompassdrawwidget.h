/***************************************************************************
                          pitchcompassdrawwidget.h  -  description
                             -------------------
    begin                : Wed Dec 15 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 ***************************************************************************/

#ifndef PITCHCOMPASSDRAWWIDGET_H
#define PITCHCOMPASSDRAWWIDGET_H

#include "drawwidget.h"
//Added by qt3to4:
#include <QResizeEvent>

class QwtCompass;

class PitchCompassDrawWidget : public QWidget {

  Q_OBJECT

  public:
    PitchCompassDrawWidget(QWidget *parent, const char* name = 0, int mode = 0);
    virtual ~PitchCompassDrawWidget();

    void resizeEvent(QResizeEvent *);

  public slots:
    void updateCompass(double time);

  private:
    QwtCompass *compass;
    bool moveNeedle;
    void blank();
    int blankCount;
    int mode;

};
#endif
