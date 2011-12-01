#include "RtAudio.h"
#include "tartini_core.h"
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <deque>
#include <numeric>

int buffer_size = 2048; // in frames
int step_size = 1024; // in frames
int sample_rate = 44100;
char str_buf[160];
char str_temp[160];
float *buffer;
char line_char = '\r';
int inDevice = 0; //default
std::deque<double> pitchHist;

void print_tuning(double pitch, double volume, bool valid) {
	static double prevPitch = 0.0;
	static double avgPitch = 0.0;
	
	if(valid) {
		if(!pitchHist.empty() && fabs(pitchHist.back() - pitch) > 0.2) pitchHist.clear();
		pitchHist.push_back(pitch);
		if(pitchHist.size() > 4) pitchHist.pop_front();
		avgPitch = std::accumulate(pitchHist.begin(), pitchHist.end(), 0.0) / pitchHist.size();
		pitch = avgPitch;
	} else {
		pitchHist.clear();
	}
	
  str_temp[0] = '\0';
  char *name = noteName(toInt(pitch));
  char name1[2] = { name[0], '\0' };
  char name2[2] = { name[1], '\0' };
	  
	if(valid) {
		double pitch_err = pitch +0.5 - floor(pitch+0.5);
		double pitch_err2 = prevPitch +0.5 - floor(prevPitch+0.5);
		int pos = int(floor(pitch_err * 49.0));
		int pos2 = int(floor(pitch_err2 * 49.0));
		int minPos = (pos < pos2) ? pos : pos2;
		int maxPos = (pos < pos2) ? pos2 : pos;
		if(fabs(pitch_err - pitch_err2) > 0.5) { minPos = maxPos = pos; }
		//if(fabs(pitch_err - pitch_err2) < 0.1) drawType = "*"; else drawType = "'";
		strcat(str_temp, " [");
		for(int j=0; j<49; j++) {
			if(j == pos) { strcat(str_temp, "X"); }
			else if(j >= minPos && j <= maxPos) { strcat(str_temp, "-"); }
			else if(j == 24) { strcat(str_temp, name1); }
			else if(j == 25) { strcat(str_temp, name2); }
			else { strcat(str_temp, " "); }
		}
		strcat(str_temp, "]");
  } else {
    //sprintf(str_temp, " [------------------------|------------------------]");
    sprintf(str_temp, " [                        |                        ]");
  }
  int dist = toInt(volume * 30.0);
	for(int j=0; j<30; j++) { strcat(str_temp, (j<dist)?"=":" "); }
	printf("%s%c", str_temp, line_char); fflush(stdout);
	prevPitch = pitch;
}

void print_devices()
{
  RtAudio *audio;

  std::cout << "Device list" << std::endl;
	std::cout << "0: Default" << std::endl;
  // Default RtAudio constructor
  try {
    audio = new RtAudio();
  }
  catch (RtError &error) {
    error.printMessage();
    exit(EXIT_FAILURE);
  }

  // Determine the number of devices available
  int devices = audio->getDeviceCount();

  // Scan through devices for various capabilities
  RtAudioDeviceInfo info;
  for (int i=1; i<=devices; i++) {line_char = '\r';

    try {
      info = audio->getDeviceInfo(i);
    }
    catch (RtError &error) {
      error.printMessage();
      break;
    }

    // Print, for example, the maximum number of output channels for each device
    std::cout << i;
    std::cout << ": " << info.name << std::endl;
    //std::cout << ": maximum output channels = " << info.outputChannels << "\n";
  }

  // Clean up
  delete audio;

}



int main(int argc, char **argv)
{
  printf("Tartini ASCII-tuner, by Philip McLeod\n");
  printf("Press Ctrl-C to quit\n");

	int param = 1;
	while(param < argc) {
		if(argv[param][0] == '-') {
			char letter = argv[param][1];
			if(letter == 'l') { print_devices(); return 0; }
			else if(letter == 'd') {
				param++;
				if(param < argc) { inDevice = atoi(argv[param]); printf("Setting Device Number = %d\n", inDevice); }
			} else if(letter == 'c') { line_char = '\n'; }
		}
    param++;
	}
	
  RtAudio *audio;
  int channels = 1;
  int num_buffers = 4;
  float *input;
  Tartini *t = new_tartini(buffer_size, double(sample_rate));
  double freq, logPower;
  bool valid;
	double pitch;
  buffer = (float*)malloc(buffer_size * sizeof(float));
	std::fill(buffer, buffer+buffer_size, 0.0f);
		
  try {
    //audio = new RtAudio(0, 0, inDevice, channels, RTAUDIO_FLOAT32, sample_rate, &buffer_size, num_buffers);
    audio = new RtAudio(0, 0, inDevice, channels, RTAUDIO_FLOAT32, sample_rate, &step_size, num_buffers);
  } catch (RtError &error) {
    error.printMessage();
    audio = NULL;
    return -1;
  }

	//printf("step_size = %d\n", step_size);
  
  RtAudioDeviceInfo info;
    try {
      info = audio->getDeviceInfo(inDevice);
    }
    catch (RtError &error) {
      error.printMessage();
      return -1;
    }
  std::cout << info.name << std::endl;

  input = (float *) audio->getStreamBuffer(); // Get a pointer to the stream buffer
  audio->startStream();


  int count = 0;
  while (true) {

    // Read a buffer of data
    try {
      audio->tickStream();
    }
    catch (RtError &error) {
      error.printMessage();
      goto cleanup;
    }

		memmove(buffer, buffer+step_size, (buffer_size-step_size)* sizeof(float));
		memcpy(buffer+(buffer_size-step_size), input, step_size * sizeof(float));
		//printf("max = %f\n", *(std::max_element(buffer, buffer+buffer_size)));
		valid = find_pitch(t, buffer, &freq, &logPower);
		pitch = freq2pitch(freq);
		print_tuning(pitch, 1.0 - (logPower / t->noiseFloor), valid);

    count += step_size;
  }


cleanup:
  audio->stopStream();
  audio->closeStream();
  delete audio;

  delete_tartini(t);
	free(buffer);
	
  return 0;
}
