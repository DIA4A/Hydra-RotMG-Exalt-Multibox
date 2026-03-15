#pragma once
#include <array>
#include <algorithm>

template <std::size_t N>
struct StringLiteral
{
    char value[N];
    constexpr StringLiteral(const char(&str)[N])
    {
        std::copy_n(str, N, value);
    }
};

template <StringLiteral Str, int Shift>
consteval auto CTCipher()
{
    constexpr std::size_t N = sizeof(Str.value);
    std::array<char, N> result;

    int nNormalizedShift = (Shift % 26 + 26) % 26;
    for (std::size_t i = 0; i < N - 1; ++i)
    {
        char c = Str.value[i];
        if (c >= 'a' && c <= 'z')
        {
            result[i] = static_cast<char>('a' + (c - 'a' + nNormalizedShift) % 26);
        }
        else if (c >= 'A' && c <= 'Z')
        {
            result[i] = static_cast<char>('A' + (c - 'A' + nNormalizedShift) % 26);
        }
        else
        {
            result[i] = c;
        }
    }
    result[N - 1] = '\0';
    return result;
}

template <StringLiteral Str, int Shift>
constexpr auto CipherStorage = CTCipher<Str, Shift>();

#define CIPHER(str, shift) (CipherStorage<str, shift>.data())