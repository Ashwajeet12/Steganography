# LSB Image Steganography
## Overview
This project implements an Image Steganography system using the Least Significant Bit (LSB) technique in C programming language. It enables hiding secret text data inside an image and extracting it back from the stego image. This technique modifies the least significant bits of the pixel values to embed data without visibly altering the image.

## Features
- Encode secret data from a text file into an image using LSB manipulation.
- Decode and recover the hidden secret data from the stego image.
- Supports handling secret file extensions and size embedding for accurate decoding.
- Command-line interface for encoding and decoding operations.
- Sample secret file `secret.txt` and carrier image `beautiful.jpg` included for demonstration.
