/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                  				         *
 *   seelook@gmail.com        						                                 *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

package net.sf.nootka;

import org.qtproject.qt5.android.QtNative;
import java.lang.String;
import android.widget.Toast;

public class Ttoast
{
  private static String m_messageData = "";

    /** @p show() */
  public static void show(String message) {
    m_messageData = message;

    new Thread() {
      public void run() {
        QtNative.activity().runOnUiThread(new Runnable() {
          public void run() {
            Toast t = Toast.makeText(QtNative.activity(), m_messageData, Toast.LENGTH_LONG);
//                   t.setGravity(Gravity.TOP|Gravity.CENTER_HORIZONTAL, 0, 0);
            t.show();
          }
          });
        }
    }.start();
  }

}
