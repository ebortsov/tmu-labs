#pragma once
#include <array>
#include <cstdint>
#include <string>

static constexpr std::array ENCODING_TABLE = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '+', '/'};

static constexpr std::array DECODING_TABLE = [] {
  std::array<char, 256> table{};
  for (size_t i = 0; i < 64; ++i) {
    table[ENCODING_TABLE[i]] = i;
  }
  return table;
}();

template <typename I>
concept ByteIter = std::input_iterator<I> && requires(I i) {
  { static_cast<std::byte>(*i) };
};

template <typename R>
concept ByteRange = std::ranges::range<R> && requires(R range) {
  { range.begin() } -> ByteIter;
};

template <typename T>
struct byte_output {}; // NOLINT

template <typename Container>
struct byte_output<std::back_insert_iterator<Container>> {
  using value_type = typename Container::value_type;
};

template <std::output_iterator<std::byte> O>
struct byte_output<O> {
  using value_type = std::byte;
};

template <typename O>
concept ByteOutput = requires() { typename byte_output<O>::value_type; };

template <ByteRange R>
std::string base64Encode(R range) {
  size_t inputLength = range.size();
  size_t outputLength = 4 * ((inputLength + 2) / 3);
  std::string out{};
  out.reserve(outputLength);
  auto iter = range.begin();

  for (size_t i = 0; i < inputLength;) {
    uint32_t octet1 = i++ < inputLength ? static_cast<uint32_t>(*iter++) : 0;
    uint32_t octet2 = i++ < inputLength ? static_cast<uint32_t>(*iter++) : 0;
    uint32_t octet3 = i++ < inputLength ? static_cast<uint32_t>(*iter++) : 0;

    uint32_t triple = (octet1 << 0x10) + (octet2 << 0x08) + octet3;

    out.push_back(ENCODING_TABLE[(triple >> 3 * 6) & 0x3F]);
    out.push_back(ENCODING_TABLE[(triple >> 2 * 6) & 0x3F]);
    out.push_back(ENCODING_TABLE[(triple >> 1 * 6) & 0x3F]);
    out.push_back(ENCODING_TABLE[(triple >> 0 * 6) & 0x3F]);
  }

  switch (inputLength % 3) {
    case 1:
      out[outputLength - 2] = '=';
      out[outputLength - 1] = '=';
      break;
    case 2:
      out[outputLength - 1] = '=';
      break;
    default:
      break;
  }

  return out;
}

template <typename R, ByteOutput O>
void base64Decode(R input, O out) {
  size_t inputLength = input.size();
  size_t outputLength = inputLength / 4 * 3;

  if (inputLength < 2) {
    return;
  }
  if (input[inputLength - 1] == '=') {
    --outputLength;
  }
  if (input[inputLength - 2] == '=') {
    --outputLength;
  }

  for (size_t i = 0, j = 0; i < inputLength;) {
    uint32_t sextet1 = input[i] == '=' ? 0 & i++ : DECODING_TABLE[input[i++]];
    uint32_t sextet2 = input[i] == '=' ? 0 & i++ : DECODING_TABLE[input[i++]];
    uint32_t sextet3 = input[i] == '=' ? 0 & i++ : DECODING_TABLE[input[i++]];
    uint32_t sextet4 = input[i] == '=' ? 0 & i++ : DECODING_TABLE[input[i++]];

    uint32_t triple = (sextet1 << 3 * 6) + (sextet2 << 2 * 6) + (sextet3 << 1 * 6) + (sextet4 << 0 * 6);

    if (j < outputLength) {
      *out++ = static_cast<typename byte_output<O>::value_type>((triple >> 2 * 8) & 0xFF);
      ++j;
    }
    if (j < outputLength) {
      *out++ = static_cast<typename byte_output<O>::value_type>((triple >> 1 * 8) & 0xFF);
      ++j;
    }
    if (j < outputLength) {
      *out++ = static_cast<typename byte_output<O>::value_type>((triple >> 0 * 8) & 0xFF);
      ++j;
    }
  }
}