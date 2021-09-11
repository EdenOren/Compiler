/*
 * This source file consists of conversion functions
 * bit to int, int to bit and more related fucntions
 */
#include "conversions.h"

/*
 * Function:  ipow 
 * --------------------
 * power function for int number
 */
int ipow(int base, int exponent) {
    int result = 1;
    for (; exponent > 0; exponent--)
    {
        result *= base;
    }
    return result;
}
/*
 * Function:  btoi 
 * --------------------
 * convert bit code to integer
 * num: bit code to convert
 * from: starting index of bit code to convert
 * to: ending index of bit code to convert
 * return: integer conversion of given bit code
 */
int btoi(unsigned long num, int from, int to) {
    int result = 0;

    /*set relevant bit code range from 0 index*/
    num >>= from;
    to = to - from;
    
    for (from = 0; from < to; from++)
    {
        result += ((num) & 1) * ipow(BINARY, from); /*covers LSB to num using it's position*/
        num >>= 1;
    }
    return result;   
}

/*
 * Function:  itob 
 * --------------------
 * convert integer to bit code
 * num:integer to convert
 * data_size: size of bit code in bytes
 * range_flag: flag 1 if num is out of range in terms of byte -> convert LSB. 0 otherwise
 * return: bit code conversion of given integer
 */
unsigned long itob(int num, int data_size, int range_flag) {
    unsigned long result = 0;
    int is_negative = 0; /*is negative flag*/
    int bit = 0, i;

    if (num < 0) /*convert num to positive & change flag*/
    {
        is_negative = 1;
        num *= -1;
    }
    for (i = 0; (i < BITS_IN_BYTE * data_size); i++)
    {
        bit = num % BINARY;
        num /= BINARY;
        result |= (bit & 1);
        if (i < BITS_IN_BYTE * data_size - 1) /*prevent last shift*/
            result <<= 1;
    }
    result = reverse(result, data_size);
    if (is_negative && !range_flag) /*add sign bit if negative num and within range*/
    {
        result = add_sign_bit(result, data_size);
    }
    return result;   
}

/*convert to negative 2nd complement*/
unsigned long add_sign_bit(unsigned long bit_num, int data_size) {
        bit_num = ~bit_num;
        bit_num++;
        return bit_num;
}

/*reverse bit code*/
unsigned long reverse(unsigned long bit_num, int data_size)
{
    int i;
    unsigned long result = 0;

    for (i = 0; i < (BITS_IN_BYTE * data_size); i++)
    {
        result |= (bit_num & 1);
        bit_num >>= 1;
        if (i < BITS_IN_BYTE * data_size - 1) /*prevent last shift*/
            result <<= 1;
    }
    return result;
}

/*add given parameter to code image in given position*/
void add_code_parameter(unsigned long param, unsigned long * code_image, int pos, int length) {
    unsigned long mask = create_mask(length);

    (*code_image) |= (mask << pos) & (param << pos );
}

/*create mask of 1's in length's size*/
unsigned long create_mask(int length) {
    unsigned long mask;
    mask = 1 << (length);
    mask -= 1;
    return mask;
}