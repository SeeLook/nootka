IF(UNIX AND NOT APPLE)    
    ##### JACK - Jack Audio Connection Kit ##############
      find_path(JACK_INCLUDE_DIR
        NAMES
          jack.h
        PATHS
          /usr/include
          /usr/include/jack
          /usr/local/include
          /opt/local/include
          /sw/include
          
      )
      if (JACK_INCLUDE_DIR)
          find_library(JACK_LIBRARY
              NAMES
              jack
              PATHS
              /usr/lib
              /usr/lib/x86_64-linux-gnu
              /usr/local/lib
              /opt/local/lib
          )
      endif (JACK_INCLUDE_DIR)
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

    if (JACK_INCLUDE_DIR)
        option(ENABLE_JACK "Enable support for JACK (Jack Audio Connection Kit)" ON)
    else (JACK_INCLUDE_DIR)    
        option(ENABLE_JACK "Enable support for JACK (Jack Audio Connection Kit)" OFF)
        set(JACK_LIBRARY_FOUND TRUE)
    endif (JACK_INCLUDE_DIR)
    
ENDIF(UNIX AND NOT APPLE)
