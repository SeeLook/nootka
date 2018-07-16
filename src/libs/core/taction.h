/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
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


#include <nootkacoreglobal.h>
#include <QtCore/qobject.h>


/**
 * Describes action (icon, text, checked).
 * It is used to share actions between Nootka tool bar under desktop
 * and left side drawer under mobile platform
 * Also it keeps QML @p Shortcut through @p QObject,
 * so if @p shortcut() is set, @p key() returns its text representation
 */
class NOOTKACORE_EXPORT Taction : public QObject
{

  Q_OBJECT

  Q_PROPERTY(QString icon READ icon WRITE setIconTag NOTIFY iconChanged)
  Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
  Q_PROPERTY(QString tip READ tip WRITE setTip NOTIFY tipChanged)
  Q_PROPERTY(int tipPos READ tipPos NOTIFY tipChanged)
  Q_PROPERTY(bool checked READ checked WRITE setChecked NOTIFY checkedChanged)
  Q_PROPERTY(bool checkable READ checkable WRITE setCheckable NOTIFY checkableChanged)
  Q_PROPERTY(QObject* shortcut READ shortcut WRITE setShortcut)
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
  explicit Taction(QObject* parent = nullptr);
  explicit Taction(const QString& txt, const QString& ico, QObject* parent = nullptr, bool isEnabled = true);
  ~Taction() override;

  QString icon() const;
  void setIconTag(const QString& ic);

  QString text() const { return m_text; }
  void setText(const QString& t);

  QString tip() const { return m_tip; }
  void setTip(const QString& t, int pos = 1);

  int tipPos() const { return m_tipPos; }

  bool checkable() const { return m_checkable; }
  void setCheckable(bool ch);

  bool checked() const { return m_checked; }
  void setChecked(bool ch);

  QObject* shortcut() { return m_shortcut; }
  void setShortcut (QObject* s);

  bool enabled() const { return m_enabled; }
  void setEnabled(bool e);

      /**
       * Emits @p shakeButton() signal to make corresponding button shaking to attract user attention
       */
  void shake() { emit shakeButton(); }

  Q_INVOKABLE void trigger();
  Q_INVOKABLE QString key();

public slots:
  void triggerSlot() { trigger(); }

signals:
  void iconChanged();
  void textChanged();
  void tipChanged();
  void triggered();
  void checkedChanged();
  void checkableChanged();
  void enabledChanged();
  void shakeButton();

private:
  bool                    m_checkable = false;
  bool                    m_checked = false;
  bool                    m_enabled = true;
  QString                 m_iconTag;
  QString                 m_text;
  QString                 m_tip;
  QObject                *m_shortcut = nullptr;
  quint8                  m_tipPos = 1; /**< It corresponds with @p QQUickItem::TransformOrigin, 1 means Top center  */
};

#endif // TACTION_H
