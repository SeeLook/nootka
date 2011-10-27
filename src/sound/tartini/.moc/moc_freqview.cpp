/****************************************************************************
** Meta object code from reading C++ file 'freqview.h'
**
** Created: Thu Jun 21 21:45:41 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/freq/freqview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'freqview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_FreqView[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x0a,
      19,    9,    9,    9, 0x0a,
      29,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FreqView[] = {
    "FreqView\0\0zoomIn()\0zoomOut()\0"
    "setAmplitudeZoom(double)\0"
};

const QMetaObject FreqView::staticMetaObject = {
    { &ViewWidget::staticMetaObject, qt_meta_stringdata_FreqView,
      qt_meta_data_FreqView, 0 }
};

const QMetaObject *FreqView::metaObject() const
{
    return &staticMetaObject;
}

void *FreqView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FreqView))
	return static_cast<void*>(const_cast< FreqView*>(this));
    return ViewWidget::qt_metacast(_clname);
}

int FreqView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ViewWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: zoomIn(); break;
        case 1: zoomOut(); break;
        case 2: setAmplitudeZoom((*reinterpret_cast< double(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
    return _id;
}
