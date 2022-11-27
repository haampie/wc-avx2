#define BUF_SIZE 262144

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>

#include <immintrin.h>

int main() {
  char buf_unaligned[BUF_SIZE + 32];
  char *buf = (char *)(((uintptr_t)buf_unaligned + 31) & -32);
  size_t num_words = 0;
  int prev = 0, curr = 0;

  while (1) {
    size_t num_read = read(STDIN_FILENO, buf, BUF_SIZE);

    __m256i space = _mm256_set1_epi8(' ');
    __m256i newline = _mm256_set1_epi8('\n');
    __m256i tab = _mm256_set1_epi8('\t');
    __m256i vtab = _mm256_set1_epi8('\v');
    __m256i feed = _mm256_set1_epi8('\f');
    __m256i carriage = _mm256_set1_epi8('\r');

    const char *p = buf;

    for (; p - buf + 32 * 4 <= num_read; p += 32 * 4) {
      __m256i bytes_1 = _mm256_load_si256((__m256i *)(p + 0));
      __m256i bytes_2 = _mm256_load_si256((__m256i *)(p + 32));
      __m256i bytes_3 = _mm256_load_si256((__m256i *)(p + 64));
      __m256i bytes_4 = _mm256_load_si256((__m256i *)(p + 96));

      unsigned int m1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_1, space));
      m1 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_1, newline));
      m1 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_1, tab));
      m1 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_1, vtab));
      m1 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_1, feed));
      m1 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_1, carriage));

      unsigned int m2 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_2, space));
      m2 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_2, newline));
      m2 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_2, tab));
      m2 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_2, vtab));
      m2 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_2, feed));
      m2 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_2, carriage));

      unsigned int m3 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_3, space));
      m3 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_3, newline));
      m3 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_3, tab));
      m3 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_3, vtab));
      m3 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_3, feed));
      m3 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_3, carriage));

      unsigned int m4 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_4, space));
      m4 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_4, newline));
      m4 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_4, tab));
      m4 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_4, vtab));
      m4 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_4, feed));
      m4 |= _mm256_movemask_epi8(_mm256_cmpeq_epi8(bytes_4, carriage));

      while (m1) {
        curr = __builtin_ffs(m1);
        if (curr > prev + 1)
          ++num_words;
        prev = curr;
        if (curr == 32)
          break;
        m1 = (m1 >> curr) << curr;
      }

      prev -= 32;

      while (m2) {
        curr = __builtin_ffs(m2);
        if (curr > prev + 1)
          ++num_words;
        prev = curr;
        if (curr == 32)
          break;
        m2 = (m2 >> curr) << curr;
      }

      prev -= 32;

      while (m3) {
        curr = __builtin_ffs(m3);
        if (curr > prev + 1)
          ++num_words;
        prev = curr;
        if (curr == 32)
          break;
        m3 = (m3 >> curr) << curr;
      }

      prev -= 32;

      while (m4) {
        curr = __builtin_ffs(m4);
        if (curr > prev + 1)
          ++num_words;
        prev = curr;
        if (curr == 32)
          break;
        m4 = (m4 >> curr) << curr;
      }

      prev -= 32;
    }

    int remainder = num_read - (p - buf);

    for (size_t j = 0; j < remainder; ++j) {
      char c = p[j];
      if (c == ' ' || c == '\n' || c == '\t' || c == '\v' || c == '\f' ||
          c == '\r') {
        curr = j + 1;
        if (curr > prev + 1)
          ++num_words;
        prev = curr;
      }
    }

    prev -= remainder;

    if (num_read < BUF_SIZE) {
      if (prev != 0)
        ++num_words;
      break;
    }
  }

  printf("%zu\n", num_words);
}
