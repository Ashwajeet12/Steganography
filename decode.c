#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "utils.h"
#include "common.h"
#include <string.h>
#include <stdlib.h>

Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    /*if (argv == NULL || argv[2] == NULL )
    {
        printf("ERROR: Insufficient or invalid arguments for decoding\n");
        return d_failure;
    }*/

    if (strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR: Source image file must be a .bmp file\n");
        return d_failure;
    }
    decInfo->stego_image_fname = argv[2];
    

    if(argv[3] == NULL)
    {
        decInfo->secret_fname = "output";
    }
    else
    {
    decInfo->secret_fname = strtok(argv[3],".");
    }

    return d_success;
}

Status open_files_decode(DecodeInfo *decInfo)
{
    // Open the source image file in read mode ("r")
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (decInfo->fptr_stego_image == NULL) {
        printf("ERROR: Could not open source image file %s\n", decInfo->stego_image_fname);
        return d_failure;
    }


    printf("Opened files for decoding successfully\n");
    return d_success;
}



Status_d decode_magic_string(DecodeInfo *decInfo)
{
    char magic_string[50]; 
    char original_magic_string[] = MAGIC_STRING; 

    // Skip the 54-byte BMP header
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if (decode_data_from_image(magic_string, 2 , decInfo->fptr_stego_image) == d_failure)
    {
        printf("ERROR: Failed to decode magic string\n");
        return d_failure;
    }

    magic_string[2] = '\0'; 
    printf("Decoded Magic String: %s\n", magic_string);

    if (strcmp(magic_string, original_magic_string) != 0)
    {
        printf("ERROR: Magic string mismatch\n");
        return d_failure;
    }

    printf("Magic string decoded successfully!\n");
    return d_success;
}

Status_d decode_data_from_image( char *data, int size, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for (int i = 0; i < size; i++)
    {
        fread(image_buffer, sizeof(char), 8, fptr_stego_image);
        
        decode_byte_from_lsb(&data[i], image_buffer);
    }
    return d_success;
}

/* Helper: Decode a byte from LSB of image data array */
Status_d decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data = 0;
    for (int i = 0; i < 8; i++)
    {
        *data = *data  | ((image_buffer[i] & 0x01) << i ); 
    }
    return d_success;


}

Status_d decode_size_from_lsb(long *size, FILE *fptr_stego_image)
{
    *size = 0;
    unsigned char image_buffer[32];

    if (fread(image_buffer, sizeof(char), 32, fptr_stego_image) != 32)
    {
        printf("ERROR: Failed to read size data\n");
        return d_failure;
    }

    // Build size from LSB to MSB: image_buffer[0] is bit 0, image_buffer[31] is bit 31
    for (int i = 0; i < 32; i++)
    {
        *size = *size | ((long)(image_buffer[i] & 0x01) << i);
    }

    printf("Decoded size: %ld\n", *size);

    if (*size <= 0 || *size > MAX_SECRET_BUF_SIZE)
    {
        printf("ERROR: Invalid secret file size decoded (%ld). Must be between 1 and %d\n", *size, MAX_SECRET_BUF_SIZE);
        return d_failure;
    }

    return d_success;
}



Status_d decode_extension_size(int *extn_size, FILE *fptr_stego_image) 
{
     // Decode the size of the file extension (typically 4 bytes)
     long size;
     if (decode_size_from_lsb(&size, fptr_stego_image) == d_failure) 
    {
         return d_failure;
    }
 
    *extn_size = (int)size; // Store the extension size
    
    printf("Decoded extension size successfully\n");
 

     return d_success;
}

Status_d decode_extension_data(char *extn_data, int extn_size, FILE *fptr_stego_image) 
{
    // Decode the extension data (e.g., ".txt")
    if (decode_data_from_image(extn_data, extn_size, fptr_stego_image) == d_failure)
    {
        return d_failure;
    }

    extn_data[extn_size] = '\0'; // Null-terminate the extension string
   
    printf("Decoded extension: %s\n", extn_data);

    return d_success;
}

Status_d decode_secret_file_data(DecodeInfo *decInfo)
{
    // step 1: decode_data_from_imge();
    // step 2: decode_byte_from_lsb();
    // write secret data into outputfile name()
    // Step 1: Decode the file size
     // Step 1: Decode the file size
     long file_size;
     if (decode_size_from_lsb(&file_size, decInfo->fptr_stego_image) == d_failure)
     {
         return d_failure;
     }
 
     printf("INFO: Decoded secret file size: %ld\n", file_size);
 
     char *data = (char *)malloc(file_size);
     if (data == NULL)
     {
         printf("ERROR: Memory allocation failed for secret data\n");
         return d_failure;
     }
 
     if (decode_data_from_image(data, file_size, decInfo->fptr_stego_image) == d_failure)
     {
         free(data);
         return d_failure;
     }
 
     if (fwrite(data, 1, file_size, decInfo->fptr_secret) != file_size)
     {
         printf("ERROR: Failed to write decoded secret data\n");
         free(data);
         return d_failure;
     }
 
     free(data);
     printf("Decoded and saved secret data successfully\n");
 
     return d_success;
}


Status_d do_decoding(DecodeInfo *decInfo)
{
     // Open files and perform decoding
    printf("Opening files for decoding\n");
    if (open_files_decode(decInfo) == d_failure)
    {
        return d_failure;
    }

    if (decode_magic_string(decInfo) == d_failure) 
    {
        return d_failure;
    }

    // Decode extension size and data
    int extn_size;
    if (decode_extension_size(&extn_size, decInfo->fptr_stego_image) == d_failure) 
    {
        return d_failure;
    }

    char *extn_data = (char *)malloc(extn_size + 1); // +1 for null terminator
    if (decode_extension_data(extn_data, extn_size, decInfo->fptr_stego_image) == d_failure) 
    {
       
        return d_failure;
    }
   
   int total_len = strlen(decInfo->secret_fname) + strlen(extn_data) + 1;
   char* final_fname = (char*)malloc(total_len);

   
    // Add the decoded extension to the secret filename
    strcpy(final_fname,decInfo->secret_fname);
    strcat(final_fname, extn_data); // Append the extension to the filename
    
    decInfo->secret_fname = final_fname;
    printf("Decoded output file successfully\n");

    decInfo->fptr_secret = fopen(decInfo->secret_fname,"w");

    // Decode secret file data
    if (decode_secret_file_data(decInfo) == d_failure) 
    {
        
        return d_failure;
    }


    free(final_fname);
    free(extn_data); // Clean up
    fclose(decInfo->fptr_stego_image); // Close the stego image file
    printf("Decoding completed successfully\n");

    return d_success;

}