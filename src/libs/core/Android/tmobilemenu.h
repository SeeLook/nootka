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

#ifndef TMOBILEMENU_H
#define TMOBILEMENU_H


#include <QtQuick/qquickitem.h>


class Taction;


/**
 * C++ logic of the main mobile menu button on the screen top-left
 */
class TmobileMenu : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
  Q_PROPERTY(bool extra READ extra NOTIFY extraChanged)
  Q_PROPERTY(QQuickItem* currentFly READ currentFly NOTIFY currentFlyChanged)
  Q_PROPERTY(QVariantList flyActions READ flyActions NOTIFY flyActionsChanged)


public:
  TmobileMenu(QQuickItem* parent = nullptr);
  ~TmobileMenu() override;

  bool pressed() const { return m_pressed; }

  bool extra() const { return m_extra; }

  QQuickItem* currentFly() { return m_currentFlyItem; }

  QVariantList flyActions() { return m_flyActions; }

  Q_INVOKABLE void addAction(Taction* a);

  Q_INVOKABLE int fingerPixels() const;

  Q_INVOKABLE qreal flyX(int itemNr);
  Q_INVOKABLE qreal flyY(int itemNr);

signals:
  void pressedChanged();
  void clicked();
  void extraChanged();
  void currentFlyChanged();
  void flyClicked();
  void flyActionsChanged();

protected:
  void mousePressEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

private:
  bool                        m_pressed = false;
  bool                        m_extra = false;
  QQuickItem                 *m_currentFlyItem = nullptr;
  QVariantList                m_flyActions;
};

#endif // TMOBILEMENU_H
