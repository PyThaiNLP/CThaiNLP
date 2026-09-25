"""
Convert number value to Thai read out.
Ported from PyThaiNLP (https://github.com/PyThaiNLP/pythainlp)
"""

from typing import Optional

_VALUES: list = [
    "",
    "หนึ่ง",
    "สอง",
    "สาม",
    "สี่",
    "ห้า",
    "หก",
    "เจ็ด",
    "แปด",
    "เก้า",
]
_PLACES: list = ["", "สิบ", "ร้อย", "พัน", "หมื่น", "แสน", "ล้าน"]
_EXCEPTIONS: dict = {"หนึ่งสิบ": "สิบ", "สองสิบ": "ยี่สิบ", "สิบหนึ่ง": "สิบเอ็ด"}


def _num_to_thaiword_block(num: int) -> str:
    """Convert a positive integer < 1,000,000 to Thai text."""
    if num == 0:
        return ""

    output = ""
    num_str = str(num)
    for place, value in enumerate(list(num_str[::-1])):
        if value != "0":
            output = _VALUES[int(value)] + _PLACES[place] + output

    for search, replac in _EXCEPTIONS.items():
        output = output.replace(search, replac)

    # เอ็ด rule: trailing หนึ่ง in ones place
    if num != 1 and output.endswith("หนึ่ง"):
        output = output[: -len("หนึ่ง")] + "เอ็ด"

    return output


def num_to_thaiword(number: Optional[int]) -> str:
    """
    Converts a number to Thai text.
    
    Args:
        number (int, optional): An integer number to be converted to Thai text.
        
    Returns:
        str: Text representing the number in Thai.
    """
    if number is None:
        return ""

    if number == 0:
        return "ศูนย์"

    number_abs = abs(number)
    number_str = str(number_abs)

    # Split into groups of 6 digits from right side
    groups: list = []
    while number_str:
        group = number_str[-6:]
        number_str = number_str[:-6]
        groups.append(group)

    output = ""
    for i in range(len(groups) - 1, -1, -1):
        group_num = int(groups[i])
        if group_num > 0:
            output += _num_to_thaiword_block(group_num)
        if i > 0:
            output += "ล้าน"

    # Global เอ็ด rule: trailing หนึ่ง in the full text
    if number_abs != 1 and output.endswith("หนึ่ง"):
        output = output[: -len("หนึ่ง")] + "เอ็ด"

    if number < 0:
        output = "ลบ" + output

    return output


def bahttext(number: float) -> str:
    """
    Converts a number to Thai text and adds a suffix "บาท" (Baht).
    Precision fixed at two decimal places to fit "สตางค์" (Satang) unit.
    
    Args:
        number (float or int): Number to be converted into Thai Baht currency format.
        
    Returns:
        str: Amount in Thai currency text.
        
    Raises:
        TypeError: If number is not int or float.
    """
    if not isinstance(number, (int, float)):
        raise TypeError(
            f"number must be a numeric type, not {type(number).__name__!r}"
        )

    if number == 0:
        return "ศูนย์บาทถ้วน"

    num_int_str, num_dec_str = f"{number:.2f}".split(".")
    num_int = int(num_int_str)
    num_dec = int(num_dec_str)

    ret = ""
    baht = num_to_thaiword(num_int)
    if baht:
        ret = "".join([ret, baht, "บาท"])

    satang = num_to_thaiword(num_dec)
    if satang and satang != "ศูนย์":
        ret = "".join([ret, satang, "สตางค์"])
    else:
        ret = "".join([ret, "ถ้วน"])

    return ret


__all__ = ["num_to_thaiword", "bahttext"]
