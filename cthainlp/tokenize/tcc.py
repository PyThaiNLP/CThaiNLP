"""
Thai Character Cluster (TCC) module for CThaiNLP.
Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
"""

from typing import List, Optional, Set
from collections.abc import Iterator

try:
    import _cthainlp
except ImportError:
    _cthainlp = None


def segment(text: Optional[str]) -> List[str]:
    """
    Subword segmentation using Thai Character Clusters (TCC).
    
    Args:
        text (str, optional): Text to be tokenized into character clusters.
        
    Returns:
        list: List of character clusters (strings).
    """
    if not text or not isinstance(text, str):
        return []
        
    if _cthainlp is not None:
        return _cthainlp.tcc_segment(text)
    
    return list(tcc(text))


def tcc(text: Optional[str]) -> Iterator[str]:
    """
    TCC generator which yields Thai Character Clusters.
    
    Args:
        text (str, optional): Text to be tokenized into character clusters.
        
    Yields:
        str: Character clusters.
    """
    if not text or not isinstance(text, str):
        return
        
    for cluster in segment(text):
        yield cluster


def tcc_pos(text: Optional[str]) -> Set[int]:
    """
    Get ending character positions of Thai Character Clusters.
    
    Args:
        text (str, optional): Text to find cluster positions for.
        
    Returns:
        set: Set of character ending positions.
    """
    if not text or not isinstance(text, str):
        return set()
        
    p_set = set()
    p = 0
    for w in tcc(text):
        p += len(w)
        p_set.add(p)
    return p_set


__all__ = ["segment", "tcc", "tcc_pos"]
