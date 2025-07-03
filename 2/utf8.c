#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint8_t get_prefix(uint8_t i) {
  uint8_t mask = 0x80;
  uint8_t prefix = 0;

  //	printf("Mask (i = %d) %d\n", i, (mask >> prefix) & i);

  while (((mask >> prefix) & i) != 0) {
    //		printf("%d\n", prefix);
    prefix++;
  }

  return prefix;
}

uint64_t parse_utf8(uint8_t *input, uint64_t input_len, uint32_t *output) {
  uint32_t saved_code_point = 0;
  uint8_t bytes_allowed = 0;
  uint8_t bytes_used = 0;
  uint64_t code_points_written = 0;

  for (int i = 0; i < input_len; i++) {
    uint8_t prefix = get_prefix(*input);
    //		printf("prefix: %d\n", prefix);

    // Check if prefix is not one and it's supposed to be a continuation byte
    if (bytes_allowed > 0 && prefix != 1) {
      // printf("Expected continuation byte, recieved non-continuation byte\n");
      exit(EXIT_FAILURE);
    }

    if (prefix == 0) {
      *output = (uint32_t)(*input);
      output++;
      code_points_written++;
    } else if (prefix == 1) {
      if (bytes_allowed == 0) {
        // printf("Continuation byte in place of starting byte\n");
        exit(EXIT_FAILURE);
      }

      if (bytes_used < bytes_allowed) {
        saved_code_point <<= 6;
        saved_code_point |= (*input & 0x3F);
        // printf("%d\n", saved_code_point);
        bytes_used++;
        if (bytes_used == bytes_allowed) {
          *output = saved_code_point;
          code_points_written++;
          saved_code_point = 0;
          bytes_allowed = 0;
          bytes_used = 0;
          output++;
        }
      }
    } else if (prefix < 5) {
      if ((i + prefix) <= input_len) {
        uint8_t shift = prefix + 1;
        saved_code_point = *input & (0xFF >> shift);
        bytes_allowed = prefix;
        bytes_used = 1;

      } else {
        // printf("Prefix too long on byte %x!\n", *input);
        exit(EXIT_FAILURE);
      }
    } else {
      exit(EXIT_FAILURE);
    }

    input++;
  }
  // printf("code_points_written: %d\n", code_points_written);
  return code_points_written;
}
