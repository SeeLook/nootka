/***************************************************************************
                          wave_stream.cpp  -  description
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
#include <qglobal.h>
//#include <stdlib.h>
//#include <stdio.h>
#include <string.h>
#include "myio.h"
#include "wave_stream.h"
#include <algorithm>
#include <errno.h>
#include "useful.h"

WaveStream::WaveStream()
{
  file = NULL;
  //_total_frames = _pos = 0;
  header_length = 0;
}

WaveStream::~WaveStream()
{
  close();
}

void WaveStream::close()
{
  if(file) {
    if(mode & F_WRITE) write_header();
    fclose(file);
    file = NULL;
  }
}

int WaveStream::open_read(const char *filename)
{
  file = fopen(filename, "rb");
  if(!file) return -1;
  mode = F_READ;
  setPos(0);
  return read_header();
}

int WaveStream::read_header()
{
  if(!file || !(mode & F_READ)) return -1;

  int len, data_type;
  char buffer[12];
  
  if(fread(buffer, 1, 12, file) != 12) goto missing_data;   // check RIFF header
  if (memcmp(buffer, "RIFF", 4) || memcmp(buffer+8, "WAVE", 4)) {
    fprintf(stderr, "WaveStream: Not a wave file\n");
    return -1;
  }
  //read format chunk
  if(fread(buffer, 1, 4, file) != 4) goto missing_data;
  if (memcmp(buffer, "fmt ", 4) != 0) {
    fprintf(stderr, "WaveStream: Format chunk not found\n");
    return -1;
  }

  len = igetl(file);          // read chunk length
  
  data_type = igetw(file);            // should be 1 for PCM data
  if (data_type != 1) {
    fprintf(stderr, "WaveStream: Not PCM data\n");
    return -1;
  }
  // TODO: 3 = float data (32bit usually)
  
  channels = igetw(file);     // mono or stereo data
  fprintf(stderr, "File: channels=%d, ", channels);

  freq = igetl(file);         // sample frequency
  fprintf(stderr, "freq=%d, ", freq);

  igetl(file);                // skip six bytes
  igetw(file);
  
  bits = igetw(file);
  fprintf(stderr, "bits=%d\n", bits);
  //TODO: add support for 12bit etc
  if ((bits % 8) != 0 ) {
    fprintf(stderr, "WaveStream: Not 8, 16, 24, or 32bit data\n");
    return -1;
  }

  len -= 16; //we have just read 16 bytes
  header_length = 36;
  
  //read though the rest of the chunk
  for(int j=0; j<len; j++) {
    fgetc(file);
    header_length++;
  }

  while(true) {
    //read data chunk
    if(fread(buffer, 1, 4, file) != 4) goto missing_data;
    len = igetl(file);          // read chunk length
    header_length += 8;
    if(memcmp(buffer, "data", 4) == 0) break;
    if(feof(file)) {
	    fprintf(stderr, "WaveStream: Data chunk not found\n");
	    return -1;
    }
    for(int j=0; j<len; j++)
	    if(fgetc(file) == EOF) {
	      fprintf(stderr, "WaveStream: Data chunk missing\n");
	      return -1;
	    }
  }
  //printf("header_length=%d\n", header_length);
  _total_frames = len / frame_size();

  return 0;

missing_data:
  fprintf(stderr, "Error reading header\n");
  return -1;
}

long WaveStream::read_bytes(void *data, long length)
{
  if(!file || !(mode & F_READ)) return 0;

  long read = fread(data, 1, length, file);
  _pos += read/frame_size();
  if(pos() > totalFrames()) _total_frames = pos();
  return read;
}

long WaveStream::read_frames(void *data, long length)
{
  if(!file || !(mode & F_READ)) return 0;

  long read = fread(data, frame_size(), length, file);
  _pos += read;
  if(pos() > totalFrames()) _total_frames = pos();
  return read;
}

int WaveStream::open_write(const char *filename, int freq_, int channels_, int bits_)
{
  freq = freq_;
  channels = channels_;
  bits = bits_;
  _pos = _total_frames = 0;
  file = fopen(filename, "wb");
  if(!file) return -1;
  mode = F_WRITE;
  write_header();
  return 0;
}

void WaveStream::write_header()
{
  if(!file || !(mode & F_WRITE)) return;

  rewind(file);

  fputs("RIFF", file);                 /* RIFF header */
  iputl(36+data_length(), file);         /* size of RIFF chunk */
  
  fputs("WAVE", file);                 /* WAV definition */
  fputs("fmt ", file);                 /* format chunk */
  iputl(16, file);                     /* size of format chunk */
  //if(sampleType == paFloat32)
  //iputw(3, file);                    /* PCM data 3=floating point*/
  //else
  iputw(1, file);                      /* PCM data 1=two's compliment int*/
  iputw(channels, file);               /* number of channels */
  iputl(freq, file);                   /* sample frequency */
  long byteRate = freq * channels * sample_size();
  iputl(byteRate, file);              /* avg. bytes per sec */
  int blockAlign = channels * sample_size();
  iputw(blockAlign, file);             /* block alignment */
  iputw(bits, file);          /* bits per sample */
  fputs("data", file);                 /* data chunk */
  iputl(data_length(), file);            /* actual data length */
}

long WaveStream::write_bytes(void *data, long length)
{
  if(!file || !(mode & F_WRITE)) return 0;

  long written = fwrite(data, 1, length, file);
  _pos += written/frame_size();
  if(pos() > totalFrames()) _total_frames = pos();
  return written;
}

long WaveStream::write_frames(void *data, long length)
{
  if(!file || !(mode & F_WRITE)) return 0;

  long written = fwrite(data, frame_size(), length, file);
  _pos += written;
  if(pos() > totalFrames()) _total_frames = pos();
  return written;
}

void WaveStream::jump_to_frame(int frame)
{
  frame = bound(frame, 0, totalFrames());
  //if(frame < 0) frame = 0;
  //if(frame > totalFrames()) { printf("over end, %d, %d\n", frame, totalFrames()); frame = totalFrames(); }
  if(fseek(file, header_length + frame*frame_size(), SEEK_SET)) {
    fprintf(stderr, "error seeking, %d\n", errno);
  }
  setPos(frame);
}

void WaveStream::jump_back(int frames)
{
  jump_to_frame(pos() - frames);
}

void WaveStream::jump_forward(int frames)
{
  jump_to_frame(pos() + frames);
}
