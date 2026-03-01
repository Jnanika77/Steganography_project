#ifndef DECODE_H
#define DECODE_H

#include<stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
    char *stego_fname;
    FILE *fptr_stego;

    char dest_fname[20];
    FILE *fptr_dest;
    
}DecodeInfo;

//open stego file
Status open_files_decode(DecodeInfo *decInfo);

//read and validate decode agrs from argv
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);

//Perform the Decoding 
Status do_decoding(DecodeInfo *decInfo);

//Skip the bmp header upto
Status skip_bmp_header(FILE *fptr_stego);
//Decode Magic String
Status decode_magic_string(FILE *stego, char *magic_string);

//Decode a lsb into byte of size of image data array
char lsb_to_byte(char *buffer);

//Decode a Lsb into byte of size of image data array
int lsb_to_size(char *buffer);

//Decode se,cret file extension size
Status decode_extn_file_size(FILE *stego, int *extn_size);

//Decode secret file extension
Status decode_extn_file(FILE *stego, char *extn, int extn_size);

//Decode secret file size
Status decode_secret_file_size(FILE *stego, int *file_size);

//Decode secret file data
Status decode_secret_data(FILE *stego, FILE *dest, int file_size);

#endif