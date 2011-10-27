/****************************************************************************
** Meta object code from reading C++ file 'hcirclewidget.h'
**
** Created: Thu Jun 21 21:45:48 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/hcircle/hcirclewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hcirclewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_HCircleWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      31,   14,   14,   14, 0x0a,
      52,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_HCircleWidget[] = {
    "HCircleWidget\0\0setZoom(double)\0"
    "setThreshold(double)\0setLowestValue(double)\0"
};

const QMetaObject HCircleWidget::staticMetaObject = {
    { &DrawWidget::staticMetaObject, qt_meta_stringdata_HCircleWidget,
      qt_meta_data_HCircleWidget, 0 }
};

const QMetaObject *HCircleWidget::metaObject() const
{
    return &staticMetaObject;
}

void *HCircleWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HCircleWidget))
	return static_cast<void*>(const_cast< HCircleWidget*>(this));
    return DrawWidget::qt_metacast(_clname);
}

int HCircleWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrawWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setZoom((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: setThreshold((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: setLowestValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
    return _id;
}
