% Lily was here -- automatically converted by /usr/local/lilypond/usr/bin/midi2ly from skala.mid
\version "2.16.0"

\layout {
  \context {
    \Voice
    \remove "Note_heads_engraver"
    \consists "Completion_heads_engraver"
    \remove "Rest_engraver"
    \consists "Completion_rest_engraver"
  }
}

\score {
  \new StaffGroup <<
      \new Staff {
  \relative c {
    
      \clef "bass_8"
      \key c \major    
      \time 4/4   
      \tempo 4 = 60
      
      \set Staff.midiInstrument="electric bass (finger)"
      
      b,,4 \ff r4 c4 r4 cis4 r4 
      | % 2
      d4 r4 dis4 r4 
      | % 3
      e4 r4 f4 r4 
      | % 4
      fis4 r4 g4 r4 
      | % 5
      gis4 r4 a4 r4 
      | % 6
      ais4 r4 b4 r4 
      | % 7
      c4 r4 cis4 r4 
      | % 8
      d4 r4 dis4 r4 
      | % 9
      e4 r4 f4 r4 
      | % 10
      fis4 r4 g4 r4 
      | % 11
      gis4 r4 a4 r4 
      | % 12
      ais4 r4 b4 r4 
      | % 13
      c4 r4 cis4 r4 
      | % 14
      d4 r4 dis4 r4 
      | % 15
      e4 r4 f4 r4 
      | % 16
      fis4 r4 g4 r4 
      | % 17
      gis4 r4 a4 r4 
      | % 18
      ais4 r4 b4 r4 
      | % 19
      c4 r4 cis4 r4 
      | % 20
      d4 r4 dis4 r4 
      | % 21
      e4 r4 f4 r4 
      | % 22
      fis4 r4 g4 r4 
      | % 23
      gis4 r4 a4 r4 
       
    }
  }
>>

\layout {}
  \midi {
    \tempo 4 = 60
  }
}


%{
convert-ly (GNU LilyPond) 2.16.2  convert-ly: Processing `'...
Applying conversion: 2.140, 2.15.7, 2.15.9, 2.15.10, 2.15.16,
2.15.17, 2.15.18, 2.15.19, 2.15.20, 2.15.25, 2.15.32, 2.15.39,
2.15.40, 2.15.42, 2.15.43, 2.16.0
%}
