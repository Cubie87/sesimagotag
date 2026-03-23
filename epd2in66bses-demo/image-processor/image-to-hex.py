import cv2
import numpy as np

# 1. Read the image as grayscale (0 flag)
img_path = 'test.png'
img = cv2.imread(img_path, 0) # img is a numpy array with values 0-255

# 2. Define a threshold
threshold_value = 128

# 3. Apply the threshold to get 0s and 1s
# If pixel value < 128, it becomes 0 (black); otherwise, it becomes 1 (white)
binary_img = (img >= threshold_value).astype(int)

# Now, 'binary_img' is a 2D numpy array containing only 0s and 1s.
# You can then process this array further or save it as a text file if needed.
#print(binary_img)


flat_bits = binary_img.flatten()

# 2. Pack the bits into bytes (automatically handles groups of 8)
packed_bytes = np.packbits(flat_bits)

# 3. Convert those bytes to hex strings
hex_output = [f"0X{b:02X}" for b in packed_bytes]

formatted_data = ", ".join(hex_output)

print(formatted_data)

with open("image_hex.txt", "w") as f:
    f.write(formatted_data)
