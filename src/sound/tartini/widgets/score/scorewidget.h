/***************************************************************************
                          scorewidget.h  -  description
                             -------------------
    begin                : Mar 26 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef SCOREWIDGET_H
#define SCOREWIDGET_H

#include "drawwidget.h"
#include "qfont.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>


class ScoreWidget : public DrawWidget {
  Q_OBJECT

  enum StemType { StemNone, StemUp, StemDown };
  enum FillType { FilledNote, EmptyNote };
  enum NoteType { DemiSemiQuaver, SemiQuaver, Quaver, Crotchet, Minum, SemiBreve };

  public:
    ScoreWidget(QWidget *parent);
    virtual ~ScoreWidget();

    void drawNote(int x, int y, StemType stepType, FillType fillType, int numFlicks);
    void drawNoteAtPitch(int x, int y, int pitch, double noteLength, double volume=1.0);
    void drawScoreSegment(Channel *ch, double leftX, int lineCenterY, double leftTime, double rightTime);
    void drawStaveSegment(int leftX, int lineCenterY, int widthX);
    void paintEvent( QPaintEvent * );
    int getStaveHeight() { return toInt(_scaleY * (_showBaseClef ? 16 : 8)); }
    int getStaveCenterY() { return toInt(_scaleY * (_showBaseClef ? 8 : 7)); }
    QSize sizeHint() const { return QSize(300, 200); }
    NoteType getNoteType(double noteLength);

    double scaleX() { return _scaleX; }
    double scaleY() { return _scaleY; }

 public slots:
    void setScaleX(double x) { _scaleX = x; }
    void setScaleY(double y) { _scaleY = y; }
    void setSharpsMode(int mode) { _useFlats = (mode!=0); }
    void setNotesMode(int mode) { _showNotes = (mode==0); }
    void setClefMode(int mode) { _showBaseClef = (mode==0); }
    void setOpaqueMode(int mode) { _showOpaqueNotes = (mode!=0); }
    void setTransposeLevel(int index) { _pitchOffset = (index - 2) * -12; }
    void setShowAllMode(int mode) { _showAllMode = (mode!=0); }

  private:
    void mousePressEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );

    //data goes here
    double _scaleY; //num. pixels between lines
    double _scaleX; //num. pixels per second
    double _boarderX;
    double _boarderY;
    bool _showBaseClef;
    bool _showNotes;
    bool _showOpaqueNotes;
    bool _useFlats;
    bool _showAllMode;
    int _pitchOffset;
    int sharpsLookup[129];
    int flatsLookup[129];
    int _fontHeight;
    int _fontWidth;
    QFont _font;
    double _lookAhead; //in fractions of a page
    double _lookAheadGap; //in fractions of a page
    bool _mouseDown;
    friend class ScoreSegmentIterator;
};

#endif
