"""
Check if it is Thai text.
Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
"""

import string
from typing import Optional

try:
    import _cthainlp
except ImportError:
    _cthainlp = None

_DEFAULT_IGNORE_CHARS: str = (
    string.whitespace + string.digits + string.punctuation
)
_TH_FIRST_CHAR_ASCII: int = 3584
_TH_LAST_CHAR_ASCII: int = 3711


def is_thai_char(ch: str) -> bool:
    """
    Check if a character is a Thai character.
    
    Args:
        ch (str): Input character.
        
    Returns:
        bool: True if ch is a Thai character, otherwise False.
    """
    if not ch or not isinstance(ch, str):
        return False
    if _cthainlp is not None:
        return _cthainlp.is_thai_char(ch)
    ch_val = ord(ch[0])
    return _TH_FIRST_CHAR_ASCII <= ch_val <= _TH_LAST_CHAR_ASCII


def isthaichar(ch: str) -> bool:
    """Check if a character is a Thai character (alias for is_thai_char)."""
    return is_thai_char(ch)


def is_thai(text: str, ignore_chars: str = ".") -> bool:
    """
    Check if every character in a string is a Thai character.
    
    Args:
        text (str): Input text.
        ignore_chars (str): Characters to be ignored, defaults to ".".
        
    Returns:
        bool: True if every character in the input string is Thai, otherwise False.
    """
    if not isinstance(text, str):
        return False
    if not text:
        return True
    if _cthainlp is not None:
        return _cthainlp.is_thai(text, ignore_chars)
    if not ignore_chars:
        ignore_chars = ""
    for ch in text:
        if ch not in ignore_chars and not is_thai_char(ch):
            return False
    return True


def isthai(text: str, ignore_chars: str = ".") -> bool:
    """Check if every character in a string is a Thai character (alias for is_thai)."""
    return is_thai(text, ignore_chars)


def count_thai(text: str, ignore_chars: Optional[str] = None) -> float:
    """
    Find proportion of Thai characters in a given text (percentage).
    
    Args:
        text (str): Input text.
        ignore_chars (str, optional): Characters to be ignored, defaults to whitespace,
                                      digits, and punctuation marks.
                                      
    Returns:
        float: Proportion of Thai characters in the text (percentage 0.0 - 100.0).
    """
    if not text or not isinstance(text, str):
        return 0.0
    if ignore_chars is None:
        ignore_chars = _DEFAULT_IGNORE_CHARS
    if _cthainlp is not None:
        return _cthainlp.count_thai(text, ignore_chars)
        
    num_thai = 0
    num_ignore = 0
    for ch in text:
        if ch in ignore_chars:
            num_ignore += 1
        elif is_thai_char(ch):
            num_thai += 1

    num_count = len(text) - num_ignore
    if num_count <= 0:
        return 0.0

    return (num_thai / num_count) * 100.0


def countthai(text: str, ignore_chars: Optional[str] = None) -> float:
    """Find proportion of Thai characters in a given text (alias for count_thai)."""
    return count_thai(text, ignore_chars)


__all__ = [
    "is_thai_char",
    "isthaichar",
    "is_thai",
    "isthai",
    "count_thai",
    "countthai",
]
