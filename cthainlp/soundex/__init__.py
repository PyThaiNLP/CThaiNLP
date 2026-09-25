"""
Thai soundex module for CThaiNLP
Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
"""

from cthainlp.soundex.lk82 import lk82
from cthainlp.soundex.udom83 import udom83


def soundex(text: str, engine: str = "udom83") -> str:
    """
    Converts Thai text into phonetic code.
    
    Args:
        text (str): Word to encode.
        engine (str): Soundex engine ('udom83' or 'lk82'). Default is 'udom83'.
        
    Returns:
        str: Soundex code.
    """
    if engine == "lk82":
        return lk82(text)
    elif engine == "udom83":
        return udom83(text)
    else:
        raise ValueError(
            f"Unsupported soundex engine '{engine}'. Options are 'udom83', 'lk82'."
        )


__all__ = ["soundex", "lk82", "udom83"]
