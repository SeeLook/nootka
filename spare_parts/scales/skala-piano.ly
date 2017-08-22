
\version "2.18.2"
% automatically converted by musicxml2ly from skala-piano.xml

\header {
    encodingsoftware = "MuseScore 2.1.0"
    encodingdate = "2017-08-22"
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
PartPOneVoiceOne =  \relative c,, {
    \clef "bass" \key c \major \numericTimeSignature\time 4/4 | % 1
    \ottava #-1 c4. \fffff r8 cis4. r8 \ottava #0 | % 2
    \ottava #-1 | % 2
    d4. r8 es4. r8 \ottava #0 | % 3
    \ottava #-1 | % 3
    e4. r8 f4. r8 \ottava #0 | % 4
    \ottava #-1 | % 4
    fis4. r8 g4. r8 \ottava #0 | % 5
    \ottava #-1 | % 5
    gis4. r8 a4. r8 \ottava #0 | % 6
    \ottava #-1 | % 6
    bes4. r8 b4. r8 \ottava #0 | % 7
    c4. r8 cis4. r8 \break | % 8
    d4. r8 es4. r8 | % 9
    e4. r8 f4. r8 | \barNumberCheck #10
    fis4. r8 g4. r8 | % 11
    gis4. r8 a4. r8 | % 12
    bes4. r8 b4. r8 | % 13
    c4. r8 cis4. r8 \break | % 14
    d4. r8 es4. r8 | % 15
    e4. r8 f4. r8 | % 16
    fis4. r8 g4. r8 | % 17
    gis4. r8 a4. r8 | % 18
    bes4. r8 b4. r8 | % 19
    \clef "treble" c4. r8 cis4. r8 | \barNumberCheck #20
    d4. r8 es4. r8 \break | % 21
    e4. r8 f4. r8 \break | % 22
    fis4. r8 g4. r8 | % 23
    gis4. r8 a4. r8 | % 24
    bes4. r8 b4. r8 | % 25
    \ottava #1 | % 25
    c4. r8 cis4. r8 \ottava #0 | % 26
    \ottava #1 | % 26
    d4. r8 es4. r8 \ottava #0 | % 27
    \ottava #1 | % 27
    e4. r8 f4. r8 \ottava #0 | % 28
    \ottava #1 | % 28
    fis4. r8 g4. r8 \ottava #0 | % 29
    \ottava #1 | % 29
    gis4. r8 a4. r8 \ottava #0 \break | \barNumberCheck #30
    \ottava #1 | \barNumberCheck #30
    bes4. r8 b4. r8 \ottava #0 | % 31
    \ottava #2 | % 31
    c4. r8 cis4. r8 \ottava #0 | % 32
    \ottava #2 | % 32
    d4. r8 es4. r8 \ottava #0 | % 33
    \ottava #2 | % 33
    e4. r8 f4. r8 \ottava #0 | % 34
    \ottava #2 | % 34
    fis4. r8 g4. r8 \ottava #0 | % 35
    \ottava #2 | % 35
    gis4. r8 a4. r8 \ottava #0 | % 36
    \ottava #2 | % 36
    bes4. r8 b4. r8 \break | % 37
    c4.  r8 cis4. r8 | % 38
    d4. r8 es4. r8 | % 39
    e4. r8 f4. r8 | \barNumberCheck #40
    fis4. r8 g4. r8 | % 41
    gis4. r8 a4. r8 | % 42
    bes4. r8 b4. r8 | % 43
    c4. r8 cis4. r8 }


% The score definition
\score {
    <<
        \new Staff <<
          \set Staff.midiInstrument = "acoustic grand"
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

