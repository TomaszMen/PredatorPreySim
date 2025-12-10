/****************************************************************************
** Meta object code from reading C++ file 'prey.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../prey.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'prey.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN4PreyE_t {};
} // unnamed namespace

template <> constexpr inline auto Prey::qt_create_metaobjectdata<qt_meta_tag_ZN4PreyE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Prey"
    };

    QtMocHelpers::UintData qt_methods {
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Prey, qt_meta_tag_ZN4PreyE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Prey::staticMetaObject = { {
    QMetaObject::SuperData::link<Organism::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN4PreyE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN4PreyE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN4PreyE_t>.metaTypes,
    nullptr
} };

void Prey::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Prey *>(_o);
    (void)_t;
    (void)_c;
    (void)_id;
    (void)_a;
}

const QMetaObject *Prey::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Prey::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN4PreyE_t>.strings))
        return static_cast<void*>(this);
    return Organism::qt_metacast(_clname);
}

int Prey::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Organism::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
