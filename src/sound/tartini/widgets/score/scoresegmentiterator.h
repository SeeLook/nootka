/***************************************************************************
                          scoresegmentiterator.h  -  description
                             -------------------
    begin                : Mar 29 2006
    copyright            : (C) 2006 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef SCORESEGMENTITERATOR_H
#define SCORESEGMENTITERATOR_H

//#include "drawwidget.h"
//#include "qfont.h"
#include "channel.h"

class ScoreWidget;

class ScoreSegmentIterator {
  ScoreWidget *sw;

  Channel *ch;
  int staveHeight;
  int halfStaveHeight;
  int numRows;
  double totalRowTime;
  double totalPageTime;
  double _curTime;

  int _curPage;
  double lookAheadGapTime;
  double lookAheadTime;
  double lookBehindTime;
  double startOfPageTime;
  double curRelPageTime;
  double endTime;
  int _numPages;
  double endOfLastPageTime;
  double lookAheadTime2;
  double lookBehindTime2;
  double lookBehindTime3;
  int curRow;
  int rowCounter;
  int subRowCounter;

  double _leftTime;
  double _rightTime;
  double _leftX;
  int _lineCenterY;
  bool _isValid;

 public :
  ScoreSegmentIterator(ScoreWidget *scoreWidget, Channel *channel) { reset(scoreWidget, channel); }
  void reset(ScoreWidget *scoreWidget, Channel *channel);
  bool next();
  bool isValid() { return _isValid; }

  double leftTime() { return _leftTime; }
  double rightTime() { return _rightTime; }
  double leftX() { return _leftX; }
  int lineCenterY() { return _lineCenterY; }
  int staveTop() { return _lineCenterY - halfStaveHeight; }
  int staveBottom() { return staveTop() + staveHeight; }
  double widthX();
  double curTime() { return _curTime; }
  int curPage() { return _curPage; }
  int numPages() { return _numPages; }
  bool contains(double t) { return (t >= _leftTime && t <= _rightTime); }
};


#endif
