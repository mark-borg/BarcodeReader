# Script that converts old-style OpenCV docs to new-style.
# Only matrices are supported right now, base64-encoded data is not supported.

import sys,re

infile = open(sys.argv[1],"r")
outfile = open(sys.argv[2], "w")
mode = 0 # neutral

outfile.write("<?xml version=\"1.0\"?>\n<opencv_storage>\n");

for l in infile.xreadlines():
    if mode == 0:
        if re.match( "^<elem.+", l ):
            elem_id = re.findall( "id *= *\"([^\"]+)\"", l )[0]
            elem_val = re.findall( "value *= *\"([^\"]+)\"", l )[0]
            outfile.write( "<%s>%s</%s>\n" % (elem_id, elem_val, elem_id))
        elif re.match( r"^ *<struct.+", l ):
            if l.find('"CvMat"') > 0:
                mat_id = re.findall( "id *= *\"([^\"]+)\"", l )[0]
                mat_size = re.findall( "size *= *\"([^\"]+)\"", l )[0].split()
                mat_dt = re.findall( "dt *= *\"([^\"]+)\"", l )[0]
                outfile.write("<%s type_id=\"opencv-matrix\">\n" % (mat_id,))
                outfile.write("<rows>%s</rows>\n" % (mat_size[0],))
                outfile.write("<cols>%s</cols>\n" % (mat_size[1],))
                outfile.write("<dt>%s</dt>\n" % (mat_dt,))
                outfile.write("<data>\n")
                mode = 1
    else:
        if re.match( r"^ *</struct>", l ):
            outfile.write("</data></%s>\n" % (mat_id,))
            mode = 0
        else:
            outfile.write(l)

outfile.write("</opencv_storage>\n")
infile.close()
outfile.close()
