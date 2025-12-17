#ifndef DECODE_H
#define DECODE_H

#include "types.h"  // contains user defined types
//#include "encode.h"

typedef struct _DecodeInfo
{
    char *stego_image_fname;
    char *secret_fname;
    char *output_fname;

    FILE *fptr_stego_image;
   // FILE *fptr_secret;
    FILE *fptr_output;

    char magic_string[50];
    uint size_secret_file;
}DecodeInfo;

/*Decoding Function Prototype*/

/*Read and validate decoding arguements */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/*Perform the Decoding*/
Status do_decoding(DecodeInfo *decInfo);

/*Get file pointers for i/p and o/p files*/
Status open_files_for_decoding(DecodeInfo *decInfo);

/*Decode magic string*/
Status decode_magic_string(char* magic_string,DecodeInfo *decInfo);

/*Decode a byte into LSB of image data array */
// Pass image_buffer (8 bytes) and a pointer to store decoded char
Status decode_byte_from_lsb(unsigned char *image_buffer, char *decoded_char);

/*Decode size (32 bit integer) from LSB*/
Status decode_size_from_lsb(int *decoded_value,unsigned char *image_buffer);

/*Decode secret file extension size*/
Status decode_secret_file_extn_size(int *extn_size,DecodeInfo *decInfo);

/*Decode secret file extension*/
Status decode_secret_file_extn(char * file_extn, int size, DecodeInfo *decInfo);

/*Decode file size*/
Status decode_file_size(int *file_size,DecodeInfo *decInfo);

/*Decode secret file data*/
Status decode_secret_file_data(int file_size, DecodeInfo *decInfo);


/*Deode size of data to LSB*/
// Status decode_size_to_lsb(char *decoded_value,char *image_buffer,DecodeInfo *decInfo);


/*Open output file*/
Status open_output_file(DecodeInfo *decInfo);


#endif