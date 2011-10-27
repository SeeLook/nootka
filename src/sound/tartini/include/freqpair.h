/***************************************************************************
                          freqpair.h  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef FREQPAIR_H
#define FREQPAIR_H

#include <vector>

#include <qmutex.h>

#include <stdio.h>

class FreqPair {
public:
  float freq, amp, phase;
  FreqPair *prev, *next;
  FreqPair() { prev=NULL; next=NULL; }
  FreqPair(float freq_, float amp_) { freq=freq_; amp=amp_; phase=0.0; prev=NULL; next=NULL; }
  FreqPair(float freq_, float amp_, float phase_) { freq=freq_; amp=amp_; phase=phase_; prev=NULL; next=NULL; }
  void unlink() {
      //if(prev) { prev->next = NULL; printf("removed one\n"); }
      if(next) { next = NULL; /*printf("removed one\n");*/ }
  }
  //void attachPrev(FreqPair *prev_) { prev = prev_; prev->next = this; }
  //void attachNext(FreqPair *next_) { next = next_; next->prev = this; }
  void attachNext(FreqPair *next_) { next = next_; }
};

class FreqHistory {
 public:
    std::vector<FreqPair> *data;
    int num;
    int end;
    QMutex *mutex;

    FreqHistory() {
	num = 128;
	end = 0;
	mutex = new QMutex();
	data = new std::vector<FreqPair>[num];
    }
    FreqHistory(const FreqHistory &f) {
	num = f.num;
	end = f.end;
	mutex = new QMutex();
	data = new std::vector<FreqPair>[num];
    }
    virtual ~FreqHistory() {
	delete[] data;
	delete mutex;
    }

    std::vector<FreqPair> *curData() { return &data[end]; }
    std::vector<FreqPair> *prevData() { return &data[((end>0)?end-1:num-1)]; }
    void lock() { mutex->lock(); }
    void unlock() { mutex->unlock(); }
    void move() {
	//for(unsigned int k=0; k<data[end].size(); k++) data[end][k].unlink();
	end++;
	if(end >= num) end -= num;
    }
    void connectToPrev() {
	std::vector<FreqPair> &prev = *prevData();
	std::vector<FreqPair> &cur = *curData();
	unsigned int prevPos=0, curPos=0;
	float freqThreshold = 10.0;
	float ampThreshold = 10.0;
	while((prevPos < prev.size()) && (curPos < cur.size())) {
	    //make connection between two consecutive (freq,amp) points if within thresholds
	    if(prev[prevPos].freq > cur[curPos].freq - freqThreshold &&
	       prev[prevPos].freq < cur[curPos].freq + freqThreshold &&
	       prev[prevPos].amp > cur[curPos].amp - ampThreshold &&
	       prev[prevPos].amp < cur[curPos].amp + ampThreshold) {
		prev[prevPos].attachNext(&cur[curPos]);
	    }

	    //increment the one with the smallest freq (because they are orded by freq)
	    if(prev[prevPos].freq < cur[curPos].freq) prevPos++;
	    else curPos++;
	}
    }
};
    
#endif

