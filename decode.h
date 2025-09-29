#ifndef DECODE_H
#define DECODE_H

#include "types.h"

/*#define MAX_SECRET_BUF_SIZE 256
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 16*/

typedef struct _DecodeInfo
{
    char *stego_image_fname; // To store the dest file name
    FILE *fptr_stego_image;  // To store the address of stego image
   
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;
} DecodeInfo;

OperationType check_operation_type(char *argv[]);
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status_d do_decoding(DecodeInfo *decInfo); // Updated to include argv
Status_d decode_magic_string(DecodeInfo *decInfo);
Status_d decode_data_from_image(char *data, int size, FILE *fptr_stego_image);

//Status_d decode_secret_file_size(long *file_size, DecodeInfo *decInfo);

Status_d decode_extension_size(int *extn_size, FILE *fptr_stego_image);
Status_d decode_extension_data(char *extn_data, int extn_size, FILE *fptr_stego_image);
Status_d decode_byte_from_lsb(char *data, char *image_buffer);

Status_d decode_size_from_lsb(long *size, FILE *fptr_stego_image);
#endif  