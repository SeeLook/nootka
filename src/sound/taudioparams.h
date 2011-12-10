#ifndef TAUDIOPARAMS_H
#define TAUDIOPARAMS_H

#include <QString>


  /** class describes audio input parameters. */
class TaudioParams {
  
public:
// audio input settings
  bool INenabled; // is audio input enabled
  QString INdevName; // input device name
  float a440diff; // difference betwen standard a1 440Hz and user prefered base pitch
  qint16 noiseLevel; // ?????
    /** If true - pitch is average of all visible pitches
     * if false - the first detected in sound over noise is taken.  */
  bool isVoice; 

// audio output settings
  bool OUTenabled;
  QString OUTdevName;
  bool midiEnabled; // default false
  QString midiPortName; // default empty to find system default
  unsigned char midiInstrNr; // default 0 - grand piano
};


#endif // TAUDIOPARAMS_H