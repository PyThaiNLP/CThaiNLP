"""
Text normalization for Thai text.
Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
"""

import re
from typing import Pattern

try:
    import _cthainlp
except ImportError:
    _cthainlp = None

_thai_consonants = "กขฃคฅฆงจฉชซฌญฎฏฐฑฒณดตถทธนบปผฝพฟภมยรลวศษสหฬอฮ"
_above_v = "\u0e31\u0e34\u0e35\u0e36\u0e37\u0e4d\u0e47"
_below_v = "\u0e38\u0e39"
_follow_v = "\u0e30\u0e32\u0e33\u0e45"
_lead_v = "\u0e40\u0e41\u0e42\u0e43\u0e44"
_tonemarks = "\u0e48\u0e49\u0e4a\u0e4b"
_thai_vowels = (
    "\u0e24\u0e26\u0e30\u0e31\u0e32\u0e33\u0e34\u0e35\u0e36\u0e37"
    + "\u0e38\u0e39\u0e40\u0e41\u0e42\u0e43\u0e44\u0e45\u0e4d\u0e47"
)

_DANGLING_CHARS: str = f"{_above_v}{_below_v}{_tonemarks}\u0e3a\u0e4c\u0e4d\u0e4e"
_RE_REMOVE_DANGLINGS: Pattern[str] = re.compile(f"^[{_DANGLING_CHARS}]+")
_RE_REMOVE_DANGLINGS_AFTER_SPACE: Pattern[str] = re.compile(
    f" +[{_DANGLING_CHARS}]+"
)

_ZERO_WIDTH_CHARS: str = "\u200b\u200c"

_REORDER_PAIRS: list = [
    ("\u0e40\u0e40", "\u0e41"),  # Sara E + Sara E -> Sara Ae
    (
        f"([{_tonemarks}\u0e4c]+)([{_above_v}{_below_v}]+)",
        "\\2\\1",
    ),
    (
        f"\u0e4d([{_tonemarks}]*)\u0e32",
        "\\1\u0e33",
    ),
    (
        f"([{_follow_v}]+)([{_tonemarks}]+)",
        "\\2\\1",
    ),
    ("([^\u0e24\u0e26])\u0e45", "\\1\u0e32"),
]

_NOREPEAT_CHARS: str = (
    f"{_follow_v}{_lead_v}{_above_v}{_below_v}\u0e3a\u0e4c\u0e4d\u0e4e"
)
_NOREPEAT_PAIRS: list = list(
    zip([f"({ch}[ ]*)+{ch}" for ch in _NOREPEAT_CHARS], _NOREPEAT_CHARS)
)

_RE_TONEMARKS: Pattern[str] = re.compile(f"[{_tonemarks}]+")
_RE_REMOVE_NEWLINES: Pattern[str] = re.compile(r"[ \n]*\n[ \n]*")

_RE_REMOVE_SPACES_BEFORE_NONBASE: Pattern[str] = re.compile(
    f"([{_thai_consonants}])(?<![{_thai_vowels}][{_thai_consonants}]) ([{_DANGLING_CHARS}])"
)


def _last_char(matchobj: re.Match) -> str:
    return matchobj.group(0)[-1]


def remove_dangling(text: str) -> str:
    """
    Remove Thai non-base characters at the beginning of text and after spaces.
    """
    if not text:
        return ""
    text = _RE_REMOVE_DANGLINGS.sub("", text)
    text = _RE_REMOVE_DANGLINGS_AFTER_SPACE.sub(" ", text)
    return text


def remove_dup_spaces(text: str) -> str:
    """
    Remove duplicate spaces. Replace multiple spaces with one space.
    """
    if not text:
        return ""
    while "  " in text:
        text = text.replace("  ", " ")
    text = _RE_REMOVE_NEWLINES.sub("\n", text)
    return text.strip()


def remove_tonemark(text: str) -> str:
    """
    Remove all Thai tone marks from the text.
    """
    if not text:
        return ""
    if _cthainlp is not None:
        return _cthainlp.remove_tonemark(text)
    for ch in _tonemarks:
        while ch in text:
            text = text.replace(ch, "")
    return text


# Alias
remove_tone = remove_tonemark


def remove_zw(text: str) -> str:
    """
    Remove zero-width characters (ZWSP, ZWNJ).
    """
    if not text:
        return ""
    for ch in _ZERO_WIDTH_CHARS:
        while ch in text:
            text = text.replace(ch, "")
    return text


def remove_spaces_before_marks(text: str) -> str:
    """
    Remove spaces before Thai tone marks and non-base characters.
    """
    if not text:
        return ""
    return _RE_REMOVE_SPACES_BEFORE_NONBASE.sub(r"\1\2", text)


def reorder_vowels(text: str) -> str:
    """
    Reorder vowels and tone marks to standard logical order.
    """
    if not text:
        return ""
    for pair in _REORDER_PAIRS:
        text = re.sub(pair[0], pair[1], text)
    return text


def remove_repeat_vowels(text: str) -> str:
    """
    Remove repeating vowels, tone marks, and signs.
    """
    if not text:
        return ""
    text = reorder_vowels(text)
    for pair in _NOREPEAT_PAIRS:
        text = re.sub(pair[0], pair[1], text)
    text = _RE_TONEMARKS.sub(_last_char, text)
    return text


def normalize(text: str) -> str:
    """
    Normalize and clean Thai text.
    """
    if not text:
        return ""
    text = remove_zw(text)
    text = remove_dup_spaces(text)
    text = remove_spaces_before_marks(text)
    text = remove_repeat_vowels(text)
    text = remove_dangling(text)
    return text


__all__ = [
    "remove_dangling",
    "remove_dup_spaces",
    "remove_tonemark",
    "remove_tone",
    "remove_zw",
    "remove_spaces_before_marks",
    "reorder_vowels",
    "remove_repeat_vowels",
    "normalize",
]
