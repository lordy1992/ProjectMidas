/****************************************************************************
** Meta object code from reading C++ file 'GestureSignaller.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../MidasGUI/GestureSignaller.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GestureSignaller.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_GestureSignaller_t {
    QByteArrayData data[11];
    char stringdata[199];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GestureSignaller_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GestureSignaller_t qt_meta_stringdata_GestureSignaller = {
    {
QT_MOC_LITERAL(0, 0, 16), // "GestureSignaller"
QT_MOC_LITERAL(1, 17, 20), // "emitRegisterSequence"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 29), // "std::vector<sequenceImageSet>"
QT_MOC_LITERAL(4, 69, 17), // "emitShowSequences"
QT_MOC_LITERAL(5, 87, 33), // "std::vector<sequenceProgressD..."
QT_MOC_LITERAL(6, 121, 15), // "emitStateString"
QT_MOC_LITERAL(7, 137, 14), // "emitPoseImages"
QT_MOC_LITERAL(8, 152, 18), // "emitToggleKeyboard"
QT_MOC_LITERAL(9, 171, 19), // "handleShowAllToggle"
QT_MOC_LITERAL(10, 191, 7) // "showAll"

    },
    "GestureSignaller\0emitRegisterSequence\0"
    "\0std::vector<sequenceImageSet>\0"
    "emitShowSequences\0std::vector<sequenceProgressData>\0"
    "emitStateString\0emitPoseImages\0"
    "emitToggleKeyboard\0handleShowAllToggle\0"
    "showAll"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GestureSignaller[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   44,    2, 0x06 /* Public */,
       4,    1,   51,    2, 0x06 /* Public */,
       6,    1,   54,    2, 0x06 /* Public */,
       7,    1,   57,    2, 0x06 /* Public */,
       8,    0,   60,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   61,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::QString, 0x80000000 | 3,    2,    2,    2,
    QMetaType::Void, 0x80000000 | 5,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,   10,

       0        // eod
};

void GestureSignaller::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        GestureSignaller *_t = static_cast<GestureSignaller *>(_o);
        switch (_id) {
        case 0: _t->emitRegisterSequence((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< std::vector<sequenceImageSet>(*)>(_a[3]))); break;
        case 1: _t->emitShowSequences((*reinterpret_cast< std::vector<sequenceProgressData>(*)>(_a[1]))); break;
        case 2: _t->emitStateString((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 3: _t->emitPoseImages((*reinterpret_cast< std::vector<sequenceImageSet>(*)>(_a[1]))); break;
        case 4: _t->emitToggleKeyboard(); break;
        case 5: _t->handleShowAllToggle((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (GestureSignaller::*_t)(int , QString , std::vector<sequenceImageSet> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GestureSignaller::emitRegisterSequence)) {
                *result = 0;
            }
        }
        {
            typedef void (GestureSignaller::*_t)(std::vector<sequenceProgressData> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GestureSignaller::emitShowSequences)) {
                *result = 1;
            }
        }
        {
            typedef void (GestureSignaller::*_t)(QString );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GestureSignaller::emitStateString)) {
                *result = 2;
            }
        }
        {
            typedef void (GestureSignaller::*_t)(std::vector<sequenceImageSet> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GestureSignaller::emitPoseImages)) {
                *result = 3;
            }
        }
        {
            typedef void (GestureSignaller::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&GestureSignaller::emitToggleKeyboard)) {
                *result = 4;
            }
        }
    }
}

const QMetaObject GestureSignaller::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GestureSignaller.data,
      qt_meta_data_GestureSignaller,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *GestureSignaller::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GestureSignaller::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_GestureSignaller.stringdata))
        return static_cast<void*>(const_cast< GestureSignaller*>(this));
    return QObject::qt_metacast(_clname);
}

int GestureSignaller::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GestureSignaller::emitRegisterSequence(int _t1, QString _t2, std::vector<sequenceImageSet> _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GestureSignaller::emitShowSequences(std::vector<sequenceProgressData> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void GestureSignaller::emitStateString(QString _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void GestureSignaller::emitPoseImages(std::vector<sequenceImageSet> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GestureSignaller::emitToggleKeyboard()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE
