/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#include "../../src/libs/core/music/trhythm.h"
#include <iostream>
#include <QtCore/qdebug.h>

int main(int argc, char *argv[])
{
  Q_UNUSED(argc)
  Q_UNUSED(argv)

  Trhythm::initialize();

  std::cout << "\033[01;32m Testing Trhythm subtraction \033[01;00m\n";
  TrhythmList rList;
  for (int r = 1; r < 6; ++r) {
    if (r != 5)
      rList << Trhythm(static_cast<Trhythm::Erhythm>(r), false, true); // dot
    rList << Trhythm(static_cast<Trhythm::Erhythm>(r)); // bare
    rList << Trhythm(static_cast<Trhythm::Erhythm>(r), false, false, true); // triplet
  }
  for (int m = 0; m < rList.size(); ++m) {
    for (int s = 0; s < rList.size(); ++s) {
      if (rList[m].duration() >= rList[s].duration() && rList[m].isTriplet() == rList[s].isTriplet()) {
        TrhythmList subList;
        rList[m].sub(rList[s], subList);
        QString out;
        for (int o = 0; o < subList.size(); ++o) {
          out += subList[o].string();
          if (o < subList.size() - 1)
            out += QLatin1String(" and ");
        }
        qDebug() << rList[m].string() << "-" << rList[s].string() << " ==> " << out;
      }
    }
  }

}
