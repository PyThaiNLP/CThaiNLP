/**
 * @file test_soundex.c
 * @brief Test suite for Thai Soundex algorithms
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "soundex.h"

int main() {
    printf("=== CThaiNLP Soundex Test Suite ===\n");
    int passed = 0;
    int total = 0;
    
    /* Test LK82 */
    {
        total++;
        char* code = soundex_lk82("ลัก");
        if (code && strcmp(code, "ร1000") == 0) {
            printf("[Test %d] LK82 'ลัก': PASS\n", total);
            passed++;
        } else {
            printf("[Test %d] LK82 'ลัก': FAIL (got %s)\n", total, code ? code : "NULL");
        }
        soundex_free(code);
    }
    
    {
        total++;
        char* code = soundex_lk82("รัก");
        if (code && strcmp(code, "ร1000") == 0) {
            printf("[Test %d] LK82 'รัก': PASS\n", total);
            passed++;
        } else {
            printf("[Test %d] LK82 'รัก': FAIL (got %s)\n", total, code ? code : "NULL");
        }
        soundex_free(code);
    }
    
    {
        total++;
        char* code = soundex_lk82("รักษ์");
        if (code && strcmp(code, "ร1000") == 0) {
            printf("[Test %d] LK82 'รักษ์': PASS\n", total);
            passed++;
        } else {
            printf("[Test %d] LK82 'รักษ์': FAIL (got %s)\n", total, code ? code : "NULL");
        }
        soundex_free(code);
    }
    
    /* Test Udom83 */
    {
        total++;
        char* code = soundex_udom83("ลัก");
        if (code && strcmp(code, "ร100000") == 0) {
            printf("[Test %d] Udom83 'ลัก': PASS\n", total);
            passed++;
        } else {
            printf("[Test %d] Udom83 'ลัก': FAIL (got %s)\n", total, code ? code : "NULL");
        }
        soundex_free(code);
    }
    
    {
        total++;
        char* code = soundex_udom83("รัก");
        if (code && strcmp(code, "ร100000") == 0) {
            printf("[Test %d] Udom83 'รัก': PASS\n", total);
            passed++;
        } else {
            printf("[Test %d] Udom83 'รัก': FAIL (got %s)\n", total, code ? code : "NULL");
        }
        soundex_free(code);
    }
    
    {
        total++;
        char* code = soundex_udom83("รักษ์");
        if (code && strcmp(code, "ร100000") == 0) {
            printf("[Test %d] Udom83 'รักษ์': PASS\n", total);
            passed++;
        } else {
            printf("[Test %d] Udom83 'รักษ์': FAIL (got %s)\n", total, code ? code : "NULL");
        }
        soundex_free(code);
    }
    
    printf("\n=== Soundex Test Summary: %d/%d passed ===\n", passed, total);
    return (passed == total) ? 0 : 1;
}
