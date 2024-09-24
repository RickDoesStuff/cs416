/*
* Add NetID and names of all project partners
eja97 Enrico Aquino
rsb204 Rohit Bellam
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NUM_TOP_BITS 4 //top bits to extract
#define BITMAP_SIZE 4 //size of the bitmap array
#define SET_BIT_INDEX 17 //bit index to set 
#define GET_BIT_INDEX 17 //bit index to read

#define BYTE_SIZE 8

static unsigned int myaddress = 4026544704;   // Binary  would be 11110000000000000011001001000000

/* 
 * Function 1: EXTRACTING OUTER (TOP-ORDER) BITS
 * shift all the numbers to the right by the total - the num of bits we want
 */
static unsigned int get_top_bits(unsigned int value,  int num_bits)
{

    // need to make sure num bits is within the range
    if(num_bits < 1 || num_bits >= 32 ) {
        printf("Invalid Num Bits\n");
        return -1;
    }

	int result = value >> (32 - num_bits);
    //printf("result: %i\n",result);
	return result;
}


/* 
 * Function 2: SETTING A BIT AT AN INDEX 
 * Function to set a bit at "index" bitmap
 */
static void set_bit_at_index(char *bitmap, int index)
{

    // 0123,4567
    // printf("sizeof(bitmap):%i\n", sizeof(bitmap));
    if (index < 0 || index >= (BYTE_SIZE * BITMAP_SIZE)) {
        printf("Invalid Index for setting bit\n");
    }
    //printf("index:%i\n",index);

    // get the byte
    // do int division to get it (17 // 8) = 2
    int byteIndex = index / BYTE_SIZE;
    // now get the bit  (17) - (2 * 8) = 1
    int bitIndex = index - (byteIndex * BYTE_SIZE);

    //printf("byteIndex:%i\n", byteIndex);
    //printf("bitIndex:%i\n", bitIndex);

    //Implement your code here	
    // move 1 to the bit index by using left shift and then set that bit into the bitmap at that byte
    bitmap[byteIndex] = bitmap[byteIndex] | (1 << bitIndex);
    return;
}


/* 
 * Function 3: GETTING A BIT AT AN INDEX 
 * Function to get a bit at "index"
 */
static int get_bit_at_index(char *bitmap, int index)
{
    // 0123,4567
    // printf("sizeof(bitmap):%i\n", sizeof(bitmap));
    if (index < 0 || index >= (BYTE_SIZE * BITMAP_SIZE)) {
        printf("Invalid Index for getting bit\n");
    }
    
    // get the byte
    // do int division to get it (17 // 8) = 2
    int byteIndex = index / BYTE_SIZE;
    // now get the bit  (17) - (2 * 8) = 1
    int bitIndex = index - (byteIndex * BYTE_SIZE);
    
	
	//Get to the location in the character bitmap array
    int isSet = (bitmap[byteIndex] & (1 << bitIndex)) != 0;
    //printf("is set:%i\n",isSet);
    //Implement your code here
    return isSet;
    
}

// // print all bits in the bitmap
// void print_bitmap_bits(char *bitmap, int bitmap_size) {
//     for (int byte_index = 0; byte_index < bitmap_size; byte_index++) {
//         for (int bit_index = 0; bit_index < 8; bit_index++) {
//             int bit = (bitmap[byte_index] >> bit_index) & 1;
//             printf("%i:%d\n", (byte_index*BYTE_SIZE)+bit_index,bit);
//         }
//     }
//     printf("\n");
// }

int main () {

    /* 
     * Function 1: Finding value of top order (outer) bits Now let’s say we
     * need to extract just the top (outer) 4 bits (1111), which is decimal 15  
    */
    unsigned int outer_bits_value = get_top_bits (myaddress , NUM_TOP_BITS);
    printf("Function 1: Outer bits value %u \n", outer_bits_value); 
    printf("\n");

    /* 
     * Function 2 and 3: Checking if a bit is set or not
     */
    char *bitmap = (char *)malloc(BITMAP_SIZE);  //We can store 32 bits (4*8-bit per character)
    memset(bitmap,0, BITMAP_SIZE); //clear everything
    
    /* 
     * Let's try to set the bit 
     */
    set_bit_at_index(bitmap, SET_BIT_INDEX);
    
    /* 
     * Let's try to read bit)
     */
    printf("Function 3: The value at %dth location %d\n", 
            GET_BIT_INDEX, get_bit_at_index(bitmap, GET_BIT_INDEX));


    /* Testing Code Meant to Visualize the Bitmap (NOT Part of main() function) */  
	
    // print_bitmap_bits(bitmap, BITMAP_SIZE);

	// set_bit_at_index(bitmap, 2);
	// set_bit_at_index(bitmap, 6);
	// set_bit_at_index(bitmap, 9);
	
	// print_bitmap_bits(bitmap, BITMAP_SIZE);

    // int bitToTest = 9;
    // if (get_bit_at_index(bitmap, bitToTest)) {
    //     printf("Check Bit at index %i and it is set.\n",bitToTest);
    // } else {
    //     printf("Check Bit at %i and it is not set.\n",bitToTest);
    // }
    // printf("setting bit\n");
    // set_bit_at_index(bitmap, bitToTest);  // Set the bit at index 35 (within the 3rd byte)
    
    // if (get_bit_at_index(bitmap, bitToTest)) {
    //     printf("Bit at index %i is set.\n",bitToTest);
    // } else {
    //     printf("Bit at index %i is not set.\n",bitToTest);
    // }

    // if (get_bit_at_index(bitmap, 5)) {
    //     printf("Bit at index %i is set.\n",bitToTest+4);
    // } else {
    //     printf("Bit at index %i is not set.\n",bitToTest+4);
    // }

    // printf("Size of bitmap: %zu bytes\n", sizeof(bitmap));
    // printf("Size of bitmap: %zu bytes\n", sizeof(bitmap[0]));

    return 0;
}