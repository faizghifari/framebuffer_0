#!/usr/bin/python3

from PIL import Image, ImageFont, ImageDraw
import sys

if len(sys.argv) < 2:
    print("Usage: gen_fonts.py fontfile")
    sys.exit(-1)
filename = sys.argv[1]

for c in list('abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789? '):
    img = Image.new("RGBA", (40,40),(255,255,255))
    draw = ImageDraw.Draw(img)
    font = ImageFont.truetype(filename, 24)
    draw.text((0,0), c, font=font, fill="black")
    
    pic = img.load()
    upper_bound = (40, 40)
    lower_bound = (0, 0)
    for x in range(40):
        for y in range(40):
            r,g,b,a = pic[x,y]
            color = (r + g + b) / 3.0
            if color <= 200:
                upper_bound = min(upper_bound[0], x), min(upper_bound[1], y)
                lower_bound = max(lower_bound[0], x), max(lower_bound[1], y)

    img = img.crop((upper_bound[0], upper_bound[1], lower_bound[0], lower_bound[1]))
    img = img.resize((20, 20), Image.ANTIALIAS)
    pic = img.load()

    file_string = ""

    for y in range(20):
        arr = []
        for x in range(20):
            r,g,b,a = pic[x,y]
            color = (r + g + b) / 3.0
            if color > 200:
                arr.append('0')
            else:
                arr.append('1')
        file_string += "".join(arr) + "\n"
    
    with open("data/"+c+".txt", "w") as f:
        f.write(file_string)