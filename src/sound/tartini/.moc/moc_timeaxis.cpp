/****************************************************************************
** Meta object code from reading C++ file 'timeaxis.h'
**
** Created: Thu Jun 21 21:45:50 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/timeaxis.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'timeaxis.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_TimeAxis[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   10,    9,    9, 0x0a,
      35,   10,    9,    9, 0x0a,
      77,   56,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_TimeAxis[] = {
    "TimeAxis\0\0time\0setLeftTime(double)\0"
    "setRightTime(double)\0leftTime_,rightTime_\0"
    "setRange(double,double)\0"
};

const QMetaObject TimeAxis::staticMetaObject = {
    { &DrawWidget::staticMetaObject, qt_meta_stringdata_TimeAxis,
      qt_meta_data_TimeAxis, 0 }
};

const QMetaObject *TimeAxis::metaObject() const
{
    return &staticMetaObject;
}

void *TimeAxis::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TimeAxis))
	return static_cast<void*>(const_cast< TimeAxis*>(this));
    return DrawWidget::qt_metacast(_clname);
}

int TimeAxis::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrawWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setLeftTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: setRightTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: setRange((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        }
        _id -= 3;
    }
    return _id;
}
