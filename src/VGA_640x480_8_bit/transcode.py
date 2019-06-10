from PIL import Image

# This code reads an RGB888 image, and makes it into an RGB232 image
# It then adds a bit for the VGA horizontal sync signal (always 1 for the
# image, outside of the image it may be 0, but that is taken care of in the
# code for the MCU.) The RGB232 + horizontal sync bit are now 8 bits -> a byte
# It then packs 4 of the bytes into a 32-bit uint32_t.
# All the data is printed in the form of a 2D C-array for incorporation in the
# MCU code

im = Image.open('image.jpg') # Can be many different formats.
pix = im.load()

# the variable that collects 4 bytes into one uint32_t
current_pix_buf_uint32 = 0

# build a string with all the output
output = "uint32_t my_image[480][160] = {{"
for y in range(480):
    for x in range(640):
        (r, g, b) = pix[x,y]
        # take the RGB top 2, 3 and 2 bits resp.
        r = r>>6 # the top 2 bits
        g = g>>5 # top 3 bits
        b = b>>6 # top 2 bits
        # shift the RGB bits into one byte and add the horizontal sync bit
        val = (r<<6) + (g<<3) + (b<<1) + 1 # the LSB is the horizontal sync which is set to one
        # shift the byte into the uint32_t
        current_pix_buf_uint32 += val<<( (3-x%4)*8 )
        # if 4 bytes have been shifted in, add it to the output
        if x%4==3:
            output += str(current_pix_buf_uint32) + ', '
            current_pix_buf_uint32 = 0 
    output += "},\n{"
output += "}\n"
# producing the output above added some unnecessary ',' and '{', and add a ';'
output = output.replace(", }", "}").replace(",\n{}", "};")

print(output)
