/***************************************************************************
 *   Copyright (C) 2014-2021 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tchunk.h"

#include <QtCore/qdebug.h>
#include <QtCore/qxmlstream.h>

Tchunk::Tchunk(const Tnote &pitch, const TfingerPos &fretPos)
    : m_pitch(pitch)
{
    m_technical.setFingerPos(fretPos);
}

Tchunk::Tchunk(const Tnote &pitch, const Ttechnical &technical)
    : m_pitch(pitch)
    , m_technical(technical)
{
}

/**
 * @p MusicXML <note> elements required order (possible used in Nootka)
 *   <pitch>
 *   <unpitched>
 *   <rest>
 *   <duration> (Required)
 *   <tie>
 *   <voice> - no voices in Nootka
 *   <instrument> - unused
 *   <type> (Optional)
 *   <dot> (Zero or more times)
 *   <accidental> - unused, <alter> in <pitch> instead
 *   <time-modification> (Optional)
 *   <stem> (Optional)
 *   <notehead> - unused
 *   <notehead-text> - unused
 *   <staff> (Optional)
 *   <beam> (0 to 8 times)
 *   <notations> (Zero or more times)
 */
void Tchunk::toXml(QXmlStreamWriter &xml, int *staffNr)
{
    xml.writeStartElement(QLatin1String("note"));
    if (m_pitch.isRest() || !m_pitch.isValid())
        xml.writeEmptyElement(QLatin1String("rest"));
    else
        m_pitch.toXml(xml); // or <pitch>
    xml.writeTextElement(QLatin1String("duration"), QString::number(m_pitch.rtm.isValid() ? m_pitch.duration() : Trhythm(Trhythm::Quarter).duration()));
    if (m_pitch.rtm.tie())
        tieToXml(xml, m_pitch.rtm.tie(), e_tie);
    xml.writeTextElement(QLatin1String("type"), m_pitch.rtm.xmlType());
    if (m_pitch.hasDot())
        xml.writeEmptyElement(QLatin1String("dot"));
    //     if (m_pitch.alter && !m_pitch.isRest() && m_pitch.isValid()) {
    //       QString accidentalTag;
    //       switch (m_pitch.alter) {
    //         case -2: accidentalTag = QStringLiteral("flat-flat"); break;
    //         case -1: accidentalTag = QStringLiteral("flat"); break;
    //         case 1: accidentalTag = QStringLiteral("sharp"); break;
    //         case 2: accidentalTag = QStringLiteral("double-sharp"); break;
    //         default: break;
    //       }
    //       if (!accidentalTag.isEmpty())
    //         xml.writeTextElement(QLatin1String("accidental"), accidentalTag);
    //     }
    if (m_pitch.rhythm() == Trhythm::NoRhythm) {
        if (!m_pitch.isRest() && m_pitch.isValid())
            xml.writeTextElement(QLatin1String("stem"), QLatin1String("none"));
    }
    if (staffNr)
        xml.writeTextElement(QLatin1String("staff"), QString::number(*staffNr));
    if (m_pitch.rtm.beam()) {
        xml.writeStartElement(QStringLiteral("beam"));
        xml.writeAttribute(QStringLiteral("number"), QStringLiteral("1"));
        xml.writeCharacters(beamToString(m_pitch.rtm.beam()));
        xml.writeEndElement(); // beam
    }
    if (m_pitch.rtm.tie() || !m_technical.isEmpty()) {
        xml.writeStartElement(QLatin1String("notations"));
        if (!m_technical.isEmpty())
            m_technical.toXml(xml);
        if (m_pitch.rtm.tie())
            tieToXml(xml, m_pitch.rtm.tie(), e_tied);
        xml.writeEndElement();
    }
    xml.writeEndElement(); // note
}

static bool tupletWarn = true, rtmWarn = true, moreDotsWarn = true, dot16Warn = true;

/**
 * So far, returned @p FALSE value is used to inform that chunk (a note) was in other voice than 'first'
 * More voices are not (and never be) supported...
 * Tie is recognized by <notations><tied type="" ></tied></notations> tag, <tie /> itself is ignored,
 * but during save, both tags are writing.
 */
quint16 Tchunk::fromXml(QXmlStreamReader &xml, int *staffNr, int *voiceNr)
{
    quint16 ok = e_xmlOK;
    int stNr = 1;
    m_pitch.setRhythm(Trhythm(Trhythm::NoRhythm));
    while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("grace")) {
            ok |= e_xmlUnsupported | e_xmlIsGrace;
            xml.skipCurrentElement();
        } else if (xml.name() == QLatin1String("chord")) {
            ok |= e_xmlUnsupported | e_xmlIsChord;
            xml.skipCurrentElement();
        } else if (xml.name() == QLatin1String("pitch"))
            m_pitch.fromXml(xml);
        else if (xml.name() == QLatin1String("type")) {
            auto rtmType = xml.readElementText();
            m_pitch.setRhythmValue(rtmType.toStdString());
            if (!rtmType.isEmpty() && m_pitch.rhythm() == Trhythm::NoRhythm) {
                if (rtmWarn) {
                    qDebug() << "[Tchunk] Unsupported rhythmic value:" << rtmType;
                    rtmWarn = false;
                }
                ok |= e_xmlUnsupported | e_xmlIsStrangeRtm;
            }
        } else if (xml.name() == QLatin1String("time-modification")) {
            ok |= e_xmlUnsupported | e_xmlIsTupletStart; // no matter start/stop - unsupported anyway
            if (tupletWarn) {
                qDebug() << "[Tchunk] Tuplets are not supported.";
                tupletWarn = false;
            }
            xml.skipCurrentElement();
        } else if (xml.name() == QLatin1String("rest")) {
            m_pitch.setRest(true);
            xml.skipCurrentElement();
        } else if (xml.name() == QLatin1String("dot")) {
            if (m_pitch.rhythm() < Trhythm::Sixteenth) {
                if (m_pitch.hasDot()) {
                    if (ok & e_xmlHasTwoDots) {
                        if (moreDotsWarn) {
                            qDebug() << "[Tchunk] More than two augmented dots are not supported!";
                            moreDotsWarn = false;
                        }
                    } else if (m_pitch.rhythm() < Trhythm::Eighth)
                        ok |= e_xmlHasTwoDots;
                } else
                    m_pitch.setDot(true);
            } else {
                if (dot16Warn) {
                    qDebug() << "[Tchunk] Sixteenth with dots are not supported!";
                    dot16Warn = false;
                }
                ok |= e_xmlUnsupported | e_xmlIsStrangeRtm;
            }
            xml.skipCurrentElement();
        } else if (xml.name() == QLatin1String("notations")) {
            while (xml.readNextStartElement()) {
                if (xml.name() == QLatin1String("technical"))
                    m_technical.fromXml(xml);
                else if (xml.name() == QLatin1String("tied")) {
                    auto type = xml.attributes().value(QStringLiteral("type"));
                    Trhythm::Etie tie = Trhythm::e_noTie;
                    if (type == QLatin1String("start"))
                        tie = m_pitch.rtm.tie() ? Trhythm::e_tieCont : Trhythm::e_tieStart; // tie was set already - means tie starts and stops on this note
                    else if (type == QLatin1String("stop"))
                        tie = m_pitch.rtm.tie() ? Trhythm::e_tieCont : Trhythm::e_tieEnd; // tie was set already - means tie starts and stops on this note
                    m_pitch.rtm.setTie(tie);
                    xml.skipCurrentElement();
                    // TODO: we are detecting tuplets in <time-modification>
                    // below tag is only for visualization, but may be useful to obtain tuplet start/stop
                    //             } else if (xml.name() == QLatin1String("tuplet")) {
                    //                 auto type = xml.attributes().value(QStringLiteral("type"));
                    //                 if (type == QLatin1String("start"))
                    //                   ok |= e_xmlUnsupported | e_xmlIsTupletStart;
                    //                 else if (type == QLatin1String("stop"))
                    //                   ok |= e_xmlUnsupported | e_xmlIsTupletStop;
                    //                 else
                    //                   qDebug() << "[Tchunk] Error in XML tuplet type! Ignored!";
                    //                 qDebug() << "[Tchunk] Tuplets are not supported.";
                    //                 xml.skipCurrentElement();
                } else
                    xml.skipCurrentElement();
            }
        } else if (xml.name() == QLatin1String("voice")) {
            int vNr = xml.readElementText().toInt();
            if (voiceNr)
                *voiceNr = vNr;
            else if (vNr != 1)
                ok |= e_xmlUnsupported;
        } else if (xml.name() == QLatin1String("staff"))
            stNr = xml.readElementText().toInt();
        else
            xml.skipCurrentElement();
    }
    if (staffNr)
        *staffNr = stNr;
    return ok;
}

QString Tchunk::beamToString(Trhythm::Ebeam b)
{
    switch (b) {
    case Trhythm::e_beamStart:
        return QStringLiteral("begin");
    case Trhythm::e_beamCont:
        return QStringLiteral("continue");
    case Trhythm::e_beamEnd:
        return QStringLiteral("end");
    default:
        return QString();
    }
}

void Tchunk::tieToXml(QXmlStreamWriter &xml, Trhythm::Etie t, Tchunk::EtieTag tag)
{
    QString tieTag = tag == e_tie ? QLatin1String("tie") : QLatin1String("tied");
    xml.writeStartElement(tieTag);
    xml.writeAttribute(QLatin1String("type"), t == Trhythm::e_tieStart ? QLatin1String("start") : QLatin1String("stop"));
    xml.writeEndElement();
    if (t == Trhythm::e_tieCont)
        tieToXml(xml, Trhythm::e_tieStart, tag);
}
