#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[]) 
{
    // char *argv[] is array of character pointers

    EncodeInfo encInfo;
    DecodeInfo decInfo;

    int ret = check_operation_type(argv);

    if(ret == e_unsupported)
    //yes -> "Error: no arguments/ invalid"
    {
        printf("Error: Invalid argument\n");
        return e_unsupported;
    }


    if(ret == e_encode)
    {
        //encoding
        int ret = read_and_validate_encode_args(argv, &encInfo);
        if(ret == e_failure)
        {
            printf("invalid args\n");
            return e_failure;
        }

        //start encoding 
        ret = do_encoding(&encInfo);
        if(ret ==e_failure)
        {
            printf("encoding failed");
            return e_failure;
        }
        else{
             
            printf("Encoding successful\n");
            return e_success;
        }

    }

    if(ret == e_decode)
    {
        //decoding
           int ret = read_and_validate_decode_args(argv, &decInfo);
           if(ret == e_failure)
           {
                printf("invalid args\n");
                return e_failure;
           }

           //start decoding 
           ret = do_decoding(&decInfo);
           if(ret == e_failure)
           {
            printf("decoding failed\n");
            return e_failure;
           }
           else
           {
            printf("Decoding successful\n");
            return e_success;
           }

    }
    
}

OperationType check_operation_type(char *argv[])
{

        if(argv[1] == NULL)
        {
            return e_unsupported;
        }

        if(strcmp(argv[1],"-e") == 0)
        {
            return e_encode;
        }

        if(strcmp(argv[1], "-d") == 0)
        {
            return e_decode;
        }

        else
        {
            return e_unsupported;
        }
        //type check is common for decode and encode 
        /*
        1.check argv[1] == NULL
            yes -> return e_unsupported

        2.(strcmp(argv[1], "-e") == 0)  //check argv[1] for -e or -d for encode or decode
            yes-> return e_encode
        
        3.(strcmp(argv[1], "-d") ==0)
            yes-> return e_decode

        4.return e_unsupported
        */
}
