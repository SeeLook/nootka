
\version "2.18.2"
% automatically converted by musicxml2ly from skala-tenor.xml

\header {
    encodingsoftware = "MuseScore 2.1.0"
    encodingdate = "2017-08-20"
    }

#(set-global-staff-size 20.0750126457)
\paper {
    paper-width = 21.0\cm
    paper-height = 29.7\cm
    top-margin = 1.0\cm
    bottom-margin = 2.0\cm
    left-margin = 1.0\cm
    right-margin = 1.0\cm
    }
\layout {
    \context { \Score
        autoBeaming = ##f
        }
    }
PartPOneVoiceOne =  \relative gis, {
    \clef "treble" \key c \major \numericTimeSignature\time 4/4 gis4. r8
    a4. r8 | % 2
    bes4. r8 b4. r8 | % 3
    c4. r8 cis4. r8 | % 4
    d4. r8 es4. r8 | % 5
    e4. r8 f4. r8 | % 6
    fis4. r8 g4. r8 | % 7
    gis4. r8 a4. r8 \break | % 8
    bes4. r8 b4. r8 | % 9
    c4. r8 cis4. r8 | \barNumberCheck #10
    d4. r8 es4. r8 | % 11
    e4. r8 f4. r8 | % 12
    fis4. r8 g4. r8 | % 13
    gis4. r8 a4. r8 | % 14
    bes4. r8 b4. r8 \break | % 15
    \ottava #1 | % 15
    c4. r8 cis4. r8 \ottava #0 | % 16
    \ottava #1 | % 16
    d4. r8 es4. r8 \ottava #0 | % 17
    \ottava #1 | % 17
    e4. r8 f4. r8 \ottava #0 }


% The score definition
\score {
    <<
        \new Staff <<
          \set Staff.midiInstrument = "tenor sax"
            \set Staff.instrumentName = "Octave Mandolin, trackB:voiceA"
            \set Staff.shortInstrumentName = "OM."
            \context Staff << 
                \context Voice = "PartPOneVoiceOne" { \PartPOneVoiceOne }
                >>
            >>
        
        >>
    \layout {}
    % To create MIDI output, uncomment the following line:
    \midi {
      \tempo 4=60
    }
    }

