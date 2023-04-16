#ifndef SRC_MY_DECIMAL_H_
#define SRC_MY_DECIMAL_H_

#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_BITS_NUM 32
#define BIG_BIT_VECTORS_NUM 7
#define BIT_VECTORS_NUM 3

#define MAX_SCALE 28
#define SCALE_BITS_END 23
#define SCALE_BITS_START 16
#define SCALE_BITS_LEN 7

#define NEGATIVE_SIGN (INT_MAX + 1U)
#define SCALE_1 65536
#define SCALE_28 1835008

#define SUCCESS 0
#define POSITIVE_INF 1
#define NEGATIVE_INF 2
#define ZERO_DIVISION 3

#define my_INFINITY 0b00000000111111110000000000000000
#define my_NEGATIVE_INFINITY 0b10000000111111110000000000000000
#define my_ZERO 0b00000000000000000000000000000000
#define my_NEGATIVE_ZERO 0b10000000000000000000000000000000
#define MASK_FIRST_BIT 0x80000000
#define MASK_FIRST_BIT_MANTISSA 0x400000
#define MASK_EXP_SCALE 0x00ff0000
#define MASK_CARRY_UNIT 0x100000000
#define CONVERTING_ERROR 1

#define my_NAN

typedef struct my_decimal {
  unsigned bits[4];
} my_decimal;

typedef struct big_decimal {
  unsigned bits[8];
} big_decimal;

// ARITHMETIC
int my_add(my_decimal value_1, my_decimal value_2, my_decimal *result);
int my_sub(my_decimal value_1, my_decimal value_2, my_decimal *result);
int my_mul(my_decimal value_1, my_decimal value_2, my_decimal *result);
int my_div(my_decimal value_1, my_decimal value_2, my_decimal *result);
int my_mod(my_decimal value_1, my_decimal value_2, my_decimal *result);

// ARITHMETIC ADDITIONAL FUNCTIONS
void bit_sum(const big_decimal *b_value_1, const big_decimal *b_value_2,
             big_decimal *b_result);
void bit_substr(const big_decimal *b_value_1, const big_decimal *b_value_2,
                big_decimal *b_result);
void placing_value_to_subtract(big_decimal *b_value, int bit, int n_bit_vector);
void bit_mult(const big_decimal *b_value_1, const big_decimal *b_value_2,
              big_decimal *b_result);
void multiply_big_decimal_by_10(big_decimal *b_value);
void bit_div(const big_decimal *b_value_1, const big_decimal *b_value_2,
             big_decimal *b_result);
void big_fractional_division(big_decimal *b_value_1, big_decimal *b_value_2,
                             big_decimal *b_result);
int big_integer_division(big_decimal *dividend, const big_decimal *divider,
                         big_decimal *int_part);
void div_scale_processing(const big_decimal *b_value_1,
                          const big_decimal *b_value_2, big_decimal *b_result);
void bit_mod(const big_decimal *b_value_1, const big_decimal *b_value_2,
             big_decimal *b_result);
void unset_sign(big_decimal *b_value);
void increase_scale(big_decimal *b_value);
// CONVERT AND SCALE
void convert_to_big_decimal(const my_decimal *value, big_decimal *b_value);
void convert_to_my_decimal(const big_decimal *b_result, my_decimal *result);
void scale_normalization(big_decimal *b_value_1, big_decimal *b_value_2);
unsigned get_big_decimal_scale(const big_decimal *b_value);
// BIG_DECIMAL COMPARISON
int is_big_greater(const big_decimal *b_value_1, const big_decimal *b_value_2);
int is_big_equal(const big_decimal *b_value_1, const big_decimal *b_value_2);
int is_big_empty(const big_decimal *b_value);
int is_big_negative(const big_decimal *b_value);
// SHIFTS
void big_right_shift(big_decimal *b_value, int right_shift);
void big_left_shift(big_decimal *b_value, int left_shift);
// ARITHMETIC OTHER FUNCTIONS
void reset_big_decimal(big_decimal *b_value);
void reset_my_decimal(my_decimal *value);
int error_processing(const big_decimal *b_value);
int is_decimal_overflow(const big_decimal *b_value);

// COMPARISON
int my_is_less(my_decimal value_1, my_decimal value_2);
int my_is_less_or_equal(my_decimal value_1, my_decimal value_2);
int my_is_greater(my_decimal value_1, my_decimal value_2);
int my_is_greater_or_equal(my_decimal value_1, my_decimal value_2);
int my_is_equal(my_decimal value_1, my_decimal value_2);
int my_is_not_equal(my_decimal value_1, my_decimal value_2);

// COMPARISON ADDITIONAL FUNCTIONS
int is_my_decimal_empty(const my_decimal *value);

// CONVERTERS
int my_from_int_to_decimal(int src, my_decimal *dst);
int my_from_float_to_decimal(float src, my_decimal *dst);
int my_from_decimal_to_int(my_decimal src, int *dst);
int my_from_decimal_to_float(my_decimal src, float *dst);

// CONVERTERS ADDITIONAL FUNCTIONS
void my_decimal_to_default(my_decimal *dec);
void setSignNegative(my_decimal *current);
void clearScale(my_decimal *current);
int getBinExp(float src);
unsigned int float_binary(float src);
int my_shift_left_scale(my_decimal *dec, unsigned int value);
my_decimal my_TruncateScale(my_decimal dec);
int FindFirstNotZeroBit(my_decimal *current);
int my_shift_right_scale(my_decimal *dec, unsigned int value);
int getSign(my_decimal *current);
int getScale(my_decimal *current);
int getBit(my_decimal current, int i);
void setSignZero(my_decimal *current);
int setScale(my_decimal *current, int scale);

// OTHER FUNCTIONS
int my_floor(my_decimal value, my_decimal *result);
int my_round(my_decimal value, my_decimal *result);
int my_truncate(my_decimal value, my_decimal *result);
int my_negate(my_decimal value, my_decimal *result);
void big_bank_round(big_decimal *field);
void big_add(big_decimal b_value_1, big_decimal b_value_2,
             big_decimal *b_result);
void big_sub(big_decimal b_value_1, big_decimal b_value_2,
             big_decimal *b_result);
void decrease_scale(big_decimal *b_value);

// BITWISE OPERATIONS FUNCTIONS
unsigned is_set_bit(unsigned number, int index);
unsigned set_bit(unsigned number, int index);
unsigned inverse_bit(unsigned number, int index);
unsigned reset_bit(unsigned number, int index);

#endif  // SRC_MY_DECIMAL_H_