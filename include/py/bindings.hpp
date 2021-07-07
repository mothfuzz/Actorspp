#ifndef __BINDINGS_H_
#define __BINDINGS_H_

#include <vector>
#include <functional>

#include <pybind11/embed.h> // everything needed for embedding
#include <pybind11/stl.h>
namespace py = pybind11;

#include "actors.hpp"
#include "bindings_player.hpp"
#include "bindings_renderer.hpp"



class PyActor: public Actor {
    public:
        using Actor::Actor;
        void update() override {
            PYBIND11_OVERRIDE(void, Actor, update);
        }
};

PYBIND11_EMBEDDED_MODULE(actors, m) {
    py::class_<Actor, PyActor>(m, "Actor")
        .def(py::init())
        .def_readonly("id", &Actor::id)
        .def("update", &Actor::update);
    py_bind_player(m);
    py_bind_renderer(m);
}

class PyVM {
    py::scoped_interpreter vm;
    public:
    PyVM() {
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(1, "../");
        py::eval_file("../include/py/lib.py");
        py::module_::import("scripts");
    }
    void load_file(std::string filename) {
        try {
            py::eval_file(filename);
        } catch(py::error_already_set& e) {
            std::cout << e.what() << std::endl;
        }
    }
};


#endif // __BINDINGS_H_
