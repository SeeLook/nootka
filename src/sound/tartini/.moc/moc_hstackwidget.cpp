/****************************************************************************
** Meta object code from reading C++ file 'hstackwidget.h'
**
** Created: Thu Jun 21 21:45:46 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/hstack/hstackwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'hstackwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_HStackWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,
      40,   13,   13,   13, 0x05,
      64,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      87,   13,   13,   13, 0x0a,
     109,   13,   13,   13, 0x0a,
     129,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_HStackWidget[] = {
    "HStackWidget\0\0windowSizeChanged(double)\0"
    "dBOffsetChanged(double)\0dBRangeChanged(double)\0"
    "setWindowSize(double)\0setDBOffset(double)\0"
    "setDBRange(double)\0"
};

const QMetaObject HStackWidget::staticMetaObject = {
    { &DrawWidget::staticMetaObject, qt_meta_stringdata_HStackWidget,
      qt_meta_data_HStackWidget, 0 }
};

const QMetaObject *HStackWidget::metaObject() const
{
    return &staticMetaObject;
}

void *HStackWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HStackWidget))
	return static_cast<void*>(const_cast< HStackWidget*>(this));
    return DrawWidget::qt_metacast(_clname);
}

int HStackWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = DrawWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: windowSizeChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: dBOffsetChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: dBRangeChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: setWindowSize((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: setDBOffset((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: setDBRange((*reinterpret_cast< double(*)>(_a[1]))); break;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void HStackWidget::windowSizeChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void HStackWidget::dBOffsetChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void HStackWidget::dBRangeChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
