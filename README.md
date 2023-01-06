# bmp.h
Library to read, manipulate and save 24-bit BMP files in C.


## Usage
In order to use it, move `bmp.h` to your project directory and include it in the header of your code, as the below.
```c
#include "bmp.h"
```


## Structures
```c
typedef struct
{
    BYTE  blue;
    BYTE  green;
    BYTE  red;
} __attribute__((__packed__))
RGBTRIPLE;
```
`RGBTRIPLE` structure describes a color consisting of relative intensities of red, green, and blue.
<br><br>

```c
typedef struct
{
    BITMAPFILEHEADER bf;
    BITMAPINFOHEADER bi;
    int height;
    int width;
    int padding;
    RGBTRIPLE **pixels;
}
bmp;
```
`bmp` structure groups metadata and a 2D array of pixels for easier manipulation of the bmp.


## Functions
```c
int bmpIn(char *filename, bmp *pt)
```
- `filename` name of the file that will be used as input.
- `*pt` adress of the variable that is going to store the data.
<br><br>

```c
int bmpOut(char *filename, bmp image)
```
- `filename` desired name of the output file.
- `image` variable that is going to export the data.


## Example
```c
#include <stdio.h>
#include "bmp.h"

int main(void) {
    bmp image;
    bmpIn("mj.bmp", &image);

    for(int i = 0; i < image.height; i++) {
        for(int j = 0; j < image.width; j++) {
            BYTE media = (image.pixels[i][j].red + image.pixels[i][j].green + image.pixels[i][j].blue)/3;
            image.pixels[i][j].red = media;
            image.pixels[i][j].green = media;
            image.pixels[i][j].blue = media;
        }
    }

    bmpOut("teste.bmp", image);
}
```
