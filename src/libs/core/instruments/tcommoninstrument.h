/***************************************************************************
 *   Copyright (C) 2017-2021 by Tomasz Bojczuk                             *
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

#ifndef TCOMMONINSTRUMENT_H
#define TCOMMONINSTRUMENT_H

#include "music/tnote.h"
#include "music/ttechnical.h"
#include "nootkacoreglobal.h"
#include <QtQuick/qquickpainteditem.h>

/**
 * Base class for all instruments
 */
class NOOTKACORE_EXPORT TcommonInstrument : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(Tnote note READ note WRITE setNote NOTIFY noteChanged)
    Q_PROPERTY(bool outOfScale READ outOfScale NOTIFY outOfScaleChanged)
    Q_PROPERTY(QQuickItem *wrongItem READ wrongItem NOTIFY correctInstrument)
    Q_PROPERTY(QQuickItem *goodItem READ goodItem NOTIFY correctInstrument)
    Q_PROPERTY(QQuickItem *extraName READ extraName WRITE setExtraName NOTIFY extraNameChanged)

public:
    TcommonInstrument(QQuickItem *parent = nullptr);
    ~TcommonInstrument() override;

    /**
     * @p TRUE when mouse cursor is over
     */
    bool active() { return m_active; }

    bool outOfScale() const { return m_outOfScale; }

    QQuickItem *wrongItem() { return p_wrongItem; }
    QQuickItem *goodItem() { return p_goodItem; }

    QQuickItem *extraName() { return m_extraNameItem; }
    void setExtraName(QQuickItem *en) { m_extraNameItem = en; }

    /**
     * Returns text of extra note name displayed on instrument during exercises.
     * Or empty string when nothing.
     */
    QString extraNoteName() const { return p_extraName; }

    Tnote note() const { return p_note; }
    virtual void setNote(const Tnote &n, quint32 noteDataValue = NO_TECHNICALS) = 0;

    /**
     * @p noteData is extra information about note needed for some instruments (guitars, bandoneon).
     * In case of guitar it is more important than note itself
     */
    virtual void askQuestion(const Tnote &n, quint32 noteData) = 0;

    /**
     * Highlight appropriate instrument part to point where to put an answer.
     * For guitar it will be a string, for bandoneon bellow direction and side
     */
    virtual void highlightAnswer(const Tnote &n, quint32 noteData) = 0;

    /**
     * Returns additional note data like position on the guitar or left/right hand on bandoneon
     */
    virtual int technical() = 0;

    /**
     * Sets color of selected item (finger, button, key - depends on instrument type)
     * to @p markColor if different than transparent, or to global color of selection
     */
    virtual void markSelected(const QColor &markColor) = 0;

    /**
     * Extra note name on instrument
     * This method in common part which just sets @p p_extraName note name
     * in given @p EnameStyle and @p textColor.
     * Every subclassing instrument has to override it
     * ant emit @p wantNoteName() with informations to precess by QML
     */
    virtual void showNoteName(Tnote::EnameStyle st, const Tnote &n, quint32 techn, const QColor &textColor);

    /**
     * Correcting answer logic:
     * @p correct() method is called by exam executor.
     * This method prepares correction and emits @p correctInstrument() signal.
     * QML side (instrument implementation) starts animation then on @p p_wrongItem
     * and when incorrect item is hidden by QML, it calls @p applyCorrect()
     * to prepare correct answer and @p p_goodItem.
     * After that QML continues animations and when finished,
     * @p finishCorrectAnim() is invoked and @p correctionFinished() signal emitted.
     */
    virtual void correct(const Tnote &n, quint32 noteData) = 0;

    virtual void applyCorrect() { }

    Q_INVOKABLE void finishCorrectAnim() { emit correctionFinished(); }

    /**
     * Changes border of given @p item but only if the item has border property (QML Rectangle)
     */
    void markBorder(QQuickItem *item, int borderWidth, const QColor &borderColor);

    /**
     * Reset wrong/good item pointers to null to be sure that
     * nothing spoils normal instrument behavior
     */
    void restoreAfterExam();

signals:
    void activeChanged();
    void noteChanged();
    void outOfScaleChanged();
    void correctInstrument();
    void correctionFinished();
    void extraNameChanged();

    /**
     * Emitted to display extra note name on the visual QML instrument.
     * @p name is note name and @p origin could be everything:
     * - position to display name text at
     * - QML item to show name on it.
     * Corresponding QML item has to understand what @p origin is.
     */
    void wantNoteName(const QString &name, const QVariant &origin);

protected:
    void hoverEnterEvent(QHoverEvent *) override;
    void hoverLeaveEvent(QHoverEvent *) override;

    void setOutOfScale(bool out);

    Tnote p_note;
    QQuickItem *p_wrongItem = nullptr;
    QQuickItem *p_goodItem = nullptr;
    QString p_extraName;

private:
    bool m_active = false;
    bool m_outOfScale = false;
    QQuickItem *m_extraNameItem = nullptr;
};

#endif // TCOMMONINSTRUMENT_H
