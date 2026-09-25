/**
 * @file soundex.h
 * @brief Thai soundex algorithms
 * 
 * Implementation of Thai phonetic algorithms (LK82, Udom83).
 * Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
 * 
 * @author CThaiNLP
 * @date 2026
 */

#ifndef CTHAINLP_SOUNDEX_H
#define CTHAINLP_SOUNDEX_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LK82 Thai soundex algorithm
 * 
 * @param text Thai input text (UTF-8 encoded)
 * @return 5-character soundex string, caller must free with soundex_free()
 */
char* soundex_lk82(const char* text);

/**
 * @brief Udom83 Thai soundex algorithm
 * 
 * @param text Thai input text (UTF-8 encoded)
 * @return 7-character soundex string, caller must free with soundex_free()
 */
char* soundex_udom83(const char* text);

/**
 * @brief Free soundex result string
 * 
 * @param code Soundex string to free
 */
void soundex_free(char* code);

#ifdef __cplusplus
}
#endif

#endif /* CTHAINLP_SOUNDEX_H */
