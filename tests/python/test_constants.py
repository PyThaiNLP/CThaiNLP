#!/usr/bin/env python3
"""
Test suite for CThaiNLP character constants
"""

import unittest
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

import cthainlp


class TestConstants(unittest.TestCase):
    """Test Thai character constants"""

    def test_constants_defined(self):
        self.assertTrue(hasattr(cthainlp, "thai_consonants"))
        self.assertEqual(len(cthainlp.thai_consonants), 44)
        self.assertTrue(cthainlp.thai_consonants.startswith("ก"))
        self.assertTrue(cthainlp.thai_consonants.endswith("ฮ"))

        self.assertTrue(hasattr(cthainlp, "thai_vowels"))
        self.assertTrue(hasattr(cthainlp, "thai_lead_vowels"))
        self.assertEqual(cthainlp.thai_lead_vowels, "เแโใไ")

        self.assertTrue(hasattr(cthainlp, "thai_follow_vowels"))
        self.assertTrue(hasattr(cthainlp, "thai_above_vowels"))
        self.assertTrue(hasattr(cthainlp, "thai_below_vowels"))
        self.assertTrue(hasattr(cthainlp, "thai_tonemarks"))
        self.assertEqual(len(cthainlp.thai_tonemarks), 4)

        self.assertTrue(hasattr(cthainlp, "thai_signs"))
        self.assertTrue(hasattr(cthainlp, "thai_letters"))
        self.assertTrue(hasattr(cthainlp, "thai_punctuations"))
        self.assertTrue(hasattr(cthainlp, "thai_digits"))
        self.assertEqual(cthainlp.thai_digits, "๐๑๒๓๔๕๖๗๘๙")

        self.assertTrue(hasattr(cthainlp, "thai_symbols"))
        self.assertEqual(cthainlp.thai_symbols, "฿")

        self.assertTrue(hasattr(cthainlp, "thai_characters"))
        self.assertTrue(hasattr(cthainlp, "thai_pangram"))
        self.assertIn("กีฬาบังลังก์", cthainlp.thai_pangram)


if __name__ == "__main__":
    unittest.main()
