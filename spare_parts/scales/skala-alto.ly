
\version "2.18.2"
% automatically converted by musicxml2ly from skala-alto.xml

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
PartPOneVoiceOne =  \relative c {
    \clef "treble" \key c \major \numericTimeSignature\time 4/4 
    c4. r8
    cis4. r8 | % 2
    d4. r8 es4. r8 | % 3
    e4. r8 f4. r8 | % 4
    fis4. r8 g4. r8 | % 5
    gis4. r8 a4. r8 | % 6
    bes4. r8 b4. r8 | % 7
    c4. r8 cis4. r8 \break | % 8
    d4. r8 es4. r8 | % 9
    e4. r8 f4. r8 | \barNumberCheck #10
    fis4. r8 g4. r8 | % 11
    gis4. r8 a4. r8 | % 12
    bes4. r8 b4. r8 | % 13
    \ottava #1 | % 13
    c4. r8 cis4. r8 \ottava #0 | % 14
    \ottava #1 | % 14
    d4. r8 es4. r8 \ottava #0 | % 15
    \ottava #1 | % 15
    e4. r8 f4. r8 \ottava #0 \break | % 16
    \ottava #1 | % 16
    fis4. r8 g4. r8 \ottava #0 | % 17
    \ottava #1 | % 17
    gis4. r8 a4. r8 \ottava #0 }


% The score definition
\score {
    <<
        \new Staff <<
            \set Staff.midiInstrument = "alto sax"
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

