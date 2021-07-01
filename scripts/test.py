from actors import *
import math
import random

class Box:
    def __init__(self, x, y, w, h):
        self.x = x
        self.y = y
        self.w = w
        self.h = h
    def contains(self, point):
        return point.x > self.x - self.w/2 and point.x < self.x+self.w/2 and point.y > self.y-self.h/2 and point.y < self.y+self.h/2

class ClickEvent():
    def __init__(self, actor, button, position):
        self.actor = actor
        self.button = button
        self.position = position
class ClickQueue(EventQueue):
    pass

class ItemPickupQueue(EventQueue):
    pass

class Item(Actor):
    def __init__(self):
        Actor.__init__(self)
        self.clicks = ClickQueue(self.id)
        self.transform = Component(self.id, Transform())
        self.owner = None
        self.sprite = None
        self.show()
    def show(self):
        self.sprite = Component(self.id, Sprite("item.png", 32, 32))
    def hide(self):
        self.sprite = None
    def drop(self):
        self.transform().position.x = self.owner.transform().position.x
        self.transform().position.y = self.owner.transform().position.y
        self.owner = None
    def update(self):
        pos = self.transform().position
        while c := self.clicks.next():
            if c.button == 1 and self.owner is None and Box(pos.x, pos.y, 32, 32).contains(c.position):
                ItemPickupQueue.dispatch(c.actor.id, self)
                self.owner = c.actor
            


left = -Window.get_width()/2
top = Window.get_height()/2

class InventorySlot(Actor):
    def __init__(self):
        Actor.__init__(self)
        self.transform = Component(self.id, Transform())
        self.transform().position.z = -0.1 #place behind items
    def show(self):
        self.sprite = Component(self.id, Sprite("inventoryslot.png"))
    def hide(self):
        self.sprite = None

class Inventory(Actor):
    def __init__(self, num_slots):
        Actor.__init__(self)
        self.clicks = ClickQueue(self.id)
        self.slots = [InventorySlot() for _ in range(num_slots)]
        for i, slot in enumerate(self.slots):
            slot.transform().position.x = left + 24 + i * 48
            slot.transform().position.y = top - 24
        self.items = []
        self.visible = False
    def show(self):
        self.visible = True
        for slot in self.slots:
            slot.show()
        for item in self.items:
            item.show()
    def hide(self):
        self.visible = False
        for slot in self.slots:
            slot.hide()
        for item in self.items:
            item.hide()
    def append(self, item):
        if len(self.items) < len(self.slots):
            item.transform().position.x = left + 24 + len(self.items) * 48
            item.transform().position.y = top - 24
            if self.visible:
                item.show()
            else:
                item.hide()
            self.items.append(item)
        else:
            item.drop()
    def update(self):
        while c := self.clicks.next():
            if c.button == 3 and self.visible:
                #delete the clicked item, if any
                for i, item in enumerate(self.items):
                    if Box(left + 24 ++ i * 48, top - 24, 48, 48).contains(c.position):
                        self.items.remove(item)
                        item.drop()
                        break
                #rearrange items
                for i, item in enumerate(self.items):
                    item.transform().position.x = left + 24 + i * 48
                    item.transform().position.y = top - 24

class Plane2D:
    def __init__(self, x1, y1, x2, y2):
        self.x1 = x1
        self.y1 = y1
        self.x2 = x2
        self.y2 = y2
        self.length = math.sqrt(((x2-x1)**2) + ((y2-y1)**2))
        self.normal = vec2((y2-y1)/self.length, -(x2-x1)/self.length)
        
class Collider(Actor):
    def __init__(self):
        Actor.__init__(self)
        self.planes = []
        self.transform = Component(self.id, Transform())

class Block(Collider):
    def __init__(self):
        Collider.__init__(self)
        self.sprite = Component(self.id, Sprite("block.png"))
        #l u r d
        self.planes = [
            #Plane2D(-16, -16, -16, 16), 
            Plane2D(-16, 16, 16, 16), 
            #Plane2D(16, 16, 16, -16), 
            Plane2D(16, -16, -16, -16),
        ]

class LUSlope(Collider):
    def __init__(self):
        Collider.__init__(self)
        self.sprite = Component(self.id, Sprite("lslope.png"))
        #l u r d
        self.planes = [
            #Plane2D(-16, -16, -16, 16), 
            #Plane2D(-16, 16, 16, 16), 
            Plane2D(-16, -16, 16, 16), #
            #Plane2D(16, 16, 16, -16), 
            Plane2D(16, -16, -16, -16),
        ]

class RUSlope(Collider):
    def __init__(self):
        Collider.__init__(self)
        self.sprite = Component(self.id, Sprite("rslope.png"))
        #l u r d
        self.planes = [
            #Plane2D(-16, -16, -16, 16), 
            Plane2D(-16, 16, 16, -16), #
            #Plane2D(-16, 16, 16, 16), 
            #Plane2D(16, 16, 16, -16), 
            Plane2D(16, -16, -16, -16),
        ]

the_map = [
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01, 0x03,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x02, 0x03, 0x00,
    0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
]
map_actors = []

for n,i in enumerate(the_map):
    a = None
    if i == 0x01:
        a = Block()
    elif i == 0x02:
        a = LUSlope()
    elif i == 0x03:
        a = RUSlope()
    if a:
        a.transform().position.x = left + 16 + ((n % 8) * 32)
        a.transform().position.y = top - 16 - ((n // 8) * 32)
        a.transform().position.z = -0.2
        map_actors.append(a)
                    
def dot(a, b):
    return a.x*b.x + a.y*b.y
def perpendicular(v):
    return vec2(-v.y, v.x)

#returns + if on one side, - if on the other, 0 if colinear
def point_on_plane(p, pl):
    return math.copysign(1, (pl.y2 - pl.y1)*p.x + (pl.x1-pl.x2)*p.y + (pl.x2*pl.y1 - pl.x1*pl.y2))

#returns min and max along a 1-dimensional line
def project(points, axis):
    dots = [dot(p, axis) for p in points]
    return min(dots), max(dots)

def move(sprite, transform, translation, circular=False):
    transform().position.x += translation.x
    transform().position.y += translation.y
    total_collisions = vec2(0, 0)
    num_collisions = 0
    colliding_plane = None
    min_overlap = float("inf")
    for collider in filter(lambda a: issubclass(type(a), Collider), map_actors):
        box = Box(transform().position.x, transform().position.y, sprite().width, sprite().height)
        points = [
            vec2(box.x-box.w/2, box.y-box.h/2),
            vec2(box.x+box.w/2, box.y-box.h/2),
            vec2(box.x-box.w/2, box.y+box.h/2),
            vec2(box.x+box.w/2, box.y+box.h/2),
        ]
        pos = collider.transform().position
        for plane in collider.planes:
            #transform the plane
            pl = Plane2D(plane.x1+pos.x, plane.y1+pos.y, plane.x2+pos.x, plane.y2+pos.y)
            side = 0
            passed = False
            for p in points:
                pp = point_on_plane(p, pl)
                if side:
                    if pp != side:
                        passed = True
                        break
                else:
                    side = pp
            if passed:
                plane_points = [vec2(pl.x1, pl.y1), vec2(pl.x2, pl.y2)]
                
                minx, maxx = project(points, vec2(1, 0))
                plx1, plx2 = project(plane_points, vec2(1, 0))
                if minx > plx2:
                    break
                if maxx < plx1:
                    break
                
                miny, maxy = project(points, vec2(0, 1))
                ply1, ply2 = project(plane_points, vec2(0, 1))
                if miny > ply2:
                    break
                if maxy < ply1:
                    break

                colliding_plane = pl
                b1, b2 = project(points, pl.normal)
                p1, p2 = project(plane_points, pl.normal) #p1 == p2
                overlap = min(abs(b1 - p1), abs(b2 - p1)) #just assume it's the smaller one

                num_collisions += 1
                total_collisions.x += colliding_plane.normal.x*overlap
                total_collisions.y += colliding_plane.normal.y*overlap

                #if overlap < min_overlap:
                #    colliding_plane = pl
                #    min_overlap = overlap


            """
            #for a polygon and a plane to intersect:
            #1. all points must overlap parallel to the plane
            #2. and the nearest and furthest points must be on opposite sides normal to the plane

            #1 - find intervals that are the parallel projection of the plane
            plane_points = [vec2(pl.x1, pl.y1), vec2(pl.x2, pl.y2)]
            p1, p2 = project(plane_points, perpendicular(pl.normal))
            b1, b2 = project(points, perpendicular(pl.normal))
            #make sure they overlap at all first
            if not (p2 > b1 and b2 > p1):
                continue

            #2 - find intervals that are the normal projection of the plane
            p1, p2 = project(plane_points, pl.normal)
            b1, b2 = project(points, pl.normal)
            #if the min and max are on different sides, then we have an intersection
            if math.copysign(1, (b1 - p1)) != math.copysign(1, (b2 - p1)):                
                if abs(max(b1, b2) - p1) < min_overlap:
                    colliding_plane = pl
                    min_overlap = max(b1, b2) - p1
        if colliding_plane:
            print("collision!")
            """

    if num_collisions > 0:
        print("num_collisions:", num_collisions)
        #transform().position.x -= colliding_plane.normal.x*min_overlap
        #transform().position.y -= colliding_plane.normal.y*min_overlap
        transform().position.x -= total_collisions.x / num_collisions
        transform().position.y -= total_collisions.y / num_collisions
            

class TestActor(Actor):
    
    def __init__(self):
        Actor.__init__(self)
        self.sprite = Component(self.id, Sprite("otherguy.png"))
        self.transform = Component(self.id, Transform())
        self.inventory = Inventory(5)
        self.item_pickups = ItemPickupQueue(self.id)
        self.show_inventory = False
        self.inventory_clicked = Window.track_keypress("i")
        self.left_mouse_clicked = Window.track_click(1)
        self.right_mouse_clicked = Window.track_click(3)

    def update(self):
        speed = 1
        if Window.is_key_down("s"):
            move(self.sprite, self.transform, vec2(0, -speed), True)
            self.transform().rotation.z = 270
        if Window.is_key_down("w"):
            move(self.sprite, self.transform, vec2(0, speed), True)
            self.transform().rotation.z = 90
        if Window.is_key_down("a"):
            move(self.sprite, self.transform, vec2(-speed, 0), True)
            self.transform().rotation.z = 180
        if Window.is_key_down("d"):
            move(self.sprite, self.transform, vec2(speed, 0), True)
            self.transform().rotation.z = 0

        if self.inventory_clicked():
            self.show_inventory = not self.show_inventory
            if self.show_inventory:
                self.inventory.show()
            else:
                self.inventory.hide()

        if self.left_mouse_clicked():
            ClickQueue.broadcast(ClickEvent(self, 1, Window.mouse_position()))
        if self.right_mouse_clicked():
            ClickQueue.broadcast(ClickEvent(self, 3, Window.mouse_position()))

        while item := self.item_pickups.next():
            self.inventory.append(item)


t = TestActor()

i = Item()
i.transform().position.x = 32
i.transform().position.y = -32


i2 = Item()
i2.transform().position.x = 48
i2.transform().position.y = -128
