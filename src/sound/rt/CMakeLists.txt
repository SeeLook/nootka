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
  find_library(JACK_LIBRARY
      NAMES
      jack
      PATHS
      /usr/lib
      /usr/lib/x86_64-linux-gnu
      /usr/local/lib
      /opt/local/lib
  )
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
  find_library(PULSE_LIBRARY
      NAMES
      pulse
      PATHS
      /usr/lib
      /usr/lib/x86_64-linux-gnu
      /usr/local/lib
      /opt/local/lib
  )
  
# option(ENABLE_PULSEAUDIO "Enable support for PulseAudio" ON)
# if(ALSA_LIBRARY)
#   add_definitions(-D__LINUX_ALSA__)
#   message(STATUS "support for ALSA\t\t TRUE")
# else(ALSA_LIBRARY)
#   message(STATUS "support for ALSA\t\t FALSE")
# endif(ALSA_LIBRARY)
# if(JACK_INCLUDE_DIR)
#   add_definitions(-D__UNIX_JACK__)
#   include_directories(${JACK_INCLUDE_DIR})
#   message(STATUS "support for JACK\t\t TRUE")
# else(JACK_INCLUDE_DIR)
#   message(STATUS "support for JACK\t\t FALSE")
# endif(JACK_INCLUDE_DIR)
# if(ENABLE_PULSEAUDIO)
#   add_definitions(-D__LINUX_PULSE__)
#   message(STATUS "support for PulseAudio\t\t TRUE")
# else(ENABLE_PULSEAUDIO)
#   message(STATUS "support for PulseAudio\t\t FALSE")
# endif(ENABLE_PULSEAUDIO)

ENDIF(UNIX AND NOT APPLE)