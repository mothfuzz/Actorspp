#ifndef __BINDINGS_PLAYER_H_
#define __BINDINGS_PLAYER_H_

#include <pybind11/embed.h>
namespace py = pybind11;

#include "player.hpp"

inline void py_bind_player(py::module& m) {
    py::class_<Player, Actor>(m, "Player")
        .def(py::init<int>(), py::arg("health") = 100)
        //.def("update", &Player::update)
        .def_readwrite("health", &Player::health);
    py::class_<Damage>(m, "Damage")
        .def(py::init<int>())
        .def_readwrite("value", &Damage::value);
    py::class_<EventQueue<Damage>>(m, "DamageQueue")
        .def(py::init<int>())
        .def("next", &EventQueue<Damage>::next)
        .def_static("dispatch", &EventQueue<Damage>::dispatch)
        .def_static("broadcast", &EventQueue<Damage>::broadcast);
}

#endif // __BINDINGS_PLAYER_H_
