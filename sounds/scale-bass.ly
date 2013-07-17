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
      
      \set Staff.midiInstrument="acoustic bass"
      
      b,,4. r8 c4. r8 cis4. r8 
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
Applying conversion: 2.14.0, 2.15.7, 2.15.9, 2.15.10, 2.15.16,
2.15.17, 2.15.18, 2.15.19, 2.15.20, 2.15.25, 2.15.32, 2.15.39,
2.15.40, 2.15.42, 2.15.43, 2.16.0
%}
