#pragma once
//---------------------------------------------------------------------------
namespace string_matching {
//---------------------------------------------------------------------------
/// Find the first occurrence of needle within text using a naive algorithm
const char* matchNaive(const char* text, const char* pattern);
//---------------------------------------------------------------------------
/// Find the first occurrence of needle within haystack using the Knuth-Morris-Pratt algorithm
const char* matchKMP(const char* text, const char* pattern);
//---------------------------------------------------------------------------
} // namespace string_matching
//---------------------------------------------------------------------------
