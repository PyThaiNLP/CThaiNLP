/**
 * @file util.h
 * @brief Thai language utility functions
 * 
 * Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
 * 
 * @author CThaiNLP
 * @date 2026
 */

#ifndef CTHAINLP_UTIL_H
#define CTHAINLP_UTIL_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if a Unicode codepoint is a Thai character (0x0E00 - 0x0E7F)
 * 
 * @param codepoint Unicode codepoint
 * @return true if Thai character, false otherwise
 */
bool is_thai_codepoint(int codepoint);

/**
 * @brief Check if a UTF-8 character string is a Thai character
 * 
 * @param utf8_char Pointer to UTF-8 character
 * @return true if Thai character, false otherwise
 */
bool is_thai_char(const char* utf8_char);

/**
 * @brief Check if every character in text is a Thai character (or in ignore_chars)
 * 
 * @param text Input UTF-8 text
 * @param ignore_chars Characters to ignore (can be NULL or empty)
 * @return true if all non-ignored characters are Thai, false otherwise
 */
bool is_thai(const char* text, const char* ignore_chars);

/**
 * @brief Find proportion of Thai characters in text (percentage 0.0 - 100.0)
 * 
 * @param text Input UTF-8 text
 * @param ignore_chars Characters to ignore (if NULL, defaults to whitespace, digits, punctuation)
 * @return Percentage of Thai characters (0.0 - 100.0)
 */
double count_thai(const char* text, const char* ignore_chars);

/**
 * @brief Convert Arabic digits ('0'-'9') to Thai digits ('๐'-'๙')
 * 
 * @param text Input UTF-8 text
 * @return Newly allocated string with Thai digits, caller must free with cthainlp_free_string()
 */
char* arabic_digit_to_thai_digit(const char* text);

/**
 * @brief Convert Thai digits ('๐'-'๙') to Arabic digits ('0'-'9')
 * 
 * @param text Input UTF-8 text
 * @return Newly allocated string with Arabic digits, caller must free with cthainlp_free_string()
 */
char* thai_digit_to_arabic_digit(const char* text);

/**
 * @brief Remove Thai tone marks (ไม้เอก, ไม้โท, ไม้ตรี, ไม้จัตวา) from text
 * 
 * @param text Input UTF-8 text
 * @return Newly allocated string without tone marks, caller must free with cthainlp_free_string()
 */
char* remove_tonemark(const char* text);

/**
 * @brief Free string returned by utility functions
 * 
 * @param str String pointer to free
 */
void cthainlp_free_string(char* str);

#ifdef __cplusplus
}
#endif

#endif /* CTHAINLP_UTIL_H */
