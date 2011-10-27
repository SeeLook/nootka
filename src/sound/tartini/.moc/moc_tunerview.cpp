/****************************************************************************
** Meta object code from reading C++ file 'tunerview.h'
**
** Created: Thu Jun 21 21:45:44 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/tuner/tunerview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tunerview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_TunerView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      16,   11,   10,   10, 0x0a,
      59,   47,   10,   10, 0x0a,
      76,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_TunerView[] = {
    "TunerView\0\0time\0slotCurrentTimeChanged(double)\0"
    "index,value\0setLed(int,bool)\0doUpdate()\0"
};

const QMetaObject TunerView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_TunerView,
      qt_meta_data_TunerView, 0 }
};

const QMetaObject *TunerView::metaObject() const
{
    return &staticMetaObject;
}

void *TunerView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TunerView))
	return static_cast<void*>(const_cast< TunerView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int TunerView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotCurrentTimeChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: setLed((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 2: doUpdate(); break;
        }
        _id -= 3;
    }
    return _id;
}
