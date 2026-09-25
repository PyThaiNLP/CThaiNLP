#!/usr/bin/env python3
"""
Test suite for CThaiNLP Python bindings
"""

import unittest
import os
import sys

# Add parent directory to path to allow importing cthainlp
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

from cthainlp import word_tokenize


class TestWordTokenize(unittest.TestCase):
    """Test cases for word_tokenize function"""
    
    def test_basic_thai_sentence(self):
        """Test basic Thai sentence segmentation"""
        text = "ฉันไปโรงเรียน"
        tokens = word_tokenize(text)
        
        # Check that we get a list
        self.assertIsInstance(tokens, list)
        
        # Check that all tokens are strings
        for token in tokens:
            self.assertIsInstance(token, str)
        
        # With default dictionary, this should segment into known words
        # Note: exact results depend on the dictionary
        self.assertGreater(len(tokens), 0)
        
        # Join should recreate original text
        self.assertEqual(''.join(tokens), text)
    
    def test_english_text(self):
        """Test English text segmentation"""
        text = "hello world"
        tokens = word_tokenize(text)
        
        self.assertIsInstance(tokens, list)
        self.assertGreater(len(tokens), 0)
        self.assertEqual(''.join(tokens), text)
    
    def test_numbers_only(self):
        """Test numbers segmentation"""
        text = "123"
        tokens = word_tokenize(text)
        
        self.assertIsInstance(tokens, list)
        self.assertEqual(tokens, ["123"])
    
    def test_mixed_content(self):
        """Test mixed Thai, English, and numbers"""
        text = "ไป ABC 123"
        tokens = word_tokenize(text)
        
        self.assertIsInstance(tokens, list)
        self.assertGreater(len(tokens), 0)
        self.assertEqual(''.join(tokens), text)
    
    def test_empty_string(self):
        """Test empty string"""
        text = ""
        tokens = word_tokenize(text)
        
        self.assertIsInstance(tokens, list)
        self.assertEqual(len(tokens), 0)
    
    def test_single_word(self):
        """Test single Thai word"""
        text = "ไป"
        tokens = word_tokenize(text)
        
        self.assertIsInstance(tokens, list)
        self.assertEqual(tokens, ["ไป"])
    
    def test_with_custom_dict(self):
        """Test with custom dictionary"""
        text = "ฉันไปโรงเรียน"
        dict_path = os.path.join(os.path.dirname(__file__), "../../data/thai_words.txt")
        
        if os.path.exists(dict_path):
            tokens = word_tokenize(text, custom_dict=dict_path)
            self.assertIsInstance(tokens, list)
            # Should properly segment with full dictionary
            self.assertIn("ฉัน", tokens)
            self.assertIn("ไป", tokens)
            self.assertIn("โรงเรียน", tokens)
    
    def test_engine_parameter(self):
        """Test engine parameter"""
        text = "ฉันไปโรงเรียน"
        tokens = word_tokenize(text, engine="newmm")
        
        self.assertIsInstance(tokens, list)
        self.assertGreater(len(tokens), 0)
    
    def test_invalid_engine(self):
        """Test invalid engine raises ValueError"""
        text = "ฉันไปโรงเรียน"
        
        with self.assertRaises(ValueError):
            word_tokenize(text, engine="invalid_engine")
    
    def test_invalid_text_type(self):
        """Test invalid text type raises TypeError"""
        with self.assertRaises(TypeError):
            word_tokenize(123)
    
    def test_nonexistent_dict(self):
        """Test nonexistent dictionary file raises FileNotFoundError"""
        text = "ฉันไปโรงเรียน"
        
        with self.assertRaises(FileNotFoundError):
            word_tokenize(text, custom_dict="/nonexistent/path/dict.txt")


class TestCompatibility(unittest.TestCase):
    """Test PyThaiNLP API compatibility"""
    
    def test_similar_to_pythainlp(self):
        """Test that API is similar to PyThaiNLP"""
        # This test verifies that the API matches PyThaiNLP's style
        from cthainlp import word_tokenize
        
        text = "ฉันไปโรงเรียน"
        
        # Should work like PyThaiNLP's word_tokenize
        tokens = word_tokenize(text, engine="newmm")
        
        self.assertIsInstance(tokens, list)
        self.assertGreater(len(tokens), 0)
    
    def test_newmm(self):
        """Test newmm module compatibility with PyThaiNLP"""
        from cthainlp import newmm
        from cthainlp import word_tokenize
        
        self.assertEqual(newmm.segment(None), [])
        self.assertEqual(newmm.segment(""), [])
        self.assertEqual(
            word_tokenize("ฉันรักภาษาไทยเพราะฉันเป็นคนไทย", engine="newmm"),
            ["ฉัน", "รัก", "ภาษาไทย", "เพราะ", "ฉัน", "เป็น", "คนไทย"],
        )
        self.assertEqual(
            word_tokenize("19...", engine="newmm"),
            ["19", "..."],
        )
        self.assertEqual(
            word_tokenize("19.", engine="newmm"),
            ["19", "."],
        )
        self.assertEqual(
            word_tokenize("19.84", engine="newmm"),
            ["19.84"],
        )
        self.assertEqual(
            word_tokenize("127.0.0.1", engine="newmm"),
            ["127.0.0.1"],
        )
        self.assertEqual(
            word_tokenize("USD1,984.42", engine="newmm"),
            ["USD", "1,984.42"],
        )
        self.assertEqual(
            word_tokenize(
                "สวัสดีครับ สบายดีไหมครับ",
                engine="newmm",
                keep_whitespace=True,
            ),
            ["สวัสดี", "ครับ", " ", "สบายดี", "ไหม", "ครับ"],
        )
        self.assertEqual(
            word_tokenize("จุ๋มง่วงนอนยัง", engine="newmm"),
            ["จุ๋ม", "ง่วงนอน", "ยัง"],
        )
        self.assertEqual(word_tokenize("จุ๋มง่วง", engine="newmm"), ["จุ๋ม", "ง่วง"])
        self.assertEqual(
            word_tokenize("จุ๋ม   ง่วง", engine="newmm", keep_whitespace=False),
            ["จุ๋ม", "ง่วง"],
        )
        self.assertNotIn(
            " ",
            word_tokenize(
                "จุ๋มง่วง",
                keep_whitespace=False,
            ),
        )
        self.assertEqual(
            word_tokenize("(คนไม่เอา)", engine="newmm"),
            ["(", "คน", "ไม่", "เอา", ")"],
        )
        self.assertEqual(
            word_tokenize("กม/ชม", engine="newmm"), ["กม", "/", "ชม"]
        )
        self.assertEqual(
            word_tokenize("สีหน้า(รถ)", engine="newmm"), ["สีหน้า", "(", "รถ", ")"]
        )

    def test_additional_tokenize_apis(self):
        """Test additional tokenization APIs ported from PyThaiNLP"""
        from cthainlp.tokenize import (
            display_cell_tokenize,
            word_detokenize,
            sent_tokenize,
            subword_tokenize,
        )
        from cthainlp import sent_tokenize as top_sent_tok
        from cthainlp import subword_tokenize as top_subword_tok

        # display_cell_tokenize
        self.assertEqual(display_cell_tokenize(""), [])
        cells = display_cell_tokenize("แม่น้ำอยู่ที่ไหน")
        self.assertEqual(cells, ['แ', 'ม่', 'น้ํ', 'า', 'อ', 'ยู่', 'ที่', 'ไ', 'ห', 'น'])

        # word_detokenize
        self.assertEqual(word_detokenize([]), "")
        self.assertEqual(word_detokenize([], output="list"), [])
        self.assertEqual(word_detokenize(["เรา", "เล่น"]), "เราเล่น")

        # sent_tokenize
        self.assertEqual(sent_tokenize(""), [])
        sents = sent_tokenize("ฉันไปประชุมเมื่อวันที่ 11 มีนาคม", engine="whitespace")
        self.assertEqual(sents, ["ฉันไปประชุมเมื่อวันที่", "11", "มีนาคม"])
        sents_nl = sent_tokenize("ฉันไปประชุม\nเมื่อวันที่ 11 มีนาคม", engine="whitespace+newline")
        self.assertEqual(sents_nl, ["ฉันไปประชุม", "เมื่อวันที่", "11", "มีนาคม"])
        self.assertEqual(top_sent_tok("ฉันไป 11 มีนาคม"), ["ฉันไป", "11", "มีนาคม"])

        # subword_tokenize
        self.assertEqual(subword_tokenize(""), [])
        subwords = subword_tokenize("ฉันไปโรงเรียน")
        self.assertEqual(subwords, ["ฉั", "น", "ไป", "โรง", "เรี", "ยน"])
        self.assertEqual(top_subword_tok("ฉันไปโรงเรียน"), ["ฉั", "น", "ไป", "โรง", "เรี", "ยน"])


def run_tests():
    """Run all tests"""
    suite = unittest.TestLoader().loadTestsFromModule(sys.modules[__name__])
    runner = unittest.TextTestRunner(verbosity=2)
    result = runner.run(suite)
    
    return 0 if result.wasSuccessful() else 1


if __name__ == "__main__":
    sys.exit(run_tests())
