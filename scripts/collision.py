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

#calculates the overlap between a pair of 1d ranges
def overlap(amin, amax, bmin, bmax):
    mx = min(amax, bmax)
    mn = max(amin, bmin)
    if mx < mn:
        return 0
    else:
        return mx - mn

#moves a sprite against a series of polygons utilizing Separating Axis Theorem
def move_sat(sprite, transform, translation, map_actors):

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

#distance from a point p to a line (l1, l2)
def line_distance(p, l1, l2):
	t = (l2.x-l1.x)*(l1.y-p.y) - (l1.x-p.x)*(l2.y-l1.y)
	return abs(t)/distance(l1, l2)

#moves a bounding sphere against arbitrary planes
#bounding sphere plane. not binary space partition
def move_bsp2(transform, radius, velocity, map_actors):
    pos = vec2(transform().position.x+velocity.x, transform().position.y+velocity.y)
    #preprocess colliders and convert them into planes
    planes = []
    for collider in filter(lambda a: issubclass(type(a), Collider), map_actors):
        poly = collider.polygon
        ppos = vec2(collider.transform().position.x, collider.transform().position.y)
        for edge in zip(poly, poly[1:] + poly[:1]):
            (p1, p2) = edge
            planes.append([p1+ppos, p2+ppos])
    for p in planes:
        radp = distance(p[0], p[1])/2.0
        midp = (p[0] + p[1])/2.0
        if (pos.x-midp.x)**2 + (pos.y-midp.y)**2 <= (radius+radp)**2:
            ldist = line_distance(pos, p[0], p[1])
            if ldist < radius:
                n = normalize(perpendicular(p[1] - p[0]))
                adj = n * dot(velocity, n)
                velocity.x -= adj.x
                velocity.y -= adj.y
    return velocity