/**
 * @file count_words.c
 * @brief CLI tool to count Thai words from a text file using CThaiNLP
 *
 * Usage:
 *   count_words [options] <filename>
 *   cat file.txt | count_words [options]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#include "cthainlp.h"

#define HASH_TABLE_SIZE 8192

typedef struct WordFreq {
    char* word;
    int count;
    struct WordFreq* next;
} WordFreq;

static unsigned int hash_string(const char* str) {
    unsigned int hash = 5381;
    int c;
    while ((c = (unsigned char)*str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_TABLE_SIZE;
}

static void add_word_frequency(WordFreq** table, const char* word, int* unique_words) {
    unsigned int idx = hash_string(word);
    WordFreq* cur = table[idx];
    while (cur) {
        if (strcmp(cur->word, word) == 0) {
            cur->count++;
            return;
        }
        cur = cur->next;
    }
    
    WordFreq* node = (WordFreq*)malloc(sizeof(WordFreq));
    if (!node) return;
    node->word = strdup(word);
    node->count = 1;
    node->next = table[idx];
    table[idx] = node;
    (*unique_words)++;
}

static void free_hash_table(WordFreq** table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        WordFreq* cur = table[i];
        while (cur) {
            WordFreq* next = cur->next;
            free(cur->word);
            free(cur);
            cur = next;
        }
        table[i] = NULL;
    }
}

static int compare_word_freq(const void* a, const void* b) {
    const WordFreq* w1 = *(const WordFreq**)a;
    const WordFreq* w2 = *(const WordFreq**)b;
    if (w2->count != w1->count) {
        return w2->count - w1->count; /* Descending by frequency */
    }
    return strcmp(w1->word, w2->word);
}

static bool is_whitespace_string(const char* s) {
    if (!s || !*s) return true;
    while (*s) {
        if (!isspace((unsigned char)*s)) {
            return false;
        }
        s++;
    }
    return true;
}

static int count_utf8_codepoints(const char* s) {
    if (!s) return 0;
    int count = 0;
    while (*s) {
        if ((*s & 0xC0) != 0x80) {
            count++;
        }
        s++;
    }
    return count;
}

static int count_text_lines(const char* s) {
    if (!s || !*s) return 0;
    int lines = 1;
    while (*s) {
        if (*s == '\n') lines++;
        s++;
    }
    return lines;
}

static char* read_entire_file(FILE* fp, size_t* out_len) {
    size_t capacity = 4096;
    size_t total = 0;
    char* buffer = (char*)malloc(capacity);
    if (!buffer) return NULL;
    
    size_t n;
    while ((n = fread(buffer + total, 1, capacity - total - 1, fp)) > 0) {
        total += n;
        if (total + 1 >= capacity) {
            capacity *= 2;
            char* new_buf = (char*)realloc(buffer, capacity);
            if (!new_buf) {
                free(buffer);
                return NULL;
            }
            buffer = new_buf;
        }
    }
    
    buffer[total] = '\0';
    if (out_len) *out_len = total;
    return buffer;
}

static const char* detect_default_dict(const char* user_dict) {
    if (user_dict) return user_dict;
    if (access("data/thai_words.txt", R_OK) == 0) {
        return "data/thai_words.txt";
    }
    if (access("../data/thai_words.txt", R_OK) == 0) {
        return "../data/thai_words.txt";
    }
    return NULL;
}

static void print_usage(const char* prog_name) {
    printf("Usage: %s [options] [filename]\n\n", prog_name);
    printf("Count Thai words from a text file using CThaiNLP word segmentation.\n");
    printf("If filename is omitted or '-', reads from standard input.\n\n");
    printf("Options:\n");
    printf("  -d, --dict <path>     Path to custom dictionary file (default: data/thai_words.txt)\n");
    printf("  -t, --top <N>         Show top N most frequent Thai words (default: 10, 0 to disable)\n");
    printf("  -w, --words-only      Output only the Thai word count number\n");
    printf("  -v, --verbose         Print all segmented tokens\n");
    printf("  -h, --help            Show this help message and exit\n\n");
    printf("Examples:\n");
    printf("  %s input.txt\n", prog_name);
    printf("  %s -d data/thai_words.txt -t 20 input.txt\n", prog_name);
    printf("  cat input.txt | %s -w\n", prog_name);
}

int main(int argc, char* argv[]) {
    const char* filename = NULL;
    const char* user_dict = NULL;
    int top_n = 10;
    bool words_only = false;
    bool verbose = false;
    
    /* Parse command-line options */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--words-only") == 0) {
            words_only = true;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dict") == 0) {
            if (i + 1 < argc) {
                user_dict = argv[++i];
            } else {
                fprintf(stderr, "Error: --dict option requires a file path argument\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--top") == 0) {
            if (i + 1 < argc) {
                top_n = atoi(argv[++i]);
                if (top_n < 0) top_n = 0;
            } else {
                fprintf(stderr, "Error: --top option requires an integer argument\n");
                return 1;
            }
        } else if (argv[i][0] == '-' && strcmp(argv[i], "-") != 0) {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        } else {
            filename = argv[i];
        }
    }
    
    /* Open input file or read from stdin */
    FILE* fp = stdin;
    bool is_stdin = false;
    if (!filename || strcmp(filename, "-") == 0) {
        fp = stdin;
        is_stdin = true;
    } else {
        fp = fopen(filename, "rb");
        if (!fp) {
            fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
            return 1;
        }
    }
    
    /* Read file content */
    size_t file_bytes = 0;
    char* text = read_entire_file(fp, &file_bytes);
    if (!is_stdin) {
        fclose(fp);
    }
    
    if (!text) {
        fprintf(stderr, "Error: Failed to read input\n");
        return 1;
    }
    
    /* Resolve dictionary */
    const char* dict_path = detect_default_dict(user_dict);
    newmm_dict_t dict = newmm_load_dict(dict_path);
    if (!dict) {
        fprintf(stderr, "Warning: Failed to load dictionary from '%s', using minimal fallback\n",
                dict_path ? dict_path : "default");
    }
    
    /* Segment text */
    int token_count = 0;
    char** tokens = NULL;
    if (dict) {
        tokens = newmm_segment_with_dict(text, dict, &token_count);
    } else {
        tokens = newmm_segment(text, NULL, &token_count);
    }
    
    if (!tokens && token_count > 0) {
        fprintf(stderr, "Error: Failed to segment text\n");
        free(text);
        if (dict) newmm_free_dict(dict);
        return 1;
    }
    
    /* Analyze tokens */
    int thai_words = 0;
    int non_thai_words = 0;
    int whitespace_tokens = 0;
    int unique_thai_words = 0;
    WordFreq* hash_table[HASH_TABLE_SIZE] = {NULL};
    
    for (int i = 0; i < token_count; i++) {
        const char* tok = tokens[i];
        if (is_whitespace_string(tok)) {
            whitespace_tokens++;
        } else if (is_thai(tok, "")) {
            thai_words++;
            add_word_frequency(hash_table, tok, &unique_thai_words);
        } else {
            non_thai_words++;
        }
    }
    
    /* Words-only output for scripts/pipelines */
    if (words_only) {
        printf("%d\n", thai_words);
    } else {
        int lines = count_text_lines(text);
        int characters = count_utf8_codepoints(text);
        
        printf("=========================================\n");
        printf(" CThaiNLP Thai Word Count Analysis\n");
        printf("=========================================\n");
        printf("Source:              %s\n", is_stdin ? "<standard input>" : filename);
        printf("Dictionary:          %s\n", dict_path ? dict_path : "(built-in minimal)");
        printf("File size (bytes):   %zu\n", file_bytes);
        printf("Total lines:         %d\n", lines);
        printf("Total characters:    %d\n", characters);
        printf("-----------------------------------------\n");
        printf("Total tokens:        %d\n", token_count);
        printf("Thai words:          %d\n", thai_words);
        printf("Non-Thai tokens:     %d\n", non_thai_words);
        printf("Whitespace tokens:   %d\n", whitespace_tokens);
        printf("Unique Thai words:   %d\n", unique_thai_words);
        printf("=========================================\n");
        
        /* Show top N most frequent words */
        if (top_n > 0 && unique_thai_words > 0) {
            WordFreq** freq_list = (WordFreq**)malloc(unique_thai_words * sizeof(WordFreq*));
            if (freq_list) {
                int count = 0;
                for (int i = 0; i < HASH_TABLE_SIZE; i++) {
                    WordFreq* cur = hash_table[i];
                    while (cur) {
                        freq_list[count++] = cur;
                        cur = cur->next;
                    }
                }
                
                qsort(freq_list, unique_thai_words, sizeof(WordFreq*), compare_word_freq);
                
                int limit = top_n < unique_thai_words ? top_n : unique_thai_words;
                printf("\nTop %d Most Frequent Thai Words:\n", limit);
                for (int i = 0; i < limit; i++) {
                    printf("  %2d. %-20s : %d\n", i + 1, freq_list[i]->word, freq_list[i]->count);
                }
                
                free(freq_list);
            }
        }
        
        /* Verbose output */
        if (verbose && token_count > 0) {
            printf("\nSegmented Tokens:\n");
            for (int i = 0; i < token_count; i++) {
                const char* type = "other";
                if (is_whitespace_string(tokens[i])) {
                    type = "whitespace";
                } else if (is_thai(tokens[i], "")) {
                    type = "thai";
                }
                printf("  [%4d] '%s' (%s)\n", i + 1, tokens[i], type);
            }
        }
    }
    
    /* Cleanup */
    free_hash_table(hash_table);
    if (tokens) {
        newmm_free_result(tokens, token_count);
    }
    if (dict) {
        newmm_free_dict(dict);
    }
    free(text);
    
    return 0;
}
