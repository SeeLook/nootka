/****************************************************************************
** Meta object code from reading C++ file 'myscrollbar.h'
**
** Created: Thu Jun 21 21:45:39 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/myscrollbar.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'myscrollbar.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MyScrollBar[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      20,   13,   12,   12, 0x05,
      41,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      71,   61,   12,   12, 0x0a,
     101,   91,   12,   12, 0x0a,
     141,  121,   12,   12, 0x0a,
     175,  165,   12,   12, 0x0a,
     205,  195,   12,   12, 0x0a,
     225,   13,   12,   12, 0x0a,
     248,  242,   12,   12, 0x0a,
     265,  242,   12,   12, 0x0a,
     283,   12,   12,   12, 0x0a,
     304,  298,   12,   12, 0x0a,
     327,  320,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MyScrollBar[] = {
    "MyScrollBar\0\0value_\0valueChanged(double)\0"
    "sliderMoved(double)\0minValue_\0"
    "setMinValue(double)\0maxValue_\0"
    "setMaxValue(double)\0minValue_,maxValue_\0"
    "setRange(double,double)\0lineStep_\0"
    "setLineStep(double)\0pageStep_\0"
    "setPageStep(double)\0setValue(double)\0"
    "value\0setIntValue(int)\0sliderMoving(int)\0"
    "sliderMoving()\0step_\0setStep(double)\0"
    "action\0actionTriggering(int)\0"
};

const QMetaObject MyScrollBar::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MyScrollBar,
      qt_meta_data_MyScrollBar, 0 }
};

const QMetaObject *MyScrollBar::metaObject() const
{
    return &staticMetaObject;
}

void *MyScrollBar::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MyScrollBar))
	return static_cast<void*>(const_cast< MyScrollBar*>(this));
    return QWidget::qt_metacast(_clname);
}

int MyScrollBar::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: sliderMoved((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: setMinValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: setMaxValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: setRange((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 5: setLineStep((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: setPageStep((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 7: setValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 8: setIntValue((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: sliderMoving((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: sliderMoving(); break;
        case 11: setStep((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 12: actionTriggering((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 13;
    }
    return _id;
}

// SIGNAL 0
void MyScrollBar::valueChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MyScrollBar::sliderMoved(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
