/**
 * @file tcc.h
 * @brief Thai Character Cluster (TCC) tokenization
 * 
 * Implementation of tokenizer according to Thai Character Clusters (TCCs)
 * rules proposed by Theeramunkong et al. 2000.
 * 
 * Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
 * 
 * @author CThaiNLP
 * @date 2026
 */

#ifndef TCC_H
#define TCC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get valid Thai Character Cluster breaking positions
 * 
 * @param text Input Thai text (UTF-8 encoded)
 * @param positions Output array of byte positions (caller must free)
 * @return Number of positions found
 */
int tcc_pos(const char* text, int** positions);

/**
 * @brief Segment text into Thai Character Clusters
 * 
 * @param text Input text (UTF-8 encoded)
 * @param token_count Output parameter for number of tokens found
 * @return Array of strings (tokens), caller must free with tcc_free_result()
 *         Returns NULL on error or empty text
 */
char** tcc_segment(const char* text, int* token_count);

/**
 * @brief Free memory allocated by tcc_segment
 * 
 * @param tokens Array of tokens returned by tcc_segment()
 * @param token_count Number of tokens in the array
 */
void tcc_free_result(char** tokens, int token_count);

#ifdef __cplusplus
}
#endif

#endif /* TCC_H */
