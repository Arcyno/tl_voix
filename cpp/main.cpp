#include "frame.h"
#include <sndfile.h>



int main(int argc, char *argv[]) {

SF_INFO sfinfo;
SNDFILE* infile;
const char* fileName = "../test3/adroite.wav";

infile = sf_open(fileName, SFM_READ, & sfinfo );



//if (! (infile = sf_open_read (fileName, &sfinfo))){   /* Open failed so print an error message. */
//    printf ("Not able to open input file %s.\n", "input.wav") ;
//    sf_perror (NULL) ;
//    return  1 ;
//} 



int ordre_lpc = 10;
int nb_mfcc = 40;



return -1;
}
