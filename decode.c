#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "decode.h"
//#include "encode.h"
#include "types.h"


/*Read and validate decoding arguements */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2] == NULL || strstr(argv[2],".bmp") == NULL)
    {
        printf("stego.bmp file is not present \n");
        //decInfo->stego_image_fname = argv[2];
        return e_failure;
    }
    printf("stego.bmp is present\n");
    decInfo->stego_image_fname = argv[2];


    if(argv[3] == NULL || strstr(argv[3],".txt") == NULL )
    {
        printf("output.txt is not present\n");
        return e_failure;
    }
    else
    {
        printf("output.txt is  present\n");
        decInfo->output_fname = argv[3];   
    }
    return e_success;
}

/*Perform the Decoding*/
Status do_decoding(DecodeInfo *decInfo)
{
    //opening files
    if(open_files_for_decoding(decInfo) == e_success )
    {
        printf("File opened successfully\n");
    }
    else
    {
        printf("File opening is unsuccessful\n");
        return e_failure;
    }

    fseek(decInfo->fptr_stego_image,54,SEEK_SET);

    // decode magic_string
    if(decode_magic_string(decInfo->magic_string, decInfo) == e_success)
    {
        printf("Magic string is decoded successfully\n");
    }
    else
    {
        printf("Magic string decoding is unsuccessful\n");
        return e_failure;
    }

    // Decode file extension size
    int extn_size;
    if(decode_secret_file_extn_size(&extn_size,decInfo) == e_success)
    {
        printf("File extension size decoded : %d\n",extn_size);
    }
    else
    {
        printf("File extension size decoding failed \n");
        return e_failure;
    }
    char *output_with_extn = NULL;
    // Decode file extension
    char file_extn[10];
    if(decode_secret_file_extn(file_extn,extn_size , decInfo) == e_success)
    {
        printf("File extension decoded :%s\n",file_extn);
    }
    else
    {
        printf("File extension decoding Failed\n");
        return e_failure;
    }
    // Update output filename with decoded extension
    output_with_extn = malloc(256);
    if(!output_with_extn)
    {
        printf("Memory allocation failed\n");
        return e_failure;
    }
    strcpy(output_with_extn,decInfo->output_fname);
    output_with_extn[255] ='\0';
    char *dot = strrchr(output_with_extn,'.');
    if(dot) *dot = '\0';

    strcat(output_with_extn,file_extn); 
    decInfo->output_fname = output_with_extn ;

    if (strlen(output_with_extn) + strlen(file_extn) >= 256)
    {
        printf("Output filename too long\n");
        free(output_with_extn);
        return e_failure;
    }
    
    // Decode secret file size
    int file_size;
    if(decode_file_size(&file_size,decInfo) == e_success)
    {
        printf("File size decoded : %d bytes\n",file_size);
    }
    else
    {
        printf("File size decoding failed\n");
        free(output_with_extn);
        return e_failure;
    }
    //open_output_file
    if(open_output_file(decInfo) == e_success)
    {
        printf("Output file opened successfully\n");
    }
    else
    {
        printf("Output file opening failed\n");
        free(output_with_extn);
        return e_failure;
    }
    //Decode secret file data
    if(decode_secret_file_data(file_size,decInfo) == e_success)
    {
        printf("Secret file data decoded successfully\n");
    }
    else
    {
        printf("Secret file data decoding failed\n");
        free(output_with_extn);
        return e_failure;
    }
    // closing the files which are opened
    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    printf("\n====Decoding completed successfully ====\n");
    printf("Output saved to : %s\n",decInfo->output_fname);

    free(output_with_extn);
    return e_success;
}

//opening files
Status open_files_for_decoding(DecodeInfo *decInfo)
{
    decInfo ->fptr_stego_image = fopen(decInfo->stego_image_fname,"rb"); //opening the file
    if(decInfo->fptr_stego_image == NULL)
    {
        printf("Stego file is not present\n");
        return e_failure;
    }
    else
    {
        printf("Stego file is opened\n");
    }
    return e_success;
}

//decode magic string
Status decode_magic_string(char* magic_string,DecodeInfo *decInfo)
{
    
    unsigned char image_buffer[32];
    // reading the  32 bits of data from stego file 
    if(fread(image_buffer,1,32,decInfo->fptr_stego_image) != 32)
    {
            printf("Error! reading the magic string size\n");
            return e_failure;
    }

    int magic_size;
    // decoding the size from LSB
    if (decode_size_from_lsb(&magic_size,image_buffer) != e_success)
    {
        printf("Error Reading magic string size\n");
            return e_failure;  // failed to decode
    }
    printf("Magic string size: %d\n",magic_size);

    //Now decode the magic string


    char decoded_string[50];
    unsigned char buffer[8];
    char c;
    int i;

    for( i = 0;i<magic_size;i++)
    {
        if(fread(buffer,1,8,decInfo->fptr_stego_image) !=8)
        {
            printf("Error reading magic string data\n");
            return e_failure;
        }
        if(decode_byte_from_lsb(buffer,&c) != e_success)
        {
            printf("Error decoding byte at position %d\n",i);
            return e_failure;
        }

        decoded_string[i] = c;
    }
    decoded_string[i] = '\0';
    //printf("Decoded magic string: %s\n",decoded_string);

    //ask user for magic string to verify
    char magic[50];
    printf("Enter the magic string for verification:");
    scanf("%s",magic);
   
    // comparing decoded string(magic string entered ) with entered magic string while encoding
    if(strcmp(decoded_string,magic) != 0)
    {
        printf("Magic string is not matching with encoded magic string!\n");
        return e_failure;
    }

    // copying magic string data 
    strcpy(decInfo->magic_string,magic);
    printf("Magic string verified successfully \n");

    return e_success;
}
// decoding secret file extension
Status decode_secret_file_extn(char * file_extn,int size,DecodeInfo *decInfo)
{
    unsigned char image_buffer[8];
    char c;

    for(int i=0;i< size;i++)
    {
        if(fread(image_buffer,1,8,decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }
        if(decode_byte_from_lsb(image_buffer,&c) != e_success)
        {
            return e_failure;
        }
        file_extn[i] = c;
    }
    file_extn[size] = '\0';

    return e_success;

}

//decoding secret file extension size 
Status decode_secret_file_extn_size(int *extn_size,DecodeInfo *decInfo)
{
   unsigned char buffer[32];
   if(fread(buffer,1,32,decInfo->fptr_stego_image) != 32)
   {
    return e_failure;
   }

    return decode_size_from_lsb(extn_size,buffer);

}
// finding the secret file size
Status decode_file_size(int *file_size,DecodeInfo *decInfo)
{
    unsigned char image_buffer[32];
    if(fread(image_buffer,1,32,decInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }
    return decode_size_from_lsb(file_size,image_buffer);
}

// reading the data from secret file and storing it into the output file
Status decode_secret_file_data(int file_size, DecodeInfo *decInfo)
{
    char *secret_data = malloc(file_size);
    if(!secret_data)
    {
        return e_failure;
    }
    unsigned char image_buffer[8];
    char c;

    for(int i =0;i<file_size;i++)
    {
        if(fread(image_buffer,1,8,decInfo->fptr_stego_image) != 8)
        {
            free(secret_data);
            return e_failure;
        }
        if(decode_byte_from_lsb(image_buffer,&c) != e_success)
        {
            free(secret_data);
            return e_failure;
        }
        secret_data[i] = c;
    }
    if(fwrite(secret_data,1,file_size,decInfo->fptr_output) != (size_t)file_size)
    {
        free(secret_data);
        return e_failure;
    }
    free(secret_data);
    return e_success;
}

// decoding size from LSB
Status decode_size_from_lsb(int *decoded_value,unsigned char *image_buffer)
{
    
    if(!decoded_value || !image_buffer)
    {
        return e_failure;
    }

    *decoded_value = 0;
    for(int i=0;i<32;i++)
    {
        
        *decoded_value = *decoded_value |  ((image_buffer[i] & 1) << i) ;
    }
   
    return e_success;
}
//decoding bytes from LSB
Status decode_byte_from_lsb(unsigned char *image_buffer,char *decoded_char)
{

    *decoded_char = 0;
   
    for(int i=0;i<8;i++)
    {
        int bit = (image_buffer[i] & 1);
        *decoded_char |= bit << i;
        //*decoded_char = *decoded_char |  (bit  << (7-i)) ;
    }
    

    return e_success;
}
//opening the output file
Status open_output_file(DecodeInfo *decInfo)
{
    decInfo->fptr_output = fopen(decInfo->output_fname,"wb");
    if(decInfo->fptr_output == NULL)
    {
        printf("Error opening the file!\n");
        return e_failure;
    }
    return e_success;
    
}