#pragma once

#include <bit>
#include <iostream>
#include <print>
#include <string>

#include "Types/Types.h"

class BitBoard {
public:
    constexpr BitBoard() : m_data(0ull) {}
    constexpr BitBoard(i32 data) = delete;
    constexpr BitBoard(u64 data) noexcept : m_data(data) {}

    constexpr BitBoard(Square sq) noexcept : m_data(1ull << sq) {}
    constexpr BitBoard(const BitBoard& bb) noexcept : m_data(bb.m_data) {}
    constexpr BitBoard(const BitBoard&& bb) noexcept : m_data(bb.m_data) {}
    constexpr ~BitBoard() = default;

    constexpr u64 raw() const noexcept { return m_data; }

    constexpr u8     Count() const noexcept { return (u8)std::popcount(m_data); }
    constexpr Square PopLSB() noexcept
    {
        Square sq = (Square)(std::countr_zero(m_data));
        m_data &= m_data - 1;
        return sq;
    }

    constexpr u64      operator*(const u64 other) const noexcept { return m_data * other; }
    constexpr BitBoard operator~() const noexcept { return ~(m_data); }
    constexpr BitBoard operator<<(int num) { return m_data << num; }
    constexpr BitBoard operator>>(int num) { return m_data >> num; }

    constexpr void     operator|=(BitBoard bb) noexcept { m_data |= bb.m_data; }
    constexpr void     operator|=(Square sq) noexcept { m_data |= BitBoard(sq).raw(); }
    constexpr BitBoard operator|(Square sq) const noexcept { return (m_data | (1ull << sq)); }
    constexpr BitBoard operator|(BitBoard bb) const noexcept
    {
        return BitBoard(m_data | bb.m_data);
    }

    constexpr void     operator&=(BitBoard bb) noexcept { m_data &= bb.m_data; }
    constexpr BitBoard operator&(Square sq) const noexcept { return (m_data & (1ull << sq)); }
    constexpr BitBoard operator&(BitBoard bb) const noexcept
    {
        return BitBoard(m_data & bb.m_data);
    }

    constexpr void operator&=(Square sq) noexcept { m_data &= BitBoard(sq); }

    constexpr BitBoard operator^(Square sq) noexcept { return (m_data ^ BitBoard(sq)); }

    constexpr void                 operator=(BitBoard bb) { m_data = bb.m_data; }
    constexpr bool                 operator==(BitBoard bb) const { return (m_data == bb.m_data); }
    friend constexpr std::ostream& operator<<(std::ostream& os, BitBoard bb);
    constexpr                      operator bool() { return (m_data); }

    std::string Str() const noexcept;

private:
    u64 m_data;
};

constexpr BitBoard RANK_1BB = BitBoard(0xffull << (0 * 8));
constexpr BitBoard RANK_2BB = BitBoard(0xffull << (1 * 8));
constexpr BitBoard RANK_3BB = BitBoard(0xffull << (2 * 8));
constexpr BitBoard RANK_4BB = BitBoard(0xffull << (3 * 8));
constexpr BitBoard RANK_5BB = BitBoard(0xffull << (4 * 8));
constexpr BitBoard RANK_6BB = BitBoard(0xffull << (5 * 8));
constexpr BitBoard RANK_7BB = BitBoard(0xffull << (6 * 8));
constexpr BitBoard RANK_8BB = BitBoard(0xffull << (7 * 8));

constexpr BitBoard FILE_1BB = BitBoard(0x01'01'01'01'01'01'01'01ull << 0);
constexpr BitBoard FILE_2BB = BitBoard(0x01'01'01'01'01'01'01'01ull << 1);
constexpr BitBoard FILE_3BB = BitBoard(0x01'01'01'01'01'01'01'01ull << 2);
constexpr BitBoard FILE_4BB = BitBoard(0x01'01'01'01'01'01'01'01ull << 3);
constexpr BitBoard FILE_5BB = BitBoard(0x01'01'01'01'01'01'01'01ull << 4);
constexpr BitBoard FILE_6BB = BitBoard(0x01'01'01'01'01'01'01'01ull << 5);
constexpr BitBoard FILE_7BB = BitBoard(0x01'01'01'01'01'01'01'01ull << 6);
constexpr BitBoard FILE_8BB = BitBoard(0x01'01'01'01'01'01'01'01ull << 7);

inline std::string BitBoard::Str() const noexcept
{
    constexpr char    on = 'X', off = ' ';
    const std::string VERT_SPACE = "\n ┼───┼───┼───┼───┼───┼───┼───┼───┼\n";
    const std::string HORZ_SPACE = " │ ";
    std::string       ret = "\n", line = HORZ_SPACE;

    for (Square i = SQ_A1; i < 64; i++) {
        if ((m_data & (1ull << i)) > 0) {
            line += on + HORZ_SPACE;
        } else {
            line += off + HORZ_SPACE;
        }

        if (i != 0 && i % 8 == 7) {
            ret  = line + VERT_SPACE + ret;
            line = HORZ_SPACE;
        }
    }

    ret = VERT_SPACE + ret;
    return ret;
}

inline constexpr std::ostream& operator<<(std::ostream& os, BitBoard bb)
{
    os << bb.m_data;
    return os;
}
