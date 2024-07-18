import sys, os
from PIL import Image

image = Image.open(sys.argv[1])
pixels = image.load()
for x in range(image.size[1]):
	print("\t",end="")
	for y in range(image.size[0]):
		print("0x%s, " % (("ff" if pixels[y,x][-1] > 30 else "00") + "".join([hex(i)[2:].zfill(2) for i in pixels[y,x][:-1]])), end="")
	print()
