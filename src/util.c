/**
 * @file util.c
 * @brief Thai language utility functions implementation
 */

#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DEFAULT_IGNORE_CHARS " \t\n\r\x0b\x0c" "0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"

static int get_utf8_codepoint(const char* str, int* byte_len) {
    if (!str || !*str) {
        *byte_len = 0;
        return 0;
    }
    unsigned char c = (unsigned char)str[0];
    if ((c & 0x80) == 0) {
        *byte_len = 1;
        return c;
    } else if ((c & 0xE0) == 0xC0) {
        *byte_len = 2;
        return ((c & 0x1F) << 6) | (str[1] & 0x3F);
    } else if ((c & 0xF0) == 0xE0) {
        *byte_len = 3;
        return ((c & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
    } else if ((c & 0xF8) == 0xF0) {
        *byte_len = 4;
        return ((c & 0x07) << 18) | ((str[1] & 0x3F) << 12) | 
               ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
    }
    *byte_len = 1;
    return c;
}

static bool codepoint_in_string(int cp, const char* str) {
    if (!str) return false;
    const char* ptr = str;
    int b_len;
    while (*ptr) {
        int str_cp = get_utf8_codepoint(ptr, &b_len);
        if (str_cp == cp) return true;
        ptr += b_len;
    }
    return false;
}

bool is_thai_codepoint(int codepoint) {
    return (codepoint >= 0x0E00 && codepoint <= 0x0E7F);
}

bool is_thai_char(const char* utf8_char) {
    if (!utf8_char || !*utf8_char) return false;
    int byte_len;
    int cp = get_utf8_codepoint(utf8_char, &byte_len);
    return is_thai_codepoint(cp);
}

bool is_thai(const char* text, const char* ignore_chars) {
    if (!text) return true;
    if (!ignore_chars) ignore_chars = ".";
    
    const char* ptr = text;
    int byte_len;
    while (*ptr) {
        int cp = get_utf8_codepoint(ptr, &byte_len);
        if (!codepoint_in_string(cp, ignore_chars)) {
            if (!is_thai_codepoint(cp)) {
                return false;
            }
        }
        ptr += byte_len;
    }
    return true;
}

double count_thai(const char* text, const char* ignore_chars) {
    if (!text || !*text) return 0.0;
    if (!ignore_chars) ignore_chars = DEFAULT_IGNORE_CHARS;
    
    int num_thai = 0;
    int num_ignore = 0;
    int total_chars = 0;
    
    const char* ptr = text;
    int byte_len;
    while (*ptr) {
        int cp = get_utf8_codepoint(ptr, &byte_len);
        total_chars++;
        if (codepoint_in_string(cp, ignore_chars)) {
            num_ignore++;
        } else if (is_thai_codepoint(cp)) {
            num_thai++;
        }
        ptr += byte_len;
    }
    
    int num_count = total_chars - num_ignore;
    if (num_count <= 0) return 0.0;
    return ((double)num_thai / num_count) * 100.0;
}

char* arabic_digit_to_thai_digit(const char* text) {
    if (!text) return NULL;
    
    /* Calculate required length */
    size_t len = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] >= '0' && text[i] <= '9') {
            len += 3; /* Thai digit is 3 bytes in UTF-8: 0xE0, 0xB9, 0x90 + d */
        } else {
            len += 1;
        }
    }
    
    char* result = (char*)malloc(len + 1);
    if (!result) return NULL;
    
    size_t out_idx = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] >= '0' && text[i] <= '9') {
            int d = text[i] - '0';
            result[out_idx++] = (char)0xE0;
            result[out_idx++] = (char)0xB9;
            result[out_idx++] = (char)(0x90 + d);
        } else {
            result[out_idx++] = text[i];
        }
    }
    result[out_idx] = '\0';
    return result;
}

char* thai_digit_to_arabic_digit(const char* text) {
    if (!text) return NULL;
    
    size_t len = strlen(text);
    char* result = (char*)malloc(len + 1);
    if (!result) return NULL;
    
    size_t in_idx = 0;
    size_t out_idx = 0;
    while (in_idx < len) {
        if (in_idx + 2 < len &&
            (unsigned char)text[in_idx] == 0xE0 &&
            (unsigned char)text[in_idx + 1] == 0xB9 &&
            (unsigned char)text[in_idx + 2] >= 0x90 &&
            (unsigned char)text[in_idx + 2] <= 0x99) {
            int d = (unsigned char)text[in_idx + 2] - 0x90;
            result[out_idx++] = '0' + d;
            in_idx += 3;
        } else {
            result[out_idx++] = text[in_idx++];
        }
    }
    result[out_idx] = '\0';
    return result;
}

char* remove_tonemark(const char* text) {
    if (!text) return NULL;
    
    size_t len = strlen(text);
    char* result = (char*)malloc(len + 1);
    if (!result) return NULL;
    
    size_t in_idx = 0;
    size_t out_idx = 0;
    while (in_idx < len) {
        /* Tone marks are 0x0E48 (0x88) to 0x0E4B (0x8B) */
        if (in_idx + 2 < len &&
            (unsigned char)text[in_idx] == 0xE0 &&
            (unsigned char)text[in_idx + 1] == 0xB9 &&
            (unsigned char)text[in_idx + 2] >= 0x88 &&
            (unsigned char)text[in_idx + 2] <= 0x8B) {
            in_idx += 3; /* skip tone mark */
        } else {
            result[out_idx++] = text[in_idx++];
        }
    }
    result[out_idx] = '\0';
    return result;
}

void cthainlp_free_string(char* str) {
    if (str) {
        free(str);
    }
}
