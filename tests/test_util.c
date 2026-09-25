/**
 * @file test_util.c
 * @brief Test suite for Thai utility functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

int main() {
    printf("=== CThaiNLP Util Test Suite ===\n");
    int passed = 0;
    int total = 0;
    
    /* Test is_thai_char */
    total++;
    if (is_thai_char("ก") && is_thai_char("๕") && is_thai_char("์") && !is_thai_char("A") && !is_thai_char("1")) {
        printf("[Test %d] is_thai_char: PASS\n", total);
        passed++;
    } else {
        printf("[Test %d] is_thai_char: FAIL\n", total);
    }
    
    /* Test is_thai */
    total++;
    if (is_thai("กาลเวลา", ".") && is_thai("กาลเวลา.", ".") && !is_thai("กาล-เวลา", ".") && is_thai("กาล-เวลา", ".-")) {
        printf("[Test %d] is_thai: PASS\n", total);
        passed++;
    } else {
        printf("[Test %d] is_thai: FAIL\n", total);
    }
    
    /* Test count_thai */
    total++;
    double c1 = count_thai("ไทยเอ็นแอลพี 3.0", NULL);
    double c2 = count_thai("PyThaiNLP 3.0", NULL);
    if (fabs(c1 - 100.0) < 0.01 && fabs(c2 - 0.0) < 0.01) {
        printf("[Test %d] count_thai: PASS\n", total);
        passed++;
    } else {
        printf("[Test %d] count_thai: FAIL (c1=%.2f, c2=%.2f)\n", total, c1, c2);
    }
    
    /* Test arabic_digit_to_thai_digit */
    total++;
    char* th_digit = arabic_digit_to_thai_digit("123,400.25 บาท");
    if (th_digit && strcmp(th_digit, "๑๒๓,๔๐๐.๒๕ บาท") == 0) {
        printf("[Test %d] arabic_digit_to_thai_digit: PASS\n", total);
        passed++;
    } else {
        printf("[Test %d] arabic_digit_to_thai_digit: FAIL (got %s)\n", total, th_digit ? th_digit : "NULL");
    }
    cthainlp_free_string(th_digit);
    
    /* Test thai_digit_to_arabic_digit */
    total++;
    char* ar_digit = thai_digit_to_arabic_digit("๑๒๓,๔๐๐.๒๕ บาท");
    if (ar_digit && strcmp(ar_digit, "123,400.25 บาท") == 0) {
        printf("[Test %d] thai_digit_to_arabic_digit: PASS\n", total);
        passed++;
    } else {
        printf("[Test %d] thai_digit_to_arabic_digit: FAIL (got %s)\n", total, ar_digit ? ar_digit : "NULL");
    }
    cthainlp_free_string(ar_digit);
    
    /* Test remove_tonemark */
    total++;
    char* notone = remove_tonemark("กิ่งก่า");
    if (notone && strcmp(notone, "กิงกา") == 0) {
        printf("[Test %d] remove_tonemark: PASS\n", total);
        passed++;
    } else {
        printf("[Test %d] remove_tonemark: FAIL (got %s)\n", total, notone ? notone : "NULL");
    }
    cthainlp_free_string(notone);
    
    printf("\n=== Util Test Summary: %d/%d passed ===\n", passed, total);
    return (passed == total) ? 0 : 1;
}
