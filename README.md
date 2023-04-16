# decimal.h
My own realization of decimal data type (using C language) for Mac and Linux. 
For build static library do make all, for tests do make test.
Usage of library like in tests.c.
## Realized features
Decimal number implemented as a four-element array of 32-bit signed integers (`int bits[4];`).
bits[0], bits[1], and bits[2] contain the low, middle, and high 32 bits of the 96-bit integer number accordingly.
bits[3] contains the scale factor and sign, and consists of following parts:
`bits[0]`, `bits[1]`, and `bits[2]` contain the low, middle, and high 32 bits of the 96-bit integer number accordingly.
`bits[3]` contains the scale factor and sign, and consists of following parts:
- Bits 0 to 15, the lower word, are unused and must be zero.
- Bits 16 to 23 must contain an exponent between 0 and 28, which indicates the power of 10 to divide the integer number.
- Bits 24 to 30 are unused and must be zero.
- Bit 31 contains the sign; 0 meaning positive, and 1 meaning negative.
## Realized functions
### Arithmetic Operators
#### int my_add(my_decimal value_1, my_decimal value_2, my_decimal *result)
Addition of two decimal
#### int my_sub(my_decimal value_1, my_decimal value_2, my_decimal *result)
Subtraction of two decimal
#### int my_mul(my_decimal value_1, my_decimal value_2, my_decimal *result)
Multiplication of two decimal
#### int my_dev(my_decimal value_1, my_decimal value_2, my_decimal *result)
Division of two decimal
#### int my_mode(my_decimal value_1, my_decimal value_2, my_decimal *result)
Modulo of two decimal
#### The functions return value:
- 0 - OK
- 1 - the number is too large or equal to infinity
- 2 - the number is too small or equal to negative infinity
- 3 - division by 0
### Comparison Operators
#### int my_is_less(my_decimal, my_decimal)
is first decimal less than second decimal
#### int my_is_less_or_equal(my_decimal, my_decimal)
is first decimal less than or equal to second decimal
#### int my_is_greater(my_decimal, my_decimal)
is first decimal greater than second decimal
#### int my_is_greater_or_equal(my_decimal, my_decimal)
is first decimal greater than or equal second decimal
#### int my_is_equal(my_decimal, my_decimal)
is first decimal equal to second decimal
#### int my_is_not_equal(my_decimal, my_decimal)
is first decimal not equal to second decimal
#### The functions return value:
- 0 - FALSE
- 1 - TRUE
### Convertors and parsers
#### int my_from_int_to_decimal(int src, my_decimal *dst)
convert int to decimal
#### int my_from_float_to_decimal(float src, my_decimal *dst)
convert float to decimal
#### int my_from_decimal_to_int(my_decimal src, int *dst)
convert decimal to int
#### int my_from_decimal_to_float(my_decimal src, float *dst)
convert decimal to float
#### The functions return value:
- 0 - OK
- 1 - convertation error
### Another functions
#### int my_floor(my_decimal value, my_decimal *result)
Rounds a specified Decimal number to the closest integer toward negative infinity.
#### int my_round(my_decimal value, my_decimal *result)
Rounds a decimal value to the nearest integer.
#### int my_truncate(my_decimal value, my_decimal *result)
Returns the integral digits of the specified Decimal; any fractional digits are discarded, including trailing zeroes.
#### int my_negate(my_decimal value, my_decimal *result)
Returns the result of multiplying the specified Decimal value by negative one.
#### The functions return value:
- 0 - OK
- 1 - calculation error