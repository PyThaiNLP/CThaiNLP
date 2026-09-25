"""
Thai soundex - LK82 system
Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
"""

try:
    import _cthainlp
except ImportError:
    _cthainlp = None

import re
from typing import Pattern
from cthainlp.util import remove_tonemark

_TRANS1: dict = str.maketrans(
    "กขฃคฅฆงจฉชฌซศษสญยฎดฏตณนฐฑฒถทธบปผพภฝฟมรลฬฤฦวหฮอ",
    "กกกกกกงจชชชซซซซยยดดตตนนททททททบปพพพฟฟมรรรรรวหหอ",
)
_TRANS2: dict = str.maketrans(
    "กขฃคฅฆงจฉชซฌฎฏฐฑฒดตถทธศษสญณนรลฬฤฦบปพฟภผฝมำยวไใหฮาๅึืเแโุูอ",
    "1111112333333333333333333444444445555555667777889AAABCDEEF",
)

_RE_KARANT: Pattern[str] = re.compile(r"จน์|มณ์|ณฑ์|ทร์|ตร์|[ก-ฮ]์|[ก-ฮ][ะ-ู]์")
_RE_SIGN: Pattern[str] = re.compile(r"[\u0e2f\u0e3a\u0e46\u0e47\u0e4d]")


def lk82(text: str) -> str:
    """
    Converts Thai text into phonetic code with LK82 soundex algorithm.
    
    Args:
        text (str): Thai word
        
    Returns:
        str: LK82 soundex code (5 characters)
    """
    if not text or not isinstance(text, str):
        return ""
        
    if _cthainlp is not None:
        return _cthainlp.soundex_lk82(text)

    text = remove_tonemark(text)
    text = _RE_KARANT.sub("", text)
    text = _RE_SIGN.sub("", text)

    if not text:
        return ""

    res = []
    if "ก" <= text[0] <= "ฮ":
        res.append(text[0].translate(_TRANS1))
        text = text[1:]
    else:
        if len(text) > 1:
            res.append(text[1].translate(_TRANS1))
        res.append(text[0].translate(_TRANS2))
        text = text[2:]

    i_v = None
    len_text = len(text)
    for i, c in enumerate(text):
        if c in "\u0e30\u0e31\u0e34\u0e35":
            i_v = i
            res.append("")
        elif c in "\u0e32\u0e36\u0e37\u0e39\u0e45":
            i_v = i
            res.append(c.translate(_TRANS2))
        elif c == "\u0e38":
            i_v = i
            if i == 0 or (text[i - 1] not in "ตธ"):
                res.append(c.translate(_TRANS2))
            else:
                res.append("")
        elif c in "\u0e2b\u0e2d":
            if i + 1 < len_text and text[i + 1] in "\u0e36\u0e37\u0e38\u0e39":
                res.append(c.translate(_TRANS2))
        elif c in "\u0e22\u0e23\u0e24\u0e26\u0e27":
            if i_v == i - 1 or (i + 1 < len_text and text[i + 1] in "\u0e36\u0e37\u0e38\u0e39"):
                res.append(c.translate(_TRANS2))
        else:
            res.append(c.translate(_TRANS2))

    res2 = [res[0]]
    for i in range(1, len(res)):
        if res[i] != res[i - 1]:
            res2.append(res[i])

    return ("".join(res2) + "0000")[:5]


__all__ = ["lk82"]
