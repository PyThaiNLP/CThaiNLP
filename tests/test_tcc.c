/**
 * @file test_tcc.c
 * @brief Test suite for Thai Character Cluster (TCC)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tcc.h"

int main() {
    printf("=== CThaiNLP TCC Test Suite ===\n");
    
    int passed = 0;
    int total = 0;
    
    /* Test 1: basic word segmentation */
    {
        total++;
        int count = 0;
        char** tokens = tcc_segment("ฉันไปโรงเรียน", &count);
        /* Expected: ['ฉั', 'น', 'ไป', 'โรง', 'เรี', 'ยน'] */
        if (tokens && count == 6 &&
            strcmp(tokens[0], "ฉั") == 0 &&
            strcmp(tokens[1], "น") == 0 &&
            strcmp(tokens[2], "ไป") == 0 &&
            strcmp(tokens[3], "โรง") == 0 &&
            strcmp(tokens[4], "เรี") == 0 &&
            strcmp(tokens[5], "ยน") == 0) {
            printf("[Test %d] TCC basic word segmentation: PASS\n", total);
            passed++;
        } else {
            printf("[Test %d] TCC basic word segmentation: FAIL (count=%d)\n", total, count);
        }
        tcc_free_result(tokens, count);
    }
    
    /* Test 2: empty string */
    {
        total++;
        int count = 0;
        char** tokens = tcc_segment("", &count);
        if (!tokens && count == 0) {
            printf("[Test %d] TCC empty string: PASS\n", total);
            passed++;
        } else {
            printf("[Test %d] TCC empty string: FAIL\n", total);
        }
        tcc_free_result(tokens, count);
    }
    
    /* Test 3: tcc_pos */
    {
        total++;
        int* positions = NULL;
        int count = tcc_pos("ฉันไปโรงเรียน", &positions);
        if (positions && count == 6) {
            printf("[Test %d] TCC positions: PASS (count=%d)\n", total, count);
            passed++;
        } else {
            printf("[Test %d] TCC positions: FAIL\n", total);
        }
        free(positions);
    }
    
    printf("\n=== TCC Test Summary: %d/%d passed ===\n", passed, total);
    return (passed == total) ? 0 : 1;
}
