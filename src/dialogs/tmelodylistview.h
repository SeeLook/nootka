/***************************************************************************
 *   Copyright (C) 2018-2021 by Tomasz Bojczuk                             *
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

#ifndef TMELODYLISTVIEW_H
#define TMELODYLISTVIEW_H

#include <QtCore/qabstractitemmodel.h>
#include <QtQuick/qquickitem.h>

class TscoreObject;
class Tmelody;
class Tlevel;
class TmelodyWrapper;

#define MELODY_LENGHT (15)

/**
 * @class TmelodyListView manages melody previews logic displayed by QML.
 * It handles operations on melodies:
 * @p loadMelody(), @p removeMelody() and @p swapMelodies().
 * Operates directly on working @p Tlevel member of @p TlevelCreatorItem.
 */
class TmelodyListView : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QObject *melodyModel READ melodyModel WRITE setMelodyModel)

public:
    explicit TmelodyListView(QQuickItem *parent = nullptr);
    ~TmelodyListView() override;

    QObject *melodyModel() { return m_melodyModel; }
    void setMelodyModel(QObject *mm);

    Q_INVOKABLE void setLevel(Tlevel *l);

    Q_INVOKABLE void loadMelody();
    Q_INVOKABLE void removeMelody(int id);
    Q_INVOKABLE void swapMelodies(int from, int to);

    Q_INVOKABLE QVariant getMelody(int melId);

    Q_INVOKABLE void divideMelody(int melId, int bars);
    Q_INVOKABLE void transpose(int semis, bool outScaleToRes, bool inInstrScale, TmelodyWrapper *wrapp);

signals:
    void appendMelody();
    void insertMelody(int melId);
    void melodiesChanged();
    void melodiesCountChanged();

protected:
    /**
     * Loads melodies from @p Tlevel after @p setLevel().
     * It has to be invoked with delay
     */
    void loadMelodies();

    void processNextXmlFile();
    void melodyImportSlot();

private:
    Tlevel *m_level = nullptr;
    QObject *m_melodyModel = nullptr;
    QStringList m_xmlFiles;
    int m_lastMelodyId = -1; /**< Index of the latest melody that was added by @p loadMelodies()  */
};

#endif // TMELODYLISTVIEW_H
