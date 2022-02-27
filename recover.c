
#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>
#include <stdint.h>
#include <ctype.h>

typedef uint8_t byte;

bool isjpg(byte *block);

int main(int argc, char *argv[])
{

    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: recover forensicfilename.raw\n");
        return 1;
    }


    // open input file
    FILE *inptr = fopen(argv[1], "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 2;
    }

    // Declare output file buffer ?
    FILE *JPG;

    // Declare temporary storage (buffer) for block.
    byte block[512];

    // bool to see if we've found any jpgs yet
    bool foundjpg = false;
    int readblock;
    int jpgnum = 0;



    while (fread(block, sizeof(block), 1, inptr) == 1)
    {

        // If we found a header
        if (isjpg(block))
        {

            // Filename will be three digits + .jpg and a null character
            char filename[8];

            // filename is defined by jpgnum which only iterates when new jpgheader is found.
            sprintf(filename, "%03.3i.jpg", jpgnum);

            // For debugging
            printf("processing file: %s\n", filename);

            if (foundjpg == true)
            {
                // Already jpg in buffer. Close/finish it.
                fclose(JPG);
            }
            else
            {
                // First jpg! Set foundjpg to true.
                foundjpg = true;
            }

            // Open new and write first block in it
            JPG = fopen(filename, "w");
            fwrite(block, sizeof(block), 1, JPG);


            jpgnum++;

        }
        // If we didn't find a header
        else
        {
            // Are we already working on a jpg?
            if (foundjpg == true)
            {
                // This is not a header and there is already a jpg, so write this block to current jpg.
                fwrite(block, sizeof(block), 1, JPG);

            }
            // We only land here if we're in the beginning and still haven't found a header.
            // We check again. If there happen to be zero headers in the whole file, loop exits and file closes.

        }

    }



    fclose(inptr);
    fclose(JPG);




}

bool isjpg(byte *block)
{
    if (block[0] == 0xff &&
        block[1] == 0xd8 &&
        block[2] == 0xff &&
        (block[3] & 0xf0) == 0xe0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
