#!/usr/bin/env python3
import argparse
import cv2
import numpy as np


def main():
    parser = argparse.ArgumentParser(
    formatter_class=argparse.RawDescriptionHelpFormatter,
    description="""Convert an image to a packed hexadecimal byte representation.

    Reads an image and converts it to a binary bitmap. 
    Pixels >= 128 are treated as white (1), pixels < 128 as black (0). 
    The resulting bits are packed into bytes and written as hexadecimal.

    Ensure the image is 296x152px and vertically orientated before parsing, if flashing to an equivalent e-ink display!

    Example:
    image-to-hex.py -i CroppedCentre.png -o image_hex.txt
    """
    )

    parser.add_argument(
        "-i", "--input",
        required=True,
        help="Path to the input image"
    )

    parser.add_argument(
        "-o", "--output",
        required=True,
        help="Path to the output text file"
    )

    args = parser.parse_args()

    # Read image as grayscale
    img = cv2.imread(args.input, 0)

    if img is None:
        parser.error(f"Could not read input image: {args.input}")

    # Define threshold
    threshold_value = 128

    # Convert pixels to 0/1
    binary_img = (img >= threshold_value).astype(int)

    # Flatten into a 1D array
    flat_bits = binary_img.flatten()

    # Pack groups of 8 bits into bytes
    packed_bytes = np.packbits(flat_bits)

    # Convert bytes to hex strings
    hex_output = [f"0X{b:02X}" for b in packed_bytes]

    formatted_data = ", ".join(hex_output)

    # Write output
    with open(args.output, "w") as f:
        f.write(formatted_data)

    print(f"Wrote {len(packed_bytes)} bytes to {args.output}")


if __name__ == "__main__":
    main()