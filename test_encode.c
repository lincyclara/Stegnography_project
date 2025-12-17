/*
Name : Lincy K
Date : 27/10/2025
Description :LSB Image Steganography is a simple and effective way to hide secret information inside an image.
             Every image is made up of tiny dots called pixels, and each pixel has color values stored as 
             numbers in binary form (0s and 1s).In this method, the last bit (called the Least Significant 
             Bit) of each pixel’s color value is replaced with a bit from the secret message. Since changing 
             this last bit makes almost no visible difference to the image, the picture looks exactly the same
             to the human eye — but secretly contains hidden data inside its pixels.
*/



#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"

#include "types.h"

int main(int argc,char *argv[])
{   
    if(argc <2){
        printf("Insufficient arguments, please follow the format\n");
        printf("For encoding use format: ./a.out -e <source.bmp> <secretfile.txt> <encodedfile.bmp>\n");
        printf("For decoding use format: ./a.out -d <encodedfile.bmp> <output.txt>\n");
        return e_failure;
    }

    if(check_operation_type(argv) == e_encode)
    {
        printf("You have choosed Encoding\n");
        if(argc < 4)
        {
            printf("Error: Insufficient arguments for encoding\n");
            printf("For encoding use format: ./a.out -e <source.bmp> <secretfile.txt> <encodedfile.bmp>\n");
            return e_failure;
        }

        EncodeInfo encInfo;
        if(read_and_validate_encode_args(argv,&encInfo) == e_success)
        {
            printf("Read and validate is successful\n");
            if(do_encoding(&encInfo) == e_success)
            {
                printf("Encoding Successful\n");
            }
            else
            { 
                printf("Encoding unsuccessfull\n");
                return e_failure;
            }
        }
        else
        {
            printf("Read and validate is unsuccessful\n");
            return e_failure;
        }

    }
    else if(check_operation_type(argv) == e_decode)
    {
        if(argc < 4)
        {
            printf("Error: Insufficient arguments for decoding\n");
            printf("For decoding use format: ./a.out -d <encodedfile.bmp> <output.txt>\n");
            return e_failure;
        }

        printf("You have choosed Decoding\n");
        DecodeInfo decInfo;
        if(read_and_validate_decode_args(argv,&decInfo) == e_success)
        {
            printf("Read and validate is successful\n");
            if(do_decoding(&decInfo) == e_success)
            {
                printf("Decoding successful\n");
            }
            else
            {
                printf("Decoding unsuccessful\n");
                return e_failure;
            }
        }
        else
        {
            printf("Read and validate is unsuccessful\n");
            return e_failure;
        }
    }
    else
    {
        printf("It is unsupported\n");
        return e_failure;
    }

    return e_success;       
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d") == 0)
    {
        return e_decode;
    }
    else{
        return e_unsupported;
    }
}