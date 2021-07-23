#ifndef __BINDINGS_RENDERER_H_
#define __BINDINGS_RENDERER_H_

#include <pybind11/embed.h> // everything needed for embedding
#include <pybind11/pytypes.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
namespace py = pybind11;

#include "renderer.hpp"
#include "window.hpp"

#include "components.hpp"

#include <glm/glm.hpp>




inline void py_bind_renderer(py::module& m) {
    py::class_<Window>(m, "Window")
        .def_static("is_key_down", &Window::is_key_down)
        .def_static("track_keypress", &Window::track_keypress)
        .def_static("track_click", &Window::track_click)
        .def_static("is_mousebutton_down", &Window::is_mousebutton_down)
        .def_static("raw_mouse_position", &Window::raw_mouse_position)
        .def_static("mouse_position", &Window::mouse_position)
        .def_static("get_width", &Window::get_width)
        .def_static("get_height", &Window::get_height);

    py::class_<Camera>(m, "Camera")
        .def(py::init<>())
        .def("set_active", &Camera::set_active);

    py::class_<Light>(m, "Light")
        .def(py::init<glm::vec3>())
        .def_readwrite("color", &Light::color);

    py::class_<Model>(m, "Model")
        .def(py::init<std::string>())
        .def_readwrite("mesh", &Model::mesh);

    py::class_<Sprite::ClipRect>(m, "ClipRect")
        .def(py::init<float, float, float, float>(), py::arg("x") = 0, py::arg("y") = 0, py::arg("w") = 0, py::arg("h") = 0)
        .def_readwrite("x", &Sprite::ClipRect::x)
        .def_readwrite("y", &Sprite::ClipRect::y)
        .def_readwrite("w", &Sprite::ClipRect::w)
        .def_readwrite("h", &Sprite::ClipRect::h);
    py::class_<Sprite>(m, "Sprite")
        .def(py::init<std::string, float, float, Sprite::ClipRect>(), py::arg("texture"), py::arg("width") = 0, py::arg("height") = 0, py::arg("clip_rect") = Sprite::ClipRect{})
        .def_readwrite("texture", &Sprite::texture)
        .def_readwrite("width", &Sprite::width)
        .def_readwrite("height", &Sprite::height)
        .def_readwrite("clip_rect", &Sprite::clip_rect);

    py::class_<Material>(m, "Material")
        .def(py::init<std::string, std::string, std::unordered_map<std::string, std::string>>(),
             py::arg("vertex_shader") = "basic100.vert",
             py::arg("fragment_shader") = "basic100.frag",
             py::arg("textures") = std::unordered_map<std::string, std::string>{})
        .def_readwrite("vertex_shader", &Material::vertex_shader)
        .def_readwrite("fragment_shader", &Material::fragment_shader)
        .def_readwrite("textures", &Material::textures);

    py::class_<Component<Model>>(m, "ModelComponent")
        .def(py::init<int, Model>())
        .def("__call__", &Component<Model>::operator(), py::return_value_policy::reference);
    py::class_<Component<Sprite>>(m, "SpriteComponent")
        .def(py::init<int, Sprite>())
        .def("__call__", &Component<Sprite>::operator(), py::return_value_policy::reference);
    py::class_<Component<Material>>(m, "MaterialComponent")
        .def(py::init<int, Material>())
        .def("__call__", &Component<Material>::operator(), py::return_value_policy::reference);
    py::class_<Component<Transform>>(m, "TransformComponent")
        .def(py::init<int, Transform>())
        .def("__call__", &Component<Transform>::operator(), py::return_value_policy::reference);
    py::class_<Component<Camera>>(m, "CameraComponent")
        .def(py::init<int, Camera>())
        .def("__call__", &Component<Camera>::operator(), py::return_value_policy::reference);
    py::class_<Component<Light>>(m, "LightComponent")
        .def(py::init<int, Light>())
        .def("__call__", &Component<Light>::operator(), py::return_value_policy::reference);

    py::class_<glm::vec2>(m, "vec2")
        .def(py::init<float, float>())
        .def_readwrite("x", &glm::vec2::x)
        .def_readwrite("y", &glm::vec2::y)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self * float())
        .def(py::self / float())
        .def(float() * py::self)
        .def(float() / py::self);
    py::class_<glm::vec3>(m, "vec3")
        .def(py::init<float, float, float>())
        .def_readwrite("x", &glm::vec3::x)
        .def_readwrite("y", &glm::vec3::y)
        .def_readwrite("z", &glm::vec3::z)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self * float())
        .def(py::self / float())
        .def(float() * py::self)
        .def(float() / py::self);
    py::class_<glm::vec4>(m, "vec4")
        .def(py::init<float, float, float, float>())
        .def_readwrite("x", &glm::vec4::x)
        .def_readwrite("y", &glm::vec4::y)
        .def_readwrite("z", &glm::vec4::z)
        .def_readwrite("w", &glm::vec4::w)
        .def(py::self + py::self)
        .def(py::self - py::self)
        .def(py::self += py::self)
        .def(py::self -= py::self)
        .def(py::self * py::self)
        .def(py::self / py::self)
        .def(py::self * float())
        .def(py::self / float())
        .def(float() * py::self)
        .def(float() / py::self);
    m.def("length", &glm::length<2, float, glm::packed_highp>);
    m.def("length", &glm::length<3, float, glm::packed_highp>);
    m.def("length", &glm::length<4, float, glm::packed_highp>);
    m.def("dot", &glm::dot<2, float, glm::packed_highp>);
    m.def("dot", &glm::dot<3, float, glm::packed_highp>);
    m.def("dot", &glm::dot<4, float, glm::packed_highp>);
    m.def("normalize", &glm::normalize<2, float, glm::packed_highp>);
    m.def("normalize", &glm::normalize<3, float, glm::packed_highp>);
    m.def("normalize", &glm::normalize<4, float, glm::packed_highp>);
    m.def("distance", &glm::distance<2, float, glm::packed_highp>);
    m.def("distance", &glm::distance<3, float, glm::packed_highp>);
    m.def("distance", &glm::distance<4, float, glm::packed_highp>);
    py::class_<Transform>(m, "Transform")
        .def(py::init<>())
        .def_readwrite("position", &Transform::position)
        .def_readwrite("rotation", &Transform::rotation)
        .def_readwrite("scale", &Transform::scale);
}


#endif // __BINDINGS_RENDERER_H_
