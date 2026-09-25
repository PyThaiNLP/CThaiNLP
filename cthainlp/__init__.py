"""
CThaiNLP - Thai Natural Language Processing Library
Python bindings for the CThaiNLP C library, ported from PyThaiNLP
"""

__version__ = "0.1.0"
__author__ = "Wannaphong Phatthiyaphaibun"

# Thai character sets and constants
thai_consonants: str = (
    "กขฃคฅฆงจฉชซฌญฎฏฐฑฒณดตถทธนบปผฝพฟภมยรลวศษสหฬอฮ"
)
thai_vowels: str = (
    "\u0e24\u0e26\u0e30\u0e31\u0e32\u0e33\u0e34\u0e35\u0e36\u0e37"
    + "\u0e38\u0e39\u0e40\u0e41\u0e42\u0e43\u0e44\u0e45\u0e4d\u0e47"
)
thai_lead_vowels: str = "\u0e40\u0e41\u0e42\u0e43\u0e44"
thai_follow_vowels: str = "\u0e30\u0e32\u0e33\u0e45"
thai_above_vowels: str = "\u0e31\u0e34\u0e35\u0e36\u0e37\u0e4d\u0e47"
thai_below_vowels: str = "\u0e38\u0e39"
thai_tonemarks: str = "\u0e48\u0e49\u0e4a\u0e4b"
thai_signs: str = "\u0e2f\u0e3a\u0e46\u0e4c\u0e4d\u0e4e"
thai_letters: str = "".join(
    [thai_consonants, thai_vowels, thai_tonemarks, thai_signs]
)
thai_punctuations: str = "\u0e4f\u0e5a\u0e5b"
thai_digits: str = "๐๑๒๓๔๕๖๗๘๙"
thai_symbols: str = "\u0e3f"
thai_characters: str = "".join(
    [thai_letters, thai_punctuations, thai_digits, thai_symbols]
)
thai_pangram: str = """กีฬาบังลังก์ ฿๑,๒๓๔,๕๖๗,๘๙๐
๏ จับฅอคนบั่นต้อง 	อาญา
ขุดฆ่าโคตรฃัตติยา 	ซ่านม้วย
ธรรมฤๅผ่อนรักษา 	ใจชั่ว โฉดแฮ
สืบอยู่เต็มศึกด้วย 	ฝุ่นฟ้ากีฬา กามฦๅ ฯ
๏ กตัญญูไป่พร้อม 	ปฐมฌาน
เกมส๎วัฒน์ปฏิภาณ 	ห่อนล้ำ
ทฤษฎีถ่อยๆ สังหาร 	เกณฑ์โทษ
โกรธจี๊ดจ๋อยจ่มถ้ำ 	อยู่เฝ้า “อตฺตา” ๚ะ๛
๑๒ กรกฎาคม ๒๕๕๘"""

from cthainlp.tokenize import (
    word_tokenize,
    sent_tokenize,
    subword_tokenize,
    display_cell_tokenize,
    word_detokenize,
)
from cthainlp.util import collate
from cthainlp.soundex import soundex
from cthainlp import newmm, tcc, tokenize, util

__all__ = [
    "__version__",
    "__author__",
    # Constants
    "thai_consonants",
    "thai_vowels",
    "thai_lead_vowels",
    "thai_follow_vowels",
    "thai_above_vowels",
    "thai_below_vowels",
    "thai_tonemarks",
    "thai_signs",
    "thai_letters",
    "thai_punctuations",
    "thai_digits",
    "thai_symbols",
    "thai_characters",
    "thai_pangram",
    # Functions
    "word_tokenize",
    "sent_tokenize",
    "subword_tokenize",
    "display_cell_tokenize",
    "word_detokenize",
    "collate",
    "soundex",
    # Modules
    "newmm",
    "tcc",
    "tokenize",
    "util",
]
