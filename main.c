/*
Name : Ashwajeet Kshirsagar
Project Name :  LSB image stegnography 
Project Description : Steganography is the practice of hiding private or sensitive information within something that appears to be nothing out to the usual. 
                      Steganography is often confused with cryptology because the two are similar in the way that they both are used to protect important information. 
                      The difference between two is that steganography involves hiding information so it appears that no information is hidden at all. 
                      If a person or persons views the object that the information is hidden inside of he or she will have no idea that there is any hidden information, 
                      therefore the person will not attempt to decrypt the information
*/



#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "utils.h"
#include "types.h"
#include <string.h>
int main(int argc, char *argv[])
{
   
 //step1 : check whether the user is passed minimum 4 arguments or not ->  true , step2 , fasle print error message 
 if (argc < 4 && (argc < 3 || strcmp(argv[1], "-d") != 0)) 
 {
  printf("ERROR: Insufficient arguments\n");
  printf("For encoding: ./a.out -e input.bmp secret.txt output.bmp\n");
  printf("For decoding: ./a.out -d stego.bmp decoded.txt\n");
  return 1;
}
 // step 2: call the check operation type function(argv[1])
 OperationType operation = check_operation_type(argv);
 // step 3 : Check returning the e_encode or not -> true go to step 4
 // step 4: Declare a variable with  Encodeinfo
 // step 5: Call the read read_and_validate_encode_args(argv, &encodeInfo)
 // step 6: return type success or failure -> failure means return terminate the program 
 //  success  -> go to page 7
 // step 7: call do_encoding function(&encodeInfo) 
 if (operation == e_encode)
 {
     
     // Step 4: Declare a variable with EncodeInfo
     EncodeInfo encInfo;

     printf("Selected Encoding\n");
     // Step 5: Call the function to read and validate arguments
     if (read_and_validate_encode_args(argv, &encInfo) == e_failure)
     {
        printf("ERROR: Argument validation failed\n");
        return 1;
     }
      // Step 6: If successful, proceed with encoding
     printf("Read and Validate encode argc and function is a success\n");
      
      // Step 7: Call do_encoding function
      if (do_encoding(&encInfo) == e_failure)
      {
        printf("ERROR: Encoding failed\n");
        return 1;
      }

      printf("SUCCESS: Encoding completed successfully\n");
  }
  else if (operation == e_decode)
  {
    
      // Step 4: Declare a variable with DecodeInfo
      DecodeInfo decInfo;

      printf("Selected Decoding\n");
      // Step 5: Call the read_and_validate_decode_args(argv, &decInfo)
      if (read_and_validate_decode_args(argv, &decInfo) == d_failure)
      {
          printf("ERROR: Argument validation failed\n");
          return 1;
      }
      // Step 6: If successful, proceed with decoding
      printf("Read and validate decode args function is a success\n");
      // Step 7: Call do_decoding function
      if (do_decoding(&decInfo) == d_failure)
      {
          printf("ERROR: Decoding failed\n");
          return 1;
      }

      printf("SUCCESS: Decoding completed successfully\n");
  }
  else
  {
      // Unsupported operation
    printf("ERROR: Unsupported operation type. Use -e for encoding or -d for decoding.\n");
    return 1;
  }

    return 0;
}
OperationType check_operation_type(char *argv[])
{
    // step 1: Check whether the symbol is containing -e or not true return e_encode, false go to step 2
    // step 2 : Check whether the symbol is containing -d or not true return e_decode, false return en_supported
  
    // Step 1: Check if the symbol is "-e" (encoding operation)
    if (strcmp(argv[1], "-e") == 0)
    {
      
      return e_encode;  // Return e_encode if the symbol is "-e"
    
    }
    
    // Step 2: Check if the symbol is "-d" (decoding operation)
    else if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;  // Return e_decode if the symbol is "-d"
    }
    
    // Step 3: If neither "-e" nor "-d", return e_unsupported (unsupported operation)
    return e_unsupported;


}