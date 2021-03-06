
from scripts.inventory import Inventory, Item, ItemPickupQueue, ClickQueue, ClickEvent
from scripts.collision import PolygonCollider, move_sat, move_bsp2

class Block(PolygonCollider):
    def __init__(self):
        Collider.__init__(self)
        self.sprite = Component(self.id, Sprite("block.png"))
        #clockwise starting from top left
        self.polygon = [
            vec2(-16, 16),
            vec2(16, 16),
            vec2(16, -16),
            vec2(-16, -16),
        ]

class LUSlope(PolygonCollider):
    def __init__(self):
        Collider.__init__(self)
        self.sprite = Component(self.id, Sprite("lslope.png"))
        self.polygon = [
            vec2(16, 16),
            vec2(16, -16),
            vec2(-16, -16),
        ]

class RUSlope(PolygonCollider):
    def __init__(self):
        Collider.__init__(self)
        self.sprite = Component(self.id, Sprite("rslope.png"))
        self.polygon = [
            vec2(-16, 16),
            vec2(16, -16),
            vec2(-16, -16),
        ]

the_map = [
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x01, 0x03,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x01, 0x00, 0x01, 0x00, 0x02, 0x03, 0x00,
    0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00,
]
map_actors = []

left = -Window.get_width()/2
top = Window.get_height()/2
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
                    


class TestActor(Actor):
    def __init__(self):
        Actor.__init__(self)
        self.sprite = Component(self.id, Sprite("player.png"))
        self.transform = Component(self.id, Transform())
        self.inventory = Inventory(5)
        self.item_pickups = ItemPickupQueue(self.id)
        self.show_inventory = False
        self.inventory_clicked = Window.track_keypress("i")
        self.left_mouse_clicked = Window.track_click(1)
        self.right_mouse_clicked = Window.track_click(3)

        self.velocity = vec2(0, 0)
        self.max_speed = 4

    def update(self):
        if Window.is_key_down("s"):
            self.velocity.y -= 0.2
            self.transform().rotation.z = 270
        if Window.is_key_down("w"):
            self.velocity.y += 0.2
            self.transform().rotation.z = 90
        if Window.is_key_down("a"):
            self.velocity.x -= 0.2
            self.transform().rotation.z = 180
        if Window.is_key_down("d"):
            self.velocity.x += 0.2
            self.transform().rotation.z = 0

        if self.velocity.x < -self.max_speed:
            self.velocity.x = -self.max_speed
        if self.velocity.x > self.max_speed:
            self.velocity.x = self.max_speed
        if self.velocity.y < -self.max_speed:
            self.velocity.y = -self.max_speed
        if self.velocity.y > self.max_speed:
            self.velocity.y = self.max_speed
        
        move_sat(self.sprite, self.transform, self.velocity, map_actors)

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