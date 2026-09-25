#!/usr/bin/env python3
"""
Test suite for CThaiNLP TCC module
"""

import unittest
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

from cthainlp import tcc
from cthainlp.tokenize import tcc as tok_tcc
from cthainlp.tokenize import subword_tokenize


class TestTCC(unittest.TestCase):
    """Test Thai Character Cluster functions"""

    def test_segment(self):
        self.assertEqual(tcc.segment(None), [])
        self.assertEqual(tcc.segment(""), [])
        res = tcc.segment("ฉันไปโรงเรียน")
        self.assertIsInstance(res, list)
        self.assertEqual("".join(res), "ฉันไปโรงเรียน")
        self.assertEqual(tcc.segment("ฉันไปโรงเรียน"), ["ฉั", "น", "ไป", "โรง", "เรี", "ยน"])

    def test_tcc_generator(self):
        gen = tcc.tcc("สวัสดี")
        items = list(gen)
        self.assertGreater(len(items), 0)
        self.assertEqual("".join(items), "สวัสดี")

    def test_tcc_pos(self):
        self.assertEqual(tcc.tcc_pos(None), set())
        self.assertEqual(tcc.tcc_pos(""), set())
        pos = tcc.tcc_pos("ฉันไปโรงเรียน")
        self.assertIsInstance(pos, set)
        self.assertIn(len("ฉันไปโรงเรียน"), pos)

    def test_module_aliases(self):
        self.assertEqual(tcc.segment("ไป"), tok_tcc.segment("ไป"))
        self.assertEqual(tcc.tcc_pos("ไป"), tok_tcc.tcc_pos("ไป"))

    def test_subword_tokenize(self):
        res = subword_tokenize("ฉันไปโรงเรียน", engine="tcc")
        self.assertEqual(res, ["ฉั", "น", "ไป", "โรง", "เรี", "ยน"])
        
        # Test keep_whitespace
        text = "ไป โรงเรียน"
        self.assertIn(" ", subword_tokenize(text, keep_whitespace=True))
        self.assertNotIn(" ", subword_tokenize(text, keep_whitespace=False))

        with self.assertRaises(ValueError):
            subword_tokenize("ฉัน", engine="invalid_engine")


if __name__ == "__main__":
    unittest.main()
