from actors import *
import math
import random

from scripts.collision import move_bsp3

class Wall(Actor):
    def __init__(self):
        Actor.__init__(self)
        self.model = Component(self.id, Model("wall.obj"))
        self.material = Component(self.id, Material("basic100.vert", "basic100.frag", {
            "albedo_tex": "flatwall.png",
            "normal_tex": "flatwall_norm.png",
            "glossy_tex": "black.png"
        }))
        self.transform = Component(self.id, Transform())
        self.transform().position = vec3(-32, 0, 0)
        self.transform().scale = vec3(32, 32, 1)
        self.normal = None
        self.points = None

walls = [
    #front
    [vec2(320, 240), vec2(-320, 240)],
    #back
    #[vec2(-320, -240), vec2(320, -240)],
    #l
    [vec2(-320, 240), vec2(-320, -240)],
    #r
    [vec2(320, -240), vec2(320, 240)]
]
wall_actors = []
for w in walls:
    wa = Wall()
    wa.points = [
        vec3(w[0].x, -64, w[0].y),
        vec3(w[0].x, 64, w[0].y),
        vec3(w[1].x, -64, w[1].y),
        vec3(w[1].x, 64, w[1].y)
    ]
    p2d = (w[0] + w[1]) / 2.0
    print(p2d.x, p2d.y)
    wa.transform().position = vec3(p2d.x, 0, p2d.y)
    wa.transform().scale = vec3(distance(w[0], w[1])/2.0, 64, 1.0)
    d2d = w[1] - w[0]
    wa.normal = normalize(vec3(d2d.y, 0, -d2d.x))
    angle = math.atan2(d2d.x, d2d.y)*180 / math.pi - 90
    print(angle)
    wa.transform().rotation = vec3(0, angle, 0)
    wall_actors.append(wa)

ramp = Wall()
ramp.points = [
    vec3(320, -32, -240),
    vec3(320, 32, 240),
    vec3(320-128, 32, 240),
    vec3(320-128, -32, -240)
]
ramp.transform().position = vec3(320-64, 0, 0)
ramp.transform().scale = vec3(64, 240, 1)
direction = vec2(240, -64)-vec2(-240, 64)
angle = math.atan2(direction.y, direction.x)*180 / math.pi - 90
ramp.transform().rotation = vec3(angle, 0, 0)
ramp.normal = normalize(vec3(0, direction.x, direction.y))
wall_actors.append(ramp)

floor = Wall()
floor.points = [
    vec3(320, -64, 240),
    vec3(-320, -64, 240),
    vec3(320, -64, -320),
    vec3(-320, -64, -320)
]
floor.transform().position = vec3(0, -64, -(320-240))
floor.transform().scale = vec3(320, 320, 1)
floor.transform().rotation = vec3(-90, 0, 0)
floor.normal = vec3(0, 1, 0)
wall_actors.append(floor)

class Player3D(Actor):
    def __init__(self):
        Actor.__init__(self)

        self.transform = Component(self.id, Transform())
        self.transform().position = vec3(0, 0, 0)
        self.camera = Component(self.id, Camera())

        self.camera().set_active()
        self.velocity = vec3(0, 0, 0)

    def update(self):
        speed = 4.0
        c = math.cos(self.transform().rotation.y)
        s = math.sin(self.transform().rotation.y)
        if Window.is_key_down("w"):
            self.velocity.z -= speed * c
            self.velocity.x += speed * s
        if Window.is_key_down("s"):
            self.velocity.z += speed * c
            self.velocity.x -= speed * s
        if Window.is_key_down("a"):
            self.velocity.x -= speed * c
            self.velocity.z -= speed * s
        if Window.is_key_down("d"):
            self.velocity.x += speed * c
            self.velocity.z += speed * s
        if Window.is_key_down("left"):
            self.transform().rotation.y -= 0.05
        if Window.is_key_down("right"):
            self.transform().rotation.y += 0.05
        self.velocity.x *= 0.8
        self.velocity.z *= 0.8
        self.velocity.y -= 0.8
        if self.velocity.y < -8:
            self.velocity.y = -8
        #try:
        self.velocity = move_bsp3(self.transform, 64, self.velocity, wall_actors)
        #except Exception as e:
        #    print(e)
        self.transform().position.x += self.velocity.x
        self.transform().position.y += self.velocity.y
        self.transform().position.z += self.velocity.z

p = Player3D()