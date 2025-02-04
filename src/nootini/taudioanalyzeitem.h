/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#ifndef TAUDIOANALYZEITEM_H
#define TAUDIOANALYZEITEM_H

#include <QtCore/qfile.h>
#include <QtQuick/qquickitem.h>

class TcommonListener;
class TnoteStruct;

#define ANALYZER TaudioAnalyzeItem::instance()

/**
 * This is a proxy QML item with C++ logic of audio/sound note data,
 */
class TaudioAnalyzeItem : public QQuickItem
{
    Q_OBJECT

public:
    explicit TaudioAnalyzeItem(QQuickItem *parent = nullptr);
    ~TaudioAnalyzeItem();

    static TaudioAnalyzeItem *instance() { return m_instance; }

    /**
     * If set, it is audio file in supported format
     * to be processed when sound is initialized.
     */
    static void processAudioFile(const QString &fileName);

signals:
    void dataReady(qreal pcmVol, qreal energy, bool onSet, const QString &note);
    void noteData(int start, int end, const QString &note);

protected:
    void soundInitSlot();
    void volumeSlot();
    void noteFinishedSlot(const TnoteStruct &n);
    void doProcess();

private:
    TcommonListener *m_sniffer = nullptr;
    static TaudioAnalyzeItem *m_instance;
    QFile m_audioFile;
};

#endif // TAUDIOANALYZEITEM_H
