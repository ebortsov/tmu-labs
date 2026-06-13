#include "StringMatching.hpp"
#include <cstddef>
#include <vector>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace string_matching {
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
unsigned length(const char* str)
// Compute the length of a string
{
    unsigned len = 0;
    while (*str++ != 0)
        len++;
    return len;
}
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
const char* matchNaive(const char* text, const char* pattern)
// Find the first occurrence of needle within haystack
{
    for (const char* textIter = text; *textIter != 0; ++textIter) {
        const char* patternIter;
        for (patternIter = pattern; *patternIter != 0; ++patternIter) {
            size_t matchLength = patternIter - pattern;

            if (*(textIter + matchLength) == 0)
                return nullptr;
            if (*patternIter != *(textIter + matchLength))
                break;
        }

        if (*patternIter == 0)
            return textIter;
    }

    return nullptr;
}
//---------------------------------------------------------------------------
const char* matchKMP(const char* text, const char* pattern)
// Find the first occurrence of needle within haystack
{
    unsigned textLength = length(text);
    unsigned patternLength = length(pattern);

    vector<unsigned> prefixTable(patternLength);
    {
        unsigned previousSuffixLength = 0;
        unsigned i = 1;

        prefixTable[0] = 0;
        while (i < patternLength) {
            if (pattern[i] == pattern[previousSuffixLength]) {
                previousSuffixLength++;
                prefixTable[i] = previousSuffixLength;
                i++;
            } else if (previousSuffixLength != 0) {
                previousSuffixLength = prefixTable[previousSuffixLength - 1];
            } else {
                prefixTable[i] = 0;
                i++;
            }
        }
    }

    unsigned textIndex = 0;
    unsigned patternIndex = 0;

    while (textIndex < textLength) {
        if (text[textIndex] == pattern[patternIndex]) {
            textIndex++;
            patternIndex++;

            if (patternIndex == patternLength)
                return text + textIndex - patternLength;
        } else {
            if (patternIndex > 0)
                patternIndex = prefixTable[patternIndex - 1];
            else
                ++textIndex;
        }
    }

    return nullptr;
}
//---------------------------------------------------------------------------
} // namespace string_matching
//---------------------------------------------------------------------------
