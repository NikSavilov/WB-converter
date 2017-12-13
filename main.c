#include "header.h"

int main(int argc, char **argv){
    char *file_path_converted,*file_path_current;
    int om_flag = 0;
    for (int k = 1; k < argc;k++){
        if (strcmp(argv[k],"-om") == 0) {
            om_flag = 1;
        }
        else if (strstr(argv[k],"-i=") == argv[k]){
            file_path_current = malloc(strlen(argv[k]));
            strcpy(file_path_current,argv[k] + 3);
        }
        else if ((strstr(argv[k],"-o=") == argv[k])&&(om_flag!=1)){
            file_path_converted = malloc(strlen(argv[k]));
            strcpy(file_path_converted,argv[k] + 3);
        }
    }
    if ((om_flag == 1)&&(file_path_converted!=NULL)) {
        printf("Invalid keys. You can't use overwrite mode and -o key together.\n");
        exit(-1);
    }
    ((om_flag == 1)) ? file_path_converted = file_path_current : 0;
    FILE *f_source;
    if ((f_source = fopen(file_path_current, "rb")) == NULL) {
        printf("Can't open file.\n");
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
    mem_check(bitmap);
    fseek(f_source,0,SEEK_SET);
    int read_success = fread(bitmap,1,fh.bfSize,f_source);
    if (read_success != fh.bfSize) {
        printf("Error of reading.--%d-%d--",read_success,fh.bfSize);
        exit(-1);
    }
    fclose(f_source);
    free(f_source);
    switch (fi.biBitCount) {
        case 4:
        case 8:     converter_pallet(fh,fi,bitmap,file_path_converted); break;
        case 24:    converter_no_pallet(fh,bitmap,file_path_converted); break;
        default:    printf("Format is not supported.");
    }
    free(file_path_converted);
    free(file_path_current);
    free(bitmap);
    return 0;
}
