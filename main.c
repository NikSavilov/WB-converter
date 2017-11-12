#include <stdio.h>
#include <stdlib.h>
#include <afxres.h>
#include <math.h>
static FILE *f_source;
static FILE *f_converted;
unsigned int read_4bytes() {
    unsigned int x = fgetc(f_source) + (fgetc(f_source) << 8) + (fgetc(f_source) << 16) + (fgetc(f_source) << 24);
    return x;
};
unsigned int read_2bytes() {
    unsigned int x = fgetc(f_source) + (fgetc(f_source) << 8);
    return x;
};
struct BITMAPFILEHEADER {
    unsigned int    signature;
    unsigned long   bfSize;
    unsigned long   bfOffBits;
};
struct BITMAPINFOHEADER {
    unsigned        version;
    unsigned short  biBitCount;
};
struct BITMAPFILEHEADER read_head() {
    struct BITMAPFILEHEADER t;
    fseek(f_source,0,SEEK_SET);
    t.signature = read_2bytes();
    t.bfSize = read_4bytes();
    fseek(f_source,4,SEEK_CUR);
    t.bfOffBits = read_4bytes();
    return t;
};
struct BITMAPINFOHEADER read_info() {
    struct BITMAPINFOHEADER t;
    t.version = read_4bytes();
    (t.version < 40) ? fseek(f_source,24,SEEK_SET) : fseek(f_source,28,SEEK_SET);
    t.biBitCount = fgetc(f_source) + (fgetc(f_source) << 8);
    return t;
};
struct CHANNELS {
    BYTE r, g, b, grey;
};
void converter_pallet (struct BITMAPFILEHEADER fh, struct BITMAPINFOHEADER fi,char *file_path_converted){
    if ((f_converted = fopen(file_path_converted, "wb"))==NULL) {
        printf("Cannot create file.\n");
        exit(-1);
    }
    fseek(f_source,0,SEEK_SET);
    BYTE *bitmap = malloc(fh.bfSize);
    fread(bitmap,1,fh.bfSize,f_source);
    int i;
    struct CHANNELS rgbset;
    for (i = 1; i <= pow(2,fi.biBitCount); i++){
        rgbset.b = bitmap[fh.bfOffBits - 4 * i];
        rgbset.g = bitmap[fh.bfOffBits - 4 * i + 1];
        rgbset.r = bitmap[fh.bfOffBits - 4 * i + 2];
        rgbset.grey = 0.299*rgbset.r + 0.587*rgbset.g + 0.114*rgbset.b;
        bitmap[fh.bfOffBits - 4 * i] = rgbset.grey;
        bitmap[fh.bfOffBits - 4 * i + 1] = rgbset.grey;
        bitmap[fh.bfOffBits - 4 * i + 2] = rgbset.grey;
    }
    int write_success = fwrite(bitmap,1,fh.bfSize,f_converted);
    if (write_success != fh.bfSize){
        printf("Error of writing.");
        exit(-1);
    }
    else {
        printf("It seems that the converting was successful. Congratulations!");
    }
    fclose(f_converted);
    fclose(f_source);
}
void converter_no_pallet(struct BITMAPFILEHEADER fh, struct BITMAPINFOHEADER fi,char *file_path_converted){
    if ((f_converted = fopen(file_path_converted,"wb"))==NULL){
        printf("Cannot create file.");
        exit(-1);
    }
    BYTE *bitmap = malloc(fh.bfSize);
    fseek(f_source,0,SEEK_SET);
    int read_success = fread(bitmap,1,fh.bfSize,f_source);
    if (read_success != fh.bfSize) {
        printf("Error of reading.");
        exit(-1);
    }
    int i,j;
    struct CHANNELS rgbset;
    if ((fh.bfSize-fh.bfOffBits)%3!=0){
        printf("Error of file structure.");
        exit(-1);
    }
    for (i=0;i<(fh.bfSize-fh.bfOffBits)/3;i++){
        rgbset.b = bitmap[fh.bfOffBits+3*i];
        rgbset.g = bitmap[fh.bfOffBits+3*i+1];
        rgbset.r = bitmap[fh.bfOffBits+3*i+2];
        rgbset.grey = 0.299*rgbset.r + 0.587*rgbset.g + 0.114*rgbset.b;
        for (j=0;j<3;j++){
            bitmap[fh.bfOffBits+3*i+j] = rgbset.grey;
        }
    }
    int write_success = fwrite(bitmap,1,fh.bfSize,f_converted);
    if (write_success != fh.bfSize){
        printf("Error of writing.");
        exit(-1);
    }
    else {
        printf("It seems that the converting was successful. Congratulations!");
    }
    fclose(f_converted);
    fclose(f_source);
}
int main(int args, char **file_path){
    if ((f_source = fopen(file_path[1], "rb")) == NULL) {
        printf("Cannot open file.\n");
        exit(-1);
    }
    struct BITMAPFILEHEADER fh = read_head();
    struct BITMAPINFOHEADER fi = read_info();
    if (fh.signature != 0x4d42){
        printf("Unknown signature.\n");
        exit(-1);
    }
    printf("File's info (signature %x).\nSize: %d bytes.\nBitmap start position: %d.\nBits per pixel: %d\n",fh.signature,fh.bfSize,fh.bfOffBits,fi.biBitCount);
    (fi.biBitCount == 4 || fi.biBitCount == 8) ? converter_pallet(fh,fi,file_path[2]): 0;
    (fi.biBitCount == 24) ? converter_no_pallet(fh,fi,file_path[2]): 0;
    return 0;
}
