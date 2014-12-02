#include "frame.h"
#include <sndfile.h>

int main(int argc, char *argv[]) {



if (! (infile = sf_open_read ("input.wav", &sfinfo))){   /* Open failed so print an error message. */
    printf ("Not able to open input file %s.\n", "input.wav") ;
    sf_perror (NULL) ;
    return  1 ;
} ;

return -1;
}
