"""
Convert digits between Thai and Arabic numerals and Thai text.
Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
"""

try:
    import _cthainlp
except ImportError:
    _cthainlp = None

_thai_arabic: dict = {
    "๐": "0", "๑": "1", "๒": "2", "๓": "3", "๔": "4",
    "๕": "5", "๖": "6", "๗": "7", "๘": "8", "๙": "9",
}

_arabic_thai: dict = {
    "0": "๐", "1": "๑", "2": "๒", "3": "๓", "4": "๔",
    "5": "๕", "6": "๖", "7": "๗", "8": "๘", "9": "๙",
}

_digit_spell: dict = {
    "0": "ศูนย์", "1": "หนึ่ง", "2": "สอง", "3": "สาม", "4": "สี่",
    "5": "ห้า", "6": "หก", "7": "เจ็ด", "8": "แปด", "9": "เก้า",
}

_spell_digit: dict = {
    "ศูนย์": "0", "หนึ่ง": "1", "สอง": "2", "สาม": "3", "สี่": "4",
    "ห้า": "5", "หก": "6", "เจ็ด": "7", "แปด": "8", "เก้า": "9",
}

_thai_arabic_trans = str.maketrans(_thai_arabic)
_arabic_thai_trans = str.maketrans(_arabic_thai)
_digit_spell_trans = str.maketrans(_digit_spell)


def thai_digit_to_arabic_digit(text: str) -> str:
    """
    Converts Thai digits (e.g. ๑, ๒, ๓) to Arabic digits (1, 2, 3).
    
    Args:
        text (str): Text with Thai digits.
        
    Returns:
        str: Text with Arabic digits.
    """
    if not isinstance(text, str):
        raise TypeError("The text must be str type.")
    if not text:
        return ""
    if _cthainlp is not None:
        return _cthainlp.thai_digit_to_arabic_digit(text)
    return text.translate(_thai_arabic_trans)


def arabic_digit_to_thai_digit(text: str) -> str:
    """
    Converts Arabic digits (e.g. 1, 2, 3) to Thai digits (๑, ๒, ๓).
    
    Args:
        text (str): Text with Arabic digits.
        
    Returns:
        str: Text with Thai digits.
    """
    if not isinstance(text, str):
        raise TypeError("The text must be str type.")
    if not text:
        return ""
    if _cthainlp is not None:
        return _cthainlp.arabic_digit_to_thai_digit(text)
    return text.translate(_arabic_thai_trans)


def digit_to_text(text: str) -> str:
    """
    Spell out digits in Thai text.
    
    Args:
        text (str): Text with digits.
        
    Returns:
        str: Text with digits spelled out in Thai.
    """
    if not isinstance(text, str):
        raise TypeError("The text must be str type.")
    if not text:
        return ""
    text = text.translate(_thai_arabic_trans)
    return text.translate(_digit_spell_trans)


def text_to_arabic_digit(text: str) -> str:
    """
    Converts spelled out digits in Thai to Arabic digits.
    
    Args:
        text (str): A digit spelled out in Thai.
        
    Returns:
        str: An Arabic digit if the text is a Thai digit word, else empty string.
    """
    if not isinstance(text, str):
        raise TypeError("The text must be str type.")
    return _spell_digit.get(text, "")


def text_to_thai_digit(text: str) -> str:
    """
    Converts spelled out digits in Thai to Thai digits.
    
    Args:
        text (str): A digit spelled out in Thai.
        
    Returns:
        str: A Thai digit if the text is a Thai digit word, else empty string.
    """
    if not isinstance(text, str):
        raise TypeError("The text must be str type.")
    arabic = text_to_arabic_digit(text)
    if arabic:
        return arabic_digit_to_thai_digit(arabic)
    return ""


__all__ = [
    "thai_digit_to_arabic_digit",
    "arabic_digit_to_thai_digit",
    "digit_to_text",
    "text_to_arabic_digit",
    "text_to_thai_digit",
]
