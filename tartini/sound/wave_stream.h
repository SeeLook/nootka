/***************************************************************************
                          wave_stream.h  -  description
                             -------------------
    begin                : 2003
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef WAVE_STREAM_H
#define WAVE_STREAM_H

#include "sound_file_stream.h"
#include <stdio.h>

class QStringList;

class WaveStream : public SoundFileStream
{
  int header_length;
 public:
  FILE *file;

  WaveStream();
  virtual ~WaveStream();

  //int header_size() { return header_length; }

  int open_read(const char *filename);
  int read_header();
  long read_bytes(void *data, long length);
  long read_frames(void *data, long length);

  int open_write(const char *filename, int freq_=44100, int channels_=2, int bits_=16);
  void write_header();
  long write_bytes(void *data, long length);
  long write_frames(void *data, long length);

  QStringList getOutputDeviceNames();

  //virtual long write_buffer(FBuffer/*<float>*/ &buffer, unsigned long length);
  //virtual long read_buffer(FBuffer/*<float>*/ &buffer, unsigned long length);

  void close();

  void jump_to_frame(int frame);
  void jump_back(int frames);
  void jump_forward(int frames);
};

#endif
