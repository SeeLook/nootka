/***************************************************************************
                          sound_file_stream.h  -  description
                             -------------------
    begin                : Sat Jul 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@pmcleod.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef SOUND_FILE_STREAM_H
#define SOUND_FILE_STREAM_H

#include "sound_stream.h"
#include <stdio.h>

class SoundFileStream : public SoundStream
{
protected:
  int _total_frames;
  int _pos; //in frames

  void setPos(int newPos) { _pos = newPos; }
public:

  SoundFileStream() { _total_frames=-1; _pos=0; };
  virtual ~SoundFileStream() {};

  int data_length() { return totalFrames() * frame_size(); }
  int totalFrames() { return _total_frames; }
  int pos() { return _pos; }
  
  virtual int open_read(const char * /*filename*/) { return 0; };
  virtual long read_bytes(void * /*data*/, long /*length*/) { return 0; };
  virtual long read_frames(void * /*data*/, long /*length*/) { return 0; };

  virtual int open_write(const char * /*filename*/, int /*freq_*/=44100, int /*channels_*/=2, int /*bits_*/=16) { return 0; };
  virtual long write_bytes(void * /*data*/, long /*length*/) { return 0; };
  virtual long write_frames(void * /*data*/, long /*length*/) { return 0; };

  virtual void close() {};

  virtual void jump_to_frame(int /*frame*/) {};
  virtual void jump_back(int /*frames*/) {};
  virtual void jump_forward(int /*frames*/) {};
};

#endif
