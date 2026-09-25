"""
Thai soundex - Udom83 system
Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
"""

try:
    import _cthainlp
except ImportError:
    _cthainlp = None

import re
from typing import Pattern

_thai_consonants: str = "กขฃคฅฆงจฉชซฌญฎฏฐฑฒณดตถทธนบปผฝพฟภมยรลวศษสหฬอฮ"
_THANTHAKHAT: str = "\u0e4c"
_RE_1: Pattern[str] = re.compile(r"รร([\u0e40-\u0e44])")
_RE_2: Pattern[str] = re.compile(
    f"รร([{_thai_consonants}][{_thai_consonants}\u0e40-\u0e44])"
)
_RE_3: Pattern[str] = re.compile(
    f"รร([{_thai_consonants}][\u0e30-\u0e39\u0e48-\u0e4c])"
)
_RE_4: Pattern[str] = re.compile(r"รร")
_RE_5: Pattern[str] = re.compile(f"ไ([{_thai_consonants}]ย)")
_RE_6: Pattern[str] = re.compile(f"[ไใ]([{_thai_consonants}])")
_RE_7: Pattern[str] = re.compile(r"\u0e33(ม[\u0e30-\u0e39])")
_RE_8: Pattern[str] = re.compile(r"\u0e33ม")
_RE_9: Pattern[str] = re.compile(r"\u0e33")
_RE_10: Pattern[str] = re.compile(
    f"จน์|มณ์|ณฑ์|ทร์|ตร์|"
    f"[{_thai_consonants}]{_THANTHAKHAT}|[{_thai_consonants}]"
    f"[\u0e30-\u0e39]{_THANTHAKHAT}"
)
_RE_11: Pattern[str] = re.compile(r"[\u0e30-\u0e4c]")

_TRANS1: dict = str.maketrans(
    "กขฃคฅฆงจฉชฌซศษสฎดฏตฐฑฒถทธณนบปผพภฝฟมญยรลฬฤฦวอหฮ",
    "กขขขขขงจชชชสสสสดดตตททททททนนบปพพพฟฟมยยรรรรรวอฮฮ",
)
_TRANS2: dict = str.maketrans(
    "มวำกขฃคฅฆงยญณนฎฏดตศษสบปพภผฝฟหอฮจฉชซฌฐฑฒถทธรฤลฦ",
    "0001111112233344444445555666666777778888889999",
)


def udom83(text: str) -> str:
    """
    Converts Thai text into phonetic code with Udom83 soundex algorithm.
    
    Args:
        text (str): Thai word
        
    Returns:
        str: Udom83 soundex code (7 characters)
    """
    if not text or not isinstance(text, str):
        return ""
        
    if _cthainlp is not None:
        return _cthainlp.soundex_udom83(text)

    text = _RE_1.sub("ัน\\1", text)
    text = _RE_2.sub("ั\\1", text)
    text = _RE_3.sub("ัน\\1", text)
    text = _RE_4.sub("ัน", text)
    text = _RE_5.sub("\\1", text)
    text = _RE_6.sub("\\1ย", text)
    text = _RE_7.sub("ม\\1", text)
    text = _RE_8.sub("ม", text)
    text = _RE_9.sub("ม", text)
    text = _RE_10.sub("", text)
    text = _RE_11.sub("", text)

    if not text:
        return ""

    sd = "".join(
        [text[0].translate(_TRANS1), text[1:].translate(_TRANS2), "000000"]
    )
    return sd[:7]


__all__ = ["udom83"]
