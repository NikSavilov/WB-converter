#include "header.h"
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
void mem_check(BYTE *x){
    if (x == NULL){
        printf("Error. Haven't got enough memory.\n");
        exit(-1);
    };
}
unsigned int read_n_bytes(int n,FILE *f_source) {
    BYTE *x = malloc(n);
    mem_check(x);
    unsigned int n_bytes = 0;
    fread(x,1,n,f_source);
    for (int i=0; i < n; i++)
        n_bytes += x[i] << (8 * i);
    free(x);
    return n_bytes;
};
void rgb_con(BYTE *bitmap,int index){
    struct CHANNELS rgbset;
    rgbset.b = bitmap[index];
    rgbset.g = bitmap[index + 1];
    rgbset.r = bitmap[index + 2];
    rgbset.grey = 0.299*rgbset.r + 0.587*rgbset.g + 0.114*rgbset.b;
    bitmap[index ] = rgbset.grey;
    bitmap[index + 1] = rgbset.grey;
    bitmap[index + 2] = rgbset.grey;
}
void converter_pallet (struct BITMAPFILEHEADER fh, struct BITMAPINFOHEADER fi, BYTE *bitmap, char *file_path_converted){
    FILE *f_converted;
    if ((f_converted = fopen(file_path_converted, "wb"))==NULL) {
        printf("Can't create file.\n");
        exit(-1);
    }
    int index;
    for (int i = 1; i <= pow(2,fi.biBitCount); i++){
        index = fh.bfOffBits - 4 * i;
        rgb_con(bitmap,index);
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
    free(f_converted);
}
void converter_no_pallet(struct BITMAPFILEHEADER fh, BYTE *bitmap, char *file_path_converted){
    FILE *f_converted;
    if ((f_converted = fopen(file_path_converted,"wb"))==NULL){
        printf("Can't create file.");
        exit(-1);
    }
    if ((fh.bfSize-fh.bfOffBits)%3!=0){
        printf("Error of file structure.");
        exit(-1);
    }
    int index;
    for (int i=0;i<(fh.bfSize-fh.bfOffBits)/3;i++){
        index = fh.bfOffBits+3*i;
        rgb_con(bitmap,index);
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
    free(f_converted);
}