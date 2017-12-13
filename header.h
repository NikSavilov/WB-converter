#ifndef BW_HEADER_H
#define BW_HEADER_H
#include <stdio.h>
#include <stdlib.h>
#include <afxres.h>
#include <math.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

/*______Structures______*/
struct BITMAPFILEHEADER {
    unsigned int    signature;
    unsigned long   bfSize;
    unsigned long   bfOffBits;
};
struct BITMAPINFOHEADER {
    unsigned        version;
    unsigned short  biBitCount;
};
struct CHANNELS {
    BYTE r, g, b, grey;
};

/*___Functions to be described in source.c___*/
struct BITMAPFILEHEADER read_head(FILE *f_source);
struct BITMAPINFOHEADER read_info(FILE *f_source);
void mem_check(BYTE *x);
void rgb_con(BYTE *bitmap,int index);
void converter_pallet (struct BITMAPFILEHEADER fh, struct BITMAPINFOHEADER fi, BYTE *bitmap, char *file_path_converted);
void converter_no_pallet(struct BITMAPFILEHEADER fh, BYTE *bitmap, char *file_path_converted);
unsigned int read_n_bytes(int n,FILE *f_source);
#endif //BW_HEADER_H
