/****************************************************************************
** Meta object code from reading C++ file 'organism.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../organism.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'organism.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8OrganismE_t {};
} // unnamed namespace

template <> constexpr inline auto Organism::qt_create_metaobjectdata<qt_meta_tag_ZN8OrganismE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Organism",
        "organismDied",
        "",
        "Organism*",
        "organism",
        "organismReproduced",
        "parent",
        "offspring",
        "organismStateChanged",
        "OrganismState",
        "newState"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'organismDied'
        QtMocHelpers::SignalData<void(Organism *)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 },
        }}),
        // Signal 'organismReproduced'
        QtMocHelpers::SignalData<void(Organism *, Organism *)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 6 }, { 0x80000000 | 3, 7 },
        }}),
        // Signal 'organismStateChanged'
        QtMocHelpers::SignalData<void(Organism *, enum OrganismState)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 9, 10 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Organism, qt_meta_tag_ZN8OrganismE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Organism::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8OrganismE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8OrganismE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8OrganismE_t>.metaTypes,
    nullptr
} };

void Organism::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Organism *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->organismDied((*reinterpret_cast<std::add_pointer_t<Organism*>>(_a[1]))); break;
        case 1: _t->organismReproduced((*reinterpret_cast<std::add_pointer_t<Organism*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<Organism*>>(_a[2]))); break;
        case 2: _t->organismStateChanged((*reinterpret_cast<std::add_pointer_t<Organism*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<enum OrganismState>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Organism* >(); break;
            }
            break;
        case 1:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Organism* >(); break;
            }
            break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Organism* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Organism::*)(Organism * )>(_a, &Organism::organismDied, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Organism::*)(Organism * , Organism * )>(_a, &Organism::organismReproduced, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Organism::*)(Organism * , OrganismState )>(_a, &Organism::organismStateChanged, 2))
            return;
    }
}

const QMetaObject *Organism::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Organism::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8OrganismE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Organism::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void Organism::organismDied(Organism * _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void Organism::organismReproduced(Organism * _t1, Organism * _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void Organism::organismStateChanged(Organism * _t1, OrganismState _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1, _t2);
}
QT_WARNING_POP
