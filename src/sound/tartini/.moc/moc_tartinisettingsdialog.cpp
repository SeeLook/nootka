/****************************************************************************
** Meta object code from reading C++ file 'tartinisettingsdialog.h'
**
** Created: Thu Jun 21 21:45:59 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../dialogs/tartinisettingsdialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tartinisettingsdialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_TartiniSettingsDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      43,   31,   23,   22, 0x0a,
      60,   22,   22,   22, 0x0a,
      79,   22,   22,   22, 0x0a,
      97,   22,   22,   22, 0x0a,
     118,   22,   22,   22, 0x0a,
     137,   22,   22,   22, 0x0a,
     166,  156,   22,   22, 0x0a,
     196,   22,   22,   22, 0x0a,
     211,   22,   22,   22, 0x0a,
     241,  234,   22,   22, 0x0a,
     266,  264,   22,   22, 0x0a,
     299,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_TartiniSettingsDialog[] = {
    "TartiniSettingsDialog\0\0QString\0"
    "initialPath\0getPath(QString)\0"
    "changeTempFolder()\0fileNameChanged()\0"
    "getBackgroundColor()\0getShading1Color()\0"
    "getShading2Color()\0obj,group\0"
    "saveSetting(QObject*,QString)\0"
    "saveSettings()\0checkAnalysisEnabled()\0"
    "choice\0onNoteRangeChoice(int)\0s\0"
    "setUnknownsToDefault(QSettings*)\0"
    "resetDefaults()\0"
};

const QMetaObject TartiniSettingsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_TartiniSettingsDialog,
      qt_meta_data_TartiniSettingsDialog, 0 }
};

const QMetaObject *TartiniSettingsDialog::metaObject() const
{
    return &staticMetaObject;
}

void *TartiniSettingsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TartiniSettingsDialog))
	return static_cast<void*>(const_cast< TartiniSettingsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int TartiniSettingsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QString _r = getPath((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 1: changeTempFolder(); break;
        case 2: fileNameChanged(); break;
        case 3: getBackgroundColor(); break;
        case 4: getShading1Color(); break;
        case 5: getShading2Color(); break;
        case 6: saveSetting((*reinterpret_cast< QObject*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 7: saveSettings(); break;
        case 8: checkAnalysisEnabled(); break;
        case 9: onNoteRangeChoice((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: setUnknownsToDefault((*reinterpret_cast< QSettings*(*)>(_a[1]))); break;
        case 11: resetDefaults(); break;
        }
        _id -= 12;
    }
    return _id;
}
