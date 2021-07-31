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

#ifndef TMELODYIMPORTITEM_H
#define TMELODYIMPORTITEM_H


#include <QtQuick/qquickitem.h>


class TmelodyPart;


/**
 * C++ proxy for QML @p MelodyImport dialog window.
 */
class TmelodyImportItem : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(QString title READ title NOTIFY melodyChanged)
  Q_PROPERTY(QList<QObject*> partsModel READ partsModel NOTIFY partsModelChanged)
  Q_PROPERTY(int globalSplitNr READ globalSplitNr WRITE setGlobalSplitNr NOTIFY globalSplitNrChanged)
  Q_PROPERTY(bool multiSelect READ multiSelect WRITE setMultiSelect NOTIFY melodyChanged)

public:
  explicit TmelodyImportItem(QQuickItem* parent = nullptr);
  ~TmelodyImportItem();

  QString title() const;

      /**
       * @p partsModel is available only when XML parsing thread is done.
       * So at the beginning it is empty.
       */
  QList<QObject*> partsModel() const;

  int globalSplitNr() const;
  void setGlobalSplitNr(int gsn);

      /**
       * @p TRUE when there are more melodies to import
       */
  bool multiSelect() const;
  void setMultiSelect(bool ms);

      /**
       * Emits @p TimportScore::importReady() signal
       * so import caller can process what is to import
       */
  Q_INVOKABLE void emitImport();

  Q_INVOKABLE void transpose(int semis, bool outScaleToRes, bool inInstrScale, TmelodyPart* part);

  Q_INVOKABLE void selectNoteInChords(int noteNr, bool fromTop);

  Q_INVOKABLE void arpeggiateChords();

      /**
       * Method invoked by QML when Import window is created.
       */
  Q_INVOKABLE void importWindowReady();

signals:
  void melodyChanged();
  void globalSplitNrChanged();
  void partsModelChanged();

};


#endif // TMELODYIMPORTITEM_H
