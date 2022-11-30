#define BUF_SIZE 262144

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <immintrin.h>

int main() {
  char buf_unaligned[BUF_SIZE + 32];
  char *buf = (char *)(((uintptr_t)buf_unaligned + 31) & -32);
  size_t num_words = 0;
  unsigned int all_whitespace_0 = -1;

  while (1) {
    size_t num_read = fread(buf, 1, BUF_SIZE, stdin);
    const char *p = buf;

    // Vectorized loop
    for (; p + 128 <= buf + num_read; p += 128) {
      __m256i vec_1 = _mm256_load_si256((__m256i *)(p + 0));
      __m256i vec_2 = _mm256_load_si256((__m256i *)(p + 32));
      __m256i vec_3 = _mm256_load_si256((__m256i *)(p + 64));
      __m256i vec_4 = _mm256_load_si256((__m256i *)(p + 96));
      __m256i whitespace_identity = _mm256_set_epi64x(0x00000d0c0b0a0900, 0x0000000000000020, 0x00000d0c0b0a0900, 0x0000000000000020);
      unsigned int all_whitespace_1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(vec_1, _mm256_shuffle_epi8(whitespace_identity, vec_1)));
      unsigned int all_whitespace_2 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(vec_2, _mm256_shuffle_epi8(whitespace_identity, vec_2)));
      unsigned int all_whitespace_3 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(vec_3, _mm256_shuffle_epi8(whitespace_identity, vec_3)));
      unsigned int all_whitespace_4 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(vec_4, _mm256_shuffle_epi8(whitespace_identity, vec_4)));
      unsigned int word_end_1 = all_whitespace_1 & ~(all_whitespace_1 & ((all_whitespace_1 << 1) | (all_whitespace_0 >> 31)));
      unsigned int word_end_2 = all_whitespace_2 & ~(all_whitespace_2 & ((all_whitespace_2 << 1) | (all_whitespace_1 >> 31)));
      unsigned int word_end_3 = all_whitespace_3 & ~(all_whitespace_3 & ((all_whitespace_3 << 1) | (all_whitespace_2 >> 31)));
      unsigned int word_end_4 = all_whitespace_4 & ~(all_whitespace_4 & ((all_whitespace_4 << 1) | (all_whitespace_3 >> 31)));
      num_words += __builtin_popcount(word_end_1);
      num_words += __builtin_popcount(word_end_2);
      num_words += __builtin_popcount(word_end_3);
      num_words += __builtin_popcount(word_end_4);
      all_whitespace_0 = all_whitespace_4;
    }

    // Clean-up loop
    int is_whitespace = (all_whitespace_0 >> 31) & 1;
    for (; p < buf + num_read; ++p) {
      if (isspace(*p)) {
        if (!is_whitespace) {
          ++num_words;
          is_whitespace = 1;
          continue;
        }
      } else {
        is_whitespace = 0;
      }
    }

    if (num_read < BUF_SIZE) {
      // the last word.
      if (!is_whitespace) ++num_words;
      break;
    }
  }

  printf("%zu\n", num_words);
}
