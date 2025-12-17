#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2] == NULL)
    {
        printf("bmp is not provided\n");
        return e_failure;
    }
    if(strstr(argv[2],".bmp") != NULL)
    {
        printf("beautiful.bmp is present\n");
        encInfo -> src_image_fname = argv[2];
    }
    else{
        printf("beautiful.bmp is not present\n");
        return e_failure;
    }
    if(argv[3] == NULL)
    {
        printf("txt is not provided\n");
        return e_failure;
    }
    if(strstr(argv[3],".txt") != NULL)
    {
        printf("secret.txt is present\n");
        encInfo -> secret_fname = argv[3];
    }
    else{
        printf("secret.txt is not present\n");
        return e_failure;
    }
    if(argv[4] == NULL)
    {
        printf("bmp is not provided\n");
        return e_failure;
    }
    if(strstr(argv[4],".bmp") != NULL)
    {
        printf("stego.bmp is present\n");
        encInfo -> stego_image_fname = argv[4];
    }
    else{
        printf("stego.bmp is not present\n");
        encInfo ->stego_image_fname = "stego.bmp";
    }
    return e_success; 
}
Status do_encoding(EncodeInfo *encInfo)
{   
    //  Opening files
    if(open_files(encInfo) == e_success)
    {
        printf("All files are opened successfully\n");
    }
    else
    {
        printf("All files are not opened successfully\n");
        return e_failure;
    }

    //Add the empty file check here
    fseek(encInfo->fptr_secret, 0, SEEK_END);
    long secret_size = ftell(encInfo->fptr_secret);
    
    if (secret_size == 0)
    {
        printf("Warning: Secret file is empty. Nothing to encode.\\n");
        return e_failure;
    }
    rewind(encInfo->fptr_secret);

    //   Checking capacity
    if(check_capacity(encInfo) == e_success)
    {
        printf("Image has enough Capacity\n");
    }
    else{
        printf("Image has not enough Capacity\n");
    }

    //  Copying bmp header file
    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
    {
        printf("BMP header file copied\n");
    }
    else{
        printf("BMP header file is not copied\n");
        return e_failure;
    }

    // entering magic string from user
    if(input_magic_string_from_user(encInfo) == e_success)
    {
        printf("Magic string entered\n");
        encInfo->magic_string_size = strlen(encInfo->magic_string);// magic string size
    }
    else
    {
        printf("Magic string entering is  unsuccessful\n");
        return e_failure;

    }

    // Encode magic string data
    if(encode_magic_string(encInfo->magic_string,encInfo) == e_success)
    {
        printf("Magic string encoded successfully\n");
    }
    else
    {
        printf("Magic string encoding is unsuccessful\n");
        return e_failure;
    }

    
    char *file_extn = strstr(encInfo ->secret_fname,"."); // getting the secret file extension
    // Encode secret file extension size
    if( encode_secret_file_extn_size(file_extn,encInfo) == e_success)
    {
        printf("secret file extension size encoded successfully\n");
    }
    else
    {
        printf("secret file extension size encoding unsuccessful\n");
        return e_failure;
    }
    // Encode secret file extension
    if(encode_secret_file_extn(file_extn,encInfo) == e_success)
    {
        printf("secret file extension encoded successfully\n");
    }
    else
    {
        printf("secret file extension encoding unsuccessful\n");
        return e_failure;
    }

    // Encode secret file size
    uint file_size = get_file_size(encInfo->fptr_secret); // getting secret file size
    if(encode_secret_file_size(file_size,encInfo) == e_success)
    {
        printf("secret file size encoded successfully\n");
    } 
    else
    {
        printf("secret file size encoding unsuccessful\n"); 
        return e_failure;
    }

    // Encode secret file data
    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("secret file data encoded successfully\n");
    }
    else
    {
        printf("secret file data encoding unsuccessful\n");
        return e_failure;
    }

    // Copy remaining image bytes from src to stego image after encoding 
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
    {
        printf("All the  remaining image data copied successfully\n");
    }
    else
    {
        printf("All the  remaining image data coping is unsuccessful\n");
        return e_failure;
    }

    //closing all the files opened.
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;
}
Status open_files(EncodeInfo *encInfo)
{
    encInfo -> fptr_src_image = fopen(encInfo ->src_image_fname,"rb"); // opening source file
    if(encInfo -> fptr_src_image == NULL)
    {
        printf("Source file is not present\n");
        return e_failure;
    }
    else{
        printf("Source file is opened\n");
    }

    encInfo ->fptr_secret = fopen(encInfo ->secret_fname,"rb"); // opening secret file
    
    if(encInfo ->fptr_secret == NULL)
    {
        printf("Secret file is not present\n");
        return e_failure;
    }
    else{
        printf("Secret file is opened\n");
    }
    
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb"); // opening stego file

    if(encInfo ->fptr_stego_image == NULL)
    {
        printf("Stego file is not present\n");
        return e_failure;
    }
    else{
        printf("Stego file is opened\n");
    }
    return e_success;
}

// Get image size 
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width,height,image_size;

    fseek(fptr_image,18,SEEK_SET); // pointing to the width data position
    fread(&width,4,1,fptr_image);  // reading the width size

    fseek(fptr_image,22,SEEK_SET); // pointing to the height data position
    fread(&height,4,1,fptr_image);  // reading the height data

    image_size = (width * height * 3); // image size = w * h * 3

    fseek(fptr_image,0,SEEK_SET); // pointing back to the 0th position

    return image_size;
}

// Get file size 
uint get_file_size(FILE *fptr)
{
    uint file_size;

    fseek(fptr,0,SEEK_END); //pointing to the end position
    file_size = (uint)ftell(fptr); // getting the file size(ie, end position gives the file size)
    fseek(fptr,0,SEEK_SET); // pointing back to 0th position

    return file_size;

}

// check capacity 
Status check_capacity(EncodeInfo *encInfo)
{
    uint image_size,file_size,required_bytes;

    image_size = get_image_size_for_bmp(encInfo->fptr_src_image); // getting image size(source file)
    file_size = get_file_size(encInfo->fptr_secret);              // getting secret file size

    encInfo ->image_capacity = image_size ; // image capacity = image size
    char *file_extn = strchr(encInfo->secret_fname,'.');

    required_bytes = (32/8) + strlen(encInfo->magic_string)*8 + (32/8) + strlen(file_extn)*8 + (32/8)  + file_size*8;  // required bytes

    // checking capacity
    if(encInfo->image_capacity >= required_bytes)
    {
        return e_success;
    }

    return e_failure;
}

// Copy bmp image header 
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    unsigned char header[54]; //   Array to hold the header (54 bytes)

    // Read 54 bytes from source BMP
    //fptr_src_image = fopen(encInfo ->src_image_fname,"rb");
    if(fread(header,sizeof(header),1,fptr_src_image) != 1){
        printf("BMP header file is not copied from source file\n");
        return e_failure;
    } 

    // write 54 bytes to destination BMP
    //fptr_dest_image = fopen(encInfo ->stego_image_fname,"wb");
    if(fwrite(header,sizeof(header),1,fptr_dest_image)!=1)
    {
        printf("BMP header file is not copied to destination file\n");
        return e_failure;
    }
    
    return  e_success;
}

// Input the magic string from user
Status input_magic_string_from_user(EncodeInfo *encInfo)
{
    char magic[50];
    //Reading Magic string from user
    printf("Enter the Magic string:");
    if(fgets(magic,sizeof(magic),stdin) == NULL)
    {
        printf("Error reading magic string\n");
        return e_failure;
    }

    magic[strcspn(magic, "\n")] = '\0';  // remove newline
    
    strcpy(encInfo->magic_string,magic);

    uint len_mg = strlen(encInfo->magic_string); // reading the size of magic string

    if (strlen(magic) >= sizeof(encInfo->magic_string))
    { 
        printf("Magic string too long \n");
        return e_failure;
    }

    //encoding length of magic string
    unsigned char buffer[32];

    // reading the 32 bits  from source file
    if(fread(buffer,1,32,encInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }
    encode_size_to_lsb(len_mg,buffer); // encoding size of the magic string before storing magic string in file 
   
    // writing the 32 bits to stego file
    if(fwrite(buffer,1,32,encInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }


    return e_success;
}

// Store Magic String (encode)
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{ 
    int len = strlen(encInfo->magic_string);  // size of magic string


    encode_data_to_image(encInfo->magic_string,len ,encInfo->fptr_src_image,encInfo->fptr_stego_image);
   
    return e_success;

}

//Encode function, which does the real encoding 
Status encode_data_to_image(char* data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    unsigned char image_buffer[8];

    for(int i=0;i<size;i++){

    fread(image_buffer,1,8,fptr_src_image); // reading data from source file
    encode_byte_to_lsb(data[i],image_buffer); // encoding
    fwrite(image_buffer,1,8,fptr_stego_image); // writing the data to the output file

    }
    return e_success;
}

// Encode a byte into LSB of image data array
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        int bit =  (data >> i)&1;

        image_buffer[i] = (image_buffer[i] &(~1)) | bit ;
    }

    return e_success;
}

/* Encode secret file extenstion size*/
Status encode_secret_file_extn_size(const char *file_extn, EncodeInfo *encInfo)
{
    int  len = strlen(file_extn); // size of file extension
    unsigned char arr[32];

    if(fread(arr,1,32,encInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }

    encode_size_to_lsb(len,arr);

    if(fwrite(arr,1,32,encInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    return e_success;
}


// Encode secret file extenstion 
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
   int size = strlen(file_extn); // getting file size
   encode_data_to_image((char *)file_extn,size,encInfo->fptr_src_image,encInfo->fptr_stego_image);

   return e_success;

}


// Encode secret file size 
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{   
    
    unsigned char arr[32];
    

    if(fread(arr,1,32,encInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }

    encode_size_to_lsb((int)file_size,arr);

    if(fwrite(arr,1,32,encInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    return e_success;
}


/*Encode size of data to lsb */
Status encode_size_to_lsb(int data,unsigned char *image_buffer)
{
    for(int i=0;i<32;i++)
    {
        int bit =  ((data >> i ) & 1);

        image_buffer[i] = (image_buffer[i] &(~1)) | bit ;

    }
    return e_success;
}


// Encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    fseek(encInfo->fptr_secret,0,SEEK_SET); // setting the position of pointer to 0th position

    char *secret = malloc(encInfo->size_secret_file); 
    if(!secret)
    {
        return e_failure;
    }

    fread(secret,1,encInfo->size_secret_file,encInfo->fptr_secret);
    if(encode_data_to_image(secret,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image) != e_success)
    {
        free(secret);
        return e_failure;
    }
    free(secret);
    return e_success; 
}

// Copy remaining image bytes from src to stego image after encoding 
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    long current = ftell(fptr_src); // Current position 
    fseek(fptr_src,0,SEEK_END); //--move to the end position--
    long end = ftell(fptr_src); // End position
    long remaining = end - current; // Get the remaining data size
    if(remaining <=0)
    {
        return e_success;
    }
    char *image_buffer = (char*)malloc(remaining);
    if(!image_buffer)
    {
        return e_failure;
    }
    fseek(fptr_src,current,SEEK_SET);
    if(fread(image_buffer,1,remaining,fptr_src) != remaining)
    {
        free(image_buffer);
        return e_failure;
    }
    if(fwrite(image_buffer,1,remaining,fptr_dest) != remaining)
    {
        
        return e_failure;
    }
    free(image_buffer);

    return e_success;
}

