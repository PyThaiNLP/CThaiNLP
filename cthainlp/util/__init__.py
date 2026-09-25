"""
Utility functions for CThaiNLP
Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
"""

from cthainlp.util.thai import (
    is_thai,
    isthai,
    is_thai_char,
    isthaichar,
    count_thai,
    countthai,
)
from cthainlp.util.digitconv import (
    arabic_digit_to_thai_digit,
    thai_digit_to_arabic_digit,
    digit_to_text,
    text_to_arabic_digit,
    text_to_thai_digit,
)
from cthainlp.util.numtoword import (
    num_to_thaiword,
    bahttext,
)
from cthainlp.util.normalize import (
    normalize,
    remove_dangling,
    remove_dup_spaces,
    remove_tonemark,
    remove_tone,
    remove_zw,
    remove_spaces_before_marks,
    reorder_vowels,
    remove_repeat_vowels,
)
from cthainlp.util.collate import collate

__all__ = [
    "is_thai",
    "isthai",
    "is_thai_char",
    "isthaichar",
    "count_thai",
    "countthai",
    "arabic_digit_to_thai_digit",
    "thai_digit_to_arabic_digit",
    "digit_to_text",
    "text_to_arabic_digit",
    "text_to_thai_digit",
    "num_to_thaiword",
    "bahttext",
    "normalize",
    "remove_dangling",
    "remove_dup_spaces",
    "remove_tonemark",
    "remove_tone",
    "remove_zw",
    "remove_spaces_before_marks",
    "reorder_vowels",
    "remove_repeat_vowels",
    "collate",
]
