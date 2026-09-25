/**
 * @file tcc.c
 * @brief Thai Character Cluster (TCC) implementation
 * 
 * Based on rules proposed by Theeramunkong et al. 2000
 * and improved rules used in PyThaiNLP's newmm
 */

#include "tcc.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Thai Unicode ranges */
#define THAI_START 0x0E00
#define THAI_END   0x0E7F

/* Thai character classes */
#define is_thai_consonant(c) ((c) >= 0x0E01 && (c) <= 0x0E2E)
#define is_thai_vowel_above(c) ((c) >= 0x0E34 && (c) <= 0x0E37)
#define is_thai_vowel_below(c) ((c) == 0x0E38 || (c) == 0x0E39)
#define is_thai_tone(c) ((c) >= 0x0E48 && (c) <= 0x0E4B)
#define is_thai_sign(c) ((c) == 0x0E4C || (c) == 0x0E4D || (c) == 0x0E4E)
#define is_thai_vowel_follow(c) ((c) >= 0x0E30 && (c) <= 0x0E33)
#define is_thai_vowel_lead(c) ((c) >= 0x0E40 && (c) <= 0x0E44)

/* UTF-8 helper functions */
static int utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

static int get_utf8_codepoint(const char* str, int* byte_len) {
    unsigned char c = (unsigned char)str[0];
    int len = utf8_char_len(c);
    int codepoint = 0;
    
    if (len == 1) {
        codepoint = c;
    } else if (len == 2) {
        codepoint = ((c & 0x1F) << 6) | (str[1] & 0x3F);
    } else if (len == 3) {
        codepoint = ((c & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
    } else if (len == 4) {
        codepoint = ((c & 0x07) << 18) | ((str[1] & 0x3F) << 12) | 
                    ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
    }
    
    *byte_len = len;
    return codepoint;
}

/* Simplified TCC detection - matches basic Thai character clusters */
static int get_tcc_length(const char* text) {
    int byte_len;
    int cp = get_utf8_codepoint(text, &byte_len);
    int total_len = byte_len;
    const char* ptr = text + byte_len;
    
    /* Leading vowel (เ, แ, โ, ใ, ไ) */
    if (is_thai_vowel_lead(cp)) {
        /* Must be followed by consonant */
        if (*ptr) {
            cp = get_utf8_codepoint(ptr, &byte_len);
            if (is_thai_consonant(cp)) {
                total_len += byte_len;
                ptr += byte_len;
                
                /* Optional: consonant */
                if (*ptr) {
                    int next_cp = get_utf8_codepoint(ptr, &byte_len);
                    if (is_thai_consonant(next_cp)) {
                        total_len += byte_len;
                        ptr += byte_len;
                    }
                }
                
                /* Optional: tone mark or other diacritics */
                while (*ptr) {
                    int next_cp = get_utf8_codepoint(ptr, &byte_len);
                    if (is_thai_tone(next_cp) || is_thai_sign(next_cp) || 
                        is_thai_vowel_above(next_cp) || is_thai_vowel_below(next_cp)) {
                        total_len += byte_len;
                        ptr += byte_len;
                    } else {
                        break;
                    }
                }
            }
        }
        return total_len;
    }
    
    /* Consonant-based cluster */
    if (is_thai_consonant(cp)) {
        /* Optional: additional consonant */
        if (*ptr) {
            int next_cp = get_utf8_codepoint(ptr, &byte_len);
            if (is_thai_consonant(next_cp)) {
                total_len += byte_len;
                ptr += byte_len;
            }
        }
        
        /* Optional: tone marks, vowels, signs */
        while (*ptr) {
            int next_cp = get_utf8_codepoint(ptr, &byte_len);
            if (is_thai_tone(next_cp) || is_thai_sign(next_cp) || 
                is_thai_vowel_above(next_cp) || is_thai_vowel_below(next_cp) ||
                is_thai_vowel_follow(next_cp)) {
                total_len += byte_len;
                ptr += byte_len;
            } else {
                break;
            }
        }
        
        return total_len;
    }
    
    /* Single character (non-Thai or standalone) */
    return byte_len;
}

int tcc_pos(const char* text, int** positions) {
    if (!text || !positions) return 0;
    
    int len = strlen(text);
    if (len == 0) return 0;
    
    /* Allocate initial array */
    int capacity = 100;
    *positions = (int*)malloc(capacity * sizeof(int));
    if (!*positions) return 0;
    
    int count = 0;
    const char* ptr = text;
    int byte_pos = 0;
    
    while (*ptr) {
        int cluster_len = get_tcc_length(ptr);
        byte_pos += cluster_len;
        
        /* Add position */
        if (count >= capacity) {
            capacity *= 2;
            int* new_positions = (int*)realloc(*positions, capacity * sizeof(int));
            if (!new_positions) {
                free(*positions);
                return 0;
            }
            *positions = new_positions;
        }
        
        (*positions)[count++] = byte_pos;
        ptr += cluster_len;
    }
    
    return count;
}

char** tcc_segment(const char* text, int* token_count) {
    if (!text || !token_count) {
        if (token_count) *token_count = 0;
        return NULL;
    }
    
    int len = strlen(text);
    if (len == 0) {
        *token_count = 0;
        return NULL;
    }
    
    int* positions = NULL;
    int count = tcc_pos(text, &positions);
    if (count <= 0 || !positions) {
        *token_count = 0;
        return NULL;
    }
    
    char** tokens = (char**)malloc(count * sizeof(char*));
    if (!tokens) {
        free(positions);
        *token_count = 0;
        return NULL;
    }
    
    int prev_pos = 0;
    for (int i = 0; i < count; i++) {
        int cur_pos = positions[i];
        int tok_len = cur_pos - prev_pos;
        tokens[i] = (char*)malloc(tok_len + 1);
        if (!tokens[i]) {
            for (int j = 0; j < i; j++) free(tokens[j]);
            free(tokens);
            free(positions);
            *token_count = 0;
            return NULL;
        }
        memcpy(tokens[i], text + prev_pos, tok_len);
        tokens[i][tok_len] = '\0';
        prev_pos = cur_pos;
    }
    
    free(positions);
    *token_count = count;
    return tokens;
}

void tcc_free_result(char** tokens, int token_count) {
    if (!tokens) return;
    for (int i = 0; i < token_count; i++) {
        if (tokens[i]) free(tokens[i]);
    }
    free(tokens);
}
