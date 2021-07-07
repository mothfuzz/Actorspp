from actors import *

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

"""Base type for Items, subclass this by overriding show()"""
class Item(Actor):
    def __init__(self):
        Actor.__init__(self)
        self.clicks = ClickQueue(self.id)
        self.transform = Component(self.id, Transform())
        self.transform().position.z = 0.1 #draw above inventory ui
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
