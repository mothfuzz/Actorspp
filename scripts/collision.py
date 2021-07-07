from actors import *
import math

"""base class for any type of polygonal colliding object"""
class Collider(Actor):
    def __init__(self):
        Actor.__init__(self)
        self.polygon = []
        self.transform = Component(self.id, Transform())

def perpendicular(v):
    return vec2(-v.y, v.x)

#returns + if on one side, - if on the other, 0 if colinear
def point_on_plane(p, pl):
    return math.copysign(1, (pl.y2 - pl.y1)*p.x + (pl.x1-pl.x2)*p.y + (pl.x2*pl.y1 - pl.x1*pl.y2))

#returns min and max along a 1-dimensional line
def project(points, axis):
    dots = [dot(p, axis) for p in points]
    return min(dots), max(dots)

def overlap(amin, amax, bmin, bmax):
    mx = min(amax, bmax)
    mn = max(amin, bmin)
    if mx < mn:
        return 0
    else:
        return mx - mn

def move(sprite, transform, translation, map_actors):

    #perform initial movement
    transform().position.x += translation.x
    transform().position.y += translation.y

    #block out of all colliders
    for collider in filter(lambda a: issubclass(type(a), Collider), map_actors):

        #for pushing out
        min_overlap = float("inf")
        min_vector = None

        #generate transformed bounding box
        box_pos = transform().position
        box_w = sprite().width
        box_h = sprite().height
        box_points = [
            vec2(box_pos.x-box_w/2, box_pos.y-box_h/2),
            vec2(box_pos.x+box_w/2, box_pos.y-box_h/2),
            vec2(box_pos.x-box_w/2, box_pos.y+box_h/2),
            vec2(box_pos.x+box_w/2, box_pos.y+box_h/2),
        ]

        #generate transformed collider
        col_pos = collider.transform().position
        col_points = []
        for point in collider.polygon:
            col_points.append(vec2(point.x+col_pos.x, point.y+col_pos.y))

        #find separating axes by iterating both shapes' edge normals
        axes = []
        for edge in zip(col_points, col_points[1:] + col_points[:1]):
            (e1, e2) = edge
            axes.append(normalize(perpendicular(e2 - e1)))
        for edge in zip(box_points, box_points[1:] + box_points[:1]):
            (e1, e2) = edge
            axes.append(normalize(perpendicular(e2 - e1)))
        for axis in axes:
            #test if there's an overlap along each axis
            (bmin, bmax) = project(box_points, axis)
            (pmin, pmax) = project(col_points, axis)
            ol = overlap(bmin, bmax, pmin, pmax)
            if not ol:
                #if there's at least 1 non-overlapping axis, we can bail early
                min_vector = None
                break
            else:
                if ol < min_overlap:
                    min_overlap = ol
                    min_vector = axis * ol

        #move our sprite the shortest distance possible to escape collision
        if min_vector:
            #make sure the move is actually oriented away from the polygon
            direct = vec2(box_pos.x, box_pos.y) - vec2(col_pos.x, col_pos.y)
            if dot(min_vector, direct) > 0:
                min_vector = min_vector * float(-1.0)
            transform().position.x -= min_vector.x
            transform().position.y -= min_vector.y
