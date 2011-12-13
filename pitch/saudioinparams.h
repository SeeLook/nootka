#ifndef SAUDIOINPARAMS_H
#define SAUDIOINPARAMS_H

#include <QString>


  /** Types of detection methods. */
enum EanalysisModes { e_MPM = 0, e_AUTOCORRELATION = 1, e_MPM_MODIFIED_CEPSTRUM = 2 };

  /** Structure describes audio input parameters. */
struct SaudioInParams {
  bool enabled; // is audio input enabled
  QString devName; // input device name
  float a440diff; // difference betwen standard a1 440Hz and user prefered base pitch
  qint16 noiseLevel;
  EanalysisModes analysisType; // Types of detection methods
  bool isVoice; 
  bool equalLoudness; // It's kinda low pass filter
  bool doingAutoNoiseFloor;
};


#endif // SAUDIOINPARAMS_H