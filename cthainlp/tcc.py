"""
TCC module for CThaiNLP
Provides Thai Character Cluster tokenization compatible with PyThaiNLP's API
"""

from cthainlp.tokenize.tcc import segment, tcc, tcc_pos

__all__ = ["segment", "tcc", "tcc_pos"]
