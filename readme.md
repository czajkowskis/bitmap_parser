# Bitmap parser
This project was created for Computer Architecture with Low-Level Programming course with my colleague Maksymilian Żmuda-Trzebiatowski.

# Introduction
The goal of the assignment was to create a program to work with bitmap files. The main functionalities are:
1. Analyzing  color distribution
2. Converting an image to grayscale
3. Simple steganograhy

# Usage
## Input formats:
To print parsed headers of a BMP file and a histogram of RGB colors (optionally decode an encoded message):

    $ ./program PATH-TO-BMP-FILE 

To convert a BMP file to grayscale:

    $ ./program PATH-TO-INPUT-BMP-FILE PATH-TO-OUTPUT-BMP-FILE

To encode a hidden message using steganography:

    $ ./program PATH-TO-INPUT-BMP PATH-TO-ENCODED-BMP "text to be hidden"