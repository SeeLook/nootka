/****************************************************************************
** Meta object code from reading C++ file 'gdata.h'
**
** Created: Thu Jun 21 21:45:49 2007
**      by: The Qt Meta Object Compiler version 59 (Qt 4.2.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../global/gdata.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gdata.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.2.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_GData[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      40,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      14,    7,    6,    6, 0x05,
      65,   45,    6,    6, 0x05,
      98,   96,    6,    6, 0x05,
     122,   96,    6,    6, 0x05,
     168,  147,    6,    6, 0x05,
     200,    6,    6,    6, 0x05,
     218,    6,    6,    6, 0x05,
     238,  234,    6,    6, 0x05,
     264,  259,    6,    6, 0x05,
     289,  259,    6,    6, 0x05,

 // slots: signature, parameters, type, tag, flags
     314,  259,    6,    6, 0x0a,
     350,  340,    6,    6, 0x0a,
     385,  373,    6,    6, 0x0a,
     419,  405,    6,    6, 0x0a,
     458,  441,    6,    6, 0x0a,
     483,  234,    6,    6, 0x0a,
     500,  259,    6,    6, 0x0a,
     521,  259,    6,    6, 0x0a,
     542,   96,    6,    6, 0x0a,
     559,   96,    6,    6, 0x0a,
     573,    6,    6,    6, 0x0a,
     602,  591,  586,    6, 0x0a,
     642,  640,  586,    6, 0x0a,
     664,    6,    6,    6, 0x0a,
     693,  691,    6,    6, 0x0a,
     723,    6,    6,    6, 0x0a,
     748,    6,    6,    6, 0x0a,
     760,    6,    6,    6, 0x0a,
     769,    6,  586,    6, 0x0a,
     776,    6,    6,    6, 0x0a,
     783,    6,    6,    6, 0x0a,
     789,    6,    6,    6, 0x0a,
     803,    6,  586,    6, 0x0a,
     819,    6,    6,    6, 0x0a,
     836,    6,    6,    6, 0x0a,
     874,  862,  854,    6, 0x0a,
     913,  899,  895,    6, 0x0a,
     958,  942,  895,    6, 0x0a,
     984,   45,    6,    6, 0x0a,
    1013,    6,    6,    6, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GData[] = {
    "GData\0\0active\0activeChannelChanged(Channel*)\0"
    "thresholdPercentage\0activeIntThresholdChanged(int)\0"
    "x\0leftTimeChanged(double)\0"
    "rightTimeChanged(double)\0leftTime_,rightTime_\0"
    "timeRangeChanged(double,double)\0"
    "channelsChanged()\0onChunkUpdate()\0key\0"
    "musicKeyChanged(int)\0type\0"
    "musicKeyTypeChanged(int)\0"
    "temperedTypeChanged(int)\0"
    "setInterpolatingType(int)\0num_steps\0"
    "setBisectionSteps(int)\0num_repeats\0"
    "setFastRepeats(int)\0amplitudeMode\0"
    "setAmplitudeMode(int)\0pitchContourMode\0"
    "setPitchContourMode(int)\0setMusicKey(int)\0"
    "setMusicKeyType(int)\0setTemperedType(int)\0"
    "setFreqA(double)\0setFreqA(int)\0"
    "pauseSound()\0bool\0sRec,sPlay\0"
    "openPlayRecord(SoundFile*,SoundFile*)\0"
    "s\0playSound(SoundFile*)\0"
    "updateViewLeftRightTimes()\0t\0"
    "updateActiveChunkTime(double)\0"
    "updateQuickRefSettings()\0beginning()\0"
    "rewind()\0play()\0stop()\0end()\0fastforward()\0"
    "closeAllFiles()\0saveActiveFile()\0"
    "closeActiveFile()\0QString\0oldFilename\0"
    "saveFileAsk(QString)\0int\0s,newFilename\0"
    "saveFile(SoundFile*,QString)\0"
    "s,theSavingMode\0closeFile(SoundFile*,int)\0"
    "resetActiveIntThreshold(int)\0"
    "doChunkUpdate()\0"
};

const QMetaObject GData::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GData,
      qt_meta_data_GData, 0 }
};

const QMetaObject *GData::metaObject() const
{
    return &staticMetaObject;
}

void *GData::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GData))
	return static_cast<void*>(const_cast< GData*>(this));
    return QObject::qt_metacast(_clname);
}

int GData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activeChannelChanged((*reinterpret_cast< Channel*(*)>(_a[1]))); break;
        case 1: activeIntThresholdChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: leftTimeChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 3: rightTimeChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: timeRangeChanged((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 5: channelsChanged(); break;
        case 6: onChunkUpdate(); break;
        case 7: musicKeyChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: musicKeyTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: temperedTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: setInterpolatingType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: setBisectionSteps((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: setFastRepeats((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: setAmplitudeMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: setPitchContourMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: setMusicKey((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: setMusicKeyType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: setTemperedType((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: setFreqA((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 19: setFreqA((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: pauseSound(); break;
        case 21: { bool _r = openPlayRecord((*reinterpret_cast< SoundFile*(*)>(_a[1])),(*reinterpret_cast< SoundFile*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 22: { bool _r = playSound((*reinterpret_cast< SoundFile*(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 23: updateViewLeftRightTimes(); break;
        case 24: updateActiveChunkTime((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 25: updateQuickRefSettings(); break;
        case 26: beginning(); break;
        case 27: rewind(); break;
        case 28: { bool _r = play();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 29: stop(); break;
        case 30: end(); break;
        case 31: fastforward(); break;
        case 32: { bool _r = closeAllFiles();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 33: saveActiveFile(); break;
        case 34: closeActiveFile(); break;
        case 35: { QString _r = saveFileAsk((*reinterpret_cast< QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 36: { int _r = saveFile((*reinterpret_cast< SoundFile*(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 37: { int _r = closeFile((*reinterpret_cast< SoundFile*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 38: resetActiveIntThreshold((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 39: doChunkUpdate(); break;
        }
        _id -= 40;
    }
    return _id;
}

// SIGNAL 0
void GData::activeChannelChanged(Channel * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GData::activeIntThresholdChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GData::leftTimeChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GData::rightTimeChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GData::timeRangeChanged(double _t1, double _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GData::channelsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void GData::onChunkUpdate()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void GData::musicKeyChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void GData::musicKeyTypeChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void GData::temperedTypeChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
