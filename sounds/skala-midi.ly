% Lily was here -- automatically converted by /usr/local/lilypond/usr/bin/midi2ly from skala.mid
\version "2.13.53"

\layout {
  \context {
    \Voice
    \remove "Note_heads_engraver"
    \consists "Completion_heads_engraver"
    \remove "Rest_engraver"
    \consists "Completion_rest_engraver"
  }
}

trackAchannelA = {


  \key c \major
    
  \time 4/4 
  

  \key c \major
  
  \tempo 4 = 60 
  
}

trackA = <<
  \context Voice = voiceA \trackAchannelA
>>


trackBchannelA = {
  
  
  \set Staff.midiInstrument="acoustic guitar (nylon)"
  
}

trackBchannelB = \relative c {
  \clef "treble_8"
  
  \set Staff.midiInstrument="acoustic guitar (nylon)"
  c,4. \fff r8 cis4. r8 
  | % 2
  d4. r8 dis4. r8 
  | % 3
  e4. r8 f4. r8 
  | % 4
  fis4. r8 g4. r8 
  | % 5
  gis4. r8 a4. r8 
  | % 6
  ais4. r8 b4. r8 
  | % 7
  c4. r8 cis4. r8 
  | % 8
  d4. r8 dis4. r8 
  | % 9
  e4. r8 f4. r8 
  | % 10
  fis4. r8 g4. r8 
  | % 11
  gis4. r8 a4. r8 
  | % 12
  ais4. r8 b4. r8 
  | % 13
  c4. r8 cis4. r8 
  | % 14
  d4. r8 dis4. r8 
  | % 15
  e4. r8 f4. r8 
  | % 16
  fis4. r8 g4. r8 
  | % 17
  gis4. r8 a4. r8 
  | % 18
  ais4. r8 b4. r8 
  | % 19
  c4. r8 cis4. r8 
  | % 20
  d4. r8 dis4. r8 
  | % 21
  e4. r8 f4. r8 
  | % 22
  fis4. r8 g4. r8 
  | % 23
  gis4. r8 a4. r8 
  | % 24
  ais4. r8 b4. r8 
  | % 25
  c4. r8 cis4. r8 
  | % 26
  d4. r8 dis4. r8 
  | % 27
  e4. 
}

trackB = <<
  \context Voice = voiceA \trackBchannelA
  \context Voice = voiceB \trackBchannelB
>>


\score {
  <<
    \context Staff=trackB \trackA
    \context Staff=trackB \trackB
  >>
  \layout {}
  \midi {
    \context {
      \Score
      tempoWholesPerMinute = #(ly:make-moment 50 4)
      }
  }
  
}
