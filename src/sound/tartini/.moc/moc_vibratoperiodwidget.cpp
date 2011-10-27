/****************************************************************************
** Meta object code from reading C++ file 'vibratoperiodwidget.h'
**
** Created: Thu Jun 21 21:46:01 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/vibrato/vibratoperiodwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vibratoperiodwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_VibratoPeriodWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x0a,
      38,   32,   20,   20, 0x0a,
      63,   32,   20,   20, 0x0a,
      90,   32,   20,   20, 0x0a,
     109,   32,   20,   20, 0x0a,
     134,   32,   20,   20, 0x0a,
     157,   32,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VibratoPeriodWidget[] = {
    "VibratoPeriodWidget\0\0doUpdate()\0value\0"
    "setSmoothedPeriods(bool)\0"
    "setDrawSineReference(bool)\0"
    "setSineStyle(bool)\0setDrawPrevPeriods(bool)\0"
    "setPeriodScaling(bool)\0setDrawComparison(bool)\0"
};

const QMetaObject VibratoPeriodWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_VibratoPeriodWidget,
      qt_meta_data_VibratoPeriodWidget, 0 }
};

const QMetaObject *VibratoPeriodWidget::metaObject() const
{
    return &staticMetaObject;
}

void *VibratoPeriodWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VibratoPeriodWidget))
	return static_cast<void*>(const_cast< VibratoPeriodWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int VibratoPeriodWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: doUpdate(); break;
        case 1: setSmoothedPeriods((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: setDrawSineReference((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: setSineStyle((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: setDrawPrevPeriods((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: setPeriodScaling((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: setDrawComparison((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 7;
    }
    return _id;
}
