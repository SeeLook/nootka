/****************************************************************************
** Meta object code from reading C++ file 'amplitudewidget.h'
**
** Created: Thu Jun 21 21:45:52 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/freq/amplitudewidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'amplitudewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_AmplitudeWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      38,   16,   16,   16, 0x05,
      60,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      94,   85,   16,   16, 0x0a,
     121,  111,   16,   16, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AmplitudeWidget[] = {
    "AmplitudeWidget\0\0rangeChanged(double)\0"
    "offsetChanged(double)\0offsetInvChanged(double)\0"
    "newRange\0setRange(double)\0newOffset\0"
    "setOffset(double)\0"
};

const QMetaObject AmplitudeWidget::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_AmplitudeWidget,
      qt_meta_data_AmplitudeWidget, 0 }
};

const QMetaObject *AmplitudeWidget::metaObject() const
{
    return &staticMetaObject;
}

void *AmplitudeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AmplitudeWidget))
	return static_cast<void*>(const_cast< AmplitudeWidget*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int AmplitudeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: rangeChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: offsetChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: offsetInvChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: setRange((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: setOffset((*reinterpret_cast< double(*)>(_a[1]))); break;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void AmplitudeWidget::rangeChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void AmplitudeWidget::offsetChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void AmplitudeWidget::offsetInvChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
