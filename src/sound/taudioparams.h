#ifndef TAUDIOPARAMS_H
#define TAUDIOPARAMS_H

#include <QString>


  /** class describes audio input parameters. */
class TaudioParams {
  
public:
  
      /** Types of detection methods. */
  enum EanalysisModes 
      { e_MPM = 0, e_AUTOCORRELATION = 1, e_MPM_MODIFIED_CEPSTRUM = 2 };
  bool INenabled; // is audio input enabled
  QString INdevName; // input device name
  float a440diff; // difference betwen standard a1 440Hz and user prefered base pitch
  qint16 noiseLevel; // ?????
  EanalysisModes analysisType; // Types of detection methods
  bool isVoice; 
  
  bool OUTenabled;
  QString OUTdevName;
};


#endif // TAUDIOPARAMS_H