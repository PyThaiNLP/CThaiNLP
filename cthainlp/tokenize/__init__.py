"""
Tokenization module for CThaiNLP
"""

import os
import re
from typing import List, Optional, Union
from collections.abc import Iterator

try:
    import _cthainlp
except ImportError:
    _cthainlp = None


def _get_default_dict_path() -> Optional[str]:
    """
    Get the default dictionary file path.
    
    Returns:
        str: Absolute path to the default dictionary file
    """
    # Get directory of this file
    module_dir = os.path.dirname(os.path.abspath(__file__))
    
    # Try cthainlp/data/thai_words.txt (when tokenize is a subpackage)
    parent_pkg_dir = os.path.dirname(module_dir)
    dict_path = os.path.join(parent_pkg_dir, "data", "thai_words.txt")
    if os.path.exists(dict_path):
        return dict_path
    
    # Try CThaiNLP/data/thai_words.txt (root development mode)
    root_dir = os.path.dirname(parent_pkg_dir)
    dict_path = os.path.join(root_dir, "data", "thai_words.txt")
    if os.path.exists(dict_path):
        return dict_path
        
    return None


def word_tokenize(
    text: str,
    engine: str = "newmm",
    custom_dict: Optional[str] = None,
    keep_whitespace: bool = True,
) -> List[str]:
    """
    Segment Thai text into words.
    
    This is the main tokenization function, compatible with PyThaiNLP's API.
    
    Args:
        text (str): Input text to tokenize (UTF-8 encoded)
        engine (str): Tokenization engine. Currently only 'newmm' is supported.
                     Default is 'newmm'.
        custom_dict (str, optional): Path to custom dictionary file (one word per line).
                                     If None, uses the default dictionary.
        keep_whitespace (bool): Whether to keep whitespace tokens in the result.
                               Default is True (whitespace is preserved).
    
    Returns:
        list: List of tokens (strings)
    
    Raises:
        RuntimeError: If tokenization fails
        ValueError: If unsupported engine is specified
        ImportError: If C extension is not properly installed
        TypeError: If text is not a string
    """
    if _cthainlp is None:
        raise ImportError(
            "CThaiNLP C extension is not installed. "
            "Please install the package properly using: pip install cthainlp"
        )
    
    if engine != "newmm":
        raise ValueError(
            f"Unsupported engine '{engine}'. Currently only 'newmm' is supported."
        )
    
    if not isinstance(text, str):
        raise TypeError(f"text must be a string, got {type(text)}")
    
    # Handle empty string case
    if not text:
        return []
    
    # Determine which dictionary to use
    if custom_dict is not None:
        # User provided a custom dictionary
        if not os.path.exists(custom_dict):
            raise FileNotFoundError(f"Dictionary file not found: {custom_dict}")
        dict_path = custom_dict
    else:
        # Use default dictionary
        dict_path = _get_default_dict_path()
    
    # Call the C extension
    tokens = _cthainlp.segment(text, dict_path)
    
    # Handle keep_whitespace parameter
    if not keep_whitespace:
        # Filter out whitespace-only tokens
        tokens = [token for token in tokens if not token.isspace()]
    
    return tokens


# Alias for compatibility
segment = word_tokenize


def subword_tokenize(
    text: str,
    engine: str = "tcc",
    keep_whitespace: bool = True,
) -> List[str]:
    """
    Tokenize text into subwords (character clusters).
    
    Args:
        text (str): Text to be tokenized into character clusters.
        engine (str): Subword tokenizer engine ('tcc'). Default is 'tcc'.
        keep_whitespace (bool): Whether to keep whitespace. Default is True.
        
    Returns:
        list: List of subwords (character clusters)
    """
    if not text or not isinstance(text, str):
        return []
    
    if engine == "tcc":
        from cthainlp.tokenize import tcc
        segments = tcc.segment(text)
    else:
        raise ValueError(f"Tokenizer '{engine}' not found.")
    
    if not keep_whitespace:
        segments = [s for s in segments if not s.isspace()]
        
    return segments


def display_cell_tokenize(text: str) -> List[str]:
    """
    Tokenizes Thai text into display cells without splitting tone marks.
    
    Args:
        text (str): text to be tokenized
        
    Returns:
        list: list of display cells
    """
    if not text or not isinstance(text, str):
        return []

    display_cells = []
    current_cell = ""
    text = text.replace("ำ", "ํา")

    for char in text:
        if re.match(r"[\u0E31\u0E34-\u0E3A\u0E47-\u0E4E]", char):
            current_cell += char
        else:
            if current_cell:
                display_cells.append(current_cell)
            current_cell = char

    if current_cell:
        display_cells.append(current_cell)

    return display_cells


def word_detokenize(
    segments: Union[List[List[str]], List[str]], output: str = "str"
) -> Union[List[List[str]], str]:
    """
    Detokenizes the list of words into text.
    
    Args:
        segments: List of sentences, each with a list of words, or a list of words.
        output (str): 'str' or 'list'
        
    Returns:
        Thai text string or list of sentences.
    """
    from cthainlp import thai_characters

    if not segments:
        return "" if output == "str" else []

    if isinstance(segments[0], str):
        segments = [segments]  # type: ignore[assignment]

    list_all: List[List[str]] = []
    for s in segments:
        list_sents: List[str] = []
        mark_index: List[int] = []
        space_index: List[int] = []
        for j, w in enumerate(s):
            if not w:
                continue
            if j > 0:
                p_w = s[j - 1]
                if (
                    w[0] not in thai_characters
                    and not w.isspace()
                    and not p_w.isspace()
                ):
                    list_sents.append(" ")
                elif p_w and p_w[0] not in thai_characters and not p_w.isspace():
                    list_sents.append(" ")
                elif w == "ๆ":
                    if not p_w.isspace():
                        list_sents.append(" ")
                    mark_index.append(j)
                elif w.isspace() and j - 1 not in space_index:
                    space_index.append(j)
                elif j - 1 in mark_index:
                    list_sents.append(" ")
            list_sents.append(w)
        list_all.append(list_sents)

    if output == "list":
        return list_all

    text_list: List[str] = []
    for sent_tokens in list_all:
        text_list.append("".join(sent_tokens))
    return " ".join(text_list)


def sent_tokenize(
    text: Union[str, List[str]],
    engine: str = "whitespace",
    keep_whitespace: bool = True,
) -> Union[List[str], List[List[str]]]:
    """
    Sentence tokenizer. Tokenizes running text into sentences.
    
    Args:
        text: Text string or list of word tokens.
        engine (str): Engine to use ('whitespace', 'whitespace+newline').
        keep_whitespace (bool): Whether to keep whitespace.
        
    Returns:
        List of sentences.
    """
    if not text or not isinstance(text, (str, list)):
        return []

    if isinstance(text, list):
        try:
            original_text = "".join(text)
        except ValueError:
            return []
    else:
        original_text = str(text)

    if engine == "whitespace":
        segments = [s for s in re.split(r" +", original_text) if s]
    elif engine == "whitespace+newline":
        segments = original_text.split()
    else:
        raise ValueError(f"Sentence tokenizer '{engine}' not supported.")

    if not keep_whitespace:
        segments = [s.strip() for s in segments if s.strip()]

    return segments


__all__ = [
    "word_tokenize",
    "segment",
    "subword_tokenize",
    "display_cell_tokenize",
    "word_detokenize",
    "sent_tokenize",
]
