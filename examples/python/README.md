# Performance Benchmark: CThaiNLP vs PyThaiNLP

This directory contains a comprehensive benchmark suite to compare CThaiNLP and PyThaiNLP across tokenization, character clusters, soundex, utilities, memory, and startup latency.

## Running the Benchmark

```bash
# Install packages first
pip install -e .       # Install CThaiNLP
pip install pythainlp  # Install PyThaiNLP

# Run full benchmark suite
python examples/python/benchmark.py

# Run benchmark and export Markdown report
python examples/python/benchmark.py --report BENCHMARK.md

# Quick run (fewer iterations)
python examples/python/benchmark.py --quick
```

Or via Makefile:

```bash
make benchmark
```

## What is Measured

1. **Word Tokenization (`newmm`)**:
   - Short sentences (10 typical Thai sentences)
   - Medium paragraph (~1.1 KB text)
   - Large document (~56 KB text)
2. **Thai Character Cluster (`tcc`)**:
   - Short sentences
   - Medium text
3. **Thai Soundex**:
   - LK82 algorithm (batch of 250 words)
   - Udom83 algorithm (batch of 250 words)
4. **Thai Text Utilities**:
   - `is_thai` validation
   - `count_thai` percentage calculation
   - `remove_tone` / `remove_tonemark`
   - Arabic to Thai digit conversion
5. **Cold-Start Latency**:
   - Time to launch Python process, import the library, and tokenize the first sentence
6. **Process Peak Memory (RSS)**:
   - Peak resident set size (RAM) of a fresh process
7. **Correctness & Output Compatibility**:
   - Sentence-by-sentence tokenization comparison between CThaiNLP and PyThaiNLP

## Sample Output

```
==========================================================================================
Benchmark Task                             | PyThaiNLP    | CThaiNLP     | Speedup   | Winner
------------------------------------------------------------------------------------------
Word Tokenize (10 Short Sentences)         |     0.34 ms  |     0.07 ms  |    5.01x | CThaiNLP 5.0x
Word Tokenize (Medium ~1.1 KB)             |     0.42 ms  |     0.03 ms  |   14.08x | CThaiNLP 14.1x
Word Tokenize (Large ~56 KB)               |    25.52 ms  |     0.71 ms  |   35.70x | CThaiNLP 35.7x
TCC Segmentation (10 Sentences)            |     0.06 ms  |     0.01 ms  |    9.48x | CThaiNLP 9.5x
TCC Segmentation (Medium ~1.1 KB)          |     0.09 ms  |     0.01 ms  |    7.41x | CThaiNLP 7.4x
Soundex LK82 (Batch of 250 words)          |     0.55 ms  |     0.04 ms  |   14.04x | CThaiNLP 14.0x
Soundex Udom83 (Batch of 250 words)        |     0.58 ms  |     0.04 ms  |   14.93x | CThaiNLP 14.9x
is_thai Validation (10 Sentences)          |     0.04 ms  |     0.01 ms  |    7.60x | CThaiNLP 7.6x
count_thai Percentage (10 Sentences)       |     0.03 ms  |     0.04 ms  |    0.72x | PyThaiNLP 1.4x
remove_tone / remove_tonemark (600 words)  |     0.18 ms  |     0.12 ms  |    1.59x | CThaiNLP 1.6x
Arabic to Thai Digits (1,000 numbers)      |     0.25 ms  |     0.28 ms  |    0.90x | PyThaiNLP 1.1x
------------------------------------------------------------------------------------------
Cold-Start Initialization                  |   515.87 ms |    48.74 ms |   10.58x | CThaiNLP 10.6x
Process Peak RSS Memory                    |    86.15 MB |    45.62 MB |   47.0% | CThaiNLP saves 47.0%
==========================================================================================
```
