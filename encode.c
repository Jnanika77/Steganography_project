#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

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

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
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

   // printf("Start scr position %ld,start stego position %ld\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));

    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2] == NULL)
    {
        printf("file with .bmp is not passed\n");
        return e_failure;
    }

    if(strstr(argv[2], ".bmp")== NULL)
    {
        printf("Enter invalid image file name\n");
        return e_failure;
    }

    encInfo->src_image_fname = argv[2];

    if(argv[3] == NULL)
    {
        printf("Enter .txt file not passed\n");
        return e_failure;
    }

    if(strstr(argv[3], ".txt") == NULL)
    {
        printf("Enter invalid sec file name\n");
        return e_failure;
    }

    encInfo->secret_fname = argv[3];

    if(argv[4] == NULL)
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    if(strstr(argv[4], ".bmp"))
    {
        encInfo->stego_image_fname = argv[4];
    }

    char *chr;
    chr = strchr(encInfo->secret_fname, '.');
    strcpy(encInfo->extn_secret_file, chr);

    printf("\n%s\n", encInfo->extn_secret_file);
    return e_success;

    /*
        1.check argv[2] == NULL
        yes-> print .bmp file not passed
                return failure
        2.check strstr(argv[2], ".bmp") == NULL
        yes->   print invalid image file name
                return failure
    
        3. encInfo->src_image_fname = argv[2];
        4. check argv[3] == NULL
        yes-> print .txt file not passed
                return failure
        5. check strstr(argv[3], ".txt") == NULL
        yes-> print invalid sec file name
                return failure
        6. encInfo->sec_fname = argv[3]
        7. check argv[4] == NULL
        yes-> encInfo->stego_image_fname = "stego.bmp"
        no->  validate and store to encInfo->stego_image_fname = argv[4]
        8. copy sec file extn to encInfo->extn_sec_file array
            1. char *chr = strchar(encInfo->sec_fname, '.')
            2. strcpy(encInfo->extn_sec_file, chr);
        9. return success

        */

        return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    int ret;
    ret = open_files(encInfo);
    if(ret == e_failure)
    {
        printf("File failed to open");
        return e_failure;
    }
    
    ret = check_capacity(encInfo);
    if(ret == e_failure)
    {
        return e_failure;
    }

    copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);

   ret = encode_magic_string(MAGIC_STRING, encInfo);
   if(ret == e_failure)
   {
        return e_failure;
   }

    ret = encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo);
   if(ret == e_failure)
   {
    return e_failure;
   }

   ret = encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
   if(ret == e_failure)
   {
        return e_failure;
   }

   int sec_file_size;

   fseek(encInfo->fptr_secret, 0, SEEK_END);
   sec_file_size= ftell(encInfo->fptr_secret);
   rewind(encInfo->fptr_secret);


   ret = encode_secret_file_size(sec_file_size, encInfo);
   if(ret == e_failure)
   {
    return e_failure;
   }

   ret = encode_secret_file_data(encInfo);
   if(ret == e_failure)
   {
    return e_failure;
   }

   ret = copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image);
   if(ret == e_failure)
   {
    return e_failure;
   }

    return e_success;
    /*
         1.ret = open_files(encInfo)
        2. check ret == failure
        yes-> print open file failed
                return failure
        3.ret = check_capacity(encInfo);
        4.check ret == failure
                return failure
        5.copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image);
        6.encode_magic_string(MAGIC_STRING, encInfo);
        7.encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo);
        8.encode_secret_file_extn(encInfo->extn_secret_file, encInfo);
        9. cal sec_file_size --> make sure to set offset back to first byte
        9.encode_secret_file_size(sec_file_size, encInfo);
        10.encode_secret_file_data(encInfo);
        11.copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);
        12.close 3 files
        13. return success

        
    */

    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
    //printf("Start scr position %ld", ftell(encInfo->fptr_src_image));


    int total_size_required;
    int image_file_size;
    int sec_size;

    int extn_size = strlen( encInfo->extn_secret_file);
    image_file_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    fseek(encInfo->fptr_src_image, 0, SEEK_SET);


    fseek(encInfo->fptr_secret, 0, SEEK_END);
    sec_size = ftell(encInfo->fptr_secret);
    //rewind(encInfo->fptr_secret);
    fseek(encInfo->fptr_secret, 0, SEEK_SET);


    total_size_required = (strlen(MAGIC_STRING) + 
                          sizeof(int) + 
                          strlen(encInfo->extn_secret_file) + 
                          sizeof(int) + 
                          sec_size)*8 + 54;

    //printf("After scr position %ld\n,After stego position %ld\n", ftell(encInfo->fptr_src_image), ftell(encInfo->fptr_stego_image));


    if(total_size_required <= image_file_size)
    {
        return e_success;
    }

    return e_failure;

    /*
        1.calculate no. of bytes needed for encoding from src file
            count = (magic_str_len +
            extn_size(int) + 
            extn_len + 
            file_size(int) + 
            file_data_len ) * 8 + 54

        2. check count <= src_file_size
        yes-> return sucess
        no-> return failure
    */

    return e_success;
}


Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    //printf("before scr position %ld\n, before stego position %ld\n", ftell(fptr_src_image), ftell(fptr_stego_image));


    char temp[54];
    fread(temp, 54, 1, fptr_src_image);
    fwrite(temp, 54, 1, fptr_stego_image);

    return e_success; 
    
    
    /*
        char *temp[55];
        fread(fptr_src_image, 54, 1, temp);

        fwrite(fptr_dest_image, 54, 1 temp);

        return success
    */
}

Status encode_byte_to_lsb( char data, char *image_buffer)
{
    int i;
    for(i =0; i< 8; i++)
    {
        char mask, bit;
        mask = 1<< (7-i);
        bit = data & mask;
        image_buffer[i] = image_buffer[i] & 0xFE;

        bit = (unsigned)bit >> (7-i);

        image_buffer[i] = image_buffer[i] | bit;  
    }

   return e_success;
}

Status encode_size_to_lsb(int data1, char *image_byte)
{
    for(int i =0; i< 32; i++)
    {
        char mask, bit;
        mask = 1<< (31-i);
        bit = data1 & mask;
        image_byte[i] = image_byte[i] & 0xFE;

        bit = (unsigned)bit >> (31-i);

        image_byte[i] = image_byte[i] | bit;
    }

   return e_success;
}




Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    // printf("Offset pos Before encoding magic string is at  %ld\n in src ", ftell(encInfo->fptr_src_image));
    // printf("Offset pos Before encoding magic string is at %ld\n in stego", ftell(encInfo->fptr_stego_image));
    char temp[8];

    for(int i = 0; magic_string[i] != '\0'; i++)
    {
        fread(temp, 1, 8,encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], temp);
        fwrite(temp, 1, 8, encInfo->fptr_stego_image);
    }

   // printf("\n%x\n", magic_string);

    // printf("Offset pos After encoding magic string is at %ld\n in src", ftell(encInfo->fptr_src_image));
    // printf("Offset pos After encoding magic string is at %ld\n in stego", ftell(encInfo->fptr_stego_image));
    printf("Encoding magic string is done\n");

    /*
        1. for (i=1; i<=2; i++)
        {
            char temp[8];
            1.read 8 bytes buffer from src file and store to temp array
            2.byte_to_lsb(magic_string[i], temp);
            3.write temp array 8 bytes to stego file
        }
    */

    return e_success;
}

Status encode_secret_file_extn_size(int file_extn_size, EncodeInfo *encInfo)
{
  // printf(" Offset position before encoding 'encode_secret_file_extn_size' extension size scr is at %ld\n", ftell(encInfo->fptr_src_image));
  // printf(" Offset position before encoding 'encode_secret_file_extn_size' extension stego size is at %ld\n", ftell(encInfo->fptr_stego_image));
   
    char temp_buffer[32];

    fread(temp_buffer, 1, 32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_extn_size,temp_buffer);
    fwrite(temp_buffer, 1, 32, encInfo->fptr_stego_image);
    printf("Encoding extension size of secret file is done.....\n");
    //printf("\n%hhx\n", temp_buffer);

  // printf("Offset position After encoding 'encode_secret_file_extn_size' extension scr size is at %ld\n", ftell(encInfo->fptr_src_image));
  // printf("Offset position After encoding 'encode_secret_file_extn_size' extension stego size is at %ld\n", ftell(encInfo->fptr_stego_image));
    /*
        char temp_buffer[32];
        1.read 32 bytes buff from src file
        2.call size_to_lsb(file_extn_size, temp_buffer)
        3.write temp_buffer to stego file
    */
    return e_success;

}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
   // printf("Offset position before encoding 'encode_secret_file_extn' extension scr is at %ld\n", ftell(encInfo->fptr_src_image));
   // printf("Offset position before encoding 'encode_secret_file_extn' extension stego is at %ld\n", ftell(encInfo->fptr_stego_image));

    for(int i =0; i< strlen(file_extn); i++)
    {
        char temp[8];
        fread(temp, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], temp);
        fwrite(temp, 1, 8, encInfo->fptr_stego_image);

    }

    //printf("Offset position After encoding 'encode_secret_file_extn' extension scr is at %ld\n", ftell(encInfo->fptr_src_image));
   // printf("Offset position After encoding 'encode_secret_file_extn' extension stego is at %ld\n", ftell(encInfo->fptr_stego_image));

    printf("Encoding secret file extension is done ....\n");
    /*
               1.run a loop 0 to file_extn_len-1 times
        {
            1.char temp[8];
            2.read 8 bytes of buffer from src file
            3.call byte_to_lsb(file_extn[i], temp);
            4. write 8 bytes temp to stego file
        }  
    */

    return e_success;
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    //printf("\nOffset position before encoding 'encode_secret_file_size' secret file size of  scr is at %ld\n", ftell(encInfo->fptr_src_image));
    //printf("Offset position before encoding 'encode_secret_file_size'secret file size of stego is at %ld\n", ftell(encInfo->fptr_stego_image));

    char temp[32];
    fread(temp, 1,32, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, temp);
    fwrite(temp, 1,32,encInfo->fptr_stego_image);

   // printf("Offset position After encoding secret file size of  scr is at %ld\n", ftell(encInfo->fptr_src_image));
    //printf("Offset position After encoding'encode_secret_file_size' secret file size of stego is at %ld\n", ftell(encInfo->fptr_stego_image));

    printf("Encoding secret file size is Done....\n");
    /*
        1. char temp[32];
        2. read 32 bytes of buff from src file
        3. cal size_to_lsb(file_size, temp);
        4. write temp to stego file
    */

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //printf("\n%ld", ftell(encInfo->fptr_secret));
    //printf("\nOffset position before encoding'encode_secret_file_data' secret file size of  scr is at %ld\n", ftell(encInfo->fptr_secret));
    //printf("Offset position before encoding 'encode_secret_file_data'secret file size of stego is at %ld\n", ftell(encInfo->fptr_stego_image));

   char temp[8];
   char ch;
   while((ch = fgetc(encInfo->fptr_secret)) != EOF)
   {
        fread(temp, 1, 8, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, temp);
        fwrite(temp, 1, 8, encInfo->fptr_stego_image);
   }

//    printf(" \nOffset position After encoding 'encode_secret_file_data' secret file size of  scr is at %ld\n", ftell(encInfo->fptr_secret));
//     printf("Offset position After encoding 'encode_secret_file_data' secret file size of stego is at %ld\n", ftell(encInfo->fptr_stego_image));
// printf("\n After encoding secret data %ld", ftell(encInfo->fptr_secret));
    printf("Encoding secret data is done....\n");
    /*
        1. run a loop until sec_file reaching EOF
        {
            1. read a ch from sec file
            2. read 8 bytes buff from src file
            3.call byte_to_lsb(ch, temp)
            4.write temp to stego file
        }
    */
   return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // printf("\nOffset position After encoding 'copy_remaining_img_data' secret file size of  scr is at %ld\n", ftell(fptr_src_image));
    // printf("Offset position After encoding 'copy_remaining_img_data' secret file size of stego is at %ld\n", ftell(fptr_stego_image));

    int ch;
    while((ch = fgetc(fptr_src_image ))!= EOF)
    {
        fwrite(&ch, 1, 1, fptr_stego_image);
    }

    // printf("\nOffset position After encoding 'copy_remaining_img_data' secret file size of  scr is at %ld\n", ftell(fptr_src_image));
    // printf("Offset position After encoding 'copy_remaining_img_data' secret file size of stego is at %ld\n", ftell(fptr_stego_image));

    printf("Encoding remaining image data is Done...\n");
    /*
        1. run a loop until src file is reaching EOF
        {
            1. read a 1 byte from src file
            2.write 1 byte to dest file
        }
    */
   return e_success;
}



