/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#ifndef TACTION_H
#define TACTION_H


#include <QtCore/qobject.h>


/**
 * @todo write docs
 */
class Taction : public QObject
{

  Q_OBJECT

  Q_PROPERTY(QString icon READ icon WRITE setIconTag NOTIFY iconChanged)
  Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
  Q_PROPERTY(QString tip READ tip WRITE setTip NOTIFY tipChanged)

public:
  explicit Taction(QObject* parent = nullptr);
  ~Taction();

  QString icon() const;
  void setIconTag(const QString& ic);

  QString text() const { return m_text; }
  void setText(const QString& t);

  QString tip() const { return m_tip; }
  void setTip(const QString& t);

  Q_INVOKABLE void trigger();

signals:
  void iconChanged();
  void textChanged();
  void tipChanged();
  void triggered();

private:
  QString                 m_iconTag;
  QString                 m_text;
  QString                 m_tip;
};

#endif // TACTION_H
