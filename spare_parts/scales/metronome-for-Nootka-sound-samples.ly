\version "2.20.0"
\language "deutsch"

\header {
  title = "Metronome for Nootka sound samples"
  tagline = ##f
}

\paper {
  #(set-paper-size "a4")
}

global = {
  \key c \major
  \time 4/4
  \tempo 4=120
}

xylophone = \relative c'' {
  \global
  g4 \fffff c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  g4 c c c |
  
}

\score {
  \new Staff \with {
    midiInstrument = "woodblock"
  } \xylophone
  \layout { }
  \midi { }
}
