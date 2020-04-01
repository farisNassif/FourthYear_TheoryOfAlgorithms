// Author :     Faris Nassif
// Module :     Theory Of Algorithms
// Summary:     A program that executes a MD5 Hash on a given input
//              This program has been adapted based on the process outlined in https://tools.ietf.org/html/rfc1321

#include <stdio.h> // Input/Output
#include <stdint.h> // Req for uint(x) unsigned int
#include <inttypes.h> // Includes formatters for output

/* 
    https://tools.ietf.org/html/rfc1321 => Page 2

    Definitions of a Word and Byte
*/
#define WORD uint32_t
#define BYTE uint8_t

/*
    [Rotate Function]
    =>  Rotates (x) left by (n) bits
*/
#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* 
    https://tools.ietf.org/html/rfc1321 => Page 4
    http://www.boblandstrom.com/the-md5-hashing-algorithm/

    Auxillary function definitions. Each recieve three 'Words'
    and produce one 'Word' as their output
*/
#define F(x,y,z) ((x & y) | (~x & z)) // XY v not(X) Z
#define G(x,y,z) ((x & z) | (y & ~z)) // XZ v Y not(Z)
#define H(x,y,z) (x ^ y ^ z)          // X xor Y xor Z
#define I(x,y,z) (y ^ (x | ~z))       // Y xor (X v not(Z))

/* 
    https://tools.ietf.org/html/rfc1321 => Page 10
    
    [FF, GG, HH, II] => Transformations for rounds 1, 2, 3, and 4
    The first 4 Paramaters for each function are the four 16 bit Words
    The fifth Paramater consists of the union block message (Input for MD5 is 64 bytes / 16 x 32 bit)
    The sixth Paramater contains one of the constants for the MD5 transform (SXX)
    The final Paramater is the corresponding constant T defined below
*/
#define FF(a,b,c,d,m,s,t) { a += F(b,c,d) + m + t; a = b + ROTL(a,s); }
#define GG(a,b,c,d,m,s,t) { a += G(b,c,d) + m + t; a = b + ROTL(a,s); }
#define HH(a,b,c,d,m,s,t) { a += H(b,c,d) + m + t; a = b + ROTL(a,s); }
#define II(a,b,c,d,m,s,t) { a += I(b,c,d) + m + t; a = b + ROTL(a,s); }

/* 
    The four constant arrays below [AA, BB, CC, DD] represent 
    the first four paramaters for the above transformation functions.

    For example, FF will be performed 16 times, GG 16 times and so on,
    The first time FF will be performed, it's first paramater (a) will be the first
    index of AA. So FF(AA[0], BB[0], CC[0], DD[0]), then FF(AA[1], BB[1], CC[1], DD[1])
    and then once, then it'll be GG(AA[15], BB[15], CC[15], DD[15]) and so on.

    It's kind of a chunky way of doing it, would probably be possible with a multi-dimensional
    array, however this way works and allow for the four hash rounds to be performed efficiently
    in a loop.
*/
const WORD AA[] = {
    0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1,
    0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1,
    0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1,
    0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1
};

const WORD BB[] = {
    1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2,
    1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2,
    1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2,
    1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2
};

const WORD CC[] = {
    2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3,
    2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3,
    2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3,
    2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3
};

const WORD DD[] = {
    3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0,
    3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0,
    3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0,
    3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0, 3, 2, 1, 0
};

/* 
    Fifth paramater for the transformation functions.
    MM being the index of the uint32_t block that needs to be accessed
*/
const WORD MM[] = {
    0, 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
    1, 6, 11,  0,  5, 10, 15,  4,  9, 14,  3,  8, 13,  2,  7, 12,
    5, 8, 11, 14,  1,  4,  7, 10, 13,  0,  3,  6,  9, 12, 15,  2,
    0, 7, 14,  5, 12,  3, 10,  1,  8, 15,  6, 13,  4, 11,  2,  9
};

/* 
    Sixth paramater for the transformation functions.
    https://tools.ietf.org/html/rfc1321 => Page 10

    Predefined constants for the MD5 Transform routine
    Specifies the per-round shift amounts
*/
const WORD S[] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

/*
    Seventh and final paramater for the transformation functions.
    https://tools.ietf.org/html/rfc1321 => Page 13 and 14

    Predefined hashing constants required for MD5
    Integer part of the sines of integers (in radians) * 2^32
*/
const WORD T[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

/* 
    https://tools.ietf.org/html/rfc1321 => Page 4

    Four 'Word' buffer initialized with hex values used in the Message Digest computation.
    These will be manipulated on each round of the MD5 hash.
*/
WORD A = 0x67452301;
WORD B = 0xefcdab89;
WORD C = 0x98badcfe;
WORD D = 0x10325476;

/*
    All union members will share the same memory location
    Different definitions depending on bit impelemtation

    Kind of an interface allowing the use of specific memory adresses.
    Takes up 64 bytes altogether
*/
typedef union { 
    uint64_t sixfour[8];
    WORD threetwo[16];
    uint8_t eight[64];
} BLOCK;

/*
    Status controller when reading the pad file

    READ: Still reading file, not EOF
    PAD0: Already started padding some 0's
    Finish: Finished all padding
*/
typedef enum {
    READ, 
    PAD0, 
    FINISH 
} PADFLAG;

/* --------------------- Perform MD5 on Blocks ----------------------- */
void md5(BLOCK *M, WORD *H) {
    WORD a, b, c, d;
    /* Initialize hash value for this chunk */
    a = H[0];
    b = H[1];
    c = H[2];
    d = H[3];

    /* Perform the four hash rounds for each chunk */
    for(int i = 0; i<64; i++) {
        if (i < 16) {
            FF(H[AA[i]], H[BB[i]], H[CC[i]], H[DD[i]], M->threetwo[MM[i]] , S[i] , T[i]) ; /* ROUND 1 */
        } else if (i < 32) {
            GG(H[AA[i]], H[BB[i]], H[CC[i]], H[DD[i]], M->threetwo[MM[i]] , S[i] , T[i]) ; /* ROUND 2 */
        } else if (i < 48) {
            HH(H[AA[i]], H[BB[i]], H[CC[i]], H[DD[i]], M->threetwo[MM[i]] , S[i] , T[i]) ; /* ROUND 3 */
        } else {
            II(H[AA[i]], H[BB[i]], H[CC[i]], H[DD[i]], M->threetwo[MM[i]] , S[i] , T[i]) ; /* ROUND 4 */
        }
    }
    
    /* Add this chunk's hash to result so far */
    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
}

/* ----------------------- Read Block by Block ----------------------- */
int nextblock(BLOCK *M, FILE *infile, uint64_t *nobits, PADFLAG *status) {

  int i;
  size_t nobytesread = fread(&M->eight, 1, 64, infile);
  *nobits += nobytesread * 8;

    /* Before stuff gets read in, need to check the value of status */  
    switch(*status) {
    /* If finished, return */
    case FINISH:
        return 0;
    case PAD0:
        // We need an all-padding block without the 1 bit.
        for (i = 0; i < 57; i++) {
            M->eight[i] = 0;
        }
        M->sixfour[7] = *nobits;
        *status = FINISH;
        return 1;
        break;  
    default:
        if (nobytesread < 56) {
            // We can put all padding in this block.
            M->eight[nobytesread] = 0x80;
            for (i = nobytesread + 1; i < 56; i++){
                M->eight[i] = 0;
            }
            M->sixfour[7] = *nobits;
            *status = FINISH;
        } 
        else if (nobytesread < 64) {
            // Otherwise we have read between 56 (incl) and 64 (excl) bytes.
            M->eight[nobytesread] = 0x80;
            for (i = nobytesread + 1; i < 64; i++) {
                M->eight[i] = 0;
            }
            *status = PAD0;
        }
        return 1;
    }
}

/* -------------------------- Main Method ---------------------------- */
int main(int argc, char *argv[]) {
    // Expect and open a single filename.
    if (argc != 2) {
        printf("Error: expected single filename as argument.\n");
        return 1;
    }

    FILE *infile = fopen(argv[1], "rb");
    if (!infile) {
        printf("Error: couldn't open file %s.\n", argv[1]);
        return 1;
    }
    // The current padded message block
    BLOCK M;
    uint64_t nobits = 0;
    PADFLAG status = READ;
    WORD H[] = {A, B, C, D};

    // Read through all of the padded message blocks.
    while (nextblock(&M, infile, &nobits, &status)) {
        // Calculate the next hash value.
        md5(&M, H);
    }



    fclose(infile);
    return 0;
} 

