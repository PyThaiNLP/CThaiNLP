/**
 * @file soundex.c
 * @brief Thai soundex algorithms implementation (LK82, Udom83)
 */

#include "soundex.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* UTF-8 decoding helper */
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

static void append_utf8(char* dest, int* dest_len, int codepoint) {
    if (codepoint < 0x80) {
        dest[(*dest_len)++] = (char)codepoint;
    } else if (codepoint < 0x800) {
        dest[(*dest_len)++] = (char)(0xC0 | (codepoint >> 6));
        dest[(*dest_len)++] = (char)(0x80 | (codepoint & 0x3F));
    } else if (codepoint < 0x10000) {
        dest[(*dest_len)++] = (char)(0xE0 | (codepoint >> 12));
        dest[(*dest_len)++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        dest[(*dest_len)++] = (char)(0x80 | (codepoint & 0x3F));
    }
}

/* LK82 translation table 1 (initial consonants) */
static int lk82_trans1(int cp) {
    switch (cp) {
        case 0x0E01: case 0x0E02: case 0x0E03: case 0x0E04: case 0x0E05: case 0x0E06:
            return 0x0E01; /* ก */
        case 0x0E07: return 0x0E07; /* ง */
        case 0x0E08: return 0x0E08; /* จ */
        case 0x0E09: case 0x0E0A: case 0x0E0C:
            return 0x0E0A; /* ช */
        case 0x0E0B: case 0x0E24: case 0x0E26: case 0x0E28: case 0x0E29: case 0x0E2A:
            return 0x0E0B; /* ซ */
        case 0x0E0D: case 0x0E22:
            return 0x0E22; /* ย */
        case 0x0E0E: case 0x0E14:
            return 0x0E14; /* ด */
        case 0x0E0F: case 0x0E15:
            return 0x0E15; /* ต */
        case 0x0E10: case 0x0E11: case 0x0E12: case 0x0E16: case 0x0E17: case 0x0E18:
            return 0x0E17; /* ท */
        case 0x0E13: case 0x0E19:
            return 0x0E19; /* น */
        case 0x0E1A: return 0x0E1A; /* บ */
        case 0x0E1B: return 0x0E1B; /* ป */
        case 0x0E1C: case 0x0E1E: case 0x0E20:
            return 0x0E1E; /* พ */
        case 0x0E1D: case 0x0E1F:
            return 0x0E1F; /* ฟ */
        case 0x0E21: return 0x0E21; /* ม */
        case 0x0E23: case 0x0E25: case 0x0E2C:
            return 0x0E23; /* ร */
        case 0x0E27: return 0x0E27; /* ว */
        case 0x0E2B: case 0x0E2E:
            return 0x0E2E; /* ฮ -> in table 'หห' so return ห (0x0E2B) */
        case 0x0E2D: return 0x0E2D; /* อ */
        default: return cp;
    }
}

/* LK82 translation table 2 */
static char lk82_trans2(int cp) {
    switch (cp) {
        case 0x0E01: case 0x0E02: case 0x0E03: case 0x0E04: case 0x0E05: case 0x0E06:
            return '1';
        case 0x0E07: return '2';
        case 0x0E08: case 0x0E09: case 0x0E0A: case 0x0E0B: case 0x0E0C:
        case 0x0E0E: case 0x0E0F: case 0x0E10: case 0x0E11: case 0x0E12:
        case 0x0E14: case 0x0E15: case 0x0E16: case 0x0E17: case 0x0E18:
        case 0x0E28: case 0x0E29: case 0x0E2A:
            return '3';
        case 0x0E0D: case 0x0E13: case 0x0E19: case 0x0E23: case 0x0E25:
        case 0x0E2C: case 0x0E24: case 0x0E26:
            return '4';
        case 0x0E1A: case 0x0E1B: case 0x0E1E: case 0x0E1F: case 0x0E20:
        case 0x0E1C: case 0x0E1D:
            return '5';
        case 0x0E21: case 0x0E33:
            return '6';
        case 0x0E22: case 0x0E27: case 0x0E44: case 0x0E43:
            return '7';
        case 0x0E2B: case 0x0E2E:
            return '8';
        case 0x0E32: case 0x0E45:
            return '9';
        case 0x0E36: case 0x0E37:
            return 'A';
        case 0x0E40: return 'B';
        case 0x0E41: return 'C';
        case 0x0E42: return 'D';
        case 0x0E38: case 0x0E39:
            return 'E';
        case 0x0E2D: return 'F';
        default: return '\0';
    }
}

/* Udom83 trans 1 */
static int udom83_trans1(int cp) {
    switch (cp) {
        case 0x0E01: return 0x0E01; /* ก */
        case 0x0E02: case 0x0E03: case 0x0E04: case 0x0E05: case 0x0E06:
            return 0x0E02; /* ข */
        case 0x0E07: return 0x0E07; /* ง */
        case 0x0E08: return 0x0E08; /* จ */
        case 0x0E09: case 0x0E0A: case 0x0E0C:
            return 0x0E0A; /* ช */
        case 0x0E0B: case 0x0E28: case 0x0E29: case 0x0E2A:
            return 0x0E2A; /* ส */
        case 0x0E0E: case 0x0E14:
            return 0x0E14; /* ด */
        case 0x0E0F: case 0x0E15:
            return 0x0E15; /* ต */
        case 0x0E10: case 0x0E11: case 0x0E12: case 0x0E16: case 0x0E17: case 0x0E18:
            return 0x0E17; /* ท */
        case 0x0E13: case 0x0E19:
            return 0x0E19; /* น */
        case 0x0E1A: return 0x0E1A; /* บ */
        case 0x0E1B: return 0x0E1B; /* ป */
        case 0x0E1C: case 0x0E1E: case 0x0E20:
            return 0x0E1E; /* พ */
        case 0x0E1D: case 0x0E1F:
            return 0x0E1F; /* ฟ */
        case 0x0E21: return 0x0E21; /* ม */
        case 0x0E0D: case 0x0E22:
            return 0x0E22; /* ย */
        case 0x0E23: case 0x0E25: case 0x0E2C: case 0x0E24: case 0x0E26:
            return 0x0E23; /* ร */
        case 0x0E27: return 0x0E27; /* ว */
        case 0x0E2D: return 0x0E2D; /* อ */
        case 0x0E2B: case 0x0E2E:
            return 0x0E2E; /* ฮ */
        default: return cp;
    }
}

/* Udom83 trans 2 */
static char udom83_trans2(int cp) {
    switch (cp) {
        case 0x0E21: case 0x0E27: case 0x0E33:
            return '0';
        case 0x0E01: case 0x0E02: case 0x0E03: case 0x0E04: case 0x0E05: case 0x0E06:
            return '1';
        case 0x0E07: return '2';
        case 0x0E22: case 0x0E0D: case 0x0E13: case 0x0E19:
            return '3';
        case 0x0E0E: case 0x0E0F: case 0x0E14: case 0x0E15: case 0x0E28: case 0x0E29: case 0x0E2A:
            return '4';
        case 0x0E1A: case 0x0E1B: case 0x0E1E: case 0x0E20:
            return '5';
        case 0x0E1C: case 0x0E1D: case 0x0E1F: case 0x0E2B: case 0x0E2D: case 0x0E2E:
            return '6';
        case 0x0E08: case 0x0E09: case 0x0E0A: case 0x0E0B: case 0x0E0C:
            return '7';
        case 0x0E10: case 0x0E11: case 0x0E12: case 0x0E16: case 0x0E17: case 0x0E18:
            return '8';
        case 0x0E23: case 0x0E24: case 0x0E25: case 0x0E26:
            return '9';
        default: return '\0';
    }
}

static bool is_karant(int cp) {
    return (cp == 0x0E4C); /* ์ */
}

/* Filter karant combinations */
static int filter_karant_and_signs(const int* in_cps, int in_len, int* out_cps) {
    int out_len = 0;
    int i = 0;
    while (i < in_len) {
        /* Check special karant patterns: จน์ มณ์ ณฑ์ ทร์ ตร์ */
        if (i + 2 < in_len && is_karant(in_cps[i + 2])) {
            int c1 = in_cps[i];
            int c2 = in_cps[i + 1];
            if ((c1 == 0x0E08 && c2 == 0x0E19) || /* จน์ */
                (c1 == 0x0E21 && c2 == 0x0E13) || /* มณ์ */
                (c1 == 0x0E13 && c2 == 0x0E11) || /* ณฑ์ */
                (c1 == 0x0E17 && c2 == 0x0E23) || /* ทร์ */
                (c1 == 0x0E15 && c2 == 0x0E23)) { /* ตร์ */
                i += 3;
                continue;
            }
        }
        /* Check [ก-ฮ][ะ-ู]์ */
        if (i + 2 < in_len && is_karant(in_cps[i + 2]) &&
            (in_cps[i] >= 0x0E01 && in_cps[i] <= 0x0E2E) &&
            (in_cps[i + 1] >= 0x0E30 && in_cps[i + 1] <= 0x0E39)) {
            i += 3;
            continue;
        }
        /* Check [ก-ฮ]์ */
        if (i + 1 < in_len && is_karant(in_cps[i + 1]) &&
            (in_cps[i] >= 0x0E01 && in_cps[i] <= 0x0E2E)) {
            i += 2;
            continue;
        }
        /* Check signs: ฯ ฺ ๆ ็ ํ */
        int cp = in_cps[i];
        if (cp == 0x0E2F || cp == 0x0E3A || cp == 0x0E46 || cp == 0x0E47 || cp == 0x0E4D) {
            i++;
            continue;
        }
        /* Tone marks: ่ ้ ๊ ๋ */
        if (cp >= 0x0E48 && cp <= 0x0E4B) {
            i++;
            continue;
        }
        out_cps[out_len++] = cp;
        i++;
    }
    return out_len;
}

char* soundex_lk82(const char* text) {
    if (!text || !*text) return strdup("");
    
    /* Decode to codepoints */
    int raw_cps[256];
    int raw_len = 0;
    const char* ptr = text;
    int b_len;
    while (*ptr && raw_len < 255) {
        raw_cps[raw_len++] = get_utf8_codepoint(ptr, &b_len);
        ptr += b_len;
    }
    
    int cps[256];
    int len = filter_karant_and_signs(raw_cps, raw_len, cps);
    if (len == 0) return strdup("");
    
    char res[256];
    int res_len = 0;
    int idx = 0;
    
    /* First character encoding */
    if (cps[0] >= 0x0E01 && cps[0] <= 0x0E2E) {
        int t1 = lk82_trans1(cps[0]);
        append_utf8(res, &res_len, t1);
        idx = 1;
    } else {
        if (len > 1) {
            int t1 = lk82_trans1(cps[1]);
            append_utf8(res, &res_len, t1);
        }
        char t2 = lk82_trans2(cps[0]);
        if (t2) res[res_len++] = t2;
        idx = 2;
    }
    
    int i_v = -1;
    for (int i = idx; i < len; i++) {
        int c = cps[i];
        if (c == 0x0E30 || c == 0x0E31 || c == 0x0E34 || c == 0x0E35) {
            i_v = i;
        } else if (c == 0x0E32 || c == 0x0E36 || c == 0x0E37 || c == 0x0E39 || c == 0x0E45) {
            i_v = i;
            char t2 = lk82_trans2(c);
            if (t2) res[res_len++] = t2;
        } else if (c == 0x0E38) {
            i_v = i;
            if (i == 0 || (cps[i - 1] != 0x0E15 && cps[i - 1] != 0x0E18)) {
                char t2 = lk82_trans2(c);
                if (t2) res[res_len++] = t2;
            }
        } else if (c == 0x0E2B || c == 0x0E2D) {
            if (i + 1 < len && (cps[i + 1] >= 0x0E36 && cps[i + 1] <= 0x0E39)) {
                char t2 = lk82_trans2(c);
                if (t2) res[res_len++] = t2;
            }
        } else if (c == 0x0E22 || c == 0x0E23 || c == 0x0E24 || c == 0x0E26 || c == 0x0E27) {
            if (i_v == i - 1 || (i + 1 < len && (cps[i + 1] >= 0x0E36 && cps[i + 1] <= 0x0E39))) {
                char t2 = lk82_trans2(c);
                if (t2) res[res_len++] = t2;
            }
        } else {
            char t2 = lk82_trans2(c);
            if (t2) res[res_len++] = t2;
        }
    }
    
    /* Remove repetitions */
    char res2[256];
    int res2_len = 0;
    if (res_len > 0) {
        /* Initial utf-8 character can be multi-byte */
        int first_char_bytes;
        get_utf8_codepoint(res, &first_char_bytes);
        memcpy(res2, res, first_char_bytes);
        res2_len = first_char_bytes;
        
        for (int i = first_char_bytes; i < res_len; i++) {
            if (res[i] != res[i - 1]) {
                res2[res2_len++] = res[i];
            }
        }
    }
    
    /* Fill with '0' until length in characters is 5 */
    int char_count = 0;
    int b = 0;
    while (b < res2_len) {
        int blen;
        get_utf8_codepoint(res2 + b, &blen);
        b += blen;
        char_count++;
    }
    while (char_count < 5 && res2_len < 250) {
        res2[res2_len++] = '0';
        char_count++;
    }
    
    /* Truncate to 5 characters */
    char_count = 0;
    b = 0;
    while (b < res2_len && char_count < 5) {
        int blen;
        get_utf8_codepoint(res2 + b, &blen);
        b += blen;
        char_count++;
    }
    res2[b] = '\0';
    
    return strdup(res2);
}

char* soundex_udom83(const char* text) {
    if (!text || !*text) return strdup("");
    
    /* Decode to codepoints */
    int raw_cps[256];
    int raw_len = 0;
    const char* ptr = text;
    int b_len;
    while (*ptr && raw_len < 255) {
        raw_cps[raw_len++] = get_utf8_codepoint(ptr, &b_len);
        ptr += b_len;
    }
    
    /* Apply Udom83 substitutions:
       รร(เ-ไ) -> ัน\1
       รร([ก-ฮ][ก-ฮเ-ไ]) -> ั\1
       รร([ก-ฮ][ะ-ู่-์]) -> ัน\1
       รร -> ัน
       ไ([ก-ฮ]ย) -> \1
       [ไใ]([ก-ฮ]) -> \1ย
       ำ(ม[ะ-ู]) -> ม\1
       ำม -> ม
       ำ -> ม
    */
    int subst_cps[512];
    int s_len = 0;
    int i = 0;
    while (i < raw_len) {
        /* รร */
        if (i + 1 < raw_len && raw_cps[i] == 0x0E23 && raw_cps[i + 1] == 0x0E23) {
            if (i + 2 < raw_len && raw_cps[i + 2] >= 0x0E40 && raw_cps[i + 2] <= 0x0E44) {
                subst_cps[s_len++] = 0x0E31; /* ั */
                subst_cps[s_len++] = 0x0E19; /* น */
                i += 2;
                continue;
            } else if (i + 3 < raw_len && (raw_cps[i + 2] >= 0x0E01 && raw_cps[i + 2] <= 0x0E2E) &&
                       ((raw_cps[i + 3] >= 0x0E01 && raw_cps[i + 3] <= 0x0E2E) ||
                        (raw_cps[i + 3] >= 0x0E40 && raw_cps[i + 3] <= 0x0E44))) {
                subst_cps[s_len++] = 0x0E31; /* ั */
                i += 2;
                continue;
            } else if (i + 3 < raw_len && (raw_cps[i + 2] >= 0x0E01 && raw_cps[i + 2] <= 0x0E2E) &&
                       ((raw_cps[i + 3] >= 0x0E30 && raw_cps[i + 3] <= 0x0E39) ||
                        (raw_cps[i + 3] >= 0x0E48 && raw_cps[i + 3] <= 0x0E4C))) {
                subst_cps[s_len++] = 0x0E31; /* ั */
                subst_cps[s_len++] = 0x0E19; /* น */
                i += 2;
                continue;
            } else {
                subst_cps[s_len++] = 0x0E31; /* ั */
                subst_cps[s_len++] = 0x0E19; /* น */
                i += 2;
                continue;
            }
        }
        /* ไ([ก-ฮ]ย) */
        if (raw_cps[i] == 0x0E44 && i + 2 < raw_len &&
            (raw_cps[i + 1] >= 0x0E01 && raw_cps[i + 1] <= 0x0E2E) &&
            raw_cps[i + 2] == 0x0E22) {
            subst_cps[s_len++] = raw_cps[i + 1];
            subst_cps[s_len++] = 0x0E22;
            i += 3;
            continue;
        }
        /* [ไใ]([ก-ฮ]) */
        if ((raw_cps[i] == 0x0E44 || raw_cps[i] == 0x0E43) && i + 1 < raw_len &&
            (raw_cps[i + 1] >= 0x0E01 && raw_cps[i + 1] <= 0x0E2E)) {
            subst_cps[s_len++] = raw_cps[i + 1];
            subst_cps[s_len++] = 0x0E22; /* ย */
            i += 2;
            continue;
        }
        /* ำ(ม[ะ-ู]) */
        if (raw_cps[i] == 0x0E33 && i + 2 < raw_len && raw_cps[i + 1] == 0x0E21 &&
            (raw_cps[i + 2] >= 0x0E30 && raw_cps[i + 2] <= 0x0E39)) {
            subst_cps[s_len++] = 0x0E21;
            i++;
            continue;
        }
        /* ำม */
        if (raw_cps[i] == 0x0E33 && i + 1 < raw_len && raw_cps[i + 1] == 0x0E21) {
            subst_cps[s_len++] = 0x0E21;
            i += 2;
            continue;
        }
        /* ำ */
        if (raw_cps[i] == 0x0E33) {
            subst_cps[s_len++] = 0x0E21;
            i++;
            continue;
        }
        subst_cps[s_len++] = raw_cps[i++];
    }
    
    /* Remove karant patterns and vowels/marks (0x0E30 - 0x0E4C) */
    int clean_cps[256];
    int c_len = 0;
    i = 0;
    while (i < s_len) {
        /* Check special karant patterns: จน์ มณ์ ณฑ์ ทร์ ตร์ */
        if (i + 2 < s_len && is_karant(subst_cps[i + 2])) {
            int c1 = subst_cps[i];
            int c2 = subst_cps[i + 1];
            if ((c1 == 0x0E08 && c2 == 0x0E19) ||
                (c1 == 0x0E21 && c2 == 0x0E13) ||
                (c1 == 0x0E13 && c2 == 0x0E11) ||
                (c1 == 0x0E17 && c2 == 0x0E23) ||
                (c1 == 0x0E15 && c2 == 0x0E23)) {
                i += 3;
                continue;
            }
        }
        /* [ก-ฮ][ะ-ู]์ */
        if (i + 2 < s_len && is_karant(subst_cps[i + 2]) &&
            (subst_cps[i] >= 0x0E01 && subst_cps[i] <= 0x0E2E) &&
            (subst_cps[i + 1] >= 0x0E30 && subst_cps[i + 1] <= 0x0E39)) {
            i += 3;
            continue;
        }
        /* [ก-ฮ]์ */
        if (i + 1 < s_len && is_karant(subst_cps[i + 1]) &&
            (subst_cps[i] >= 0x0E01 && subst_cps[i] <= 0x0E2E)) {
            i += 2;
            continue;
        }
        /* Vowels and marks: 0x0E30 - 0x0E4C */
        int cp = subst_cps[i];
        if (cp >= 0x0E30 && cp <= 0x0E4C) {
            i++;
            continue;
        }
        clean_cps[c_len++] = cp;
        i++;
    }
    
    if (c_len == 0) return strdup("");
    
    char res[256];
    int res_len = 0;
    
    /* First character via trans1 */
    int t1 = udom83_trans1(clean_cps[0]);
    append_utf8(res, &res_len, t1);
    
    /* Remaining via trans2 */
    for (int j = 1; j < c_len; j++) {
        char t2 = udom83_trans2(clean_cps[j]);
        if (t2) res[res_len++] = t2;
    }
    
    /* Append 000000 */
    memcpy(res + res_len, "000000", 6);
    res_len += 6;
    res[res_len] = '\0';
    
    /* Truncate to 7 characters */
    int char_count = 0;
    int b = 0;
    while (b < res_len && char_count < 7) {
        int blen;
        get_utf8_codepoint(res + b, &blen);
        b += blen;
        char_count++;
    }
    res[b] = '\0';
    
    return strdup(res);
}

void soundex_free(char* code) {
    if (code) free(code);
}
