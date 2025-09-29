#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "utils.h"
#include <string.h>
#include "common.h"
#include <stdlib.h>
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of the secret file data and return it
    fseek(fptr, 0, SEEK_END); // Move to the end of the file
    uint file_size = ftell(fptr); // Get the current file pointer position (size)
    fseek(fptr, 0, SEEK_SET); // Reset file pointer to the start
    printf("Secret file size: %u\n", file_size);
    return file_size;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{

    // Step 1 : Check argv[2] is .bmp or not -> false print error message ,return e_failure,
    // true -> store argv[2] into structure member

    // Step 2 : Check argv[3] is .txt or not -> false print error message,return e_failure,
    // true -> store argv[3] into structure member

    // Step 3 : Check argv[4] is NULL or or not -> if it is NULL then store the default.bmp into structure member
    // false -> Check whether argv[4] is having the .bmp or not ->false print error message,return e _failure
    // true -> store argv[4] into structure member

    // Step 4 : All are get valid once argument then print success message , return e_success

    if (strstr(argv[2], ".bmp") == NULL) 
    {
        printf("ERROR: Source image file must be a .bmp file\n");
        return e_failure;
    }
    encInfo->src_image_fname = argv[2];

    // Check if the secret file is a .txt file
    if ((strstr(argv[3], ".txt") != NULL ) || (strstr(argv[3], ".c") != NULL ) || (strstr(argv[3], ".sh") != NULL )) 
    {
        encInfo->secret_fname = argv[3];
       
    }
    else{
        printf("ERROR: Secret file must be a .txt file\n");
        return e_failure;
    }
    

    // If no stego image file is given, use a default name
    if (argv[4] == NULL) 
    {
        encInfo->stego_image_fname = "output.bmp";  // Default output file name
    } 
    else 
    {
        if (strstr(argv[4], ".bmp") == NULL) 
        {
            printf("ERROR: Stego image file must be a .bmp file\n");
            return e_failure;
        }
        encInfo->stego_image_fname = argv[4];
    }

   
    return e_success;  // Return success if validation passed
}

Status open_files_encode(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    // Step 1 : call the get_image_size_for_bmp(encinfo->fptr_src_iamge) ,store into encInfo->image_capacity;
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image); // Get image capacity
    // Step 2 : call the get_file_size(encInfo->fptr_secret_file), store into encInfo->size_secret_file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret); // Get secret file size

    // Step 3 : image capacity > (54+16+32+32+32+(size_secret_file_data * 8));
    //  true -> print success message , return e_success
    //  false ->print error message , return e_failure;
    if (encInfo->image_capacity < (54 + 16 + 32 + 32 + 32 + (encInfo->size_secret_file * 8)))
    {
        printf("ERROR: Not enough space in image to store secret data.\n");
        return e_failure;
    }

    return e_success;

}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // char imageBuffer[54]
    // Step 1: Rewind the fptr_src_image
    // Step 2 : Read 54 bytes from the src image store into imageBuffer
    // Step 3 : Copy the imageBuffer to fptr_dest_image
    // Step 4 : return e_success
    char imageBuffer[54];  // BMP header size is 54 bytes

    fseek(fptr_src_image, 0, SEEK_SET); // Rewind source image
    fread(imageBuffer, sizeof(char), 54, fptr_src_image); // Read BMP header
    fwrite(imageBuffer, sizeof(char), 54, fptr_dest_image); // Write header to destination image

    return e_success;

}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{

    // Step 1 : Find the size of magic String stroe into size;
    int size = strlen(magic_string);
    
    // Step 2 : Call the encodedataimage(magic_string,size,//File pointers)
    if (encode_data_to_image(magic_string, size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Failed to encode magic string\n");
        return e_failure;
    }

    return e_success;
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{

    
    // char imageBuffer[32];
    // Step1 : Read the 32 bytes from from src store into imageBuffer
    // Step2 : Call the encode size to lsb (size, imageBuffer)
    // Step 3 : Write the 32 bytes to the destination image
    // return e_success;
    char imageBuffer[32];
    fread(imageBuffer, sizeof(char), 32, encInfo->fptr_src_image);
    encode_size_to_lsb(size, imageBuffer);
    fwrite(imageBuffer, sizeof(char), 32, encInfo->fptr_stego_image);
    
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    // Step1 : call the encode_data_to_image(file_extn,sizeof the extn ,FILE POINTERS)
    //  Step 2 : return e_success;
    
    if (encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Failed to encode secret file extension.\n");
        return e_failure;
    }

    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // char imageBuffer[32];
    // Step1 : Read the 32 bytes from from src store into imageBuffer
    // Step2 : Call the encode size to lsb (size, imageBuffer)
    // Step 3 : Write the 32 bytes to the destination image
    // return e_success;
   
    char imageBuffer[32];
    fread(imageBuffer, sizeof(char), 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, imageBuffer);
   
    fwrite(imageBuffer, sizeof(char), 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{

    // char data[encInfo->size_secret_file];
    // Step 1: Rewind the secret_file_fptr;
    // Step 2 : Read the contents of the address from secret.txt
    // Step 3 : call the encode_data_image(data,encInfo->size_secret_file,FILE POINTERS)
    // Step 4 : return e_success;
    char *data = (char *)malloc(encInfo->size_secret_file);
    fseek(encInfo->fptr_secret, 0, SEEK_SET); // Rewind secret file
    fread(data, 1, encInfo->size_secret_file, encInfo->fptr_secret); // Read secret file data

    if (encode_data_to_image(data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        free(data);
        printf("ERROR: Failed to encode secret file data.\n");
        return e_failure;
    }

    free(data);
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    // Step 1; Read remaining the data charcter by charcter and store into the
    // dest image upto end of the file
    // Step 2 : return e_success;
    char buffer[1024];
    size_t bytesRead ;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        fwrite(buffer, 1, bytesRead, fptr_dest);
       
    }
    
    return e_success;

 
}

Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // char imageBuffer[8];
    // Step 1 : Read 8 Bytes from src image store into imageBuffer  ;
    // Step 2 : Call the encode_byte_to_lsb(data[i],imageBuffer)
    // Step 3 : write the imageBuffer to dest_image
    // Step 4 : Do the same process upto the size
    // Step 5 : return e_success
    unsigned char imageBuffer[8];

  //  printf("%d",size);
    for (int i = 0; i < size; i++)
    {
       
        fread(imageBuffer, sizeof(char), 8, fptr_src_image); // Read 8 bytes from the source image
        encode_byte_to_lsb(data[i], imageBuffer); // Encode one byte of secret data
        fwrite(imageBuffer, sizeof(char), 8, fptr_stego_image); // Write to stego image
    }

    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Step 1: Clear a bit in the imageBuffer[i]
    // Step 2: Get a bit from data
    // Step 3: Replace the got bit into imageBuffer[i] 0th lsb position
    // Step 4: Repeat the same process 8 times
    for (int i = 0; i < 8; i++)
    {
      //  printf("%d ",image_buffer[i]);
        // Clear the LSB of the image byte
        image_buffer[i] &= 0xFE;

        // Extract the LSB from the data byte and insert it into the image buffer
        image_buffer[i] |= ((data >> i) & 0x01);
   
    }

    return e_success;
    // eeturn e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    // Step 1: Clear a bit in the imageBuffer[i]
    // Step 2: Get a bit from size
    // Step 3: Replace the got bit into imag[i] 0th lsb position
    // Step 4: Repeat the sameprocess 32 times
    // Step 5:return e_success;
  

    for (int i = 0; i < 32; i++)
    {
        imageBuffer[i]  = imageBuffer[i] & 0xFE;              // Clear LSB
        imageBuffer[i]  = imageBuffer[i] | ((size >> i) & 0x01); // Shift size right by i, take LSB, and set it
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{

    // Step 1 : Call the openfilesFunction(encInfo)
    // Step 2 : Check success or failure -> success Go to step 3 ,
    //  false - > print error message, return e_failure
    if(open_files_encode(encInfo) == e_failure)
    {
        return e_failure;
    }
    printf("Opened files successfully \n");


    // Step 3 : Call the check capacity(encinfo) true -> print success meassge Go to step 4,
    //  false return e_failure
    if (check_capacity(encInfo) == e_failure)
    {
        return e_failure;
    }
    printf("Checked Capacity successfully\n");

    // Step 4 : Call the copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_dest_image)
    // true -> print success message Go to step 5;
    // false -> print print error message , return e_failure
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }
    printf("Copied bmp header succeesfully\n");

     // Set file pointers to start of pixel data
    fseek(encInfo->fptr_src_image, 54, SEEK_SET);
    fseek(encInfo->fptr_stego_image, 54, SEEK_SET);
  

    // Step 5 : Call the encode_magic_string(MAGIC_STRING,encInfo);
    // true ->print the success message ,Go to Step 6
    // false ->print the error message
     if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        return e_failure;
    }
    printf("Encoded magic string Successfully\n");
   

    // Step 6 : Extract the .txt from the secret_file_name
    // And store into the structure member extn_secret_file find the size of extn
    char *extn = strstr(encInfo->secret_fname, "."); // Find the last dot in the filename
    if (extn == NULL)
    {
        printf("ERROR: Secret file has no extension.\n");
        return e_failure; // Return failure if no extension is found
    }
    int extn_size = strlen(extn);
    strcpy(encInfo->extn_secret_file, extn); // Copy the extension
    printf("Encoded Secret file extension extracted: %s\n", encInfo->extn_secret_file);
   

    // Step 7 : Call the function encode_secret_file_extn_size(secret file extn size , encInfo );
    // true - > print success message Go to the step 8,
    // false -> print the error message , return e_failure;
    if (encode_secret_file_extn_size(extn_size, encInfo) == e_failure)
    {
        printf("ERROR: Failed to encode secret file extension size into the image.\n");
        return e_failure; // Return failure if encoding extension size fails
    }
    printf("Encoded secret file extn size successfully\n");
  

    // Step 8 :Call the function encode_secret_file_extn(file_extn, encInfo);
    // true -> print success message Go to the step 9 ;
    // false ->print error message return failure ;
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("ERROR: Failed to encode secret file extension into the image.\n");
        return e_failure; // Return failure if encoding extension fails
    }
    printf("Encoded secret file extn successfully\n");
  

    // Step 9 : Call the encode_secret_file_size(enInfo->size_secret_file, EncodeInfo *encInfo);
    // true -> print success message Go to the step 10 ;
    // false ->print error message return failure ;
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("ERROR: Failed to encode secret file size into the image.\n");
        return e_failure; // Return failure if encoding secret file size fails
    }
    printf("Encoded secret file size successfully\n");

    // Step 10 : Call the secret_file_data(encInfo)
    // true - > print success message Go to the step 11,
    // false -> print the error message , return e_failure;
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("ERROR: Failed to encode secret file data into the image.\n");
        return e_failure; // Return failure if encoding secret file data fails
    }
    printf("Encoded secret file data successfully\n");
    // Step 11 : Call the copyremainingdata(FILEPOINTERS)
    // true - > print success message Go to the step 12,
    // false -> print the error message , return e_failure;
    

  
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("ERROR: Failed to copy remaining image data into the stego image.\n");
        return e_failure; // Return failure if copying remaining data fails
    }
    printf("Copied remaining data successfully \n");

    // Step 12 : Close file pointers
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    printf("Files closed successfully.\n");
  
     return e_success;
}