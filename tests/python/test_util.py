#!/usr/bin/env python3
"""
Test suite for CThaiNLP utility module
"""

import unittest
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '../..')))

import cthainlp.util as u
from cthainlp import collate


class TestUtilThai(unittest.TestCase):
    """Test Thai text checking and character utilities"""

    def test_is_thai_char(self):
        self.assertTrue(u.is_thai_char("ก"))
        self.assertTrue(u.is_thai_char("๕"))
        self.assertTrue(u.is_thai_char("์"))
        self.assertFalse(u.is_thai_char("A"))
        self.assertFalse(u.is_thai_char("1"))
        self.assertFalse(u.is_thai_char(""))
        # deprecated alias
        self.assertTrue(u.isthaichar("ก"))

    def test_is_thai(self):
        self.assertTrue(u.is_thai("กาลเวลา"))
        self.assertTrue(u.is_thai("กาลเวลา."))
        self.assertFalse(u.is_thai("กาล-เวลา"))
        self.assertTrue(u.is_thai("กาล-เวลา", ignore_chars=".-"))
        self.assertTrue(u.is_thai("กาล-เวลา +66", ignore_chars="01234567890+-., "))
        self.assertFalse(u.is_thai("English"))
        # deprecated alias
        self.assertTrue(u.isthai("กาลเวลา"))

    def test_count_thai(self):
        self.assertEqual(u.count_thai(""), 0.0)
        self.assertEqual(u.count_thai("ไทยเอ็นแอลพี 3.0"), 100.0)
        self.assertEqual(u.count_thai("PyThaiNLP 3.0"), 0.0)
        self.assertEqual(u.count_thai("ใช้งาน PyThaiNLP 3.0"), 40.0)
        # deprecated alias
        self.assertEqual(u.countthai("ไทยเอ็นแอลพี 3.0"), 100.0)


class TestUtilDigitConv(unittest.TestCase):
    """Test digit conversion utilities"""

    def test_arabic_digit_to_thai_digit(self):
        self.assertEqual(
            u.arabic_digit_to_thai_digit("เป็นจำนวน 123,400.25 บาท"),
            "เป็นจำนวน ๑๒๓,๔๐๐.๒๕ บาท",
        )
        self.assertEqual(u.arabic_digit_to_thai_digit(""), "")
        with self.assertRaises(TypeError):
            u.arabic_digit_to_thai_digit(123)

    def test_thai_digit_to_arabic_digit(self):
        self.assertEqual(
            u.thai_digit_to_arabic_digit("เป็นจำนวน ๑๒๓,๔๐๐.๒๕ บาท"),
            "เป็นจำนวน 123,400.25 บาท",
        )
        self.assertEqual(u.thai_digit_to_arabic_digit(""), "")
        with self.assertRaises(TypeError):
            u.thai_digit_to_arabic_digit(123)

    def test_digit_to_text(self):
        self.assertEqual(u.digit_to_text("123"), "หนึ่งสองสาม")
        self.assertEqual(u.digit_to_text("๕๖๗"), "ห้าหกเจ็ด")
        self.assertEqual(
            u.digit_to_text("เบอร์โทร 0812345678"),
            "เบอร์โทร ศูนย์แปดหนึ่งสองสามสี่ห้าหกเจ็ดแปด",
        )

    def test_text_to_arabic_digit(self):
        self.assertEqual(u.text_to_arabic_digit("ศูนย์"), "0")
        self.assertEqual(u.text_to_arabic_digit("หนึ่ง"), "1")
        self.assertEqual(u.text_to_arabic_digit("แปด"), "8")
        self.assertEqual(u.text_to_arabic_digit("เก้า"), "9")
        self.assertEqual(u.text_to_arabic_digit("สิบ"), "")

    def test_text_to_thai_digit(self):
        self.assertEqual(u.text_to_thai_digit("ศูนย์"), "๐")
        self.assertEqual(u.text_to_thai_digit("หนึ่ง"), "๑")
        self.assertEqual(u.text_to_thai_digit("แปด"), "๘")
        self.assertEqual(u.text_to_thai_digit("เก้า"), "๙")
        self.assertEqual(u.text_to_thai_digit("สิบ"), "")


class TestUtilNumToWord(unittest.TestCase):
    """Test number to Thai words and bahttext utilities"""

    def test_num_to_thaiword(self):
        self.assertEqual(u.num_to_thaiword(None), "")
        self.assertEqual(u.num_to_thaiword(0), "ศูนย์")
        self.assertEqual(u.num_to_thaiword(1), "หนึ่ง")
        self.assertEqual(u.num_to_thaiword(10), "สิบ")
        self.assertEqual(u.num_to_thaiword(11), "สิบเอ็ด")
        self.assertEqual(u.num_to_thaiword(20), "ยี่สิบ")
        self.assertEqual(u.num_to_thaiword(21), "ยี่สิบเอ็ด")
        self.assertEqual(u.num_to_thaiword(100), "หนึ่งร้อย")
        self.assertEqual(u.num_to_thaiword(101), "หนึ่งร้อยเอ็ด")
        self.assertEqual(u.num_to_thaiword(1000), "หนึ่งพัน")
        self.assertEqual(u.num_to_thaiword(1000000), "หนึ่งล้าน")
        self.assertEqual(u.num_to_thaiword(1000001), "หนึ่งล้านเอ็ด")
        self.assertEqual(u.num_to_thaiword(-15), "ลบสิบห้า")

    def test_bahttext(self):
        self.assertEqual(u.bahttext(0), "ศูนย์บาทถ้วน")
        self.assertEqual(u.bahttext(1), "หนึ่งบาทถ้วน")
        self.assertEqual(u.bahttext(1.50), "หนึ่งบาทห้าสิบสตางค์")
        self.assertEqual(u.bahttext(21), "ยี่สิบเอ็ดบาทถ้วน")
        self.assertEqual(u.bahttext(101.25), "หนึ่งร้อยเอ็ดบาทยี่สิบห้าสตางค์")
        with self.assertRaises(TypeError):
            u.bahttext("100")


class TestUtilNormalize(unittest.TestCase):
    """Test text normalization utilities"""

    def test_remove_dangling(self):
        self.assertEqual(u.remove_dangling("๊ก"), "ก")
        self.assertEqual(u.remove_dangling("คำ ่ที่สอง"), "คำ ที่สอง")

    def test_remove_dup_spaces(self):
        self.assertEqual(u.remove_dup_spaces("ก    ข    ค"), "ก ข ค")
        self.assertEqual(u.remove_dup_spaces(""), "")

    def test_remove_tonemark(self):
        self.assertEqual(u.remove_tonemark("กิ่งก่า"), "กิงกา")
        self.assertEqual(u.remove_tone("กิ่งก่า"), "กิงกา")

    def test_remove_zw(self):
        self.assertEqual(u.remove_zw("สวัสดี\u200bครับ"), "สวัสดีครับ")
        self.assertEqual(u.remove_zw("ภาษา\u200cไทย"), "ภาษาไทย")

    def test_remove_spaces_before_marks(self):
        self.assertEqual(u.remove_spaces_before_marks("พ ุ่มดอกไม้"), "พุ่มดอกไม้")

    def test_reorder_vowels(self):
        self.assertEqual(u.reorder_vowels("เเปลก"), "แปลก")

    def test_remove_repeat_vowels(self):
        self.assertEqual(u.remove_repeat_vowels("นานาาา"), "นานา")
        self.assertEqual(u.remove_repeat_vowels("ดีีีี"), "ดี")

    def test_normalize(self):
        self.assertEqual(u.normalize("เเปลก"), "แปลก")
        self.assertEqual(u.normalize("นานาาา"), "นานา")
        self.assertEqual(u.normalize("ดีีีี"), "ดี")
        self.assertEqual(u.normalize("พ ุ่มดอกไม้"), "พุ่มดอกไม้")


class TestUtilCollate(unittest.TestCase):
    """Test Thai collation"""

    def test_collate(self):
        words = ["ไก่", "เกิด", "กาล", "เป็ด", "หมู", "วัว", "วันที่"]
        expected_asc = ["กาล", "เกิด", "ไก่", "เป็ด", "วันที่", "วัว", "หมู"]
        expected_desc = ["หมู", "วัว", "วันที่", "เป็ด", "ไก่", "เกิด", "กาล"]

        self.assertEqual(u.collate(words), expected_asc)
        self.assertEqual(collate(words), expected_asc)
        self.assertEqual(u.collate(words, reverse=True), expected_desc)


if __name__ == "__main__":
    unittest.main()
