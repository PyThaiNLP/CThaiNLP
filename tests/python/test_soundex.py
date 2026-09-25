#!/usr/bin/env python3
"""
Test suite for CThaiNLP soundex module
"""

import unittest
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

from cthainlp.soundex import soundex, lk82, udom83
from cthainlp import soundex as top_soundex


class TestSoundex(unittest.TestCase):
    """Test Thai soundex algorithms"""

    def test_lk82(self):
        self.assertEqual(lk82(""), "")
        self.assertEqual(lk82("ลัก"), "ร1000")
        self.assertEqual(lk82("รัก"), "ร1000")
        self.assertEqual(lk82("รักษ์"), "ร1000")

    def test_udom83(self):
        self.assertEqual(udom83(""), "")
        self.assertEqual(udom83("ลัก"), "ร100000")
        self.assertEqual(udom83("รัก"), "ร100000")
        self.assertEqual(udom83("รักษ์"), "ร100000")

    def test_soundex_function(self):
        # Default engine is udom83
        self.assertEqual(soundex("รัก"), "ร100000")
        self.assertEqual(soundex("รัก", engine="udom83"), "ร100000")
        self.assertEqual(soundex("รัก", engine="lk82"), "ร1000")
        self.assertEqual(top_soundex("รัก"), "ร100000")

        with self.assertRaises(ValueError):
            soundex("รัก", engine="invalid_engine")


if __name__ == "__main__":
    unittest.main()
