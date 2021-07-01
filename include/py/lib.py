import actors;

class EventQueueMeta(type):
    def __init__(cls, name, bases, attrs):
        cls.event_queues = {}

class EventQueue(metaclass=EventQueueMeta):
    #def __new__(cls, )
    def __init__(self, id):
        self.id = id
        type(self).event_queues[self.id] = []
    def __del__(self):
        del type(self).event_queues[self.id]
    def next(self):
        if type(self).event_queues[self.id]:
            return type(self).event_queues[self.id].pop(0)
        else:
            return None
    @classmethod
    def dispatch(cls, id, e):
        cls.event_queues[id].append(e)
    @classmethod
    def broadcast(cls, e):
        for id in cls.event_queues.keys():
            cls.event_queues[id].append(e)

actors.EventQueue = EventQueue

class Component:
    def __new__(cls, actor_id, value):
        classname = value.__class__.__name__
        return actors.__dict__[classname+"Component"](actor_id, value)

actors.Component = Component
