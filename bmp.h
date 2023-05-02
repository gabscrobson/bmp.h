#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef uint8_t  BYTE;
typedef uint32_t DWORD;
typedef int32_t  LONG;
typedef uint16_t WORD;

/*
* BITMAPFILEHEADER
* The BITMAPFILEHEADER structure contains information about the type, size,
* and layout of a file that contains a DIB [device-independent bitmap].
* http://msdn.microsoft.com/en-us/library/dd183374(VS.85).aspx.
*/
typedef struct
{
    WORD   bfType;
    DWORD  bfSize;
    WORD   bfReserved1;
    WORD   bfReserved2;
    DWORD  bfOffBits;
} __attribute__((__packed__))
BITMAPFILEHEADER;

/*
*   BITMAPINFOHEADER:
*   The BITMAPINFOHEADER structure contains information about the
*   dimensions and color format of a DIB [device-independent bitmap].
*   http://msdn.microsoft.com/en-us/library/dd183376(VS.85).aspx.
*/
typedef struct
{
    DWORD  biSize;
    LONG   biWidth;
    LONG   biHeight;
    WORD   biPlanes;
    WORD   biBitCount;
    DWORD  biCompression;
    DWORD  biSizeImage;
    LONG   biXPelsPerMeter;
    LONG   biYPelsPerMeter;
    DWORD  biClrUsed;
    DWORD  biClrImportant;
} __attribute__((__packed__))
BITMAPINFOHEADER;

/*
*   RGBTRIPLE:
*   This structure describes a color consisting of relative intensities of
*   red, green, and blue.
*   http://msdn.microsoft.com/en-us/library/aa922590.aspx.
*/
typedef struct
{
    BYTE  blue;
    BYTE  green;
    BYTE  red;
}
RGBTRIPLE;

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

bmp* bmpIn(char *filename) {
    bmp *result = malloc(sizeof(bmp));

    // open file
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("Could not open %s.\n", filename);
        return NULL;
    }

    // READ FILE HEADER
    fread(&result->bf, sizeof(BITMAPFILEHEADER), 1, fp);

    // READ INFO HEADER
    fread(&result->bi, sizeof(BITMAPINFOHEADER), 1, fp);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (result->bf.bfType != 0x4d42 || result->bf.bfOffBits != 54 || result->bi.biSize != 40 ||
        result->bi.biBitCount != 24 || result->bi.biCompression != 0)
    {
        fclose(fp);
        printf("Unsupported file format.\n");
        return NULL;
    }

    // height and width from info header
    result->height = abs(result->bi.biHeight);
    result->width = result->bi.biWidth;

    // allocate memory for a 2D array of pixels
    // result->pixels = calloc(result->height, result->width * sizeof(RGBTRIPLE)); RGBTRIPLE (*pixelsBuffer)[result->width] = calloc(result->height, result->width * sizeof(RGBTRIPLE));

    result->pixels = (RGBTRIPLE**)malloc(result->height * sizeof(RGBTRIPLE*));
    for(int i = 0; i < result->height; i++)
        result->pixels[i] = (RGBTRIPLE*)malloc(result->width * sizeof(RGBTRIPLE));

    if (result->pixels == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(fp);
        return NULL;
    }

    // determine padding for scanlines
    result->padding =  (4 - (result->width * sizeof(RGBTRIPLE)) % 4) % 4;

    // iterate over scanlines
    for (int i = 0; i < result->height; i++)
    {
        // read row into pixel array
        fread(result->pixels[i], sizeof(RGBTRIPLE), result->width, fp);

        // skip over padding
        fseek(fp, result->padding, SEEK_CUR);
    }

    fclose(fp);

    return result;
}

void bmpOut(char *filename, bmp *image) {

    FILE * outptr = fopen(filename, "w");

    // write BITMAPFILEHEADER
    fwrite(&image->bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write BITMAPINFOHEADER
    fwrite(&image->bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // write new pixels to outfile
    for (int i = 0; i < image->height; i++)
    {
        // write row to outfile
        fwrite(image->pixels[i], sizeof(RGBTRIPLE), image->width, outptr);

        // write padding at end of row
        for (int k = 0; k < image->padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    fclose(outptr);

}
