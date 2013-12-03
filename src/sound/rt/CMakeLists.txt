IF(UNIX AND NOT APPLE)    
    ##### PulseAudio ###########################################
      find_path(PULSE_INCLUDE_DIR
        NAMES
          pulseaudio.h
        PATHS
          /usr/include
          /usr/include/pulse
          /usr/local/include
          /opt/local/include
          /sw/include
          
      )
      if (PULSE_INCLUDE_DIR)
          find_library(PULSESIMPLE_LIBRARY
              NAMES
              pulse-simple
              PATHS
              /usr/lib
              /usr/lib/x86_64-linux-gnu
              /usr/local/lib
              /opt/local/lib
          )
          find_library(PULSEERROR_LIBRARY
              NAMES
              pulse
              PATHS
              /usr/lib
              /usr/lib/x86_64-linux-gnu
              /usr/local/lib
              /opt/local/lib
          )
      endif (PULSE_INCLUDE_DIR)
      
    if (PULSE_INCLUDE_DIR)
        option(ENABLE_PULSEAUDIO "Enable support for PulseAudio" ON)
    else(PULSE_INCLUDE_DIR)
        option(ENABLE_PULSEAUDIO "Enable support for PulseAudio" OFF)
        set(PULSEERROR_LIBRARY_FOUND TRUE)
        set(PULSESIMPLE_LIBRARY_FOUND TRUE)
    endif (PULSE_INCLUDE_DIR)
    
ENDIF(UNIX AND NOT APPLE)
