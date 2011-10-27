/****************************************************************************
** Meta object code from reading C++ file 'vibratotimeaxis.h'
**
** Created: Thu Jun 21 21:46:01 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/vibrato/vibratotimeaxis.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vibratotimeaxis.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_VibratoTimeAxis[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      30,   28,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_VibratoTimeAxis[] = {
    "VibratoTimeAxis\0\0doUpdate()\0x\0"
    "setZoomFactorX(double)\0"
};

const QMetaObject VibratoTimeAxis::staticMetaObject = {
    { &DrawWidget::staticMetaObject, qt_meta_stringdata_VibratoTimeAxis,
      qt_meta_data_VibratoTimeAxis, 0 }
};

const QMetaObject *VibratoTimeAxis::metaObject() const
{
    return &staticMetaObject;
}

void *VibratoTimeAxis::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VibratoTimeAxis))
	return static_cast<void*>(const_cast< VibratoTimeAxis*>(this));
    return DrawWidget::qt_metacast(_clname);
}

int VibratoTimeAxis::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrawWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: doUpdate(); break;
        case 1: setZoomFactorX((*reinterpret_cast< double(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}
