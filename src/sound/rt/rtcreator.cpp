/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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

#include "rtcreator.h"

RtAudio* getRtAudio() {
  RtAudio *rta = new RtAudio();
// #if defined(__LINUX_PULSE__)
//   if (rta->getCurrentApi() != RtAudio::UNIX_JACK) {
//     bool isPulse = true;
//     RtAudio *rtPulse;
//     try {
//       rtPulse = new RtAudio(RtAudio::LINUX_PULSE);
//     } catch (RtError& e) {
//         isPulse = false;
//     }
//       if (isPulse && rtPulse->getDeviceCount() > 0) {
//         delete rta;
//         rta = rtPulse;
//       }
//   }
// #endif
  return rta;
}