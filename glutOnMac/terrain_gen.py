#!/usr/bin/python
# coding=utf8

import sys
import getopt
import random
from copy import deepcopy

attenuation = 0.4
size = 0

def usage():
    print 'Usage: terrain_gen.py -iITER -rRANGE -dDISP FILE'
    print '    ITER:  number of iterations to perform'
    print '    RANGE: generate terrain in square (-RANGE, -RANGE) -- (RANGE, RANGE)'
    print '    DISP:  random displacement in every iteration'
    print '    FILE:  the C++ header file to output'

# def getvalue(pts, i, j):
#     n = len(pts)
#     if i < 0: i = 0
#     if i >= n: i = n-1
#     if j < 0: j = 0
#     if j >= n: j = n-1
#     return pts[i][j]

#def diamond_square(pts, disp, i0, j0, i2, j2):
def diamond_square(pts, disp, diff):
    i = 0
    while i < size:
        j = 0
        while j < size:
            pts[i][j] = random.uniform(-disp, disp) + pts[i-diff][j-diff]
            + pts[i-diff][j+diff] + pts[i+diff][j-diff] + pts[i+diff][j+diff] 
            j = j + 2*diff
        i = i + 2*diff
    i = 0
    while i < size:
        j = 0
        while j < size:
            if i-2*diff >= 0:
                val = (pts[i][j] + pts[i-diff][j-diff] + pts[i-diff][j+diff] + pts[i-2*diff][j])/4
            else:
                val = (pts[i][j] + pts[i-diff][j-diff] + pts[i-diff][j+diff])/3
            pts[i-diff][j] = val + random.uniform(-disp, disp)

            if i+2*diff < size:
                val = (pts[i][j] + pts[i+diff][j-diff] + pts[i+diff][j+diff] + pts[i+2*diff][j])/4
            else:
                val = (pts[i][j] + pts[i+diff][j-diff] + pts[i+diff][j+diff])/3
            pts[i+diff][j] = val + random.uniform(-disp, disp)

            if j-2*diff >= 0:
                val = (pts[i][j] + pts[i-diff][j-diff] + pts[i+diff][j-diff] + pts[i][j-2*diff])/4
            else:
                val = (pts[i][j] + pts[i-diff][j-diff] + pts[i+diff][j-diff])/3
            pts[i][j-diff] = val + random.uniform(-disp, disp)

            if j+2*diff < size:
                val = (pts[i][j] + pts[i-diff][j+diff] + pts[i+diff][j+diff] + pts[i][j+2*diff])/4
            else:
                val = (pts[i][j] + pts[i-diff][j+diff] + pts[i+diff][j+diff])/3
            pts[i][j+diff] = val + random.uniform(-disp, disp)

            j = j + diff
        i = i + diff
    disp = disp * attenuation
    diff = diff / 2
    if diff > 0:
        # tail-recursive, so cool
        diamond_square(pts, disp, diff)


def make_face_indices(size):
    for i in xrange(size-1):
        for j in xrange(size-1):
            if (i%2 + j) % 2 == 0:
                yield (i*size + j, (i+1)*size + j, i*size + j+1)
                yield ((i+1)*size + j, (i+1)*size + j+1, i*size + j+1)
            else:
                yield (i*size + j, (i+1)*size + j, (i+1)*size + j+1)
                yield (i*size + j, (i+1)*size + j+1, i*size + j+1)

def four_pts_avg(pts, i, j):
    avg = pts[i][j] + pts[i-1 if i>1 else i][j] + pts[i+1 if i+1<size else i][j]
    + pts[i][j-1 if j>1 else j] + pts[i][j+1 if j+1<n else j]
    return avg/4.0

def smooth(pts):
    new_pts = deepcopy(pts)
    for i in xrange(size):
        for j in xrange(size):
            new_pts[i][j] = four_pts_avg(pts, i, j)
            #new_pts[i][j] = pts[i][j]
    return new_pts

def generate_terrain(iter, disp):
    "return points(2d list of heights of correspoding coordinates) and\
    faces(triangles, linear indices in points)"
    size = 2**iter + 1
    points = [[random.uniform(-disp, disp)*0.1 for i in xrange(size)] for j in xrange(size)]
    faces = list(make_face_indices(size))
    random.seed()
    #points[size/2][size/2] = random.uniform(-disp, disp)
    diff = (size-1)/2
    diamond_square(points, disp, diff)
    return points, faces

def print_pts(points, range):
    l = len(points)
    mid = (l - 1) / 2
    # (x, y, z) = (r*j/mid-r, r-r*i/mid, z)
    pts = ['{ %g, %g, %g }' % (range*j/mid - range, range - range*i/mid, points[i][j])
            for i in xrange(l) for j in xrange(l)]
    l = l*l
    lines = [' ,'.join(pts[i:i+4]) for i in xrange(0, l, 4)]
    return ',\n'.join(lines)

def print_indices(indices):
    l = len(indices)
    lines = []
    for i in xrange(0, l, 5):
        lines.append(' ,'.join(['{ %d, %d, %d }' % indices[i + j] for j in xrange(min(5, l-i))]))
    return ',\n'.join(lines)

def output_cppheader(points, faces, range, header):
    with open('header.tmpl') as f:
        template = f.read()
    fill_dict = {
        'headerdef':header.split('.')[0].upper(),
        'npts':len(points)**2, 'nfaces':len(faces),
        'coordinates':print_pts(points, range),
        'indices':print_indices(faces)
    }
    with open(header, 'w') as output:
        output.write(template % fill_dict)
    #print points
    #print faces

def main():
    i = 3
    r = 1.0
    d = 1.0
    hdr = 'terrain.h'
    try:
        opts, args = getopt.getopt(sys.argv[1:], 'hi:r:d:')
        for opt, arg in iter(opts):
            if opt == '-h':
                usage()
                return
            elif opt == '-i':
                try:
                    i = int(arg)
                except:
                    print '-i requires a integer'
                    return
            elif opt == '-r':
                try:
                    r = float(arg)
                    if r < 0.01:
                        raise Exception()
                except:
                    print '-r requires a float'
                    return
            elif opt == '-d':
                try:
                    r = float(arg)
                    if r < 0.01:
                        raise Exception()
                except:
                    print '-d requires a float'
                    return
        if args:
            hdr = args[0]
    except getopt.GetoptError, e:
        usage()
        return
    pts, faces = generate_terrain(i, d)
    output_cppheader(pts, faces, r, hdr)

if __name__ == '__main__':
    main()
