# CThaiNLP

![Build and Test](https://github.com/wannaphong/CThaiNLP/actions/workflows/test.yml/badge.svg)

C implementation of Thai Natural Language Processing tools, ported from [PyThaiNLP](https://github.com/PyThaiNLP/pythainlp).

## Installation

### Python Package (Recommended)

```bash
pip install cthainlp
```

Or install from source:

```bash
git clone https://github.com/wannaphong/CThaiNLP.git
cd CThaiNLP
pip install -e .
```

### C Library

See [Building](#building) section below.

## Features

- **newmm**: Dictionary-based maximal matching word segmentation constrained by Thai Character Cluster (TCC) boundaries
- **tcc**: Thai Character Cluster segmentation and position detection
- **util**: Thai character checks, text counting, digit conversion (Arabic ↔ Thai), numbers to words (`num_to_thaiword`, `bahttext`), text normalization (`normalize`, `remove_tone`, `remove_dup_spaces`), and dictionary collation (`collate`)
- **soundex**: Thai phonetic algorithms (`lk82`, `udom83`)
- **Character constants**: `thai_consonants`, `thai_vowels`, `thai_digits`, `thai_tonemarks`, `thai_characters`, `thai_pangram`, etc.
- **Tokenization**: `word_tokenize`, `subword_tokenize`, `sent_tokenize`, `display_cell_tokenize`, `word_detokenize`
- **C Library** (`libcthainlp.a`) and unified C header (`cthainlp.h`) with sub-headers (`newmm.h`, `tcc.h`, `util.h`, `soundex.h`)
- **Python bindings** with PyThaiNLP-compatible API

## Quick Start

### Python

```python
from cthainlp import word_tokenize, soundex, collate
from cthainlp.util import bahttext, num_to_thaiword, is_thai, normalize

# Word Tokenization
text = "ฉันไปโรงเรียน"
print(word_tokenize(text))  # ['ฉัน', 'ไป', 'โรงเรียน']

# Number to Thai Words & Bahttext
print(num_to_thaiword(101))  # 'หนึ่งร้อยเอ็ด'
print(bahttext(5611.50))     # 'ห้าพันหกร้อยสิบเอ็ดบาทห้าสิบสตางค์'

# Thai Soundex
print(soundex("รัก", engine="udom83"))  # 'ร100000'
print(soundex("รัก", engine="lk82"))    # 'ร1000'

# Text Normalization
print(normalize("เเปลก"))  # 'แปลก'

# Thai Collation (Alphabetical sorting)
words = ["ไก่", "เกิด", "กาล", "เป็ด", "หมู"]
print(collate(words))  # ['กาล', 'เกิด', 'ไก่', 'เป็ด', 'หมู']
```

### C

```c
#include "cthainlp.h"
#include <stdio.h>

int main() {
    // Word Segmentation
    int count;
    char** tokens = newmm_segment("ฉันไปโรงเรียน", "data/thai_words.txt", &count);
    for (int i = 0; i < count; i++) {
        printf("%s\n", tokens[i]);
    }
    newmm_free_result(tokens, count);

    // Utilities
    char* th_digit = arabic_digit_to_thai_digit("123");
    printf("Thai digits: %s\n", th_digit); // ๑๒๓
    cthainlp_free_string(th_digit);

    // Soundex
    char* code = soundex_udom83("รัก");
    printf("Soundex: %s\n", code); // ร100000
    soundex_free(code);

    return 0;
}
```

## Building

### Prerequisites

- GCC or compatible C compiler
- Make
- Python 3.8+ (for Python bindings)

### C Library Compilation

```bash
make
```

This will create:
- Static library: `lib/libcthainlp.a`
- Example program: `build/example_basic`
- Test suites: `build/test_newmm`, `build/test_tcc`, `build/test_util`, `build/test_soundex`

### Python Package Installation

Install the Python bindings:

```bash
pip install -e .
```

Or build from source:

```bash
python setup.py build
python setup.py install
```

## Usage & API Reference

### Python API

#### Tokenization (`cthainlp.tokenize` / `cthainlp`)

```python
from cthainlp import word_tokenize, sent_tokenize, subword_tokenize
from cthainlp.tokenize import display_cell_tokenize, word_detokenize

# Word segmentation
word_tokenize("ฉันไปโรงเรียน", engine="newmm")

# Subword / TCC segmentation
subword_tokenize("ฉันไปโรงเรียน", engine="tcc")
# ['ฉั', 'น', 'ไป', 'โรง', 'เรี', 'ยน']

# Sentence tokenization
sent_tokenize("ฉันไปประชุมเมื่อวันที่ 11 มีนาคม", engine="whitespace")

# Display cells (characters with tone marks attached)
display_cell_tokenize("แม่น้ำ")

# Detokenize words to text
word_detokenize(["เรา", "เล่น"]) # 'เราเล่น'
```

#### Thai Character Clusters (`cthainlp.tcc` / `cthainlp.tokenize.tcc`)

```python
from cthainlp import tcc

# Segment into clusters
clusters = tcc.segment("ฉันไปโรงเรียน")

# Cluster generator
for c in tcc.tcc("สวัสดี"):
    print(c)

# Cluster ending positions
positions = tcc.tcc_pos("ฉันไปโรงเรียน")
```

#### Utilities (`cthainlp.util`)

```python
from cthainlp.util import (
    is_thai,
    is_thai_char,
    count_thai,
    arabic_digit_to_thai_digit,
    thai_digit_to_arabic_digit,
    digit_to_text,
    num_to_thaiword,
    bahttext,
    normalize,
    remove_tone,
    remove_dup_spaces,
    collate,
)

# Text checking
is_thai("ภาษาไทย")      # True
is_thai_char("ก")       # True
count_thai("ไทย 123")   # 100.0 (ignoring digits/whitespace)

# Digit conversion
arabic_digit_to_thai_digit("123")  # '๑๒๓'
thai_digit_to_arabic_digit("๑๒๓")  # '123'
digit_to_text("123")               # 'หนึ่งสองสาม'

# Number to Thai words
num_to_thaiword(101)    # 'หนึ่งร้อยเอ็ด'
bahttext(101.25)        # 'หนึ่งร้อยเอ็ดบาทยี่สิบห้าสตางค์'

# Normalization
normalize("เเปลก")       # 'แปลก'
remove_tone("กิ่งก่า")    # 'กิงกา'
remove_dup_spaces("ก   ข") # 'ก ข'

# Collation
collate(["ไก่", "เกิด", "กาล"]) # ['กาล', 'เกิด', 'ไก่']
```

#### Soundex (`cthainlp.soundex` / `cthainlp`)

```python
from cthainlp.soundex import soundex, lk82, udom83

# Default engine is udom83
soundex("รัก")                 # 'ร100000'
soundex("รัก", engine="lk82")  # 'ร1000'
lk82("รัก")                    # 'ร1000'
udom83("รัก")                  # 'ร100000'
```

#### Character Constants (`cthainlp`)

```python
import cthainlp

print(cthainlp.thai_consonants)   # 'กขฃคฅฆงจฉชซฌญฎฏฐฑฒณดตถทธนบปผฝพฟภมยรลวศษสหฬอฮ'
print(cthainlp.thai_vowels)       # Thai vowel characters
print(cthainlp.thai_digits)       # '๐๑๒๓๔๕๖๗๘๙'
print(cthainlp.thai_tonemarks)    # Thai tone marks
print(cthainlp.thai_characters)   # All Thai Unicode characters
print(cthainlp.thai_pangram)      # Thai pangram
```

### C Library API

Headers in `include/`:
- `cthainlp.h`: Umbrella header
- `newmm.h`: `newmm_segment`, `newmm_segment_with_dict`, `newmm_load_dict`, `newmm_free_dict`, `newmm_free_result`
- `tcc.h`: `tcc_segment`, `tcc_pos`, `tcc_free_result`
- `util.h`: `is_thai`, `is_thai_char`, `is_thai_codepoint`, `count_thai`, `arabic_digit_to_thai_digit`, `thai_digit_to_arabic_digit`, `remove_tonemark`, `cthainlp_free_string`
- `soundex.h`: `soundex_lk82`, `soundex_udom83`, `soundex_free`

### Compile Your C Program

```bash
gcc your_program.c -I./include -L./lib -lcthainlp -o your_program
```

### Running C Examples

#### 1. Basic Tokenization Example

```bash
./build/example_basic "ฉันไปโรงเรียน"
```

#### 2. Word Count CLI Tool (`count_words`)

Count Thai words from a text file, compute unique word counts, and list word frequencies:

```bash
# Analyze a text file
./build/count_words examples/sample_thai.txt

# Show top 20 most frequent Thai words
./build/count_words -t 20 examples/sample_thai.txt

# Output word count only (convenient for shell pipelines)
cat examples/sample_thai.txt | ./build/count_words -w

# Use a custom dictionary
./build/count_words -d data/thai_words.txt examples/sample_thai.txt
```

### Running Tests

#### Python Tests

```bash
python -m unittest discover -s tests/python
```

#### C Tests

```bash
make test
```

## Project Structure

```
CThaiNLP/
├── include/
│   ├── cthainlp.h          # Umbrella public header
│   ├── newmm.h             # Word segmentation header
│   ├── tcc.h               # Thai Character Cluster header
│   ├── util.h              # Utility functions header
│   └── soundex.h           # Thai soundex header
├── src/
│   ├── newmm.c             # Main newmm implementation
│   ├── trie.c              # Trie data structure
│   ├── trie.h              # Trie internal header
│   ├── tcc.c               # Thai Character Cluster implementation
│   ├── tcc.h               # TCC internal header
│   ├── util.c              # Utility functions implementation
│   └── soundex.c           # Soundex implementation
├── python/
│   └── cthainlp_wrapper.c  # Python C extension wrapper
├── cthainlp/
│   ├── __init__.py         # Package root & character constants
│   ├── newmm.py            # newmm module
│   ├── tcc.py              # TCC convenience module
│   ├── tokenize/           # Tokenization subpackage
│   │   ├── __init__.py     # word_tokenize, sent_tokenize, subword_tokenize, etc.
│   │   └── tcc.py          # TCC tokenization submodule
│   ├── util/               # Utilities subpackage
│   │   ├── __init__.py     # util root exports
│   │   ├── thai.py         # is_thai, is_thai_char, count_thai
│   │   ├── digitconv.py    # Arabic ↔ Thai digit conversion
│   │   ├── numtoword.py    # num_to_thaiword, bahttext
│   │   ├── normalize.py    # Text normalization
│   │   └── collate.py      # Thai alphabetical collation
│   └── soundex/            # Soundex subpackage
│       ├── __init__.py     # soundex function
│       ├── lk82.py         # LK82 soundex
│       └── udom83.py       # Udom83 soundex
├── tests/
│   ├── test_newmm.c        # newmm C test suite
│   ├── test_tcc.c          # TCC C test suite
│   ├── test_util.c         # Util C test suite
│   ├── test_soundex.c      # Soundex C test suite
│   └── python/
│       ├── test_tokenize.py # Tokenize Python tests
│       ├── test_tcc.py      # TCC Python tests
│       ├── test_util.py     # Util Python tests
│       ├── test_soundex.py  # Soundex Python tests
│       └── test_constants.py # Constants Python tests
├── examples/
│   ├── example_basic.c     # Basic tokenization example
│   ├── count_words.c       # Word count CLI example tool
│   ├── sample_thai.txt     # Sample Thai text file
│   └── python/
│       ├── example_basic.py # Python usage example
│       └── benchmark.py    # Performance benchmark
├── data/
│   └── thai_words.txt      # Default word dictionary
├── setup.py                # Python package setup
├── pyproject.toml          # Python build configuration
├── Makefile                # Build configuration
└── README.md               # Documentation
```

## Credits

- Original PyThaiNLP implementation: [PyThaiNLP Project](https://github.com/PyThaiNLP/pythainlp)
- newmm algorithm: Based on work by Korakot Chaovavanich
- TCC rules: Theeramunkong et al. 2000
- LK82 soundex: Vichit Lorchirachoonkul 1982
- Udom83 soundex: Wannee Udompanich 1983

## License

Apache License 2.0 (following PyThaiNLP's license)