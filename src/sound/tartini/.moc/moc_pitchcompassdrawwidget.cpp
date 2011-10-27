/****************************************************************************
** Meta object code from reading C++ file 'pitchcompassdrawwidget.h'
**
** Created: Thu Jun 21 21:45:42 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../widgets/pitchcompass/pitchcompassdrawwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pitchcompassdrawwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_PitchCompassDrawWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      29,   24,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PitchCompassDrawWidget[] = {
    "PitchCompassDrawWidget\0\0time\0"
    "updateCompass(double)\0"
};

const QMetaObject PitchCompassDrawWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PitchCompassDrawWidget,
      qt_meta_data_PitchCompassDrawWidget, 0 }
};

const QMetaObject *PitchCompassDrawWidget::metaObject() const
{
    return &staticMetaObject;
}

void *PitchCompassDrawWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PitchCompassDrawWidget))
	return static_cast<void*>(const_cast< PitchCompassDrawWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int PitchCompassDrawWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: updateCompass((*reinterpret_cast< double(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}
