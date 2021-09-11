#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#include <stdio.h>
#include "stage_one_func.h"

#define DECIMAL 10
#define BINARY 2

int ipow(int base, int exponent); /*power with integer*/
int btoi(unsigned long num, int from, int to); /*bit to integer*/
unsigned long itob(int num, int data_size, int range_flag); /*integer to bit*/
unsigned long reverse(unsigned long bit_num, int data_size); /*reverse bit code*/
unsigned long add_sign_bit(unsigned long bit_num, int data_size); /*convert to negative 2nd complement*/
unsigned long create_mask(int length);  /*create mask of 1's in length's size*/

/*add given parameter to code image in given position*/
void add_code_parameter(unsigned long param, unsigned long * code_image, int pos, int length);

#endif /* CONVERSIONS_H */