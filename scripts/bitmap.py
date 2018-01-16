#!/usr/bin/python3

from PIL import Image
import sys

if len(sys.argv) < 2:
    print("Usage: bitmap.py filename")
    sys.exit(-1)

filename = sys.argv[1]

im = Image.open(filename, 'r')
im = im.resize((20, 20), Image.ANTIALIAS)
pic = im.load()

for y in range(20):
    arr = []
    for x in range(20):
        r, g, b = pic[x,y]
        color = (r + g + b) / 3.0
        if color > 200:
            arr.append('0')
        else:
            arr.append('1')
    print("".join(arr))