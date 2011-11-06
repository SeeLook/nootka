/***************************************************************************
                          scorsegmentiterator.cpp  -  description
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
#include <qpixmap.h>
#include <qpainter.h>

/*
#include "scorewidget.h"
#include "gdata.h"
#include "channel.h"
#include "analysisdata.h"
#include "useful.h"
#include "qfontmetrics.h"
#include "qpen.h"
*/

#include "scorewidget.h"
#include "scoresegmentiterator.h"

double ScoreSegmentIterator::widthX() {
  return (_rightTime - _leftTime) * sw->_scaleX;
}

/** Initalise the iterator to just before the first segment
  * @param scoreWidget The ScoreWidget to iterate through it's segments
  */
void ScoreSegmentIterator::reset(ScoreWidget *scoreWidget, Channel *channel)
{
  sw = scoreWidget;
  ch = channel;
  staveHeight = sw->getStaveHeight();
  halfStaveHeight = sw->getStaveCenterY();
  numRows = std::max((sw->height() - toInt(sw->_boarderY*2)) / staveHeight, 1);
  totalRowTime = (double)(sw->width() - sw->_boarderX*2) / sw->_scaleX;
  totalPageTime = totalRowTime * numRows;
  if(ch) {
    _curTime = ch->timeAtChunk(ch->currentChunk());
    _curPage = (int)floor(_curTime / totalPageTime);
    lookAheadGapTime = sw->_lookAheadGap*totalPageTime;
    lookAheadTime = _curTime + sw->_lookAhead*totalPageTime;
    lookBehindTime = lookAheadTime - totalPageTime;
    startOfPageTime = floor(_curTime / totalPageTime) * totalPageTime;
    curRelPageTime = _curTime - startOfPageTime;
    endTime = ch->totalTime();
    _numPages = (int)ceil(endTime / totalPageTime);
    endOfLastPageTime = _numPages * totalPageTime;
    //if(endOfLastPageTime > lookAheadTime) lookAheadTime = endOfLastPageTime;
    lookAheadTime2 = (_curPage == _numPages-1) ? std::min(endTime, lookAheadTime) : lookAheadTime;
    lookAheadTime2 = (_curPage == 0) ? endOfLastPageTime : lookAheadTime2;
    //if(curPage == 0) lookAheadTime = endOfLastPageTime;
    //double lookBehindTime2 = (curPage == numPages-1) ? startOfPageTime - _lookAheadGap*totalPageTime : lookBehindTime;
    //double lookBehindTime2 = (curPage == numPages-1) ? lookBehindTime+totalPageTime : lookBehindTime;
    //double lookBehindTime2 = (curPage == numPages-1) ? std::min(lookBehindTime, endTime-totalPageTime) : lookBehindTime;
    lookBehindTime2 = std::min(lookBehindTime, endTime-totalPageTime);
    //FIXME: The ending page isn't drawn correctly
    lookBehindTime2 = (_curPage == 0) ? lookBehindTime+totalPageTime : lookBehindTime2;
    lookBehindTime3 = std::min(lookBehindTime, endTime-totalPageTime);
    //if(curPage == numPages-1) lookBehindTime = startOfPageTime - _lookAheadGap*totalPageTime;
    curRow = (int)floor(curRelPageTime / totalRowTime);
    //QString pageString;
    //p.drawText(_fontWidth, height()-4, pageString.sprintf("Page %d of %d", curPage+1, numPages));
  }
  rowCounter = 0;
  subRowCounter = -1;
  _leftTime = _rightTime = _leftX = 0.0;
  _lineCenterY = 0;
  _isValid = false;
}

/** move to the next score segment
  * @return true, if their was another score segment
  */
bool ScoreSegmentIterator::next()
{
  if(ch) {
    while(rowCounter < numRows) {
      int j = rowCounter;
      double startOfRowTime = startOfPageTime + j*totalRowTime;
      double endOfRowTime = startOfRowTime + totalRowTime;
      //double curRelRowTime = _curTime - startOfRowTime;
      _lineCenterY = toInt(sw->_boarderY) + halfStaveHeight + staveHeight*j;
      while(++subRowCounter < 4) {
        switch(subRowCounter) {
        case 1:
          if(startOfRowTime < lookBehindTime3) { //draw any parts of the next page
            _leftTime = startOfRowTime+totalPageTime;
            _rightTime = std::min(endOfRowTime, lookBehindTime3) + totalPageTime;
            _leftX = (double)sw->_boarderX;
            //drawScoreRow(ch, (double)_boarderX, lineCenterY, startOfRowTime+totalPageTime, rightTime+totalPageTime);
            return (_isValid = true);
          }
          break;
        case 2:
          if(endOfRowTime > lookBehindTime3+lookAheadGapTime && startOfRowTime < lookAheadTime2) { //normal case
            _leftTime = std::max(startOfRowTime, lookBehindTime3 + lookAheadGapTime);
            _rightTime = std::min(startOfRowTime+totalRowTime, lookAheadTime2);
            _leftX = (double)sw->_boarderX + (_leftTime-startOfRowTime)*sw->_scaleX;
            //printf("leftTime=%f, rightTime=%d\n", leftTime, rightTime);
            //drawScoreRow(ch, (double)_boarderX + (leftTime-startOfRowTime)*_scaleX, lineCenterY, leftTime, rightTime);
            return (_isValid = true);
          }
          break;
        case 3:
          //if(endOfRowTime-totalPageTime > lookBehindTime2) {
          if(endOfRowTime-totalPageTime > lookBehindTime2 + lookAheadGapTime) {
            _leftTime = std::max(startOfRowTime-totalPageTime, lookBehindTime2 + lookAheadGapTime);
            _leftTime = std::min(_leftTime, endOfRowTime-totalPageTime);
            _rightTime = endOfRowTime-totalPageTime;
            _leftX = (double)sw->_boarderX + (_leftTime-(startOfRowTime-totalPageTime))*sw->_scaleX;
            //drawScoreRow(ch, (double)_boarderX + (leftTime-(startOfRowTime-totalPageTime))*_scaleX, lineCenterY, leftTime, rightTime);
            return (_isValid = true);
          }
        }
      }
      rowCounter++;
      subRowCounter = -1;
    }
  } else {
    while(rowCounter < numRows) {
      double startOfRowTime = startOfPageTime + rowCounter*totalRowTime;
      double endOfRowTime = startOfRowTime + totalRowTime;
      _lineCenterY = toInt(sw->_boarderY) + halfStaveHeight + staveHeight*rowCounter;
      _leftX = sw->_boarderX;
      _leftTime = startOfRowTime;
      _rightTime = endOfRowTime;
      rowCounter++;
      return (_isValid = true);
    }
  }
  return (_isValid = false);
}
