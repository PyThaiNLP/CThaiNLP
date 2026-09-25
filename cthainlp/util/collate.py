"""
Thai collation (sorting according to Thai dictionary order).
Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
"""

import re
from typing import Iterable, List, Pattern

_RE_TONE: Pattern[str] = re.compile(r"[็-์]")
_RE_LV_C: Pattern[str] = re.compile(r"([เ-ไ])([ก-ฮ])")


def _thkey(word: str) -> str:
    cv = _RE_TONE.sub("", word)
    cv = _RE_LV_C.sub(r"\2\1", cv)
    tone_match = _RE_TONE.search(word)
    tone = tone_match.group() if tone_match else ""
    return cv + tone


def collate(data: Iterable[str], reverse: bool = False) -> List[str]:
    """
    Sorts strings according to Thai dictionary order.
    
    Args:
        data (Iterable[str]): Words to sort.
        reverse (bool): Whether to sort in descending order.
        
    Returns:
        list: Sorted list of strings.
    """
    return sorted(data, key=_thkey, reverse=reverse)


__all__ = ["collate"]
