#!/usr/bin/python
import sys
def main(fname):
    with open(fname, 'r') as f:
        lines = []
        nf = 0
        nv = 0
        nvn = 0
        for line in f:
            if line[:2] == 'v ':
                nv = nv + 1
                lines.append(line)
            elif line[:3] == 'vn ':
                nvn = nvn + 1
                lines.append(line)
            elif line[:2] == 'f ':
                nf = nf + 1
                v = []
                for pt in line.split()[1:]:
                    refs = pt.split('/')
                    v.append(int(refs[0]))
                    v.append(int(refs[2]))
                lines.append('f %d//%d %d//%d %d//%d\n' % tuple(v))
        with open('clean-'+fname, 'w') as fout:
            fout.write('#faces: %d\n' % nf)
            fout.write('#vertices: %d\n' % nv)
            fout.write('#normals: %d\n' % nvn)
            fout.writelines(lines)

if __name__ == '__main__':
    main(sys.argv[1])