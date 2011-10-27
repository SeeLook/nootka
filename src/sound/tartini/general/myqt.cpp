#include "myqt.h"

/** Gives a color between a and b, using ratio (a number between 0 and 1)
*/
QColor colorBetween(QColor a, QColor b, double ratio)
{
  //myassert(ratio >= 0.0 && ratio <= 1.0);
  if(ratio < 0.0) ratio = 0.0;
  if(ratio > 1.0) ratio = 1.0;
  return QColor(
    a.red()+int(ratio*float(b.red()-a.red())),
    a.green()+int(ratio*float(b.green()-a.green())),
    a.blue()+int(ratio*float(b.blue()-a.blue())));
}
