#include <stdio.h>
#include <stdlib.h>
#include <afxres.h>
#include <math.h>
unsigned int read_n_bytes(int n,FILE *f_source) {
    BYTE *x = malloc(n);
    unsigned int n_bytes = 0;
    fread(x,1,n,f_source);
    for (int i=0; i < n; i++)
        n_bytes += x[i] << (8 * i);
    free(x);
    return n_bytes;
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
struct BITMAPFILEHEADER read_head(FILE *f_source) {
    struct BITMAPFILEHEADER t;
    fseek(f_source,0,SEEK_SET);
    t.signature = read_n_bytes(2,f_source);
    t.bfSize = read_n_bytes(4,f_source);
    fseek(f_source,4,SEEK_CUR);
    t.bfOffBits = read_n_bytes(4,f_source);
    return t;
};
struct BITMAPINFOHEADER read_info(FILE *f_source) {
    struct BITMAPINFOHEADER t;
    t.version = read_n_bytes(4,f_source);
    (t.version < 40) ? fseek(f_source,24,SEEK_SET) : fseek(f_source,28,SEEK_SET);
    t.biBitCount = read_n_bytes(2,f_source);
    return t;
};
struct CHANNELS {
    BYTE r, g, b, grey;
};
void converter_pallet (struct BITMAPFILEHEADER fh, struct BITMAPINFOHEADER fi, BYTE *bitmap, char *file_path_converted){
    FILE *f_converted;
    if ((f_converted = fopen(file_path_converted, "wb"))==NULL) {
        printf("Cannot create file.\n");
        exit(-1);
    }
    struct CHANNELS rgbset;
    for (int i = 1; i <= pow(2,fi.biBitCount); i++){
        int index = fh.bfOffBits - 4 * i;
        rgbset.b = bitmap[index];
        rgbset.g = bitmap[index + 1];
        rgbset.r = bitmap[index + 2];
        rgbset.grey = 0.299*rgbset.r + 0.587*rgbset.g + 0.114*rgbset.b;
        bitmap[index ] = rgbset.grey;
        bitmap[index + 1] = rgbset.grey;
        bitmap[index + 2] = rgbset.grey;
    }
    int write_success = fwrite(bitmap,1,fh.bfSize,f_converted);
    fclose(f_converted);
    if (write_success != fh.bfSize){
        printf("Error of writing.");
        exit(-1);
    }
    else {
        printf("It seems that the converting was successful. Congratulations!");
    }
    free(bitmap);
}
void converter_no_pallet(struct BITMAPFILEHEADER fh, BYTE *bitmap, char *file_path_converted){
    FILE *f_converted;
    if ((f_converted = fopen(file_path_converted,"wb"))==NULL){
        printf("Cannot create file.");
        exit(-1);
    }
    struct CHANNELS rgbset;
    if ((fh.bfSize-fh.bfOffBits)%3!=0){
        printf("Error of file structure.");
        exit(-1);
    }
    for (int i=0;i<(fh.bfSize-fh.bfOffBits)/3;i++){
        int index = fh.bfOffBits+3*i;
        rgbset.b = bitmap[index];
        rgbset.g = bitmap[index + 1];
        rgbset.r = bitmap[index + 2];
        rgbset.grey = 0.299*rgbset.r + 0.587*rgbset.g + 0.114*rgbset.b;
        bitmap[index ] = rgbset.grey;
        bitmap[index + 1] = rgbset.grey;
        bitmap[index + 2] = rgbset.grey;
    }
    int write_success = fwrite(bitmap,1,fh.bfSize,f_converted);
    fclose(f_converted);
    if (write_success != fh.bfSize){
        printf("Error of writing.");
        exit(-1);
    }
    else {
        printf("It seems that the converting was successful. Congratulations!");
    }
    free(bitmap);
}
int main(int argc, char **argv){
    char *file_path_converted,*file_path_current;
    if (argc == 3){
        file_path_current = argv[1];
        file_path_converted = argv[2];
    }
    else if (argc == 2){
        file_path_converted = argv[1];
        file_path_current = argv[1];
    }
    else {
        printf("No arguments detected.");
        exit(-1);
    }
    FILE *f_source;
    if ((f_source = fopen(file_path_current, "rb")) == NULL) {
        printf("Cannot open file.\n");
        exit(-1);
    }
    struct BITMAPFILEHEADER fh = read_head(f_source);
    struct BITMAPINFOHEADER fi = read_info(f_source);
    if (fh.signature != 0x4d42){
        printf("Unknown signature.\n");
        exit(-1);
    }
    printf("File's info (signature %x).\nSize: %d bytes.\nBitmap start position: %d.\nBits per pixel: %d\n",fh.signature,fh.bfSize,fh.bfOffBits,fi.biBitCount);
    BYTE *bitmap = malloc(fh.bfSize);
    fseek(f_source,0,SEEK_SET);
    int read_success = fread(bitmap,1,fh.bfSize,f_source);
    if (read_success != fh.bfSize) {
        printf("Error of reading.--%d-%d--",read_success,fh.bfSize);
        exit(-1);
    }
    fclose(f_source);
    (fi.biBitCount == 4 || fi.biBitCount == 8) ? converter_pallet(fh,fi,bitmap,file_path_converted): 0;
    (fi.biBitCount == 24) ? converter_no_pallet(fh,bitmap,file_path_converted): 0;
    return 0;
}
