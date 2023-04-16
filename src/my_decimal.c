#include "my_decimal.h"

// S21_ARITHMETIC
int my_add(my_decimal value_1, my_decimal value_2, my_decimal *result) {
  /* Функция для суммы 2х значений my_decimal */

  big_decimal b_value_1 = {0}, b_value_2 = {0}, b_result = {0};

  convert_to_big_decimal(&value_1, &b_value_1);
  convert_to_big_decimal(&value_2, &b_value_2);

  big_add(b_value_1, b_value_2, &b_result);
  big_bank_round(&b_result);

  errno = error_processing(&b_result);

  !errno ? convert_to_my_decimal(&b_result, result)
         : reset_my_decimal(result);

  return errno;
}

int my_sub(my_decimal value_1, my_decimal value_2, my_decimal *result) {
  /* Функция для вычитания 2х значений my_decimal */

  my_negate(value_2, &value_2);
  errno = my_add(value_1, value_2, result);

  return errno;
}

int my_mul(my_decimal value_1, my_decimal value_2, my_decimal *result) {
  /* Функция для умножения 2х значений my_decimal */

  big_decimal b_value_1 = {0}, b_value_2 = {0}, b_result = {0};

  convert_to_big_decimal(&value_1, &b_value_1);
  convert_to_big_decimal(&value_2, &b_value_2);

  bit_mult(&b_value_1, &b_value_2, &b_result);
  big_bank_round(&b_result);

  errno = error_processing(&b_result);

  !errno ? convert_to_my_decimal(&b_result, result)
         : reset_my_decimal(result);

  return errno;
}

int my_div(my_decimal value_1, my_decimal value_2, my_decimal *result) {
  /* Функция для деления 2х значений my_decimal */

  big_decimal b_value_1 = {0}, b_value_2 = {0}, b_result = {0};

  convert_to_big_decimal(&value_1, &b_value_1);
  convert_to_big_decimal(&value_2, &b_value_2);

  if (!is_big_empty(&b_value_2)) {
    bit_div(&b_value_1, &b_value_2, &b_result);
    big_bank_round(&b_result);
  }

  errno =
      error_processing(&b_result) + (is_big_empty(&b_value_2) * ZERO_DIVISION);

  !errno ? convert_to_my_decimal(&b_result, result)
         : reset_my_decimal(result);

  return errno;
}

int my_mod(my_decimal value_1, my_decimal value_2, my_decimal *result) {
  /* Функция для взятия остатка 2х значений my_decimal */

  big_decimal b_value_1 = {0}, b_value_2 = {0}, b_result = {0};

  convert_to_big_decimal(&value_1, &b_value_1);
  convert_to_big_decimal(&value_2, &b_value_2);

  if (!is_big_empty(&b_value_2)) {
    bit_mod(&b_value_1, &b_value_2, &b_result);
    big_bank_round(&b_result);
  }

  errno =
      error_processing(&b_result) + (is_big_empty(&b_value_2) * ZERO_DIVISION);

  !errno ? convert_to_my_decimal(&b_result, result)
         : reset_my_decimal(result);

  return errno;
}

// ARITHMETIC ADDITIONAL FUNCTIONS
void multiply_big_decimal_by_10(big_decimal *b_value) {
  /* Функция для умножения значения big_decimal на 10 в операции
  выравнивания scale'ов */

  big_decimal big_bit_shift_1 = *b_value, big_bit_shift_3 = *b_value;

  big_left_shift(&big_bit_shift_1, 1);
  big_left_shift(&big_bit_shift_3, 3);

  reset_big_decimal(b_value);
  bit_sum(&big_bit_shift_1, &big_bit_shift_3, b_value);
}

void bit_sum(const big_decimal *b_value_1, const big_decimal *b_value_2,
             big_decimal *b_result) {
  /* Функция для побитового сложения 2х значений big_decimal */

  big_decimal t_b_value_1 = *b_value_1, t_b_value_2 = *b_value_2;

  unsigned next = 0;

  reset_big_decimal(b_result);

  for (int n_bit_vector = 0; n_bit_vector < BIG_BIT_VECTORS_NUM;
       n_bit_vector++) {
    for (int bit = 0; bit < INT_BITS_NUM; bit++) {
      if ((is_set_bit(t_b_value_1.bits[n_bit_vector], bit) ^
           is_set_bit(t_b_value_2.bits[n_bit_vector], bit)) ^
          next) {
        b_result->bits[n_bit_vector] =
            set_bit(b_result->bits[n_bit_vector], bit);
        next = 0;
      }

      if (is_set_bit(t_b_value_1.bits[n_bit_vector], bit) &
          is_set_bit(t_b_value_2.bits[n_bit_vector], bit)) {
        next = 1;
      }
    }
  }
}

void big_add(big_decimal b_value_1, big_decimal b_value_2,
             big_decimal *b_result) {
  /* Функция для сложения 2х нормализированных значений big_decimal */

  char sign_1 =
           is_set_bit(b_value_1.bits[BIG_BIT_VECTORS_NUM], INT_BITS_NUM - 1),
       sign_2 =
           is_set_bit(b_value_2.bits[BIG_BIT_VECTORS_NUM], INT_BITS_NUM - 1);

  scale_normalization(&b_value_1, &b_value_2);
  b_result->bits[BIG_BIT_VECTORS_NUM] =
      is_big_greater(&b_value_1, &b_value_2)
          ? b_value_1.bits[BIG_BIT_VECTORS_NUM]
          : b_value_2.bits[BIG_BIT_VECTORS_NUM];

  if ((!sign_1 && !sign_2) || (sign_1 && sign_2)) {
    bit_sum(&b_value_1, &b_value_2, b_result);
  } else {
    is_big_greater(&b_value_1, &b_value_2)
        ? bit_substr(&b_value_1, &b_value_2, b_result)
        : bit_substr(&b_value_2, &b_value_1, b_result);
  }
}

void bit_substr(const big_decimal *b_value_1, const big_decimal *b_value_2,
                big_decimal *b_result) {
  /* Функция для побитового вычитания 2х значений big_decimal. Вычитание всегда
   * происходит из b_value_1 */

  big_decimal t_b_value_1 = *b_value_1, t_b_value_2 = *b_value_2;

  reset_big_decimal(b_result);

  for (int n_bit_vector = 0; n_bit_vector < BIG_BIT_VECTORS_NUM;
       n_bit_vector++) {
    for (int bit = 0; bit < INT_BITS_NUM; bit++) {
      if (!is_set_bit(t_b_value_1.bits[n_bit_vector], bit) &&
          is_set_bit(t_b_value_2.bits[n_bit_vector], bit)) {
        placing_value_to_subtract(&t_b_value_1, bit, n_bit_vector);
        b_result->bits[n_bit_vector] =
            set_bit(b_result->bits[n_bit_vector], bit);
      } else if (is_set_bit(t_b_value_1.bits[n_bit_vector], bit) &&
                 !is_set_bit(t_b_value_2.bits[n_bit_vector], bit)) {
        b_result->bits[n_bit_vector] =
            set_bit(b_result->bits[n_bit_vector], bit);
      }
    }
  }
}

void big_sub(big_decimal b_value_1, big_decimal b_value_2,
             big_decimal *b_result) {
  /* Функция для вычитания 2х нормализированных значений big_decimal */

  b_value_2.bits[BIG_BIT_VECTORS_NUM] =
      inverse_bit(b_value_2.bits[BIG_BIT_VECTORS_NUM], INT_BITS_NUM - 1);
  big_add(b_value_1, b_value_2, b_result);
}

void placing_value_to_subtract(big_decimal *b_value, int bit,
                               int n_bit_vector) {
  /* Функция для занимания 1 у старшего разряда у вычитаемого(большего) значения
  big_decimal при битовом вычитании */

  unsigned placed_value = 0;

  for (; !placed_value && n_bit_vector < BIG_BIT_VECTORS_NUM; n_bit_vector++) {
    for (; !placed_value && bit < INT_BITS_NUM; bit++) {
      if (!is_set_bit(b_value->bits[n_bit_vector], bit)) {
        b_value->bits[n_bit_vector] = set_bit(b_value->bits[n_bit_vector], bit);
      } else {
        b_value->bits[n_bit_vector] =
            reset_bit(b_value->bits[n_bit_vector], bit);
        placed_value = 1;
      }
    }
    bit = 0;
  }
}

void bit_mult(const big_decimal *b_value_1, const big_decimal *b_value_2,
              big_decimal *b_result) {
  /* Функция для побитового умножения 2х значений big_decimal */

  big_decimal t_b_value_1 = *b_value_1;
  big_decimal t_b_value_2 = *b_value_2;
  big_decimal t_b_result = {0};
  unsigned mult_scale = b_value_1->bits[BIG_BIT_VECTORS_NUM] +
                        b_value_2->bits[BIG_BIT_VECTORS_NUM];

  while (!is_big_empty(&t_b_value_2)) {
    if (is_set_bit(t_b_value_2.bits[0], 0)) {
      reset_big_decimal(b_result);
      bit_sum(&t_b_result, &t_b_value_1, b_result);
      t_b_result = *b_result;
    }

    big_right_shift(&t_b_value_2, 1);
    big_left_shift(&t_b_value_1, 1);
  }

  b_result->bits[BIG_BIT_VECTORS_NUM] = mult_scale;
}

void bit_div(const big_decimal *b_value_1, const big_decimal *b_value_2,
             big_decimal *b_result) {
  /* Функция для побитового деления 2х значений big_decimal */

  big_decimal t_b_value_1 = *b_value_1, t_b_value_2 = *b_value_2;

  big_fractional_division(&t_b_value_1, &t_b_value_2, b_result);
  div_scale_processing(&t_b_value_1, &t_b_value_2, b_result);
}

void big_fractional_division(big_decimal *b_value_1, big_decimal *b_value_2,
                             big_decimal *b_result) {
  /* Функция для побитового деления 2х значений big_decimal. */

  // big_decimal t_b_result = {0};
  int is_integer_division =
      big_integer_division(b_value_1, b_value_2, b_result);

  while (!is_integer_division &&
         (get_big_decimal_scale(b_result) < MAX_SCALE || !b_result->bits[0])) {
    big_decimal t_b_result = {0};
    multiply_big_decimal_by_10(b_value_1);

    t_b_result = *b_result;
    multiply_big_decimal_by_10(&t_b_result);
    increase_scale(b_result);

    is_integer_division = big_integer_division(b_value_1, b_value_2, b_result);
    bit_sum(&t_b_result, b_result, b_result);
  }
}

int big_integer_division(big_decimal *dividend, const big_decimal *divider,
                         big_decimal *int_part) {
  /* Функция для целочисленного побитового деления 2х значений big_decimal. В
  b_result помещается результат целочисленного деления. Возвращает 1, если в
  результате деления не осталось дробной части(6/2) и 0, если она есть(7/2) */

  big_decimal t_divider = *divider, div_count = {{1, 0, 0, 0, 0, 0, 0, 0}};

  reset_big_decimal(int_part);

  // alignment
  while (is_big_greater(dividend, &t_divider)) {
    big_left_shift(&t_divider, 1);
    big_left_shift(&div_count, 1);
  }
  // bit div processing
  do {
    if (is_big_greater(dividend, &t_divider) ||
        is_big_equal(dividend, &t_divider)) {
      bit_substr(dividend, &t_divider, dividend);
      bit_sum(&div_count, int_part, int_part);
    }

    big_right_shift(&t_divider, 1);
    big_right_shift(&div_count, 1);
  } while (!is_big_empty(&div_count));

  return is_big_empty(dividend);
}

void div_scale_processing(const big_decimal *b_value_1,
                          const big_decimal *b_value_2, big_decimal *b_result) {
  /* Функция для обработки scale'а результирующего значения big_decimal b_result
   */

  unsigned b_value_1_scale = get_big_decimal_scale(b_value_1),
           b_value_2_scale = get_big_decimal_scale(b_value_2),
           div_scale = b_value_1->bits[BIG_BIT_VECTORS_NUM] -
                       b_value_2->bits[BIG_BIT_VECTORS_NUM];

  if (b_value_1_scale < b_value_2_scale) {
    while (b_value_1_scale != b_value_2_scale) {
      multiply_big_decimal_by_10(b_result);
      b_value_1_scale += 1;
    }

    b_result->bits[BIG_BIT_VECTORS_NUM] +=
        NEGATIVE_SIGN *
        (is_big_negative(b_value_1) ^ is_big_negative(b_value_2));
  } else {
    b_result->bits[BIG_BIT_VECTORS_NUM] += div_scale;
  }
}

void bit_mod(const big_decimal *b_value_1, const big_decimal *b_value_2,
             big_decimal *b_result) {
  /* Функция для побитового деления по остатку 2х значений big_decimal */

  big_decimal t_b_value_1 = *b_value_1, t_b_value_2 = *b_value_2,
              scaled_t_b_value_1 = {0};

  reset_big_decimal(b_result);

  scale_normalization(&t_b_value_1, &t_b_value_2);
  scaled_t_b_value_1 = t_b_value_1;

  if (is_big_greater(&t_b_value_2, &t_b_value_1)) {
    *b_result = t_b_value_1;
  } else {
    big_integer_division(&t_b_value_1, &t_b_value_2, b_result);
    bit_mult(&t_b_value_2, b_result, b_result);
    bit_substr(&scaled_t_b_value_1, b_result, b_result);
  }

  unset_sign(b_result);
  b_result->bits[BIG_BIT_VECTORS_NUM] +=
      NEGATIVE_SIGN * (is_big_negative(b_value_1));
}

void unset_sign(big_decimal *b_value) {
  /* Функция для изменения знака значения big_decimal на положительный */

  b_value->bits[BIG_BIT_VECTORS_NUM] =
      reset_bit(b_value->bits[BIG_BIT_VECTORS_NUM], INT_BITS_NUM - 1);
}

void increase_scale(big_decimal *b_value) {
  /* Функция для увеличения scale'а значения big_decimal на 1 */

  b_value->bits[BIG_BIT_VECTORS_NUM] += SCALE_1;
}

void decrease_scale(big_decimal *b_value) {
  /* Функция для уменьшения скейла значения big_decimal на 1 */

  b_value->bits[BIG_BIT_VECTORS_NUM] -= SCALE_1;
}
// CONVERT AND SCALE
void convert_to_big_decimal(const my_decimal *value, big_decimal *b_value) {
  /* Функция для конвертации my_decimal --> big_decimal */

  b_value->bits[BIG_BIT_VECTORS_NUM] = value->bits[BIT_VECTORS_NUM];

  for (int n_bit_vector = 0; n_bit_vector < BIT_VECTORS_NUM; n_bit_vector++) {
    b_value->bits[n_bit_vector] = value->bits[n_bit_vector];
  }
}

void convert_to_my_decimal(const big_decimal *b_result, my_decimal *result) {
  /* Функция для конвертации big_decimal --> my_decimal */

  result->bits[BIT_VECTORS_NUM] = b_result->bits[BIG_BIT_VECTORS_NUM];

  for (int n_bit_vector = 0; n_bit_vector < BIT_VECTORS_NUM; n_bit_vector++) {
    result->bits[n_bit_vector] = b_result->bits[n_bit_vector];
  }
}

void scale_normalization(big_decimal *b_value_1, big_decimal *b_value_2) {
  /* Функция для нормализации степеней 2х значений big_decimal */

  int scale_1 = get_big_decimal_scale(b_value_1),
      scale_2 = get_big_decimal_scale(b_value_2),
      scale_substr = abs(scale_1 - scale_2);

  while (scale_substr--) {
    scale_1 < scale_2
        ? (multiply_big_decimal_by_10(b_value_1), increase_scale(b_value_1))
        : (multiply_big_decimal_by_10(b_value_2), increase_scale(b_value_2));
  }
}

unsigned get_big_decimal_scale(const big_decimal *b_value) {
  /* Функция для получения scale'а экземпляра big_decimal в 10-ом виде */

  unsigned b_value_scale = 0;

  for (int n_bit_vector = SCALE_BITS_START; n_bit_vector < SCALE_BITS_END;
       n_bit_vector++) {
    if (is_set_bit(b_value->bits[BIG_BIT_VECTORS_NUM], n_bit_vector)) {
      b_value_scale = set_bit(b_value_scale, n_bit_vector - SCALE_BITS_START);
    }
  }

  return b_value_scale;
}
// BIG_DECIMAL COMPARISON
int is_big_greater(const big_decimal *b_value_1, const big_decimal *b_value_2) {
  /* Функция для определения, является ли значение big_decimal b_value_1 больше
   * b_value_2 */

  int is_b_value_1_greater = 0;
  int getting_result = 0;

  for (int n_bit_vector = BIG_BIT_VECTORS_NUM - 1; n_bit_vector >= 0;
       n_bit_vector--) {
    if (b_value_1->bits[n_bit_vector] > b_value_2->bits[n_bit_vector]) {
      is_b_value_1_greater = 1;
      getting_result = 1;
    } else if (b_value_1->bits[n_bit_vector] < b_value_2->bits[n_bit_vector]) {
      getting_result = 1;
    }
    if (getting_result) {
      break;
    }
  }

  return is_b_value_1_greater;
}

int is_big_empty(const big_decimal *b_value) {
  /* Функция для проверки значения big_decimal на равенство 0 */

  int is_big_empty = 1;

  for (int n_bit_vector = 0; is_big_empty && n_bit_vector < BIG_BIT_VECTORS_NUM;
       n_bit_vector++) {
    if (b_value->bits[n_bit_vector]) {
      is_big_empty -= 1;
    }
  }

  return is_big_empty;
}

int is_big_equal(const big_decimal *b_value_1, const big_decimal *b_value_2) {
  /* Функция для определения равенства 2х значений big_decimal */

  int comparison_result = 1;  // Equal

  for (int n_bit_vector = 0;
       comparison_result && n_bit_vector < BIG_BIT_VECTORS_NUM;
       n_bit_vector++) {
    comparison_result *=
        b_value_1->bits[n_bit_vector] == b_value_2->bits[n_bit_vector];
  }

  return comparison_result;
}

int is_big_negative(const big_decimal *b_value) {
  /* Функция для определения знака значения big_decimal */

  return is_set_bit(b_value->bits[BIG_BIT_VECTORS_NUM], INT_BITS_NUM - 1);
}
// SHIFTS
void big_left_shift(big_decimal *b_value, int left_shift) {
  /* Смещение битов в битовых векторах big_decimal на shift позиций влево */

  left_shift %= INT_BITS_NUM;

  if (left_shift > 0) {
    for (int n_bit_vector = BIG_BIT_VECTORS_NUM - 2; n_bit_vector >= 0;
         n_bit_vector--) {
      for (int next_bit_vector_bit = left_shift - 1, bit = INT_BITS_NUM - 1;
           bit >= INT_BITS_NUM - left_shift; bit--) {
        if (is_set_bit(b_value->bits[n_bit_vector], bit)) {
          b_value->bits[n_bit_vector + 1] =
              set_bit(b_value->bits[n_bit_vector + 1], next_bit_vector_bit);
        }

        next_bit_vector_bit -= 1;
      }
      b_value->bits[n_bit_vector] <<= left_shift;
    }
  }
}

void big_right_shift(big_decimal *b_value, int right_shift) {
  /* Смещение битов в битовых векторах big_decimal на shift позиций вправо */

  right_shift %= INT_BITS_NUM;

  if (right_shift > 0) {
    for (int n_bit_vector = 0; n_bit_vector < BIG_BIT_VECTORS_NUM;
         n_bit_vector++) {
      for (int bit = 0; bit < right_shift; bit++) {
        b_value->bits[n_bit_vector] >>= 1;

        if (is_set_bit(b_value->bits[n_bit_vector + 1], bit)) {
          b_value->bits[n_bit_vector] =
              set_bit(b_value->bits[n_bit_vector], INT_BITS_NUM - 1);
        }
      }
    }
  }
}
// ARITHMETIC OTHER FUNCTIONS
void reset_big_decimal(big_decimal *b_value) {
  /* Функция для обнуления значений всех битовых векторов значения big_decimal
   */

  for (int n_bit_vector = 0; n_bit_vector < BIG_BIT_VECTORS_NUM;
       n_bit_vector++) {
    b_value->bits[n_bit_vector] = 0;
  }
}

void reset_my_decimal(my_decimal *value) {
  /* Функция для обнуления значений всех битовых векторов значения my_decimal
   */

  for (int n_bit_vector = 0; n_bit_vector <= BIT_VECTORS_NUM; n_bit_vector++) {
    value->bits[n_bit_vector] = 0;
  }
}

int error_processing(const big_decimal *b_value) {
  /* Функция для обработки ошибок в арифметических функциях my_decimal */

  errno = 0;

  if (is_decimal_overflow(b_value) && !is_big_negative(b_value)) {
    errno = POSITIVE_INF;
  } else if (get_big_decimal_scale(b_value) > MAX_SCALE ||
             (is_decimal_overflow(b_value) && is_big_negative(b_value))) {
    errno = NEGATIVE_INF;
  }

  return errno;
}

int is_decimal_overflow(const big_decimal *b_value) {
  /* Функция для проверки переполнения значения my_decimal, записанного
     в big_decimal */

  big_decimal decimal_size = {{UINT_MAX, UINT_MAX, UINT_MAX, 0, 0, 0, 0, 0}};

  return is_big_greater(b_value, &decimal_size);
}

// S21_COMPARISON
int my_is_less(my_decimal value_1, my_decimal value_2) {
  /* Функция для определения, является ли значение my_decimal value_1 меньше
   * value_2 */

  return !my_is_greater_or_equal(value_1, value_2);
}

int my_is_less_or_equal(my_decimal value_1, my_decimal value_2) {
  /* Функция для определения, является ли значение my_decimal value_1 меньшим
     либо равным value_2 */

  return my_is_less(value_1, value_2) || my_is_equal(value_1, value_2);
}

int my_is_greater(my_decimal value_1, my_decimal value_2) {
  /* Функция для определения, является ли значение my_decimal value_1 больше
   * value_2 */

  big_decimal b_value_1 = {0}, b_value_2 = {0};

  int value_1_sign = value_1.bits[BIT_VECTORS_NUM] >= NEGATIVE_SIGN,
      value_2_sign = value_2.bits[BIT_VECTORS_NUM] >= NEGATIVE_SIGN,
      are_signs_equal = !(value_1_sign ^ value_2_sign),
      are_signs_negative = value_1_sign && value_2_sign;

  int are_values_zero =
          is_my_decimal_empty(&value_1) && is_my_decimal_empty(&value_2),
      is_value_1_greater = (!value_1_sign && value_2_sign) * !are_values_zero;

  if (are_signs_equal && !are_values_zero) {
    convert_to_big_decimal(&value_1, &b_value_1);
    convert_to_big_decimal(&value_2, &b_value_2);

    scale_normalization(&b_value_1, &b_value_2);
    is_value_1_greater =
        (are_signs_negative && !is_big_equal(&b_value_1, &b_value_2))
            ? !is_big_greater(&b_value_1, &b_value_2)
            : is_big_greater(&b_value_1, &b_value_2);
  }

  return is_value_1_greater;
}

int my_is_greater_or_equal(my_decimal value_1, my_decimal value_2) {
  /* Функция для определения, является ли значение my_decimal value_1 большим
     либо равным value_2 */

  return my_is_greater(value_1, value_2) || my_is_equal(value_1, value_2);
}

int my_is_equal(my_decimal value_1, my_decimal value_2) {
  /* Функция для определения равенства 2х значений my_decimal */

  big_decimal b_value_1 = {0}, b_value_2 = {0};

  int sign_1 = value_1.bits[BIT_VECTORS_NUM] >= NEGATIVE_SIGN,
      sign_2 = value_2.bits[BIT_VECTORS_NUM] >= NEGATIVE_SIGN,
      are_signs_equal = !(sign_1 ^ sign_2);

  int are_values_zero =
          is_my_decimal_empty(&value_1) && is_my_decimal_empty(&value_2),
      are_values_equal = are_values_zero;

  if (are_signs_equal && !are_values_zero) {
    convert_to_big_decimal(&value_1, &b_value_1);
    convert_to_big_decimal(&value_2, &b_value_2);

    scale_normalization(&b_value_1, &b_value_2);

    are_values_equal = is_big_equal(&b_value_1, &b_value_2) || are_values_zero;
  }

  return are_values_equal;
}

int my_is_not_equal(my_decimal value_1, my_decimal value_2) {
  /* Функция для определения неравенства 2х значений my_decimal */

  return !my_is_equal(value_1, value_2);
}
// S21_COMPARISON ADDITIONAL FUNCTIONS
int is_my_decimal_empty(const my_decimal *value) {
  /* Функция для проверки значения big_decimal на равенство 0 */

  int is_my_decimal_empty = 1;

  for (int n_bit_vector = 0;
       is_my_decimal_empty && n_bit_vector < BIT_VECTORS_NUM; n_bit_vector++) {
    if (value->bits[n_bit_vector]) {
      is_my_decimal_empty -= 1;
    }
  }

  return is_my_decimal_empty;
}

// CONVERTERS
int my_from_int_to_decimal(int src, my_decimal *dst) {
  int flag = 0;
  my_decimal_to_default(dst);
  if (src < 0) {
    setSignNegative(dst);
    dst->bits[0] = -src;
  } else {
    clearScale(dst);
    dst->bits[0] = src;
  }
  dst->bits[1] = my_ZERO;
  dst->bits[2] = my_ZERO;
  return flag;
}

int my_from_float_to_decimal(float src, my_decimal *dst) {
  int flag = 0, minus = 0;
  my_decimal_to_default(dst);
  int binexp = getBinExp(src);
  if (src < 0) minus = 1;
  if ((src < 1e-28 && src > 0) || (src > -1e-28 && src < 0)) {
    dst->bits[0] = my_ZERO;
    dst->bits[1] = my_ZERO;
    dst->bits[2] = my_ZERO;
    dst->bits[3] = my_ZERO;
    flag = 1;
  } else if ((src == NAN) || (binexp > 95)) {
    dst->bits[0] = my_ZERO;
    dst->bits[1] = my_ZERO;
    dst->bits[2] = my_ZERO;
    dst->bits[3] = minus ? my_NEGATIVE_ZERO : my_ZERO;
    flag = 1;
  } else if (binexp > -95) {
    int scale = 0;
    double result = 1.0;
    int mask = MASK_FIRST_BIT_MANTISSA;
    unsigned int fbits = float_binary(src);
    if (minus) src *= -1;
    for (; !(int)src; src *= 10, scale++) {
    }
    for (; src > 10; src /= 10, scale--) {
    }
    for (int i = 1; mask; mask >>= 1, i++) {
      if (fbits & mask) result += pow(2, -i);
    }
    result *= pow(2, binexp) * pow(10, 8 + scale);
    if (scale > 0) {
      for (; result < 10000000; result *= 10) {
      }
    }
    long int tmp = 0L;
    tmp = round(result);
    int mod = 0;
    for (; tmp >= 10000000;) {
      mod = tmp % 10;
      tmp = round(tmp);
      tmp /= 10;
    }
    for (; scale + 7 > 29; mod = tmp % 10, tmp /= 10, scale--) {
    }
    if (mod > 4) tmp++;
    for (; tmp % 10 == 0; mod = tmp % 10, tmp /= 10, scale--) {
    }
    my_from_int_to_decimal((int)tmp, dst);
    for (; scale + 7 <= 0; my_shift_left_scale(dst, 1), scale++) {
    }
    setScale(dst, scale + 6);
    if (minus) setSignNegative(dst);
  }
  return flag;
}

int my_from_decimal_to_int(my_decimal src, int *dst) {
  int flag = 0;
  if ((src.bits[3] != my_INFINITY) && (src.bits[3] != my_NEGATIVE_INFINITY) &&
      dst) {
    if (getSign(&src)) {
      setSignZero(&src);
      src = my_TruncateScale(src);
      setSignNegative(&src);
    } else {
      src = my_TruncateScale(src);
    }
    if (FindFirstNotZeroBit(&src) > 31) {
      flag = 1;
    } else {
      if (!getSign(&src) && (src.bits[0] <= INT_MAX))
        *dst = src.bits[0];
      else if (getSign(&src) && (src.bits[0] <= (unsigned int)INT_MIN))
        *dst = -src.bits[0];
      else
        flag = 1;
    }
  } else {
    flag = 1;
  }
  return flag;
}

int my_from_decimal_to_float(my_decimal src, float *dst) {
  int flag = 0;
  if (src.bits[3] == my_INFINITY || src.bits[3] == my_NEGATIVE_INFINITY) {
    flag = 1;
  } else {
    int scale = 0;
    double res = 0.0;
    for (int i = 0; i < 96; i++) {
      if (getBit(src, i)) {
        res += pow(2, i);
      }
    }
    if (getSign(&src)) {
      setSignZero(&src);
      scale = getScale(&src);
      setSignNegative(&src);
    } else {
      scale = getScale(&src);
    }
    if (scale != 0) {
      for (int i = scale; i > 0; i--) {
        res /= 10.0;
      }
    }
    if (getSign(&src))
      *dst = (float)res * (-1);
    else
      *dst = (float)res;
  }
  return flag;
}
// CONVERTERS ADDITIONAL FUNCTIONS
void my_decimal_to_default(my_decimal *dec) {
  dec->bits[0] = my_ZERO;
  dec->bits[1] = my_ZERO;
  dec->bits[2] = my_ZERO;
  dec->bits[3] = my_ZERO;
}

void setSignNegative(my_decimal *current) {
  unsigned int sign = 1u;
  current->bits[3] |= sign << 31;
}

void clearScale(my_decimal *current) { current->bits[3] = 0x00000000; }

int getBinExp(float src) {
  int i = 7, exp = 0;
  unsigned fbits = float_binary(src);
  fbits <<= 1u;
  for (unsigned int mask = MASK_FIRST_BIT; i >= 0; mask >>= 1, i--) {
    if (!!(fbits & mask)) exp += pow(2, i);
  }
  return exp - 127;
}

unsigned int float_binary(float src) {
  unsigned int fbits = *((unsigned int *)&src);
  return fbits;
}

int my_shift_left_scale(my_decimal *dec, unsigned int value) {
  int flag = 0;
  unsigned int exp = getScale(dec);
  if ((value + exp) < 29) {
    my_decimal res = *dec;
    for (unsigned int s = 0; s < value && !flag; s++, exp++) {
      unsigned int carry = 0;
      for (int i = 0; i < 3; i++) {
        unsigned long long tmp = (unsigned long long)res.bits[i] * 10 + carry;
        carry = tmp / MASK_CARRY_UNIT;
        res.bits[i] = tmp % MASK_CARRY_UNIT;
      }
      if (carry > 0) flag = 1;
    }
    if (!flag) {
      *dec = res;
      dec->bits[3] = (dec->bits[3] & MASK_FIRST_BIT) | (exp << 16);
    }
  } else {
    flag = 1;
  }
  return flag;
}

my_decimal my_TruncateScale(my_decimal dec) {
  unsigned int exp = getScale(&dec);
  my_shift_right_scale(&dec, exp);
  return dec;
}

int FindFirstNotZeroBit(my_decimal *current) {
  int index = 0, flag = 0;
  for (unsigned int i = 0; i <= 2; i++) {
    for (unsigned int mask = MASK_FIRST_BIT; mask; mask >>= 1, index++) {
      if (current->bits[i] & mask) flag = index;
    }
  }
  return flag;
}

int my_shift_right_scale(my_decimal *dec, unsigned int value) {
  int flag = 0;
  unsigned int exp = getScale(dec);
  if (value <= exp) {
    my_decimal res = *dec;
    for (unsigned int s = 0; s < value; s++, exp--) {
      unsigned int carry = 0;
      for (int i = 2; i >= 0; i--) {
        unsigned long long tmp =
            (unsigned long long)res.bits[i] + carry * MASK_CARRY_UNIT;
        carry = tmp % 10;
        res.bits[i] = tmp / 10;
      }
    }
    *dec = res;
    dec->bits[3] = (dec->bits[3] & MASK_FIRST_BIT) | (exp << 16);
  } else {
    flag = 1;
  }
  return flag;
}

int getSign(my_decimal *current) { return current->bits[3] >> 31; }

int getScale(my_decimal *current) { return (current->bits[3] >> 16); }

int getBit(my_decimal current, int i) {
  unsigned int mask = 1 << i % 32;
  int count = i / 32;
  return !!(current.bits[count] & mask);
}

void setSignZero(my_decimal *current) {
  unsigned int sign = 1u;
  current->bits[3] &= ~(sign << 31);
}

int setScale(my_decimal *current, int scale) {
  return current->bits[3] = scale << 16;
}

// OTHER FUNCTIONS
int my_floor(my_decimal value, my_decimal *result) {
  /* Функция округляет указанное Decimal число до ближайшего целого
     числа в сторону отрицательной бесконечности */

  int minus = 0;

  errno = 0;
  if (is_set_bit(value.bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1))) {
    minus = 1;
    value.bits[BIT_VECTORS_NUM] =
        reset_bit(value.bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1));
  }
  if (value.bits[BIT_VECTORS_NUM] > SCALE_28) {
    errno = 1;
  } else {
    if (value.bits[BIT_VECTORS_NUM] > 0) {
      my_truncate(value, result);
      if (minus && (value.bits[0] || value.bits[1] || value.bits[2])) {
        my_decimal one = {{1, 0, 0, 0}};
        my_decimal temp = {{0}};

        my_add(*result, one, &temp);
        *result = temp;
      }
    } else {
      *result = value;
    }
    if (minus) {
      result->bits[BIT_VECTORS_NUM] =
          set_bit(result->bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1));
    }
  }
  return errno;
}

int my_round(my_decimal value, my_decimal *result) {
  /* Функция округляет Decimal до ближайшего целого числа */
  int minus = 0;

  errno = 0;
  if (is_set_bit(value.bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1))) {
    minus = 1;
    value.bits[BIT_VECTORS_NUM] =
        reset_bit(value.bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1));
  }
  if (value.bits[BIT_VECTORS_NUM] > SCALE_28) {
    errno = 1;
  } else {
    if (value.bits[BIT_VECTORS_NUM] > 0) {
      my_decimal five = {{5, 0, 0, SCALE_1}};
      my_decimal temp = {{0}};

      my_add(value, five, &temp);
      value = temp;
      my_truncate(value, result);
    } else {
      *result = value;
    }
    if (minus) {
      result->bits[BIT_VECTORS_NUM] =
          set_bit(result->bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1));
    }
  }
  return errno;
}

int my_truncate(my_decimal value, my_decimal *result) {
  /* Функция возвращает целые цифры указанного Decimal числа; любые
     дробные цифры отбрасываются, включая конечные нули. */
  int minus = 0;

  errno = 0;
  if (is_set_bit(value.bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1))) {
    minus = 1;
    value.bits[BIT_VECTORS_NUM] =
        reset_bit(value.bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1));
  }
  if (value.bits[BIT_VECTORS_NUM] > SCALE_28) {
    errno = 1;
  } else {
    if (value.bits[BIT_VECTORS_NUM] > 0) {
      big_decimal b_ten = {{10, 0, 0, 0, 0, 0, 0, 0}};
      big_decimal b_value = {{0}};

      convert_to_big_decimal(&value, &b_value);

      while (b_value.bits[BIG_BIT_VECTORS_NUM]) {
        big_decimal b_tmp = {0};

        decrease_scale(&b_value);
        big_integer_division(&b_value, &b_ten, &b_tmp);
        b_tmp.bits[BIG_BIT_VECTORS_NUM] = b_value.bits[BIG_BIT_VECTORS_NUM];
        b_value = b_tmp;
      }
      convert_to_my_decimal(&b_value, &value);
    }
    *result = value;
    if (minus) {
      result->bits[BIT_VECTORS_NUM] =
          set_bit(result->bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1));
    }
  }
  return errno;
}

int my_negate(my_decimal value, my_decimal *result) {
  /* Фунция возвращает результат умножения указанного Decimal на -1. */
  int minus = 0;

  errno = 0;
  if (is_set_bit(value.bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1))) {
    minus = 1;
    value.bits[BIT_VECTORS_NUM] =
        reset_bit(value.bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1));
  }
  if (value.bits[BIT_VECTORS_NUM] > SCALE_28) {
    errno = 1;
  } else {
    *result = value;
    if (!minus) {
      result->bits[BIT_VECTORS_NUM] =
          set_bit(result->bits[BIT_VECTORS_NUM], (INT_BITS_NUM - 1));
    }
  }

  return errno;
}

void big_bank_round(big_decimal *field) {
  /* Функция для округления биг_децимала при привышении максимального скейла
   * децимала */

  int minus = 0;

  if (is_set_bit(field->bits[BIG_BIT_VECTORS_NUM], (INT_BITS_NUM - 1))) {
    field->bits[BIG_BIT_VECTORS_NUM] =
        reset_bit(field->bits[BIG_BIT_VECTORS_NUM], (INT_BITS_NUM - 1));
    minus = 1;
  }
  if (field->bits[BIG_BIT_VECTORS_NUM] > SCALE_28 ||
      (field->bits[3] || field->bits[4] || field->bits[5] || field->bits[6])) {
    big_decimal temp_field = *field;
    big_decimal temp_comparision = {{0}};
    big_decimal ten = {{10, 0, 0, 0, 0, 0, 0, 0}};

    while (((temp_field.bits[BIG_BIT_VECTORS_NUM] > SCALE_28) ||
            temp_field.bits[3] || temp_field.bits[4] || temp_field.bits[5] ||
            temp_field.bits[6]) &&
           (temp_field.bits[BIG_BIT_VECTORS_NUM] > 0) &&
           (!is_big_greater(&ten, &temp_field))) {
      big_decimal field_temp = {{0}};

      decrease_scale(&temp_field);
      big_integer_division(&temp_field, &ten, &field_temp);
      field_temp.bits[BIG_BIT_VECTORS_NUM] =
          temp_field.bits[BIG_BIT_VECTORS_NUM];
      temp_field = field_temp;
    }
    big_sub(*field, temp_field, &temp_comparision);
    big_decimal result = temp_field;
    unsigned int result_scale = result.bits[BIG_BIT_VECTORS_NUM];
    big_decimal one = {{1, 0, 0, 0, 0, 0, 0, result_scale}};
    big_decimal five = {{5, 0, 0, 0, 0, 0, 0, (SCALE_1 + result_scale)}};

    scale_normalization(&five, &temp_comparision);

    if (is_set_bit(result.bits[0], 0)) {
      if (is_big_greater(&temp_comparision, &five) ||
          is_big_equal(&temp_comparision, &five)) {
        big_decimal temp_result = {0};
        big_add(result, one, &temp_result);
        result = temp_result;
      }
    } else {
      if (is_big_greater(&temp_comparision, &five)) {
        big_decimal temp_result = {0};
        big_add(result, one, &temp_result);
        result = temp_result;
      }
    }
    *field = result;
  }
  if (minus) {
    field->bits[BIG_BIT_VECTORS_NUM] =
        set_bit(field->bits[BIG_BIT_VECTORS_NUM], (INT_BITS_NUM - 1));
  }
}

// BITWISE OPERATIONS
unsigned is_set_bit(unsigned number, int index) {
  /* Функция для проверки положения бита по индексу. Возвращает 1, если бит == 1
  и 0, если бит == 0 */

  return (number & (1u << index)) != 0;
}

unsigned set_bit(unsigned number, int index) {
  /* Функция для установки бита в 1 по указанному индексу */

  return number | (1u << index);
}

unsigned inverse_bit(unsigned number, int index) {
  /* Функция для инвертирования бита по индексу */

  return number ^ (1u << index);
}

unsigned reset_bit(unsigned number, int index) {
  /* Функция для обнуления бита */

  return number & ~(1u << index);
}
