#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"

Status open_files_decode(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_stego = fopen(decInfo->stego_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_fname);

    	return e_failure;
    }

    printf("Stego file opened successfully...\n");
    //printf("Starting stego file offset position at %ld\n", ftell(decInfo->fptr_stego));

    // No failure return e_success
    return e_success;
}


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    //check for argc[2] is .bmp file or not
    if(argv[2] ==NULL)
    {
        printf(".bmp file is not passed");
        return e_failure;
    }

    if((strstr(argv[2], ".bmp")) == NULL)
    {
        printf("Invalid image file name\n");
        return e_failure;
    }

    decInfo->stego_fname = argv[2];
    // store in decInfo -> fname

    if(argv[3] == NULL)
    {
        strcpy(decInfo->dest_fname, "output");
        return e_success;
        //store default name for output file to default
    }

    if(strchr(argv[3], '.' ) != NULL)
    {
        printf("Output file should not contain extension\n");
       return e_failure; 
    }

    strcpy(decInfo->dest_fname, argv[3]);

   // printf("%s", argv[3]);

    return e_success;

    /*
        1.check argv[2] is having ".bmp" extn
        yes-> store argv[2] to decInfo

        2. if argv[3] is NULL
        yes-> store default name for output file to decInfo

        3. check argv[3] is not having '.'
        yes-> store argv[3] to decInfo
        no-> invalid
    */

}

Status do_decoding(DecodeInfo *decInfo)
{
    
    int ret;
    ret = open_files_decode(decInfo);
    if(ret == e_failure)
    {
        printf("File not opened\n");
        return e_failure;
    }

    //printf("file opened successfully\n");

   skip_bmp_header(decInfo->fptr_stego);
    
    if(ftell(decInfo->fptr_stego) != 54)
    {
        printf("Failed to skip BMP header\n");
        return e_failure;
    }

    char magic_string[50];
    char user_mg[50];
    decode_magic_string(decInfo->fptr_stego, magic_string);

    printf("Enter magic string: ");
    scanf("%s", user_mg);

    if(strcmp(magic_string, user_mg) !=0)
    {
        printf("Invalide magic string\n");
        fclose(decInfo->fptr_stego);
        return e_failure;
    }
    printf("Magic string is correct\n");

    int extn_size;
    ret = decode_extn_file_size(decInfo->fptr_stego, &extn_size);
    if(ret == e_failure)
    {
        return e_failure;
    }

    char extn[10];
     ret = decode_extn_file(decInfo->fptr_stego, extn, extn_size);
     if(ret == e_failure)
     {
        return e_failure;
     }

     strcat(decInfo->dest_fname, extn);

     decInfo->fptr_dest = fopen(decInfo->dest_fname, "w");
     if(decInfo->fptr_dest == NULL)
     {
        perror("fopen");
        fclose(decInfo->fptr_stego);
        return e_failure;
     }

     int file_size;
     decode_secret_file_size(decInfo->fptr_stego, &file_size);

     decode_secret_data(decInfo->fptr_stego, decInfo->fptr_dest, file_size);

     fclose(decInfo->fptr_stego);
     fclose(decInfo->fptr_dest);


     return e_success;


    /*
        1.open stego file
        2.call skip_bmp_header(decInfo->fptr_stego)
        3.char magic_string[50];
        4.decode_magic_string(decInfo->fprt_stego, magic_string)
        5.read magic string from user
        6. compare user entered and decoded magic strings are same?
        no-> err: invalid magic string 
                return failure
        7. int extn_size
        8.decode_extn_size(decInfo->fptr_stego, &extn_size)
        9.char extn[10];
        10. decode_extn(decInfo->fptr_stego, extn, extn_size)
        11. concatinate output file name with decoded extn
        12. open output file
        13. int file_size;
        14. decode_sec_file_size(decInfo->stego, &file_size)
        15. decode_sec_data(decInfo->fptr_stego, decInfo->fptr_dest, file_size)
        16. close files
        17. return success
    */
}


Status skip_bmp_header(FILE *fptr_stego)
{
    fseek(fptr_stego, 54, SEEK_SET);

    // printf("after  skip bmp header offset is at %ld\n", ftell(fptr_stego));

    printf(".bmp header skipped successfully...\n");
    return e_success;
}

char lsb_to_byte(char *buffer) // it takes only buffer and returns the decode data of 8 bytes
{
    
  char data = 0;
    for (int i = 0; i < 8; i++)
    {
        char fetch_bit;
        fetch_bit = buffer[i] & 1;
        fetch_bit = fetch_bit << (7-i);
        data = data | fetch_bit;
    }

    return data;
    /*
    1. run a loop for 8 times
      get lsb of each index of buffer and store to ith position of data variable
      return data
    */
}

int lsb_to_size(char *buffer)
{
    int data = 0;

    for (int i = 0; i < 32; i++)
    {
        int fetch_bit;
        fetch_bit = buffer[i] & 1;        
        fetch_bit = fetch_bit << (31 - i);     
        data = data | fetch_bit;         
    }
    return data;
}


Status decode_magic_string(FILE *stego, char *magic_string)
{
    char temp[8];
    //printf(" Before magic string decoding offset position is at %ld\n", ftell(stego));

    for(int i =0; i<2; i++)
    {
        fread(temp, 1, 8, stego);
        magic_string[i] = lsb_to_byte(temp);   
    }

    magic_string[2] = '\0';

    printf("Magic string decoded successfully...\n");
    //printf("After magic string decoding offset position is at  %ld\n", ftell(stego));

    return e_success;

    //declare one empty array --> char magic_string[50];
     /*
    for(i=0; i<2; i++)
    {
        1. read 8 bytes buffer from stego file
            char temp[8];
        2. magic_string[i] = lsb_to_byte(temp)
    }
    */

  
  return e_success;
}


Status decode_extn_file_size(FILE *stego, int *extn_size)
{
    //printf("before 'extn file size' decoding  offset position is at  %ld\n", ftell(stego));
     
    char buffer[32];
     fread(buffer, 1,32, stego);

     *extn_size = lsb_to_size(buffer);

     printf("Entension size decoded successfull!...\n");
     //printf("%d\n", *extn_size);
      //printf("After 'extn file size' decoding offset position is at %ld\n", ftell(stego));

     return e_success;
 /*
    1.read 32 bytes buffer
    2.*extn_size = lsb_to_size(buffer)     */

} 

Status decode_extn_file(FILE *stego, char *extn, int extn_size)
{

    //printf("Beofre extension decoding offset position is at %ld\n", ftell(stego));
    char temp[8];
    int i;

    for(i =0; i<= extn_size-1; i++)
    {
        fread(temp, 8, 1, stego);
        extn[i] = lsb_to_byte(temp);
    }

    extn[i] = '\0';

    printf("File Extension decoded successfully!....\n");
    //printf("%s\n", extn);
    //printf("After extension decoding offset position is at %ld\n", ftell(stego));

    return e_success;
    /*
    run a loop for 0 to extn_size-1
    1. read 8 bytes temp buffer;
    2. extn[i] = lsb_to_byte(temp)
    
    */
}


Status decode_secret_file_size(FILE *stego, int *file_size)
{
    //printf("Beofre secret file size decoding offset position is at %ld\n", ftell(stego));

    char temp[32];
    fread(temp, 1, 32, stego);
    *file_size = lsb_to_size(temp);


    printf("File size decoded successfully!.....\n");
    //printf("%d\n", *file_size);
    //printf("Beofre secret file size decoding offset position is at %ld\n", ftell(stego));

    return e_success;
     /*
    1.read 32 bytes buffer
    2.*fie_size = lsb_to_size(buffer);
    */

} 

Status decode_secret_data(FILE *stego, FILE *dest, int file_size)
{
    char buffer[8];
    char ch;
    //printf("Beofre secret data decoding offset position is at %ld\n", ftell(stego));

    for(int i=0; i< file_size; i++)
    {
        fread(buffer, 1, 8, stego);
        ch = lsb_to_byte(buffer);
        fwrite(&ch, 1,1, dest);
    }

    printf("Secret file data decoded successfully...\n");
    //printf("Beofre secret data decoding offset position is at %ld\n", ftell(stego));

    return e_success;
     /*
        run a loop for file_size times 
        1.read 8 byte temp buffer;
        2.char ch =lsb_to_byte(temp_buffer);
        3. write ch to dest file
    */

}




















