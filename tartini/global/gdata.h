/***************************************************************************
                          gdata.h  -  
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
#ifndef GDATA_H
#define GDATA_H

#define TARTINI_NAME_STR      "Tartini1.2"
#define TARTINI_VERSION_STR   "1.2.0"
//#define TARTINI_VERSION       1.2

#include <Qt>
#include <qapplication.h>
#include <qobject.h>

#include <qmutex.h>
//Added by qt3to4:
#include <QPixmap>

#include <vector>
//#include "sound_file_stream.h"
//#include "audio_stream.h"
#include "audio_thread.h"
//#include "chirp_xform.h"
//#include "settings.h"
#include <QSettings>

#ifndef WINDOWS
#include <sys/time.h>
#endif
#include <qcolor.h>
#include "array2d.h"
#include "useful.h"
#include "view.h"
#include "analysisdata.h"
extern int gMusicKey;

#ifndef WINDOWS
//for multi-threaded profiling
extern struct itimerval profiler_value;
extern struct itimerval profiler_ovalue;
#endif

#define STREAM_STOP     0
#define STREAM_FORWARD  1
#define STREAM_PAUSE    2
#define STREAM_UPDATE   3

#define SOUND_PLAY      0x01
#define SOUND_REC       0x02
#define SOUND_PLAY_REC  0x03

enum AnalysisModes { MPM, AUTOCORRELATION, MPM_MODIFIED_CEPSTRUM };

#define NUM_WIN_SIZES 5
extern int frame_window_sizes[NUM_WIN_SIZES];
extern const char *frame_window_strings[NUM_WIN_SIZES];

#define NUM_STEP_SIZES 6
extern float step_sizes[NUM_STEP_SIZES];
extern const char *step_size_strings[NUM_STEP_SIZES];

#define NUM_PITCH_METHODS 8
extern const char *pitch_method_strings[NUM_PITCH_METHODS];

#define NUM_INTERPOLATING_TYPES 3
extern const char *interpolating_type_strings[NUM_INTERPOLATING_TYPES];

class FBuffer;
class SoundStream;
class SoundFileStream;
class AudioStream;
class FWinFunc;
class Filter;
class SoundFile;
class Channel;

class GData : public QObject
{
  Q_OBJECT

public:
  enum SavingModes { ALWAYS_ASK, NEVER_SAVE, ALWAYS_SAVE };

  GData(/*int buffer_size_, int winfunc_, float step_size_*/);
  virtual ~GData();

  QSettings *qsettings;

  int soundMode;

  AudioStream *audio_stream;
  bool need_update;

  std::vector<Filter*> filter_hp; //highpass filter
  std::vector<Filter*> filter_lp; //lowpass filter

  //bool equalLoudness;
  //bool useMasking;
  //bool useRidgeFile; //output a file with ridges found
  double cur_note;
  float peakThreshold;
  float correlationThreshold;

  bool doingStuff;
  AudioThread audioThread;
  int sync_flag;

  QMutex frameCounterMutex;
  int frameCounter;

  //char *inputFile;

  //int in_channels;
  //int process_channels;
  //int out_channels;
  //int pitch_method[2]; //a pitch method for each channel
  int interpolating_type;
  int bisection_steps;
  int fast_correlation_repeats;
  int running;
  bool using_coefficients_table;
  //chirp_xform fct;QColor myLineColor1(32, 32, 32);

  //float *fct_in_data;
  //float *fct_out_data;
  //FrameRGB *fct_draw_data;

  std::vector<SoundFile*> soundFiles;
  std::vector<Channel*> channels;
  std::vector<QColor> lineColor;
  int nextColorIndex;

  View *view;

  void setActiveChannel(Channel *toActive);
  Channel* getActiveChannel() { return activeChannel; }
  SoundFile* getActiveSoundFile();

private:
  Channel *activeChannel; /**< Pointer to the active channel */ 
  bool _doingHarmonicAnalysis;
  bool _doingFreqAnalysis;
  bool _doingEqualLoudness;
  bool _doingAutoNoiseFloor;
  int _doingActiveAnalysis;
  int _doingActiveFFT;
  int _doingActiveCepstrum;
  bool _doingDetailedPitch;
  int _fastUpdateSpeed;
  int _slowUpdateSpeed;
  bool _polish;
  bool _showMeanVarianceBars;
  int _savingMode;
  bool _vibratoSineStyle;
  //int _musicKey;
  int _musicKeyType;
  int _temperedType;
  bool _mouseWheelZooms;
  double _freqA;
  double _semitoneOffset;

  //double _noiseThreshold;
  //double _noiseThresholdDB;
  //double _changenessThreshold;
  int _amplitudeMode;
  int _pitchContourMode;
  int _analysisType;
  double _dBFloor;
  double amp_thresholds[NUM_AMP_MODES][2];
  double amp_weights[NUM_AMP_MODES];

  QPixmap *_drawingBuffer;
  QColor _backgroundColor;
  QColor _shading1Color;
  QColor _shading2Color; 
  
  double _leftTime; /**< The lower bound of the start times of all channels */
  double _rightTime; /**< The upper bound of the finish times of all channels */
  double _topPitch; /**< The highest possible note pitch allowed (lowest possible is 0) */

public:
  QPixmap*  drawingBuffer() { return _drawingBuffer; }
  void      setLeftTime(double x); /**< Allows you to specify the leftmost time a file starts */
  void      setRightTime(double x); /**< Allows you to specify the rightmost time a file starts */
  double    leftTime() { return _leftTime; } /**< Returns the leftmost time a file starts */
  double    rightTime() { return _rightTime; }/**< Returns the rightmost time a file stops */
  double    totalTime() { return _rightTime - _leftTime; } /**< Returns the total number of seconds the files take up */
  double    topPitch() { return _topPitch; } /**< Returns the top note pitch the programme allows */
  void      setTopPitch(double y); /**< Allows you to specify the top note pitch the programme should allow */
  void      setDoingActiveAnalysis(bool x) { _doingActiveAnalysis += (x) ? 1 : -1; }
  void      setDoingActiveFFT(bool x) { _doingActiveFFT += (x) ? 1 : -1; }
  void      setDoingActiveCepstrum(bool x) { _doingActiveCepstrum += (x) ? 1 : -1; setDoingActiveFFT(x); }
  void      setDoingDetailedPitch(bool x) { _doingDetailedPitch = x; }
  QColor    getNextColor();

  int       getAnalysisBufferSize(int rate);
  int       getAnalysisStepSize(int rate);
  bool      doingHarmonicAnalysis() { return _doingHarmonicAnalysis; }
  bool      doingAutoNoiseFloor() { return _doingAutoNoiseFloor; }
  bool      doingEqualLoudness() { return _doingEqualLoudness; }
  bool      doingFreqAnalysis() { return _doingFreqAnalysis; }
  bool      doingActiveAnalysis() { return _doingActiveAnalysis; }
  bool      doingActiveFFT() { return _doingActiveFFT; }
  bool      doingActiveCepstrum() { return _doingActiveCepstrum; }
  bool      doingDetailedPitch() { return _doingDetailedPitch; }
  bool      doingActive() { return (doingActiveAnalysis() || doingActiveFFT() || doingActiveCepstrum()); }
  bool      vibratoSineStyle() { return _vibratoSineStyle; }
  int       amplitudeMode() { return _amplitudeMode; }
  int       pitchContourMode() { return _pitchContourMode; }
  int       fastUpdateSpeed() { return _fastUpdateSpeed; }
  int       slowUpdateSpeed() { return _slowUpdateSpeed; }
  bool      mouseWheelZooms() { return _mouseWheelZooms; }

  //double  noiseThreshold() { return _noiseThreshold; }
  //double  noiseThresholdDB() { return _noiseThresholdDB; }
  //double  changenessThreshold() { return _changenessThreshold; }
  //void    setNoiseThresholdDB(double noiseThresholdDB_);
  //void    setChangenessThreshold(double changenessThreshold_);
  void      setAmpThreshold(int mode, int index, double value);
  double    ampThreshold(int mode, int index);
  void      setAmpWeight(int mode, double value);
  double    ampWeight(int mode);
  int       analysisType() { return _analysisType; }
  bool      polish() { return _polish; }
  //void    setPolish(bool polish_) { _polish = polish_; }
  bool      showMeanVarianceBars() { return _showMeanVarianceBars; }
  //void    setShowMeanVarianceBars(bool showMeanVarianceBars_) { _showMeanVarianceBars = showMeanVarianceBars_; }
  int       savingMode() { return _savingMode; }

  QColor    backgroundColor() { return _backgroundColor; }
  QColor    shading1Color() { return _shading1Color; }
  QColor    shading2Color() { return _shading2Color; }
  QString   getFilenameString();
  void      addFileToList(SoundFile *s);
  void      removeFileFromList(SoundFile *s);
  void      clearFreqLookup();
  void      clearAmplitudeLookup();
  void      recalcScoreThresholds();
  int       getActiveIntThreshold();
  double    dBFloor() { return _dBFloor; }
  void      setDBFloor(double dBFloor_) { _dBFloor = dBFloor_; }
  double&   rmsFloor() { return amp_thresholds[AMPLITUDE_RMS][0]; } //in dB
  double&   rmsCeiling() { return amp_thresholds[AMPLITUDE_RMS][1]; } //in dB

  int       musicKey()     { return gMusicKey; }
  int       musicKeyType() { return _musicKeyType; }
  int       temperedType() { return _temperedType; }
  double    freqA() { return _freqA; }
  double    semitoneOffset() { return _semitoneOffset; }

signals:
  void      activeChannelChanged(Channel *active);
  void      activeIntThresholdChanged(int thresholdPercentage);
  void      leftTimeChanged(double x);
  void      rightTimeChanged(double x);
  void      timeRangeChanged(double leftTime_, double rightTime_);
  void      channelsChanged();
  void      onChunkUpdate();

  void      musicKeyChanged(int key);
  void      musicKeyTypeChanged(int type);
  void      temperedTypeChanged(int type);

public slots:
  //void    setBuffers(int freq, int channels);
  //void    setFrameWindowSize(int index);
  //void    setWinFunc(int index);
  //void    setPitchMethod(int channel, int index) { pitch_method[channel] = index; }
  void      setInterpolatingType(int type) { interpolating_type = type; }
  void      setBisectionSteps(int num_steps) { bisection_steps = num_steps; }
  void      setFastRepeats(int num_repeats) { fast_correlation_repeats = num_repeats; }
  void      setAmplitudeMode(int amplitudeMode);
  void      setPitchContourMode(int pitchContourMode);

  void      setMusicKey(int key)      { if(gMusicKey != key) { gMusicKey = key; emit musicKeyChanged(key); } }
  void      setMusicKeyType(int type) { if(_musicKeyType != type) { _musicKeyType = type; emit musicKeyTypeChanged(type); } }
  void      setTemperedType(int type);
  void      setFreqA(double x);
  void      setFreqA(int x) { setFreqA(double(x)); }

  //void    setStepSize(int index);
  void      pauseSound();
  //void    openMicrophone();
  bool      openPlayRecord(SoundFile *sRec, SoundFile *sPlay);
  bool      playSound(SoundFile *s);
  //void    jump_forward(int frames);
  void      updateViewLeftRightTimes();
  void      updateActiveChunkTime(double t);
  void      updateQuickRefSettings();
  
  void      beginning();
  void      rewind();
  bool      play();
  void      stop();
  void      end();
  void      fastforward();

  bool      closeAllFiles();
  void      saveActiveFile();
  void      closeActiveFile();
  QString   saveFileAsk(QString oldFilename);
  int       saveFile(SoundFile *s, QString newFilename);
  int       closeFile(SoundFile *s, int theSavingMode/*, bool ask=true*/);
  void      resetActiveIntThreshold(int thresholdPercentage);

  void      doChunkUpdate();
};

extern GData *gdata;

#endif
