#ifndef PROBFD_UTILS_FORMAT_H
#define PROBFD_UTILS_FORMAT_H

#include <version>

#ifdef __cpp_lib_format_ranges

#include <format>

#else

#include "probfd/concepts.h"

#include <concepts>
#include <format>
#include <memory>
#include <type_traits>

namespace probfd {

template <class _Rollback>
struct exception_guard_exceptions {
    exception_guard_exceptions() = delete;

    constexpr explicit exception_guard_exceptions(_Rollback rollback)
        : rollback_(std::move(rollback))
        , completed_(false)
    {
    }

    constexpr exception_guard_exceptions(
        exception_guard_exceptions&&
            other) noexcept(std::is_nothrow_move_constructible_v<_Rollback>)
        : rollback_(std::move(other.rollback_))
        , completed_(other.completed_)
    {
        other.completed_ = true;
    }

    exception_guard_exceptions(exception_guard_exceptions const&) = delete;
    exception_guard_exceptions&
    operator=(exception_guard_exceptions const&) = delete;
    exception_guard_exceptions&
    operator=(exception_guard_exceptions&&) = delete;

    constexpr void complete() noexcept { completed_ = true; }

    constexpr ~exception_guard_exceptions()
    {
        if (!completed_) rollback_();
    }

private:
    _Rollback rollback_;
    bool completed_;
};

#ifdef _LIBCPP_HAS_NO_EXCEPTIONS
template <class _Rollback>
using exception_guard = exception_guardnoexceptions<_Rollback>;
#else
template <class _Rollback>
using exception_guard = exception_guard_exceptions<_Rollback>;
#endif

template <class _Rollback>
constexpr exception_guard<_Rollback> make_exception_guard(_Rollback rollback)
{
    return exception_guard<_Rollback>(std::move(rollback));
}

/// The character type specializations of \ref formatter.
template <class _CharT>
concept fmt_char_type = std::same_as<_CharT, char>
#ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
                        || std::same_as<_CharT, wchar_t>
#endif
    ;

template <class _Alloc>
[[nodiscard]]
constexpr auto allocate_at_least(_Alloc& alloc, size_t n)
{
    return std::allocator_traits<_Alloc>::allocate_at_least(alloc, n);
}

template <fmt_char_type _CharT>
class retarget_buffer {
    using _Alloc = std::allocator<_CharT>;

public:
    using value_type = _CharT;

    struct iterator {
        using difference_type = ptrdiff_t;
        using value_type = _CharT;

        constexpr explicit iterator(retarget_buffer& buffer)
            : buffer_(std::addressof(buffer))
        {
        }
        constexpr iterator& operator=(const _CharT& c)
        {
            buffer_->push_back(c);
            return *this;
        }
        constexpr iterator& operator=(_CharT&& c)
        {
            buffer_->push_back(c);
            return *this;
        }

        constexpr iterator& operator*() { return *this; }
        constexpr iterator& operator++() { return *this; }
        constexpr iterator operator++(int) { return *this; }
        retarget_buffer* buffer_;
    };

    retarget_buffer(const retarget_buffer&) = delete;
    retarget_buffer& operator=(const retarget_buffer&) = delete;

    explicit retarget_buffer(size_t size_hint)
    {
        // When the initial size is very small a lot of resizes happen
        // when elements added. So use a hard-coded minimum size.
        //
        // Note a size < 2 will not work
        // - 0 there is no buffer, while push_back requires 1 empty element.
        // - 1 multiplied by the grow factor is 1 and thus the buffer never
        //   grows.
        auto result = allocate_at_least(
            alloc_,
            std::max(size_hint, 256 / sizeof(_CharT)));
        ptr_ = result.ptr;
        capacity_ = result.count;
    }

    ~retarget_buffer()
    {
        std::ranges::destroy_n(ptr_, size_);
        std::allocator_traits<_Alloc>::deallocate(alloc_, ptr_, capacity_);
    }

    iterator make_output_iterator() { return iterator{*this}; }

    void push_back(_CharT c)
    {
        std::construct_at(ptr_ + size_, c);
        ++size_;

        if (size_ == capacity_) grow_buffer();
    }

    template <fmt_char_type _InCharT>
    void copy(std::basic_string_view<_InCharT> str)
    {
        size_t n = str.size();
        if (size_ + n >= capacity_)
            // Push_back requires the buffer to have room for at least one
            // character.
            grow_buffer(size_ + n + 1);

        std::uninitialized_copy_n(str.data(), n, ptr_ + size_);
        size_ += n;
    }

    template <
        std::contiguous_iterator _Iterator,
        class _UnaryOperation,
        fmt_char_type _InCharT =
            typename std::iterator_traits<_Iterator>::value_type>
    void transform(_Iterator first, _Iterator last, _UnaryOperation operation)
    {
        size_t n = static_cast<size_t>(last - first);
        if (size_ + n >= capacity_)
            // Push_back requires the buffer to have room for at least one
            // character.
            grow_buffer(size_ + n + 1);

        std::uninitialized_default_construct_n(ptr_ + size_, n);
        std::transform(first, last, ptr_ + size_, std::move(operation));
        size_ += n;
    }

    void fill(size_t n, _CharT value)
    {
        if (size_ + n >= capacity_)
            // Push_back requires the buffer to have room for at least one
            // character.
            grow_buffer(size_ + n + 1);

        std::uninitialized_fill_n(ptr_ + size_, n, value);
        size_ += n;
    }

    std::basic_string_view<_CharT> view() { return {ptr_, size_}; }

private:
    void grow_buffer() { grow_buffer(capacity_ * 1.6); }

    void grow_buffer(size_t capacity)
    {
        auto result = allocate_at_least(alloc_, capacity);
        auto guard = make_exception_guard([&] {
            std::allocator_traits<_Alloc>::deallocate(
                alloc_,
                result.ptr,
                result.count);
        });
        // This shouldn't throw, but just to be safe. Note that at -O1 this
        // guard is optimized away so there is no runtime overhead.
        std::uninitialized_move_n(ptr_, size_, result.ptr);
        guard.complete();
        std::ranges::destroy_n(ptr_, size_);
        std::allocator_traits<_Alloc>::deallocate(alloc_, ptr_, capacity_);

        ptr_ = result.ptr;
        capacity_ = result.count;
    }

    [[no_unique_address]]
    _Alloc alloc_;
    _CharT* ptr_;
    size_t capacity_;
    size_t size_{0};
};

#ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
template <fmt_char_type _CharT>
inline constexpr const _CharT*
statically_widen(const char* str, const wchar_t* wstr)
{
    if constexpr (std::same_as<_CharT, char>)
        return str;
    else
        return wstr;
}
#define STATICALLY_WIDEN(_CharT, str) statically_widen<_CharT>(str, L##str)
#else // _LIBCPP_HAS_NO_WIDE_CHARACTERS

// Without this indirection the unit test test/libcxx/modules_include.sh.cpp
// fails for the CI build "No wide characters". This seems like a bug.
// TODO FMT investigate why this is needed.
template <fmt_char_type _CharT>
inline constexpr const _CharT* statically_widen(const char* str)
{
    return str;
}
#define STATICALLY_WIDEN(_CharT, str) statically_widen<_CharT>(str)
#endif // _LIBCPP_HAS_NO_WIDE_CHARACTERS

namespace width_estimation_table {

/// The entries of the characters with an estimated width of 2.
///
/// Contains the entries for [format.string.std]/12
///  -  Any code point with the East_Asian_Width="W" or East_Asian_Width="F"
///     Derived Extracted Property as described by UAX #44
/// - U+4DC0 - U+4DFF (Yijing Hexagram Symbols)
/// - U+1F300 - U+1F5FF (Miscellaneous Symbols and Pictographs)
/// - U+1F900 - U+1F9FF (Supplemental Symbols and Pictographs)
///
/// The data is generated from
/// - https://www.unicode.org/Public/UCD/latest/ucd/EastAsianWidth.txt
/// - The "overrides" in [format.string.std]/12
///
/// The format of EastAsianWidth.txt is two fields separated by a semicolon.
/// Field 0: Unicode code point value or range of code point values
/// Field 1: East_Asian_Width property, consisting of one of the following
/// values:
///         "A", "F", "H", "N", "Na", "W"
///  - All code points, assigned or unassigned, that are not listed
///      explicitly are given the value "N".
///  - The unassigned code points in the following blocks def to "W":
///         CJK Unified Ideographs Extension A: U+3400..U+4DBF
///         CJK Unified Ideographs:             U+4E00..U+9FFF
///         CJK Compatibility Ideographs:       U+F900..U+FAFF
///  - All undesignated code points in Planes 2 and 3, whether inside or
///      outside of allocated blocks, def to "W":
///         Plane 2:                            U+20000..U+2FFFD
///         Plane 3:                            U+30000..U+3FFFD
///
/// The table is similar to the table
///  extended_grapheme_custer_property_boundary::entries
/// which explains the details of these classes. The only difference is this
/// table lacks a property, thus having more bits available for the size.
///
/// The maximum code point that has an estimated width of 2 is U+3FFFD. This
/// value can be encoded in 18 bits. Thus the upper 3 bits of the code point
/// are always 0. These 3 bits are used to enlarge the offset range. This
/// optimization reduces the table in Unicode 15 from 184 to 104 entries,
/// saving 320 bytes.
///
/// The data has 2 values:
/// - bits [0, 13] The size of the range, allowing 16384 elements.
/// - bits [14, 31] The lower bound code point of the range. The upper bound of
///   the range is lower bound + size.
inline constexpr uint32_t entries[107] = {
    0x0440005f /* 00001100 - 0000115f [   96] */, //
    0x08c68001 /* 0000231a - 0000231b [    2] */, //
    0x08ca4001 /* 00002329 - 0000232a [    2] */, //
    0x08fa4003 /* 000023e9 - 000023ec [    4] */, //
    0x08fc0000 /* 000023f0 - 000023f0 [    1] */, //
    0x08fcc000 /* 000023f3 - 000023f3 [    1] */, //
    0x097f4001 /* 000025fd - 000025fe [    2] */, //
    0x09850001 /* 00002614 - 00002615 [    2] */, //
    0x0992000b /* 00002648 - 00002653 [   12] */, //
    0x099fc000 /* 0000267f - 0000267f [    1] */, //
    0x09a4c000 /* 00002693 - 00002693 [    1] */, //
    0x09a84000 /* 000026a1 - 000026a1 [    1] */, //
    0x09aa8001 /* 000026aa - 000026ab [    2] */, //
    0x09af4001 /* 000026bd - 000026be [    2] */, //
    0x09b10001 /* 000026c4 - 000026c5 [    2] */, //
    0x09b38000 /* 000026ce - 000026ce [    1] */, //
    0x09b50000 /* 000026d4 - 000026d4 [    1] */, //
    0x09ba8000 /* 000026ea - 000026ea [    1] */, //
    0x09bc8001 /* 000026f2 - 000026f3 [    2] */, //
    0x09bd4000 /* 000026f5 - 000026f5 [    1] */, //
    0x09be8000 /* 000026fa - 000026fa [    1] */, //
    0x09bf4000 /* 000026fd - 000026fd [    1] */, //
    0x09c14000 /* 00002705 - 00002705 [    1] */, //
    0x09c28001 /* 0000270a - 0000270b [    2] */, //
    0x09ca0000 /* 00002728 - 00002728 [    1] */, //
    0x09d30000 /* 0000274c - 0000274c [    1] */, //
    0x09d38000 /* 0000274e - 0000274e [    1] */, //
    0x09d4c002 /* 00002753 - 00002755 [    3] */, //
    0x09d5c000 /* 00002757 - 00002757 [    1] */, //
    0x09e54002 /* 00002795 - 00002797 [    3] */, //
    0x09ec0000 /* 000027b0 - 000027b0 [    1] */, //
    0x09efc000 /* 000027bf - 000027bf [    1] */, //
    0x0ac6c001 /* 00002b1b - 00002b1c [    2] */, //
    0x0ad40000 /* 00002b50 - 00002b50 [    1] */, //
    0x0ad54000 /* 00002b55 - 00002b55 [    1] */, //
    0x0ba00019 /* 00002e80 - 00002e99 [   26] */, //
    0x0ba6c058 /* 00002e9b - 00002ef3 [   89] */, //
    0x0bc000d5 /* 00002f00 - 00002fd5 [  214] */, //
    0x0bfc004e /* 00002ff0 - 0000303e [   79] */, //
    0x0c104055 /* 00003041 - 00003096 [   86] */, //
    0x0c264066 /* 00003099 - 000030ff [  103] */, //
    0x0c41402a /* 00003105 - 0000312f [   43] */, //
    0x0c4c405d /* 00003131 - 0000318e [   94] */, //
    0x0c640053 /* 00003190 - 000031e3 [   84] */, //
    0x0c7bc02f /* 000031ef - 0000321e [   48] */, //
    0x0c880027 /* 00003220 - 00003247 [   40] */, //
    0x0c943fff /* 00003250 - 0000724f [16384] */, //
    0x1c94323c /* 00007250 - 0000a48c [12861] */, //
    0x29240036 /* 0000a490 - 0000a4c6 [   55] */, //
    0x2a58001c /* 0000a960 - 0000a97c [   29] */, //
    0x2b002ba3 /* 0000ac00 - 0000d7a3 [11172] */, //
    0x3e4001ff /* 0000f900 - 0000faff [  512] */, //
    0x3f840009 /* 0000fe10 - 0000fe19 [   10] */, //
    0x3f8c0022 /* 0000fe30 - 0000fe52 [   35] */, //
    0x3f950012 /* 0000fe54 - 0000fe66 [   19] */, //
    0x3f9a0003 /* 0000fe68 - 0000fe6b [    4] */, //
    0x3fc0405f /* 0000ff01 - 0000ff60 [   96] */, //
    0x3ff80006 /* 0000ffe0 - 0000ffe6 [    7] */, //
    0x5bf80004 /* 00016fe0 - 00016fe4 [    5] */, //
    0x5bfc0001 /* 00016ff0 - 00016ff1 [    2] */, //
    0x5c0017f7 /* 00017000 - 000187f7 [ 6136] */, //
    0x620004d5 /* 00018800 - 00018cd5 [ 1238] */, //
    0x63400008 /* 00018d00 - 00018d08 [    9] */, //
    0x6bfc0003 /* 0001aff0 - 0001aff3 [    4] */, //
    0x6bfd4006 /* 0001aff5 - 0001affb [    7] */, //
    0x6bff4001 /* 0001affd - 0001affe [    2] */, //
    0x6c000122 /* 0001b000 - 0001b122 [  291] */, //
    0x6c4c8000 /* 0001b132 - 0001b132 [    1] */, //
    0x6c540002 /* 0001b150 - 0001b152 [    3] */, //
    0x6c554000 /* 0001b155 - 0001b155 [    1] */, //
    0x6c590003 /* 0001b164 - 0001b167 [    4] */, //
    0x6c5c018b /* 0001b170 - 0001b2fb [  396] */, //
    0x7c010000 /* 0001f004 - 0001f004 [    1] */, //
    0x7c33c000 /* 0001f0cf - 0001f0cf [    1] */, //
    0x7c638000 /* 0001f18e - 0001f18e [    1] */, //
    0x7c644009 /* 0001f191 - 0001f19a [   10] */, //
    0x7c800002 /* 0001f200 - 0001f202 [    3] */, //
    0x7c84002b /* 0001f210 - 0001f23b [   44] */, //
    0x7c900008 /* 0001f240 - 0001f248 [    9] */, //
    0x7c940001 /* 0001f250 - 0001f251 [    2] */, //
    0x7c980005 /* 0001f260 - 0001f265 [    6] */, //
    0x7cc0034f /* 0001f300 - 0001f64f [  848] */, //
    0x7da00045 /* 0001f680 - 0001f6c5 [   70] */, //
    0x7db30000 /* 0001f6cc - 0001f6cc [    1] */, //
    0x7db40002 /* 0001f6d0 - 0001f6d2 [    3] */, //
    0x7db54002 /* 0001f6d5 - 0001f6d7 [    3] */, //
    0x7db70003 /* 0001f6dc - 0001f6df [    4] */, //
    0x7dbac001 /* 0001f6eb - 0001f6ec [    2] */, //
    0x7dbd0008 /* 0001f6f4 - 0001f6fc [    9] */, //
    0x7df8000b /* 0001f7e0 - 0001f7eb [   12] */, //
    0x7dfc0000 /* 0001f7f0 - 0001f7f0 [    1] */, //
    0x7e4000ff /* 0001f900 - 0001f9ff [  256] */, //
    0x7e9c000c /* 0001fa70 - 0001fa7c [   13] */, //
    0x7ea00008 /* 0001fa80 - 0001fa88 [    9] */, //
    0x7ea4002d /* 0001fa90 - 0001fabd [   46] */, //
    0x7eafc006 /* 0001fabf - 0001fac5 [    7] */, //
    0x7eb3800d /* 0001face - 0001fadb [   14] */, //
    0x7eb80008 /* 0001fae0 - 0001fae8 [    9] */, //
    0x7ebc0008 /* 0001faf0 - 0001faf8 [    9] */, //
    0x80003fff /* 00020000 - 00023fff [16384] */, //
    0x90003fff /* 00024000 - 00027fff [16384] */, //
    0xa0003fff /* 00028000 - 0002bfff [16384] */, //
    0xb0003ffd /* 0002c000 - 0002fffd [16382] */, //
    0xc0003fff /* 00030000 - 00033fff [16384] */, //
    0xd0003fff /* 00034000 - 00037fff [16384] */, //
    0xe0003fff /* 00038000 - 0003bfff [16384] */, //
    0xf0003ffd /* 0003c000 - 0003fffd [16382] */};

/// The upper bound entry of EastAsianWidth.txt.
///
/// Values greater than this value may have more than 18 significant bits.
/// They always have a width of 1. This property makes it possible to store
/// the table in its compact form.
inline constexpr uint32_t table_upper_bound = 0x0003fffd;

/// Returns the estimated width of a Unicode code point.
///
/// \\pre The code point is a valid Unicode code point.
[[nodiscard]]
constexpr int estimated_width(const char32_t code_point) noexcept
{
    // Since table_upper_bound contains the unshifted range do the
    // comparison without shifting.
    if (code_point > table_upper_bound) [[unlikely]]
        return 1;

    // When the code-point is less than the first element in the table
    // the lookup is quite expensive. Since quite some scripts are in
    // that range, it makes sense to validate that first.
    // The std_format_spec_string_unicode benchmark gives a measurable
    // improvement.
    if (code_point < (entries[0] >> 14)) return 1;

    ptrdiff_t i =
        std::ranges::upper_bound(entries, (code_point << 14) | 0x3fffu) -
        entries;
    if (i == 0) return 1;

    --i;
    uint32_t upper_bound = (entries[i] >> 14) + (entries[i] & 0x3fffu);
    return 1 + (code_point <= upper_bound);
}

} // namespace width_estimation_table

namespace extended_grapheme_custer_property_boundary {

enum class property : uint8_t {
    // Values generated from the data files.
    CR,
    Control,
    Extend,
    Extended_Pictographic,
    L,
    LF,
    LV,
    LVT,
    Prepend,
    Regional_Indicator,
    SpacingMark,
    T,
    V,
    ZWJ,

    // The properies below aren't stored in the "database".

    // Text position properties.
    sot,
    eot,

    // The code unit has none of above properties.
    none
};

/// The entries of the extended grapheme cluster bondary property table.
///
/// The data is generated from
/// -
/// https://www.unicode.org/Public/UCD/latest/ucd/auxiliary/GraphemeBreakProperty.txt
/// - https://www.unicode.org/Public/UCD/latest/ucd/emoji/emoji-data.txt
///
/// The data has 3 values
/// - bits [0, 3] The property. One of the values generated from the datafiles
///   of \ref property
/// - bits [4, 10] The size of the range.
/// - bits [11, 31] The lower bound code point of the range. The upper bound of
///   the range is lower bound + size.
///
/// The 7 bits for the size allow a maximum range of 128 elements. Some ranges
/// in the Unicode tables are larger. They are stored in multiple consecutive
/// ranges in the data table. An alternative would be to store the sizes in a
/// separate 16-bit value. The original MSVC STL code had such an approach, but
/// this approach uses less space for the data and is about 4% faster in the
/// following benchmark.
/// libcxx/benchmarks/std_format_spec_string_unicode.bench.cpp
// clang-format off
 inline constexpr uint32_t entries[1496] = {
    0x00000091,
    0x00005005,
    0x00005811,
    0x00006800,
    0x00007111,
    0x0003fa01,
    0x00054803,
    0x00056801,
    0x00057003,
    0x001806f2,
    0x00241862,
    0x002c8ac2,
    0x002df802,
    0x002e0812,
    0x002e2012,
    0x002e3802,
    0x00300058,
    0x003080a2,
    0x0030e001,
    0x00325942,
    0x00338002,
    0x0036b062,
    0x0036e808,
    0x0036f852,
    0x00373812,
    0x00375032,
    0x00387808,
    0x00388802,
    0x003981a2,
    0x003d30a2,
    0x003f5882,
    0x003fe802,
    0x0040b032,
    0x0040d882,
    0x00412822,
    0x00414842,
    0x0042c822,
    0x00448018,
    0x0044c072,
    0x00465172,
    0x00471008,
    0x004719f2,
    0x0048180a,
    0x0049d002,
    0x0049d80a,
    0x0049e002,
    0x0049f02a,
    0x004a0872,
    0x004a483a,
    0x004a6802,
    0x004a701a,
    0x004a8862,
    0x004b1012,
    0x004c0802,
    0x004c101a,
    0x004de002,
    0x004df002,
    0x004df81a,
    0x004e0832,
    0x004e381a,
    0x004e581a,
    0x004e6802,
    0x004eb802,
    0x004f1012,
    0x004ff002,
    0x00500812,
    0x0050180a,
    0x0051e002,
    0x0051f02a,
    0x00520812,
    0x00523812,
    0x00525822,
    0x00528802,
    0x00538012,
    0x0053a802,
    0x00540812,
    0x0054180a,
    0x0055e002,
    0x0055f02a,
    0x00560842,
    0x00563812,
    0x0056480a,
    0x0056581a,
    0x00566802,
    0x00571012,
    0x0057d052,
    0x00580802,
    0x0058101a,
    0x0059e002,
    0x0059f012,
    0x005a000a,
    0x005a0832,
    0x005a381a,
    0x005a581a,
    0x005a6802,
    0x005aa822,
    0x005b1012,
    0x005c1002,
    0x005df002,
    0x005df80a,
    0x005e0002,
    0x005e081a,
    0x005e302a,
    0x005e502a,
    0x005e6802,
    0x005eb802,
    0x00600002,
    0x0060082a,
    0x00602002,
    0x0061e002,
    0x0061f022,
    0x0062083a,
    0x00623022,
    0x00625032,
    0x0062a812,
    0x00631012,
    0x00640802,
    0x0064101a,
    0x0065e002,
    0x0065f00a,
    0x0065f802,
    0x0066001a,
    0x00661002,
    0x0066181a,
    0x00663002,
    0x0066381a,
    0x0066501a,
    0x00666012,
    0x0066a812,
    0x00671012,
    0x0067980a,
    0x00680012,
    0x0068101a,
    0x0069d812,
    0x0069f002,
    0x0069f81a,
    0x006a0832,
    0x006a302a,
    0x006a502a,
    0x006a6802,
    0x006a7008,
    0x006ab802,
    0x006b1012,
    0x006c0802,
    0x006c101a,
    0x006e5002,
    0x006e7802,
    0x006e801a,
    0x006e9022,
    0x006eb002,
    0x006ec06a,
    0x006ef802,
    0x006f901a,
    0x00718802,
    0x0071980a,
    0x0071a062,
    0x00723872,
    0x00758802,
    0x0075980a,
    0x0075a082,
    0x00764062,
    0x0078c012,
    0x0079a802,
    0x0079b802,
    0x0079c802,
    0x0079f01a,
    0x007b88d2,
    0x007bf80a,
    0x007c0042,
    0x007c3012,
    0x007c68a2,
    0x007cca32,
    0x007e3002,
    0x00816832,
    0x0081880a,
    0x00819052,
    0x0081c812,
    0x0081d81a,
    0x0081e812,
    0x0082b01a,
    0x0082c012,
    0x0082f022,
    0x00838832,
    0x00841002,
    0x0084200a,
    0x00842812,
    0x00846802,
    0x0084e802,
    0x008805f4,
    0x008b047c,
    0x008d457b,
    0x009ae822,
    0x00b89022,
    0x00b8a80a,
    0x00b99012,
    0x00b9a00a,
    0x00ba9012,
    0x00bb9012,
    0x00bda012,
    0x00bdb00a,
    0x00bdb862,
    0x00bdf07a,
    0x00be3002,
    0x00be381a,
    0x00be48a2,
    0x00bee802,
    0x00c05822,
    0x00c07001,
    0x00c07802,
    0x00c42812,
    0x00c54802,
    0x00c90022,
    0x00c9183a,
    0x00c93812,
    0x00c9482a,
    0x00c9801a,
    0x00c99002,
    0x00c9985a,
    0x00c9c822,
    0x00d0b812,
    0x00d0c81a,
    0x00d0d802,
    0x00d2a80a,
    0x00d2b002,
    0x00d2b80a,
    0x00d2c062,
    0x00d30002,
    0x00d31002,
    0x00d32872,
    0x00d3685a,
    0x00d39892,
    0x00d3f802,
    0x00d581e2,
    0x00d80032,
    0x00d8200a,
    0x00d9a062,
    0x00d9d80a,
    0x00d9e002,
    0x00d9e84a,
    0x00da1002,
    0x00da181a,
    0x00db5882,
    0x00dc0012,
    0x00dc100a,
    0x00dd080a,
    0x00dd1032,
    0x00dd301a,
    0x00dd4012,
    0x00dd500a,
    0x00dd5822,
    0x00df3002,
    0x00df380a,
    0x00df4012,
    0x00df502a,
    0x00df6802,
    0x00df700a,
    0x00df7822,
    0x00df901a,
    0x00e1207a,
    0x00e16072,
    0x00e1a01a,
    0x00e1b012,
    0x00e68022,
    0x00e6a0c2,
    0x00e7080a,
    0x00e71062,
    0x00e76802,
    0x00e7a002,
    0x00e7b80a,
    0x00e7c012,
    0x00ee03f2,
    0x01005801,
    0x01006002,
    0x0100680d,
    0x01007011,
    0x01014061,
    0x0101e003,
    0x01024803,
    0x010300f1,
    0x01068202,
    0x01091003,
    0x0109c803,
    0x010ca053,
    0x010d4813,
    0x0118d013,
    0x01194003,
    0x011c4003,
    0x011e7803,
    0x011f48a3,
    0x011fc023,
    0x01261003,
    0x012d5013,
    0x012db003,
    0x012e0003,
    0x012fd833,
    0x01300053,
    0x013038b3,
    0x0130a713,
    0x01348753,
    0x013840a3,
    0x0138a003,
    0x0138b003,
    0x0138e803,
    0x01390803,
    0x01394003,
    0x01399813,
    0x013a2003,
    0x013a3803,
    0x013a6003,
    0x013a7003,
    0x013a9823,
    0x013ab803,
    0x013b1843,
    0x013ca823,
    0x013d0803,
    0x013d8003,
    0x013df803,
    0x0149a013,
    0x01582823,
    0x0158d813,
    0x015a8003,
    0x015aa803,
    0x01677822,
    0x016bf802,
    0x016f01f2,
    0x01815052,
    0x01818003,
    0x0181e803,
    0x0184c812,
    0x0194b803,
    0x0194c803,
    0x05337832,
    0x0533a092,
    0x0534f012,
    0x05378012,
    0x05401002,
    0x05403002,
    0x05405802,
    0x0541181a,
    0x05412812,
    0x0541380a,
    0x05416002,
    0x0544001a,
    0x0545a0fa,
    0x05462012,
    0x05470112,
    0x0547f802,
    0x05493072,
    0x054a38a2,
    0x054a901a,
    0x054b01c4,
    0x054c0022,
    0x054c180a,
    0x054d9802,
    0x054da01a,
    0x054db032,
    0x054dd01a,
    0x054de012,
    0x054df02a,
    0x054f2802,
    0x05514852,
    0x0551781a,
    0x05518812,
    0x0551981a,
    0x0551a812,
    0x05521802,
    0x05526002,
    0x0552680a,
    0x0553e002,
    0x05558002,
    0x05559022,
    0x0555b812,
    0x0555f012,
    0x05560802,
    0x0557580a,
    0x05576012,
    0x0557701a,
    0x0557a80a,
    0x0557b002,
    0x055f181a,
    0x055f2802,
    0x055f301a,
    0x055f4002,
    0x055f481a,
    0x055f600a,
    0x055f6802,
    0x05600006,
    0x056009a7,
    0x0560e006,
    0x0560e9a7,
    0x0561c006,
    0x0561c9a7,
    0x0562a006,
    0x0562a9a7,
    0x05638006,
    0x056389a7,
    0x05646006,
    0x056469a7,
    0x05654006,
    0x056549a7,
    0x05662006,
    0x056629a7,
    0x05670006,
    0x056709a7,
    0x0567e006,
    0x0567e9a7,
    0x0568c006,
    0x0568c9a7,
    0x0569a006,
    0x0569a9a7,
    0x056a8006,
    0x056a89a7,
    0x056b6006,
    0x056b69a7,
    0x056c4006,
    0x056c49a7,
    0x056d2006,
    0x056d29a7,
    0x056e0006,
    0x056e09a7,
    0x056ee006,
    0x056ee9a7,
    0x056fc006,
    0x056fc9a7,
    0x0570a006,
    0x0570a9a7,
    0x05718006,
    0x057189a7,
    0x05726006,
    0x057269a7,
    0x05734006,
    0x057349a7,
    0x05742006,
    0x057429a7,
    0x05750006,
    0x057509a7,
    0x0575e006,
    0x0575e9a7,
    0x0576c006,
    0x0576c9a7,
    0x0577a006,
    0x0577a9a7,
    0x05788006,
    0x057889a7,
    0x05796006,
    0x057969a7,
    0x057a4006,
    0x057a49a7,
    0x057b2006,
    0x057b29a7,
    0x057c0006,
    0x057c09a7,
    0x057ce006,
    0x057ce9a7,
    0x057dc006,
    0x057dc9a7,
    0x057ea006,
    0x057ea9a7,
    0x057f8006,
    0x057f89a7,
    0x05806006,
    0x058069a7,
    0x05814006,
    0x058149a7,
    0x05822006,
    0x058229a7,
    0x05830006,
    0x058309a7,
    0x0583e006,
    0x0583e9a7,
    0x0584c006,
    0x0584c9a7,
    0x0585a006,
    0x0585a9a7,
    0x05868006,
    0x058689a7,
    0x05876006,
    0x058769a7,
    0x05884006,
    0x058849a7,
    0x05892006,
    0x058929a7,
    0x058a0006,
    0x058a09a7,
    0x058ae006,
    0x058ae9a7,
    0x058bc006,
    0x058bc9a7,
    0x058ca006,
    0x058ca9a7,
    0x058d8006,
    0x058d89a7,
    0x058e6006,
    0x058e69a7,
    0x058f4006,
    0x058f49a7,
    0x05902006,
    0x059029a7,
    0x05910006,
    0x059109a7,
    0x0591e006,
    0x0591e9a7,
    0x0592c006,
    0x0592c9a7,
    0x0593a006,
    0x0593a9a7,
    0x05948006,
    0x059489a7,
    0x05956006,
    0x059569a7,
    0x05964006,
    0x059649a7,
    0x05972006,
    0x059729a7,
    0x05980006,
    0x059809a7,
    0x0598e006,
    0x0598e9a7,
    0x0599c006,
    0x0599c9a7,
    0x059aa006,
    0x059aa9a7,
    0x059b8006,
    0x059b89a7,
    0x059c6006,
    0x059c69a7,
    0x059d4006,
    0x059d49a7,
    0x059e2006,
    0x059e29a7,
    0x059f0006,
    0x059f09a7,
    0x059fe006,
    0x059fe9a7,
    0x05a0c006,
    0x05a0c9a7,
    0x05a1a006,
    0x05a1a9a7,
    0x05a28006,
    0x05a289a7,
    0x05a36006,
    0x05a369a7,
    0x05a44006,
    0x05a449a7,
    0x05a52006,
    0x05a529a7,
    0x05a60006,
    0x05a609a7,
    0x05a6e006,
    0x05a6e9a7,
    0x05a7c006,
    0x05a7c9a7,
    0x05a8a006,
    0x05a8a9a7,
    0x05a98006,
    0x05a989a7,
    0x05aa6006,
    0x05aa69a7,
    0x05ab4006,
    0x05ab49a7,
    0x05ac2006,
    0x05ac29a7,
    0x05ad0006,
    0x05ad09a7,
    0x05ade006,
    0x05ade9a7,
    0x05aec006,
    0x05aec9a7,
    0x05afa006,
    0x05afa9a7,
    0x05b08006,
    0x05b089a7,
    0x05b16006,
    0x05b169a7,
    0x05b24006,
    0x05b249a7,
    0x05b32006,
    0x05b329a7,
    0x05b40006,
    0x05b409a7,
    0x05b4e006,
    0x05b4e9a7,
    0x05b5c006,
    0x05b5c9a7,
    0x05b6a006,
    0x05b6a9a7,
    0x05b78006,
    0x05b789a7,
    0x05b86006,
    0x05b869a7,
    0x05b94006,
    0x05b949a7,
    0x05ba2006,
    0x05ba29a7,
    0x05bb0006,
    0x05bb09a7,
    0x05bbe006,
    0x05bbe9a7,
    0x05bcc006,
    0x05bcc9a7,
    0x05bda006,
    0x05bda9a7,
    0x05be8006,
    0x05be89a7,
    0x05bf6006,
    0x05bf69a7,
    0x05c04006,
    0x05c049a7,
    0x05c12006,
    0x05c129a7,
    0x05c20006,
    0x05c209a7,
    0x05c2e006,
    0x05c2e9a7,
    0x05c3c006,
    0x05c3c9a7,
    0x05c4a006,
    0x05c4a9a7,
    0x05c58006,
    0x05c589a7,
    0x05c66006,
    0x05c669a7,
    0x05c74006,
    0x05c749a7,
    0x05c82006,
    0x05c829a7,
    0x05c90006,
    0x05c909a7,
    0x05c9e006,
    0x05c9e9a7,
    0x05cac006,
    0x05cac9a7,
    0x05cba006,
    0x05cba9a7,
    0x05cc8006,
    0x05cc89a7,
    0x05cd6006,
    0x05cd69a7,
    0x05ce4006,
    0x05ce49a7,
    0x05cf2006,
    0x05cf29a7,
    0x05d00006,
    0x05d009a7,
    0x05d0e006,
    0x05d0e9a7,
    0x05d1c006,
    0x05d1c9a7,
    0x05d2a006,
    0x05d2a9a7,
    0x05d38006,
    0x05d389a7,
    0x05d46006,
    0x05d469a7,
    0x05d54006,
    0x05d549a7,
    0x05d62006,
    0x05d629a7,
    0x05d70006,
    0x05d709a7,
    0x05d7e006,
    0x05d7e9a7,
    0x05d8c006,
    0x05d8c9a7,
    0x05d9a006,
    0x05d9a9a7,
    0x05da8006,
    0x05da89a7,
    0x05db6006,
    0x05db69a7,
    0x05dc4006,
    0x05dc49a7,
    0x05dd2006,
    0x05dd29a7,
    0x05de0006,
    0x05de09a7,
    0x05dee006,
    0x05dee9a7,
    0x05dfc006,
    0x05dfc9a7,
    0x05e0a006,
    0x05e0a9a7,
    0x05e18006,
    0x05e189a7,
    0x05e26006,
    0x05e269a7,
    0x05e34006,
    0x05e349a7,
    0x05e42006,
    0x05e429a7,
    0x05e50006,
    0x05e509a7,
    0x05e5e006,
    0x05e5e9a7,
    0x05e6c006,
    0x05e6c9a7,
    0x05e7a006,
    0x05e7a9a7,
    0x05e88006,
    0x05e889a7,
    0x05e96006,
    0x05e969a7,
    0x05ea4006,
    0x05ea49a7,
    0x05eb2006,
    0x05eb29a7,
    0x05ec0006,
    0x05ec09a7,
    0x05ece006,
    0x05ece9a7,
    0x05edc006,
    0x05edc9a7,
    0x05eea006,
    0x05eea9a7,
    0x05ef8006,
    0x05ef89a7,
    0x05f06006,
    0x05f069a7,
    0x05f14006,
    0x05f149a7,
    0x05f22006,
    0x05f229a7,
    0x05f30006,
    0x05f309a7,
    0x05f3e006,
    0x05f3e9a7,
    0x05f4c006,
    0x05f4c9a7,
    0x05f5a006,
    0x05f5a9a7,
    0x05f68006,
    0x05f689a7,
    0x05f76006,
    0x05f769a7,
    0x05f84006,
    0x05f849a7,
    0x05f92006,
    0x05f929a7,
    0x05fa0006,
    0x05fa09a7,
    0x05fae006,
    0x05fae9a7,
    0x05fbc006,
    0x05fbc9a7,
    0x05fca006,
    0x05fca9a7,
    0x05fd8006,
    0x05fd89a7,
    0x05fe6006,
    0x05fe69a7,
    0x05ff4006,
    0x05ff49a7,
    0x06002006,
    0x060029a7,
    0x06010006,
    0x060109a7,
    0x0601e006,
    0x0601e9a7,
    0x0602c006,
    0x0602c9a7,
    0x0603a006,
    0x0603a9a7,
    0x06048006,
    0x060489a7,
    0x06056006,
    0x060569a7,
    0x06064006,
    0x060649a7,
    0x06072006,
    0x060729a7,
    0x06080006,
    0x060809a7,
    0x0608e006,
    0x0608e9a7,
    0x0609c006,
    0x0609c9a7,
    0x060aa006,
    0x060aa9a7,
    0x060b8006,
    0x060b89a7,
    0x060c6006,
    0x060c69a7,
    0x060d4006,
    0x060d49a7,
    0x060e2006,
    0x060e29a7,
    0x060f0006,
    0x060f09a7,
    0x060fe006,
    0x060fe9a7,
    0x0610c006,
    0x0610c9a7,
    0x0611a006,
    0x0611a9a7,
    0x06128006,
    0x061289a7,
    0x06136006,
    0x061369a7,
    0x06144006,
    0x061449a7,
    0x06152006,
    0x061529a7,
    0x06160006,
    0x061609a7,
    0x0616e006,
    0x0616e9a7,
    0x0617c006,
    0x0617c9a7,
    0x0618a006,
    0x0618a9a7,
    0x06198006,
    0x061989a7,
    0x061a6006,
    0x061a69a7,
    0x061b4006,
    0x061b49a7,
    0x061c2006,
    0x061c29a7,
    0x061d0006,
    0x061d09a7,
    0x061de006,
    0x061de9a7,
    0x061ec006,
    0x061ec9a7,
    0x061fa006,
    0x061fa9a7,
    0x06208006,
    0x062089a7,
    0x06216006,
    0x062169a7,
    0x06224006,
    0x062249a7,
    0x06232006,
    0x062329a7,
    0x06240006,
    0x062409a7,
    0x0624e006,
    0x0624e9a7,
    0x0625c006,
    0x0625c9a7,
    0x0626a006,
    0x0626a9a7,
    0x06278006,
    0x062789a7,
    0x06286006,
    0x062869a7,
    0x06294006,
    0x062949a7,
    0x062a2006,
    0x062a29a7,
    0x062b0006,
    0x062b09a7,
    0x062be006,
    0x062be9a7,
    0x062cc006,
    0x062cc9a7,
    0x062da006,
    0x062da9a7,
    0x062e8006,
    0x062e89a7,
    0x062f6006,
    0x062f69a7,
    0x06304006,
    0x063049a7,
    0x06312006,
    0x063129a7,
    0x06320006,
    0x063209a7,
    0x0632e006,
    0x0632e9a7,
    0x0633c006,
    0x0633c9a7,
    0x0634a006,
    0x0634a9a7,
    0x06358006,
    0x063589a7,
    0x06366006,
    0x063669a7,
    0x06374006,
    0x063749a7,
    0x06382006,
    0x063829a7,
    0x06390006,
    0x063909a7,
    0x0639e006,
    0x0639e9a7,
    0x063ac006,
    0x063ac9a7,
    0x063ba006,
    0x063ba9a7,
    0x063c8006,
    0x063c89a7,
    0x063d6006,
    0x063d69a7,
    0x063e4006,
    0x063e49a7,
    0x063f2006,
    0x063f29a7,
    0x06400006,
    0x064009a7,
    0x0640e006,
    0x0640e9a7,
    0x0641c006,
    0x0641c9a7,
    0x0642a006,
    0x0642a9a7,
    0x06438006,
    0x064389a7,
    0x06446006,
    0x064469a7,
    0x06454006,
    0x064549a7,
    0x06462006,
    0x064629a7,
    0x06470006,
    0x064709a7,
    0x0647e006,
    0x0647e9a7,
    0x0648c006,
    0x0648c9a7,
    0x0649a006,
    0x0649a9a7,
    0x064a8006,
    0x064a89a7,
    0x064b6006,
    0x064b69a7,
    0x064c4006,
    0x064c49a7,
    0x064d2006,
    0x064d29a7,
    0x064e0006,
    0x064e09a7,
    0x064ee006,
    0x064ee9a7,
    0x064fc006,
    0x064fc9a7,
    0x0650a006,
    0x0650a9a7,
    0x06518006,
    0x065189a7,
    0x06526006,
    0x065269a7,
    0x06534006,
    0x065349a7,
    0x06542006,
    0x065429a7,
    0x06550006,
    0x065509a7,
    0x0655e006,
    0x0655e9a7,
    0x0656c006,
    0x0656c9a7,
    0x0657a006,
    0x0657a9a7,
    0x06588006,
    0x065889a7,
    0x06596006,
    0x065969a7,
    0x065a4006,
    0x065a49a7,
    0x065b2006,
    0x065b29a7,
    0x065c0006,
    0x065c09a7,
    0x065ce006,
    0x065ce9a7,
    0x065dc006,
    0x065dc9a7,
    0x065ea006,
    0x065ea9a7,
    0x065f8006,
    0x065f89a7,
    0x06606006,
    0x066069a7,
    0x06614006,
    0x066149a7,
    0x06622006,
    0x066229a7,
    0x06630006,
    0x066309a7,
    0x0663e006,
    0x0663e9a7,
    0x0664c006,
    0x0664c9a7,
    0x0665a006,
    0x0665a9a7,
    0x06668006,
    0x066689a7,
    0x06676006,
    0x066769a7,
    0x06684006,
    0x066849a7,
    0x06692006,
    0x066929a7,
    0x066a0006,
    0x066a09a7,
    0x066ae006,
    0x066ae9a7,
    0x066bc006,
    0x066bc9a7,
    0x066ca006,
    0x066ca9a7,
    0x066d8006,
    0x066d89a7,
    0x066e6006,
    0x066e69a7,
    0x066f4006,
    0x066f49a7,
    0x06702006,
    0x067029a7,
    0x06710006,
    0x067109a7,
    0x0671e006,
    0x0671e9a7,
    0x0672c006,
    0x0672c9a7,
    0x0673a006,
    0x0673a9a7,
    0x06748006,
    0x067489a7,
    0x06756006,
    0x067569a7,
    0x06764006,
    0x067649a7,
    0x06772006,
    0x067729a7,
    0x06780006,
    0x067809a7,
    0x0678e006,
    0x0678e9a7,
    0x0679c006,
    0x0679c9a7,
    0x067aa006,
    0x067aa9a7,
    0x067b8006,
    0x067b89a7,
    0x067c6006,
    0x067c69a7,
    0x067d4006,
    0x067d49a7,
    0x067e2006,
    0x067e29a7,
    0x067f0006,
    0x067f09a7,
    0x067fe006,
    0x067fe9a7,
    0x0680c006,
    0x0680c9a7,
    0x0681a006,
    0x0681a9a7,
    0x06828006,
    0x068289a7,
    0x06836006,
    0x068369a7,
    0x06844006,
    0x068449a7,
    0x06852006,
    0x068529a7,
    0x06860006,
    0x068609a7,
    0x0686e006,
    0x0686e9a7,
    0x0687c006,
    0x0687c9a7,
    0x0688a006,
    0x0688a9a7,
    0x06898006,
    0x068989a7,
    0x068a6006,
    0x068a69a7,
    0x068b4006,
    0x068b49a7,
    0x068c2006,
    0x068c29a7,
    0x068d0006,
    0x068d09a7,
    0x068de006,
    0x068de9a7,
    0x068ec006,
    0x068ec9a7,
    0x068fa006,
    0x068fa9a7,
    0x06908006,
    0x069089a7,
    0x06916006,
    0x069169a7,
    0x06924006,
    0x069249a7,
    0x06932006,
    0x069329a7,
    0x06940006,
    0x069409a7,
    0x0694e006,
    0x0694e9a7,
    0x0695c006,
    0x0695c9a7,
    0x0696a006,
    0x0696a9a7,
    0x06978006,
    0x069789a7,
    0x06986006,
    0x069869a7,
    0x06994006,
    0x069949a7,
    0x069a2006,
    0x069a29a7,
    0x069b0006,
    0x069b09a7,
    0x069be006,
    0x069be9a7,
    0x069cc006,
    0x069cc9a7,
    0x069da006,
    0x069da9a7,
    0x069e8006,
    0x069e89a7,
    0x069f6006,
    0x069f69a7,
    0x06a04006,
    0x06a049a7,
    0x06a12006,
    0x06a129a7,
    0x06a20006,
    0x06a209a7,
    0x06a2e006,
    0x06a2e9a7,
    0x06a3c006,
    0x06a3c9a7,
    0x06a4a006,
    0x06a4a9a7,
    0x06a58006,
    0x06a589a7,
    0x06a66006,
    0x06a669a7,
    0x06a74006,
    0x06a749a7,
    0x06a82006,
    0x06a829a7,
    0x06a90006,
    0x06a909a7,
    0x06a9e006,
    0x06a9e9a7,
    0x06aac006,
    0x06aac9a7,
    0x06aba006,
    0x06aba9a7,
    0x06ac8006,
    0x06ac89a7,
    0x06ad6006,
    0x06ad69a7,
    0x06ae4006,
    0x06ae49a7,
    0x06af2006,
    0x06af29a7,
    0x06b00006,
    0x06b009a7,
    0x06b0e006,
    0x06b0e9a7,
    0x06b1c006,
    0x06b1c9a7,
    0x06b2a006,
    0x06b2a9a7,
    0x06b38006,
    0x06b389a7,
    0x06b46006,
    0x06b469a7,
    0x06b54006,
    0x06b549a7,
    0x06b62006,
    0x06b629a7,
    0x06b70006,
    0x06b709a7,
    0x06b7e006,
    0x06b7e9a7,
    0x06b8c006,
    0x06b8c9a7,
    0x06b9a006,
    0x06b9a9a7,
    0x06ba8006,
    0x06ba89a7,
    0x06bb6006,
    0x06bb69a7,
    0x06bc4006,
    0x06bc49a7,
    0x06bd816c,
    0x06be5b0b,
    0x07d8f002,
    0x07f000f2,
    0x07f100f2,
    0x07f7f801,
    0x07fcf012,
    0x07ff80b1,
    0x080fe802,
    0x08170002,
    0x081bb042,
    0x08500822,
    0x08502812,
    0x08506032,
    0x0851c022,
    0x0851f802,
    0x08572812,
    0x08692032,
    0x08755812,
    0x0877e822,
    0x087a30a2,
    0x087c1032,
    0x0880000a,
    0x08800802,
    0x0880100a,
    0x0881c0e2,
    0x08838002,
    0x08839812,
    0x0883f822,
    0x0884100a,
    0x0885802a,
    0x08859832,
    0x0885b81a,
    0x0885c812,
    0x0885e808,
    0x08861002,
    0x08866808,
    0x08880022,
    0x08893842,
    0x0889600a,
    0x08896872,
    0x088a281a,
    0x088b9802,
    0x088c0012,
    0x088c100a,
    0x088d982a,
    0x088db082,
    0x088df81a,
    0x088e1018,
    0x088e4832,
    0x088e700a,
    0x088e7802,
    0x0891602a,
    0x08917822,
    0x0891901a,
    0x0891a002,
    0x0891a80a,
    0x0891b012,
    0x0891f002,
    0x08920802,
    0x0896f802,
    0x0897002a,
    0x08971872,
    0x08980012,
    0x0898101a,
    0x0899d812,
    0x0899f002,
    0x0899f80a,
    0x089a0002,
    0x089a083a,
    0x089a381a,
    0x089a582a,
    0x089ab802,
    0x089b101a,
    0x089b3062,
    0x089b8042,
    0x08a1a82a,
    0x08a1c072,
    0x08a2001a,
    0x08a21022,
    0x08a2280a,
    0x08a23002,
    0x08a2f002,
    0x08a58002,
    0x08a5881a,
    0x08a59852,
    0x08a5c80a,
    0x08a5d002,
    0x08a5d81a,
    0x08a5e802,
    0x08a5f00a,
    0x08a5f812,
    0x08a6080a,
    0x08a61012,
    0x08ad7802,
    0x08ad801a,
    0x08ad9032,
    0x08adc03a,
    0x08ade012,
    0x08adf00a,
    0x08adf812,
    0x08aee012,
    0x08b1802a,
    0x08b19872,
    0x08b1d81a,
    0x08b1e802,
    0x08b1f00a,
    0x08b1f812,
    0x08b55802,
    0x08b5600a,
    0x08b56802,
    0x08b5701a,
    0x08b58052,
    0x08b5b00a,
    0x08b5b802,
    0x08b8e822,
    0x08b91032,
    0x08b9300a,
    0x08b93842,
    0x08c1602a,
    0x08c17882,
    0x08c1c00a,
    0x08c1c812,
    0x08c98002,
    0x08c9884a,
    0x08c9b81a,
    0x08c9d812,
    0x08c9e80a,
    0x08c9f002,
    0x08c9f808,
    0x08ca000a,
    0x08ca0808,
    0x08ca100a,
    0x08ca1802,
    0x08ce882a,
    0x08cea032,
    0x08ced012,
    0x08cee03a,
    0x08cf0002,
    0x08cf200a,
    0x08d00892,
    0x08d19852,
    0x08d1c80a,
    0x08d1d008,
    0x08d1d832,
    0x08d23802,
    0x08d28852,
    0x08d2b81a,
    0x08d2c822,
    0x08d42058,
    0x08d450c2,
    0x08d4b80a,
    0x08d4c012,
    0x08e1780a,
    0x08e18062,
    0x08e1c052,
    0x08e1f00a,
    0x08e1f802,
    0x08e49152,
    0x08e5480a,
    0x08e55062,
    0x08e5880a,
    0x08e59012,
    0x08e5a00a,
    0x08e5a812,
    0x08e98852,
    0x08e9d002,
    0x08e9e012,
    0x08e9f862,
    0x08ea3008,
    0x08ea3802,
    0x08ec504a,
    0x08ec8012,
    0x08ec981a,
    0x08eca802,
    0x08ecb00a,
    0x08ecb802,
    0x08f79812,
    0x08f7a81a,
    0x08f80012,
    0x08f81008,
    0x08f8180a,
    0x08f9a01a,
    0x08f9b042,
    0x08f9f01a,
    0x08fa0002,
    0x08fa080a,
    0x08fa1002,
    0x09a180f1,
    0x09a20002,
    0x09a238e2,
    0x0b578042,
    0x0b598062,
    0x0b7a7802,
    0x0b7a8b6a,
    0x0b7c7832,
    0x0b7f2002,
    0x0b7f801a,
    0x0de4e812,
    0x0de50031,
    0x0e7802d2,
    0x0e798162,
    0x0e8b2802,
    0x0e8b300a,
    0x0e8b3822,
    0x0e8b680a,
    0x0e8b7042,
    0x0e8b9871,
    0x0e8bd872,
    0x0e8c2862,
    0x0e8d5032,
    0x0e921022,
    0x0ed00362,
    0x0ed1db12,
    0x0ed3a802,
    0x0ed42002,
    0x0ed4d842,
    0x0ed508e2,
    0x0f000062,
    0x0f004102,
    0x0f00d862,
    0x0f011812,
    0x0f013042,
    0x0f047802,
    0x0f098062,
    0x0f157002,
    0x0f176032,
    0x0f276032,
    0x0f468062,
    0x0f4a2062,
    0x0f8007f3,
    0x0f8407f3,
    0x0f886823,
    0x0f897803,
    0x0f8b6053,
    0x0f8bf013,
    0x0f8c7003,
    0x0f8c8893,
    0x0f8d6b83,
    0x0f8f3199,
    0x0f9008e3,
    0x0f90d003,
    0x0f917803,
    0x0f919083,
    0x0f91e033,
    0x0f924ff3,
    0x0f964ff3,
    0x0f9a4ff3,
    0x0f9e4b13,
    0x0f9fd842,
    0x0fa007f3,
    0x0fa407f3,
    0x0fa803d3,
    0x0faa37f3,
    0x0fae37f3,
    0x0fb23093,
    0x0fb407f3,
    0x0fbba0b3,
    0x0fbeaaa3,
    0x0fc06033,
    0x0fc24073,
    0x0fc2d053,
    0x0fc44073,
    0x0fc57513,
    0x0fc862e3,
    0x0fc9e093,
    0x0fca3ff3,
    0x0fce3ff3,
    0x0fd23ff3,
    0x0fd63b83,
    0x0fe007f3,
    0x0fe407f3,
    0x0fe807f3,
    0x0fec07f3,
    0x0ff007f3,
    0x0ff407f3,
    0x0ff807f3,
    0x0ffc07d3,
    0x700001f1,
    0x700105f2,
    0x700407f1,
    0x700807f2,
    0x700c06f2,
    0x700f87f1,
    0x701387f1,
    0x701787f1,
    0x701b87f1,
    0x701f87f1,
    0x702387f1,
    0x702787f1,
    0x702b87f1,
    0x702f87f1,
    0x703387f1,
    0x703787f1,
    0x703b87f1,
    0x703f87f1,
    0x704387f1,
    0x704787f1,
    0x704b87f1,
    0x704f87f1,
    0x705387f1,
    0x705787f1,
    0x705b87f1,
    0x705f87f1,
    0x706387f1,
    0x706787f1,
    0x706b87f1,
    0x706f87f1,
    0x707387f1,
    0x707787f1,
    0x707b87f1,
    0x707f80f1};
// clang-format on

/// Returns the extended grapheme cluster bondary property of a code point.
[[nodiscard]]
constexpr property get_property(const char32_t code_point) noexcept
{
    // The algorithm searches for the upper bound of the range and, when found,
    // steps back one entry. This algorithm is used since the code point can be
    // anywhere in the range. After a lower bound is found the next step is to
    // compare whether the code unit is indeed in the range.
    //
    // Since the entry contains a code unit, size, and property the code point
    // being sought needs to be adjusted. Just shifting the code point to the
    // proper position doesn't work; suppose an entry has property 0, size 1,
    // and lower bound 3. This results in the entry 0x1810.
    // When searching for code point 3 it will search for 0x1800, find 0x1810
    // and moves to the previous entry. Thus the lower bound value will never
    // be found.
    // The simple solution is to set the bits belonging to the property and
    // size. Then the upper bound for code point 3 will return the entry after
    // 0x1810. After moving to the previous entry the algorithm arrives at the
    // correct entry.
    ptrdiff_t i =
        std::ranges::upper_bound(entries, (code_point << 11) | 0x7ffu) -
        entries;
    if (i == 0) return property::none;

    --i;
    uint32_t upper_bound = (entries[i] >> 11) + ((entries[i] >> 4) & 0x7f);
    if (code_point <= upper_bound)
        return static_cast<property>(entries[i] & 0xf);

    return property::none;
}

} // namespace extended_grapheme_custer_property_boundary

namespace indic_conjunct_break {

enum class property : uint8_t {
    // Values generated from the data files.
    Consonant,
    Extend,
    Linker,

    // The code unit has none of above properties.
    none
};

/// The entries of the indic conjunct break property table.
///
/// The data is generated from
/// -  https://www.unicode.org/Public/UCD/latest/ucd/DerivedCoreProperties.txt
///
/// The data has 3 values
/// - bits [0, 1] The property. One of the values generated from the datafiles
///   of \ref property
/// - bits [2, 10] The size of the range.
/// - bits [11, 31] The lower bound code point of the range. The upper bound of
///   the range is lower bound + size.
///
/// The 9 bits for the size allow a maximum range of 512 elements. Some ranges
/// in the Unicode tables are larger. They are stored in multiple consecutive
/// ranges in the data table. An alternative would be to store the sizes in a
/// separate 16-bit value. The original MSVC STL code had such an approach, but
/// this approach uses less space for the data and is about 4% faster in the
/// following benchmark.
/// libcxx/benchmarks/std_format_spec_string_unicode.bench.cpp
// clang-format off
 inline constexpr uint32_t entries[201] = {
    0x00180139,
    0x001a807d,
    0x00241811,
    0x002c88b1,
    0x002df801,
    0x002e0805,
    0x002e2005,
    0x002e3801,
    0x00308029,
    0x00325851,
    0x00338001,
    0x0036b019,
    0x0036f815,
    0x00373805,
    0x0037500d,
    0x00388801,
    0x00398069,
    0x003f5821,
    0x003fe801,
    0x0040b00d,
    0x0040d821,
    0x00412809,
    0x00414811,
    0x0042c809,
    0x0044c01d,
    0x0046505d,
    0x00471871,
    0x0048a890,
    0x0049e001,
    0x004a6802,
    0x004a880d,
    0x004ac01c,
    0x004bc01c,
    0x004ca84c,
    0x004d5018,
    0x004d9000,
    0x004db00c,
    0x004de001,
    0x004e6802,
    0x004ee004,
    0x004ef800,
    0x004f8004,
    0x004ff001,
    0x0051e001,
    0x0054a84c,
    0x00555018,
    0x00559004,
    0x0055a810,
    0x0055e001,
    0x00566802,
    0x0057c800,
    0x0058a84c,
    0x00595018,
    0x00599004,
    0x0059a810,
    0x0059e001,
    0x005a6802,
    0x005ae004,
    0x005af800,
    0x005b8800,
    0x0060a84c,
    0x0061503c,
    0x0061e001,
    0x00626802,
    0x0062a805,
    0x0062c008,
    0x0065e001,
    0x0068a894,
    0x0069d805,
    0x006a6802,
    0x0071c009,
    0x0072400d,
    0x0075c009,
    0x0076400d,
    0x0078c005,
    0x0079a801,
    0x0079b801,
    0x0079c801,
    0x007b8805,
    0x007ba001,
    0x007bd00d,
    0x007c0001,
    0x007c1009,
    0x007c3005,
    0x007e3001,
    0x0081b801,
    0x0081c805,
    0x00846801,
    0x009ae809,
    0x00b8a001,
    0x00be9001,
    0x00bee801,
    0x00c54801,
    0x00c9c809,
    0x00d0b805,
    0x00d30001,
    0x00d3a81d,
    0x00d3f801,
    0x00d58035,
    0x00d5f83d,
    0x00d9a001,
    0x00db5821,
    0x00dd5801,
    0x00df3001,
    0x00e1b801,
    0x00e68009,
    0x00e6a031,
    0x00e71019,
    0x00e76801,
    0x00e7a001,
    0x00e7c005,
    0x00ee00fd,
    0x01006801,
    0x01068031,
    0x01070801,
    0x0107282d,
    0x01677809,
    0x016bf801,
    0x016f007d,
    0x01815015,
    0x0184c805,
    0x05337801,
    0x0533a025,
    0x0534f005,
    0x05378005,
    0x05416001,
    0x05470045,
    0x05495809,
    0x054d9801,
    0x05558001,
    0x05559009,
    0x0555b805,
    0x0555f005,
    0x05560801,
    0x0557b001,
    0x055f6801,
    0x07d8f001,
    0x07f1003d,
    0x080fe801,
    0x08170001,
    0x081bb011,
    0x08506801,
    0x08507801,
    0x0851c009,
    0x0851f801,
    0x08572805,
    0x0869200d,
    0x08755805,
    0x0877e809,
    0x087a3029,
    0x087c100d,
    0x08838001,
    0x0883f801,
    0x0885d001,
    0x08880009,
    0x08899805,
    0x088b9801,
    0x088e5001,
    0x0891b001,
    0x08974805,
    0x0899d805,
    0x089b3019,
    0x089b8011,
    0x08a23001,
    0x08a2f001,
    0x08a61801,
    0x08ae0001,
    0x08b5b801,
    0x08b95801,
    0x08c1d001,
    0x08c9f001,
    0x08ca1801,
    0x08d1a001,
    0x08d23801,
    0x08d4c801,
    0x08ea1001,
    0x08ea2005,
    0x08ecb801,
    0x08fa1001,
    0x0b578011,
    0x0b598019,
    0x0de4f001,
    0x0e8b2801,
    0x0e8b3809,
    0x0e8b7011,
    0x0e8bd81d,
    0x0e8c2819,
    0x0e8d500d,
    0x0e921009,
    0x0f000019,
    0x0f004041,
    0x0f00d819,
    0x0f011805,
    0x0f013011,
    0x0f047801,
    0x0f098019,
    0x0f157001,
    0x0f17600d,
    0x0f27600d,
    0x0f468019,
    0x0f4a2019};
// clang-format on

/// Returns the indic conjuct break property of a code point.
[[nodiscard]]
constexpr property get_property(const char32_t code_point) noexcept
{
    // The algorithm searches for the upper bound of the range and, when found,
    // steps back one entry. This algorithm is used since the code point can be
    // anywhere in the range. After a lower bound is found the next step is to
    // compare whether the code unit is indeed in the range.
    //
    // Since the entry contains a code unit, size, and property the code point
    // being sought needs to be adjusted. Just shifting the code point to the
    // proper position doesn't work; suppose an entry has property 0, size 1,
    // and lower bound 3. This results in the entry 0x1810.
    // When searching for code point 3 it will search for 0x1800, find 0x1810
    // and moves to the previous entry. Thus the lower bound value will never
    // be found.
    // The simple solution is to set the bits belonging to the property and
    // size. Then the upper bound for code point 3 will return the entry after
    // 0x1810. After moving to the previous entry the algorithm arrives at the
    // correct entry.
    ptrdiff_t i =
        std::ranges::upper_bound(entries, (code_point << 11) | 0x7ffu) -
        entries;
    if (i == 0) return property::none;

    --i;
    uint32_t upper_bound =
        (entries[i] >> 11) + ((entries[i] >> 2) & 0b1'1111'1111);
    if (code_point <= upper_bound)
        return static_cast<property>(entries[i] & 0b11);

    return property::none;
}

} // namespace indic_conjunct_break

namespace unicode {

struct consume_result {
    // When status == ok it contains the decoded code point.
    // Else it contains the replacement character U+FFFD
    char32_t code_point : 31;

    enum : char32_t {
        // Consumed a well-formed code point.
        ok = 0,
        // Encountered invalid UTF-8
        error = 1
    } status : 1 {ok};
};

inline constexpr char32_t replacement_character = U'\ufffd';

inline constexpr consume_result consume_result_error{
    replacement_character,
    consume_result::error};

[[nodiscard]]
inline constexpr bool is_surrogate(char32_t value)
{
    return value >= 0xd800 && value <= 0xdfff;
}

[[nodiscard]]
inline constexpr bool is_code_point(char32_t value)
{
    return value <= 0x10ffff;
}

[[nodiscard]]
inline constexpr bool is_scalar_value(char32_t value)
{
    return unicode::is_code_point(value) && !unicode::is_surrogate(value);
}

template <std::contiguous_iterator _Iterator>
    requires std::same_as<std::iter_value_t<_Iterator>, char>
constexpr bool is_continuation(_Iterator charr, int count)
{
    do {
        if ((*charr & 0b1100'0000) != 0b1000'0000) return false;
        --count;
        ++charr;
    } while (count);
    return true;
}

/// Helper class to extract a code unit from a Unicode character range.
///
/// The stored range is a view. There are multiple specialization for different
/// character types.
template <class _CharT>
class code_point_view;

/// UTF-8 specialization.
template <>
class code_point_view<char> {
    using _Iterator = std::basic_string_view<char>::const_iterator;

public:
    constexpr explicit code_point_view(_Iterator first, _Iterator last)
        : first_(first)
        , last_(last)
    {
    }

    constexpr bool at_end() const noexcept { return first_ == last_; }
    constexpr _Iterator position() const noexcept { return first_; }

    [[nodiscard]]
    constexpr consume_result consume() noexcept
    {
        // Based on the number of leading 1 bits the number of code units in the
        // code point can be determined. See
        // https://en.wikipedia.org/wiki/UTF-8#Encoding
        switch (std::countl_one(static_cast<unsigned char>(*first_))) {
        default:
        case 0: return {static_cast<unsigned char>(*first_++)};

        case 2: {
            if (last_ - first_ < 2 || !unicode::is_continuation(first_ + 1, 1))
                [[unlikely]]
                break;

            char32_t value = static_cast<unsigned char>(*first_++) & 0x1f;
            value <<= 6;
            value |= static_cast<unsigned char>(*first_++) & 0x3f;

            // These values should be encoded in 1 UTF-8 code unit.
            if (value < 0x0080) [[unlikely]]
                return consume_result_error;

            return {value};
        }

        case 3: {
            if (last_ - first_ < 3 || !unicode::is_continuation(first_ + 1, 2))
                [[unlikely]]
                break;

            char32_t value = static_cast<unsigned char>(*first_++) & 0x0f;
            value <<= 6;
            value |= static_cast<unsigned char>(*first_++) & 0x3f;
            value <<= 6;
            value |= static_cast<unsigned char>(*first_++) & 0x3f;

            // These values should be encoded in 1 or 2 UTF-8 code units.
            if (value < 0x0800) [[unlikely]]
                return consume_result_error;

            // A surrogate value is always encoded in 3 UTF-8 code units.
            if (unicode::is_surrogate(value)) [[unlikely]]
                return consume_result_error;

            return {value};
        }

        case 4: {
            if (last_ - first_ < 4 || !unicode::is_continuation(first_ + 1, 3))
                [[unlikely]]
                break;

            char32_t value = static_cast<unsigned char>(*first_++) & 0x07;
            value <<= 6;
            value |= static_cast<unsigned char>(*first_++) & 0x3f;
            value <<= 6;
            value |= static_cast<unsigned char>(*first_++) & 0x3f;
            value <<= 6;
            value |= static_cast<unsigned char>(*first_++) & 0x3f;

            // These values should be encoded in 1, 2, or 3 UTF-8 code units.
            if (value < 0x10000) [[unlikely]]
                return consume_result_error;

            // A value too large is always encoded in 4 UTF-8 code units.
            if (!unicode::is_code_point(value)) [[unlikely]]
                return consume_result_error;

            return {value};
        }
        }
        // An invalid number of leading ones can be garbage or a code unit in
        // the middle of a code point. By consuming one code unit the parser may
        // get "in sync" after a few code units.
        ++first_;
        return consume_result_error;
    }

private:
    _Iterator first_;
    _Iterator last_;
};

class extended_grapheme_cluster_break {
    using EGC_property = extended_grapheme_custer_property_boundary::property;
    using inCB_property = indic_conjunct_break::property;

public:
    constexpr explicit extended_grapheme_cluster_break(
        char32_t first_code_point)
        : prev_code_point_(first_code_point)
        , prev_property_(
              extended_grapheme_custer_property_boundary::get_property(
                  first_code_point))
    {
        // Initializes the active rule.
        if (prev_property_ == EGC_property::Extended_Pictographic)
            active_rule_ = rule::GB11_emoji;
        else if (prev_property_ == EGC_property::Regional_Indicator)
            active_rule_ = rule::GB12_GB13_regional_indicator;
        else if (
            indic_conjunct_break::get_property(first_code_point) ==
            inCB_property::Consonant)
            active_rule_ = rule::GB9c_indic_conjunct_break;
    }

    [[nodiscard]]
    constexpr bool operator()(char32_t next_code_point)
    {
        EGC_property next_property =
            extended_grapheme_custer_property_boundary::get_property(
                next_code_point);
        bool result = evaluate(next_code_point, next_property);
        prev_code_point_ = next_code_point;
        prev_property_ = next_property;
        return result;
    }

    // The code point whose break propery are considered during the next
    // evaluation cyle.
    [[nodiscard]]
    constexpr char32_t current_code_point() const
    {
        return prev_code_point_;
    }

private:
    // The naming of the identifiers matches the Unicode standard.
    // NOLINTBEGIN(readability-identifier-naming)

    [[nodiscard]]
    constexpr bool
    evaluate(char32_t next_code_point, EGC_property next_property)
    {
        switch (active_rule_) {
        case rule::none: return evaluate_none(next_code_point, next_property);
        case rule::GB9c_indic_conjunct_break:
            return evaluate_GB9c_indic_conjunct_break(
                next_code_point,
                next_property);
        case rule::GB11_emoji:
            return evaluate_GB11_emoji(next_code_point, next_property);
        case rule::GB12_GB13_regional_indicator:
            return evaluate_GB12_GB13_regional_indicator(
                next_code_point,
                next_property);
        }
    }

    constexpr bool
    evaluate_none(char32_t next_code_point, EGC_property next_property)
    {
        // *** Break at the start and end of text, unless the text is empty. ***

        // *** Do not break between a CR and LF. Otherwise, break before and
        // after controls. ***
        if (prev_property_ == EGC_property::CR &&
            next_property == EGC_property::LF) // GB3
            return false;

        if (prev_property_ == EGC_property::Control ||
            prev_property_ == EGC_property::CR ||
            prev_property_ == EGC_property::LF) // GB4
            return true;

        if (next_property == EGC_property::Control ||
            next_property == EGC_property::CR ||
            next_property == EGC_property::LF) // GB5
            return true;

        // *** Do not break Hangul syllable sequences. ***
        if (prev_property_ == EGC_property::L &&
            (next_property == EGC_property::L ||
             next_property == EGC_property::V ||
             next_property == EGC_property::LV ||
             next_property == EGC_property::LVT)) // GB6
            return false;

        if ((prev_property_ == EGC_property::LV ||
             prev_property_ == EGC_property::V) &&
            (next_property == EGC_property::V ||
             next_property == EGC_property::T)) // GB7
            return false;

        if ((prev_property_ == EGC_property::LVT ||
             prev_property_ == EGC_property::T) &&
            next_property == EGC_property::T) // GB8
            return false;

        // *** Do not break before extending characters or ZWJ. ***
        if (next_property == EGC_property::Extend ||
            next_property == EGC_property::ZWJ)
            return false; // GB9

        // *** Do not break before SpacingMarks, or after Prepend characters.
        // ***
        if (next_property == EGC_property::SpacingMark) // GB9a
            return false;

        if (prev_property_ == EGC_property::Prepend) // GB9b
            return false;

        // *** Do not break within certain combinations with
        // Indic_Conjunct_Break (InCB)=Linker. ***
        if (indic_conjunct_break::get_property(next_code_point) ==
            inCB_property::Consonant) {
            active_rule_ = rule::GB9c_indic_conjunct_break;
            GB9c_indic_conjunct_break_state_ =
                GB9c_indic_conjunct_break_state::Consonant;
            return true;
        }

        // *** Do not break within emoji modifier sequences or emoji zwj
        // sequences. ***
        if (next_property == EGC_property::Extended_Pictographic) {
            active_rule_ = rule::GB11_emoji;
            GB11_emoji_state_ = GB11_emoji_state::Extended_Pictographic;
            return true;
        }

        // *** Do not break within emoji flag sequences ***

        // That is, do not break between regional indicator (RI) symbols if
        // there is an odd number of RI characters before the break point.
        if (next_property == EGC_property::Regional_Indicator) { // GB12 + GB13
            active_rule_ = rule::GB12_GB13_regional_indicator;
            return true;
        }

        // *** Otherwise, break everywhere. ***
        return true; // GB999
    }

    [[nodiscard]]
    constexpr bool evaluate_GB9c_indic_conjunct_break(
        char32_t next_code_point,
        EGC_property next_property)
    {
        inCB_property breakk =
            indic_conjunct_break::get_property(next_code_point);
        if (breakk == inCB_property::none) {
            active_rule_ = rule::none;
            return evaluate_none(next_code_point, next_property);
        }

        switch (GB9c_indic_conjunct_break_state_) {
        case GB9c_indic_conjunct_break_state::Consonant:
            if (breakk == inCB_property::Extend) {
                return false;
            }
            if (breakk == inCB_property::Linker) {
                GB9c_indic_conjunct_break_state_ =
                    GB9c_indic_conjunct_break_state::Linker;
                return false;
            }
            active_rule_ = rule::none;
            return evaluate_none(next_code_point, next_property);

        case GB9c_indic_conjunct_break_state::Linker:
            if (breakk == inCB_property::Extend) {
                return false;
            }
            if (breakk == inCB_property::Linker) {
                return false;
            }
            if (breakk == inCB_property::Consonant) {
                GB9c_indic_conjunct_break_state_ =
                    GB9c_indic_conjunct_break_state::Consonant;
                return false;
            }
            active_rule_ = rule::none;
            return evaluate_none(next_code_point, next_property);
        }
    }

    [[nodiscard]]
    constexpr bool
    evaluate_GB11_emoji(char32_t next_code_point, EGC_property next_property)
    {
        switch (GB11_emoji_state_) {
        case GB11_emoji_state::Extended_Pictographic:
            if (next_property == EGC_property::Extend) {
                GB11_emoji_state_ = GB11_emoji_state::Extend;
                return false;
            }
            [[fallthrough]];
        case GB11_emoji_state::Extend:
            if (next_property == EGC_property::ZWJ) {
                GB11_emoji_state_ = GB11_emoji_state::ZWJ;
                return false;
            }
            if (next_property == EGC_property::Extend) return false;
            active_rule_ = rule::none;
            return evaluate_none(next_code_point, next_property);

        case GB11_emoji_state::ZWJ:
            if (next_property == EGC_property::Extended_Pictographic) {
                GB11_emoji_state_ = GB11_emoji_state::Extended_Pictographic;
                return false;
            }
            active_rule_ = rule::none;
            return evaluate_none(next_code_point, next_property);
        }
    }

    [[nodiscard]]
    constexpr bool evaluate_GB12_GB13_regional_indicator(
        char32_t next_code_point,
        EGC_property next_property)
    {
        active_rule_ = rule::none;
        if (next_property == EGC_property::Regional_Indicator) return false;
        return evaluate_none(next_code_point, next_property);
    }

    char32_t prev_code_point_;
    EGC_property prev_property_;

    enum class rule {
        none,
        GB9c_indic_conjunct_break,
        GB11_emoji,
        GB12_GB13_regional_indicator,
    };
    rule active_rule_ = rule::none;

    enum class GB11_emoji_state {
        Extended_Pictographic,
        Extend,
        ZWJ,
    };
    GB11_emoji_state GB11_emoji_state_ =
        GB11_emoji_state::Extended_Pictographic;

    enum class GB9c_indic_conjunct_break_state {
        Consonant,
        Linker,
    };

    GB9c_indic_conjunct_break_state GB9c_indic_conjunct_break_state_ =
        GB9c_indic_conjunct_break_state::Consonant;

    // NOLINTEND(readability-identifier-naming)
};

template <class _CharT>
class extended_grapheme_cluster_view {
    using _Iterator = typename std::basic_string_view<_CharT>::const_iterator;

public:
    constexpr explicit extended_grapheme_cluster_view(
        _Iterator first,
        _Iterator last)
        : code_point_view_(first, last)
        , at_break_(code_point_view_.consume().code_point)
    {
    }

    struct cluster {
        /// The first code point of the extended grapheme cluster.
        ///
        /// The first code point is used to estimate the width of the extended
        /// grapheme cluster.
        char32_t code_point_;

        /// Points one beyond the last code unit in the extended grapheme
        /// cluster.
        ///
        /// It's expected the caller has the start position and thus can
        /// determine the code unit range of the extended grapheme cluster.
        _Iterator last_;
    };

    [[nodiscard]]
    constexpr cluster consume()
    {
        char32_t code_point = at_break_.current_code_point();
        _Iterator position = code_point_view_.position();
        while (!code_point_view_.at_end()) {
            if (at_break_(code_point_view_.consume().code_point)) break;
            position = code_point_view_.position();
        }
        return {code_point, position};
    }

private:
    code_point_view<_CharT> code_point_view_;
    extended_grapheme_cluster_break at_break_;
};

} // namespace unicode

inline constexpr uint32_t number_max = INT32_MAX;

[[noreturn]]
inline void throw_format_error(const char* s)
{
    throw std::format_error(s);
}

template <class Tp>
constexpr void set_debug_format(Tp& formatter)
{
    if constexpr (requires { formatter.set_debug_format(); })
        formatter.set_debug_format();
}

template <std::contiguous_iterator Iterator>
struct parse_number_result {
    Iterator last;
    uint32_t value;
};

template <std::contiguous_iterator Iterator>
constexpr parse_number_result<Iterator>
parse_number(Iterator begin, Iterator end_input)
{
    using _CharT = std::iter_value_t<Iterator>;
    static_assert(
        number_max == INT32_MAX,
        "The algorithm is implemented based on this value.");
    /*
     * Limit the input to 9 digits, otherwise we need two checks during every
     * iteration:
     * - Are we at the end of the input?
     * - Does the value exceed width of an uint32_t? (Switching to uint64_t
     * would have the same issue, but with a higher maximum.)
     */
    Iterator end = end_input - begin > 9 ? begin + 9 : end_input;
    uint32_t value = *begin - _CharT('0');
    while (++begin != end) {
        if (*begin < _CharT('0') || *begin > _CharT('9')) return {begin, value};

        value = value * 10 + *begin - _CharT('0');
    }

    if (begin != end_input && *begin >= _CharT('0') && *begin <= _CharT('9')) {
        /*
         * There are more than 9 digits, do additional validations:
         * - Does the 10th digit exceed the maximum allowed value?
         * - Are there more than 10 digits?
         * (More than 10 digits always overflows the maximum.)
         */
        uint64_t v = uint64_t(value) * 10 + *begin++ - _CharT('0');
        if (v > number_max || (begin != end_input && *begin >= _CharT('0') &&
                               *begin <= _CharT('9')))
            throw_format_error(
                "The numeric value of the format specifier is too large");

        value = v;
    }

    return {begin, value};
}

template <std::contiguous_iterator Iterator>
constexpr parse_number_result<Iterator>
parse_zero(Iterator begin, Iterator, auto& parse_ctx)
{
    parse_ctx.check_arg_id(0);
    return {++begin, 0}; // can never be larger than the maximum.
}

template <std::contiguous_iterator Iterator>
constexpr parse_number_result<Iterator>
parse_automatic(Iterator begin, Iterator, auto& parse_ctx)
{
    size_t value = parse_ctx.next_arg_id();
    return {begin, uint32_t(value)};
}

template <std::contiguous_iterator Iterator>
constexpr parse_number_result<Iterator>
parse_manual(Iterator begin, Iterator end, auto& parse_ctx)
{
    parse_number_result<Iterator> r = parse_number(begin, end);
    parse_ctx.check_arg_id(r.value);
    return r;
}

template <std::contiguous_iterator Iterator>
constexpr parse_number_result<Iterator>
parse_arg_id2(Iterator begin, Iterator end, auto& parse_ctx)
{
    using _CharT = std::iter_value_t<Iterator>;
    switch (*begin) {
    case _CharT('0'): return parse_zero(begin, end, parse_ctx);

    case _CharT(':'):
        // This case is conditionally valid. It's allowed in an arg-id in the
        // replacement-field, but not in the std-format-spec. The caller can
        // provide a better diagnostic, so accept it here unconditionally.
    case _CharT('}'): return parse_automatic(begin, end, parse_ctx);
    }
    if (*begin < _CharT('0') || *begin > _CharT('9'))
        throw_format_error(
            "The argument index starts with an invalid character");

    return parse_manual(begin, end, parse_ctx);
}

} // namespace probfd

namespace probfd {

[[noreturn]]
inline void
throw_invalid_option_format_error(const char* id, const char* option)
{
    throw_format_error((std::string("The format specifier for ") + id +
                        " does not allow the " + option + " option")
                           .c_str());
}

[[noreturn]]
inline void throw_invalid_type_format_error(const char* id)
{
    throw_format_error(
        (std::string("The type option contains an invalid value for ") + id +
         " formatting argument")
            .c_str());
}

template <std::contiguous_iterator Iterator, class _ParseContext>
constexpr parse_number_result<Iterator>
parse_arg_id(Iterator begin, Iterator end, _ParseContext& ctx)
{
    using _CharT = std::iter_value_t<Iterator>;
    // This function is a wrapper to call the real parser. But it does the
    // validation for the pre-conditions and post-conditions.
    if (begin == end)
        throw_format_error("End of input while parsing an argument index");

    parse_number_result r = parse_arg_id(begin, end, ctx);

    if (r.last == end || *r.last != _CharT('}'))
        throw_format_error("The argument index is invalid");

    ++r.last;
    return r;
}

template <class _Context>
constexpr uint32_t substitute_arg_id(std::basic_format_arg<_Context> format_arg)
{
    return std::visit_format_arg(
        []<typename T>(T arg) -> uint32_t {
            if constexpr (std::same_as<T, std::monostate>)
                throw_format_error(
                    "The argument index value is too large for the number of "
                    "arguments supplied");

            if constexpr (
                std::same_as<T, int> || std::same_as<T, unsigned int> || //
                std::same_as<T, long long> ||
                std::same_as<T, unsigned long long>) {
                if constexpr (std::signed_integral<T>) {
                    if (arg < 0)
                        throw_format_error(
                            "An argument index may not have a negative value");
                }

                using _CT = std::common_type_t<T, decltype(number_max)>;
                if (static_cast<_CT>(arg) > static_cast<_CT>(number_max))
                    throw_format_error(
                        "The value of the argument index exceeds its maximum "
                        "value");

                return arg;
            } else
                throw_format_error(
                    "Replacement argument isn't a standard signed or unsigned "
                    "integer type");
        },
        format_arg);
}

struct fields {
    uint16_t sign_ : 1 {false};
    uint16_t alternate_form_ : 1 {false};
    uint16_t zero_padding_ : 1 {false};
    uint16_t precision_ : 1 {false};
    uint16_t locale_specific_form_ : 1 {false};
    uint16_t type_ : 1 {false};
    uint16_t use_range_fill_ : 1 {false};
    uint16_t clear_brackets_ : 1 {false};
    uint16_t consume_all_ : 1 {false};
};

inline constexpr fields fields_bool{
    .locale_specific_form_ = true,
    .type_ = true,
    .consume_all_ = true};
inline constexpr fields fields_integral{
    .sign_ = true,
    .alternate_form_ = true,
    .zero_padding_ = true,
    .locale_specific_form_ = true,
    .type_ = true,
    .consume_all_ = true};
inline constexpr fields fields_floating_point{
    .sign_ = true,
    .alternate_form_ = true,
    .zero_padding_ = true,
    .precision_ = true,
    .locale_specific_form_ = true,
    .type_ = true,
    .consume_all_ = true};
inline constexpr fields fields_string{
    .precision_ = true,
    .type_ = true,
    .consume_all_ = true};
inline constexpr fields fields_pointer{
    .zero_padding_ = true,
    .type_ = true,
    .consume_all_ = true};

inline constexpr fields fields_tuple{
    .use_range_fill_ = true,
    .clear_brackets_ = true};
inline constexpr fields fields_range{
    .use_range_fill_ = true,
    .clear_brackets_ = true};
inline constexpr fields fields_fill_align_width{};

enum class alignment : uint8_t { def, left, center, right, zero_padding };

enum class sign : uint8_t { def, minus, plus, space };

enum class type : uint8_t {
    def = 0,
    string,
    binary_lower_case,
    binary_upper_case,
    octal,
    decimal,
    hexadecimal_lower_case,
    hexadecimal_upper_case,
    pointer_lower_case,
    pointer_upper_case,
    charr,
    hexfloat_lower_case,
    hexfloat_upper_case,
    scientific_lower_case,
    scientific_upper_case,
    fixed_lower_case,
    fixed_upper_case,
    general_lower_case,
    general_upper_case,
    debug
};

inline constexpr uint32_t create_type_mask(type t)
{
    uint32_t shift = static_cast<uint32_t>(t);
    if (shift == 0) return 1;

    if (shift > 31) throw_format_error("The type does not fit in the mask");

    return 1 << shift;
}

inline constexpr uint32_t type_mask_integer =
    create_type_mask(type::binary_lower_case) |      //
    create_type_mask(type::binary_upper_case) |      //
    create_type_mask(type::decimal) |                //
    create_type_mask(type::octal) |                  //
    create_type_mask(type::hexadecimal_lower_case) | //
    create_type_mask(type::hexadecimal_upper_case);

struct sstd {
    alignment alignment_ : 3;
    sign sign_ : 2;
    bool alternate_form_ : 1;
    bool locale_specific_form_ : 1;
    type type_;
};

struct chrono {
    alignment alignment_ : 3;
    bool locale_specific_form_ : 1;
    bool hour_ : 1;
    bool weekday_name_ : 1;
    bool weekday_ : 1;
    bool day_of_year_ : 1;
    bool week_of_year_ : 1;
    bool month_name_ : 1;
};

template <class _CharT>
struct code_point;

template <>
struct code_point<char> {
    char data[4] = {' '};
};

template <>
struct code_point<wchar_t> {
    wchar_t data[4 / sizeof(wchar_t)] = {L' '};
};

template <class _CharT>
struct parsed_specifications {
    union {
        alignment alignment_ : 3;
        sstd std_;
        chrono chrono_;
    };

    /// The requested width.
    ///
    /// When the format-spec used an arg-id for this field it has already been
    /// replaced with the value of that arg-id.
    int32_t width_;

    /// The requested precision.
    ///
    /// When the format-spec used an arg-id for this field it has already been
    /// replaced with the value of that arg-id.
    int32_t precision_;

    code_point<_CharT> fill_;

    constexpr bool has_width() const { return width_ > 0; }

    constexpr bool has_precision() const { return precision_ >= 0; }
};

template <class _CharT>
class parser {
public:
    template <class _ParseContext>
    constexpr typename _ParseContext::iterator
    parse(_ParseContext& ctx, fields fields)
    {
        auto begin = ctx.begin();
        auto end = ctx.end();
        if (begin == end || *begin == _CharT('}') ||
            (fields.use_range_fill_ && *begin == _CharT(':')))
            return begin;

        if (parse_fill_align(begin, end) && begin == end) return begin;

        if (fields.sign_) {
            if (parse_sign(begin) && begin == end) return begin;
        } else if (std::is_constant_evaluated() && parse_sign(begin)) {
            throw_format_error(
                "The format specification does not allow the sign option");
        }

        if (fields.alternate_form_) {
            if (parse_alternate_form(begin) && begin == end) return begin;
        } else if (
            std::is_constant_evaluated() && parse_alternate_form(begin)) {
            throw_format_error(
                "The format specifier does not allow the alternate form "
                "option");
        }

        if (fields.zero_padding_) {
            if (parse_zero_padding(begin) && begin == end) return begin;
        } else if (std::is_constant_evaluated() && parse_zero_padding(begin)) {
            throw_format_error(
                "The format specifier does not allow the zero-padding option");
        }

        if (parse_width(begin, end, ctx) && begin == end) return begin;

        if (fields.precision_) {
            if (parse_precision(begin, end, ctx) && begin == end) return begin;
        } else if (
            std::is_constant_evaluated() && parse_precision(begin, end, ctx)) {
            throw_format_error(
                "The format specifier does not allow the precision option");
        }

        if (fields.locale_specific_form_) {
            if (parse_locale_specific_form(begin) && begin == end) return begin;
        } else if (
            std::is_constant_evaluated() && parse_locale_specific_form(begin)) {
            throw_format_error(
                "The format specifier does not allow the locale-specific form "
                "option");
        }

        if (fields.clear_brackets_) {
            if (parse_clear_brackets(begin) && begin == end) return begin;
        } else if (
            std::is_constant_evaluated() && parse_clear_brackets(begin)) {
            throw_format_error(
                "The format specifier does not allow the n option");
        }

        if (fields.type_) parse_type(begin);

        if (!fields.consume_all_) return begin;

        if (begin != end && *begin != _CharT('}'))
            throw_format_error(
                "The format specifier should consume the input or end with a "
                "'}'");

        return begin;
    }

    constexpr void
    validate(fields fields, const char* id, uint32_t type_mask = -1) const
    {
        if (!fields.sign_ && sign_ != sign::def) {
            if consteval {
                throw_format_error(
                    "The format specifier does not allow the sign option");
            } else {
                throw_invalid_option_format_error(id, "sign");
            }
        }

        if (!fields.alternate_form_ && alternate_form_) {
            if consteval {
                throw_format_error(
                    "The format specifier does not allow the alternate form "
                    "option");
            } else {
                throw_invalid_option_format_error(id, "alternate form");
            }
        }

        if (!fields.zero_padding_ && alignment_ == alignment::zero_padding) {
            if consteval {
                throw_format_error(
                    "The format specifier does not allow the zero-padding "
                    "option");
            } else {
                throw_invalid_option_format_error(id, "zero-padding");
            }
        }

        if (!fields.precision_ &&
            precision_ !=
                -1) { // Works both when the precision has a value or an arg-id.
            if consteval {
                throw_format_error(
                    "The format specifier does not allow the precision option");
            } else {
                throw_invalid_option_format_error(id, "precision");
            }
        }

        if (!fields.locale_specific_form_ && locale_specific_form_) {
            if consteval {
                throw_format_error(
                    "The format specifier does not allow the locale-specific "
                    "form option");
            } else {
                throw_invalid_option_format_error(id, "locale-specific form");
            }
        }

        if ((create_type_mask(type_) & type_mask) == 0) {
            if consteval {
                throw_format_error(
                    "The format specifier uses an invalid value for the type "
                    "option");
            } else {
                throw_invalid_type_format_error(id);
            }
        }
    }

    /// \returns the `parsed_specifications` with the resolved dynamic sizes..
    parsed_specifications<_CharT> get_parsed_std_specifications(auto& ctx) const
    {
        return parsed_specifications<_CharT>{
            .std_ =
                sstd{
                    .alignment_ = alignment_,
                    .sign_ = sign_,
                    .alternate_form_ = alternate_form_,
                    .locale_specific_form_ = locale_specific_form_,
                    .type_ = type_},
            .width_{get_width(ctx)},
            .precision_{get_precision(ctx)},
            .fill_{fill_}};
    }

    parsed_specifications<_CharT>
    get_parsed_chrono_specifications(auto& ctx) const
    {
        return parsed_specifications<_CharT>{
            .chrono_ =
                chrono{
                    .alignment_ = alignment_,
                    .locale_specific_form_ = locale_specific_form_,
                    .hour_ = hour_,
                    .weekday_name_ = weekday_name_,
                    .weekday_ = weekday_,
                    .day_of_year_ = day_of_year_,
                    .week_of_year_ = week_of_year_,
                    .month_name_ = month_name_},
            .width_{get_width(ctx)},
            .precision_{get_precision(ctx)},
            .fill_{fill_}};
    }

    alignment alignment_ : 3 {alignment::def};
    sign sign_ : 2 {sign::def};
    bool alternate_form_ : 1 {false};
    bool locale_specific_form_ : 1 {false};
    bool clear_brackets_ : 1 {false};
    type type_{type::def};

    // These flags are only used for formatting chrono. Since the struct has
    // padding space left it's added to this structure.
    bool hour_ : 1 {false};

    bool weekday_name_ : 1 {false};
    bool weekday_ : 1 {false};

    bool day_of_year_ : 1 {false};
    bool week_of_year_ : 1 {false};

    bool month_name_ : 1 {false};

    uint8_t reserved_0_ : 2 {0};
    uint8_t reserved_1_ : 6 {0};
    // These two flags are only used internally and not part of the
    // parsed_specifications. Therefore put them at the end.
    bool width_as_arg_ : 1 {false};
    bool precision_as_arg_ : 1 {false};

    /// The requested width, either the value or the arg-id.
    int32_t width_{0};

    /// The requested precision, either the value or the arg-id.
    int32_t precision_{-1};

    code_point<_CharT> fill_{};

private:
    constexpr bool parse_alignment(_CharT c)
    {
        switch (c) {
        default:
        case _CharT('<'): alignment_ = alignment::left; return true;

        case _CharT('^'): alignment_ = alignment::center; return true;

        case _CharT('>'): alignment_ = alignment::right; return true;
        }
        return false;
    }

    constexpr void validate_fill_character(_CharT fill)
    {
        // The forbidden fill characters all code points formed from a single
        // code unit, thus the check can be omitted when more code units are
        // used.
        if (fill == _CharT('{'))
            throw_format_error("The fill option contains an invalid value");
    }

#ifndef _LIBCPP_HAS_NO_UNICODE
    // range-fill and tuple-fill are identical
    template <std::contiguous_iterator Iterator>
        requires std::same_as<_CharT, char>
#ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
                 || (std::same_as<_CharT, wchar_t> && sizeof(wchar_t) == 2)
#endif
    constexpr bool parse_fill_align(Iterator& begin, Iterator end)
    {
        unicode::code_point_view<_CharT> view{begin, end};
        unicode::consume_result consumed = view.consume();
        if (consumed.status != unicode::consume_result::ok)
            throw_format_error(
                "The format specifier contains malformed Unicode characters");

        if (view.position() < end && parse_alignment(*view.position())) {
            ptrdiff_t code_units = view.position() - begin;
            if (code_units == 1)
                // The forbidden fill characters all are code points encoded
                // in one code unit, thus the check can be omitted when more
                // code units are used.
                validate_fill_character(*begin);

            std::copy_n(begin, code_units, std::addressof(fill_.data[0]));
            begin += code_units + 1;
            return true;
        }

        if (!parse_alignment(*begin)) return false;

        ++begin;
        return true;
    }

#ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
    template <std::contiguous_iterator Iterator>
        requires(std::same_as<_CharT, wchar_t> && sizeof(wchar_t) == 4)
    constexpr bool parse_fill_align(Iterator& begin, Iterator end)
    {
        if (begin + 1 != end && parse_alignment(*(begin + 1))) {
            if (!unicode::is_scalar_value(*begin))
                throw_format_error("The fill option contains an invalid value");

            validate_fill_character(*begin);

            fill_.data[0] = *begin;
            begin += 2;
            return true;
        }

        if (!parse_alignment(*begin)) return false;

        ++begin;
        return true;
    }

#endif // _LIBCPP_HAS_NO_WIDE_CHARACTERS

#else // _LIBCPP_HAS_NO_UNICODE
    // range-fill and tuple-fill are identical
    template <std::contiguous_iterator Iterator>
    constexpr bool parse_fill_align(Iterator& begin, Iterator end)
    {
        _LIBCPP_ASSERT_VALID_ELEMENT_ACCESS(
            begin != end,
            "when called with an empty input the function will cause "
            "undefined behavior by evaluating data not in the input");
        if (begin + 1 != end) {
            if (parse_alignment(*(begin + 1))) {
                validate_fill_character(*begin);

                fill_.data[0] = *begin;
                begin += 2;
                return true;
            }
        }

        if (!parse_alignment(*begin)) return false;

        ++begin;
        return true;
    }

#endif // _LIBCPP_HAS_NO_UNICODE

    template <std::contiguous_iterator Iterator>
    constexpr bool parse_sign(Iterator& begin)
    {
        switch (*begin) {
        case _CharT('-'): sign_ = sign::minus; break;
        case _CharT('+'): sign_ = sign::plus; break;
        case _CharT(' '): sign_ = sign::space; break;
        default: return false;
        }
        ++begin;
        return true;
    }

    template <std::contiguous_iterator Iterator>
    constexpr bool parse_alternate_form(Iterator& begin)
    {
        if (*begin != _CharT('#')) return false;

        alternate_form_ = true;
        ++begin;
        return true;
    }

    template <std::contiguous_iterator Iterator>
    constexpr bool parse_zero_padding(Iterator& begin)
    {
        if (*begin != _CharT('0')) return false;

        if (alignment_ == alignment::def) alignment_ = alignment::zero_padding;
        ++begin;
        return true;
    }

    template <std::contiguous_iterator Iterator>
    constexpr bool parse_width(Iterator& begin, Iterator end, auto& ctx)
    {
        if (*begin == _CharT('0'))
            throw_format_error(
                "The width option should not have a leading zero");

        if (*begin == _CharT('{')) {
            parse_number_result r = parse_arg_id(++begin, end, ctx);
            width_as_arg_ = true;
            width_ = r.value;
            begin = r.last;
            return true;
        }

        if (*begin < _CharT('0') || *begin > _CharT('9')) return false;

        parse_number_result r = parse_number(begin, end);
        width_ = r.value;
        begin = r.last;
        return true;
    }

    template <std::contiguous_iterator Iterator>
    constexpr bool parse_precision(Iterator& begin, Iterator end, auto& ctx)
    {
        if (*begin != _CharT('.')) return false;

        ++begin;
        if (begin == end)
            throw_format_error(
                "End of input while parsing format specifier precision");

        if (*begin == _CharT('{')) {
            parse_number_result arg_id = parse_arg_id(++begin, end, ctx);
            precision_as_arg_ = true;
            precision_ = arg_id.value;
            begin = arg_id.last;
            return true;
        }

        if (*begin < _CharT('0') || *begin > _CharT('9'))
            throw_format_error(
                "The precision option does not contain a value or an argument "
                "index");

        parse_number_result r = parse_number(begin, end);
        precision_ = r.value;
        precision_as_arg_ = false;
        begin = r.last;
        return true;
    }

    template <std::contiguous_iterator Iterator>
    constexpr bool parse_locale_specific_form(Iterator& begin)
    {
        if (*begin != _CharT('L')) return false;

        locale_specific_form_ = true;
        ++begin;
        return true;
    }

    template <std::contiguous_iterator Iterator>
    constexpr bool parse_clear_brackets(Iterator& begin)
    {
        if (*begin != _CharT('n')) return false;

        clear_brackets_ = true;
        ++begin;
        return true;
    }

    template <std::contiguous_iterator Iterator>
    constexpr void parse_type(Iterator& begin)
    {
        // Determines the type. It does not validate whether the selected type
        // is valid. Most formatters have optional fields that are only allowed
        // for certain types. These parsers need to do validation after the type
        // has been parsed. So its easier to implement the validation for all
        // types in the specific parse function.
        switch (*begin) {
        case 'A': type_ = type::hexfloat_upper_case; break;
        case 'B': type_ = type::binary_upper_case; break;
        case 'E': type_ = type::scientific_upper_case; break;
        case 'F': type_ = type::fixed_upper_case; break;
        case 'G': type_ = type::general_upper_case; break;
        case 'X': type_ = type::hexadecimal_upper_case; break;
        case 'a': type_ = type::hexfloat_lower_case; break;
        case 'b': type_ = type::binary_lower_case; break;
        case 'c': type_ = type::charr; break;
        case 'd': type_ = type::decimal; break;
        case 'e': type_ = type::scientific_lower_case; break;
        case 'f': type_ = type::fixed_lower_case; break;
        case 'g': type_ = type::general_lower_case; break;
        case 'o': type_ = type::octal; break;
        case 'p': type_ = type::pointer_lower_case; break;
        case 'P': type_ = type::pointer_upper_case; break;
        case 's': type_ = type::string; break;
        case 'x': type_ = type::hexadecimal_lower_case; break;
        case '?': type_ = type::debug; break;
        default: return;
        }
        ++begin;
    }

    int32_t get_width(auto& ctx) const
    {
        if (!width_as_arg_) return width_;

        return substitute_arg_id(ctx.arg(width_));
    }

    int32_t get_precision(auto& ctx) const
    {
        if (!precision_as_arg_) return precision_;

        return substitute_arg_id(ctx.arg(precision_));
    }
};

// Validates whether the reserved bitfields don't change the size.
static_assert(sizeof(parser<char>) == 16);
#ifndef _LIBCPP_HAS_NO_WIDE_CHARACTERS
static_assert(sizeof(parser<wchar_t>) == 16);
#endif

constexpr void process_display_type_string(type type)
{
    switch (type) {
    case type::def:
    case type::string:
    case type::debug: break;

    default:
        throw_format_error(
            "The type option contains an invalid value for a string formatting "
            "argument");
    }
}

template <class _CharT>
constexpr void
process_display_type_bool_string(parser<_CharT>& parser, const char* id)
{
    parser.validate(fields_bool, id);
    if (parser.alignment_ == alignment::def)
        parser.alignment_ = alignment::left;
}

template <class _CharT>
constexpr void process_display_type_char(parser<_CharT>& parser, const char* id)
{
    process_display_type_bool_string(parser, id);
}

template <class _CharT>
constexpr void process_parsed_bool(parser<_CharT>& parser, const char* id)
{
    switch (parser.type_) {
    case type::def:
    case type::string: process_display_type_bool_string(parser, id); break;

    case type::binary_lower_case:
    case type::binary_upper_case:
    case type::octal:
    case type::decimal:
    case type::hexadecimal_lower_case:
    case type::hexadecimal_upper_case: break;

    default: throw_invalid_type_format_error(id);
    }
}

template <class _CharT>
constexpr void process_parsed_char(parser<_CharT>& parser, const char* id)
{
    switch (parser.type_) {
    case type::def:
    case type::charr:
    case type::debug: process_display_type_char(parser, id); break;

    case type::binary_lower_case:
    case type::binary_upper_case:
    case type::octal:
    case type::decimal:
    case type::hexadecimal_lower_case:
    case type::hexadecimal_upper_case: break;

    default: throw_invalid_type_format_error(id);
    }
}

template <class _CharT>
constexpr void process_parsed_integer(parser<_CharT>& parser, const char* id)
{
    switch (parser.type_) {
    case type::def:
    case type::binary_lower_case:
    case type::binary_upper_case:
    case type::octal:
    case type::decimal:
    case type::hexadecimal_lower_case:
    case type::hexadecimal_upper_case: break;

    case type::charr: process_display_type_char(parser, id); break;

    default: throw_invalid_type_format_error(id);
    }
}

template <class _CharT>
constexpr void
process_parsed_floating_point(parser<_CharT>& parser, const char* id)
{
    switch (parser.type_) {
    case type::def:
    case type::hexfloat_lower_case:
    case type::hexfloat_upper_case:
        // Precision specific behavior will be handled later.
        break;
    case type::scientific_lower_case:
    case type::scientific_upper_case:
    case type::fixed_lower_case:
    case type::fixed_upper_case:
    case type::general_lower_case:
    case type::general_upper_case:
        if (!parser.precision_as_arg_ && parser.precision_ == -1)
            // Set the def precision for the call to to_chars.
            parser.precision_ = 6;
        break;

    default: throw_invalid_type_format_error(id);
    }
}

constexpr void process_display_type_pointer(type type, const char* id)
{
    switch (type) {
    case type::def:
    case type::pointer_lower_case:
    case type::pointer_upper_case: break;

    default: throw_invalid_type_format_error(id);
    }
}

template <std::contiguous_iterator Iterator>
struct column_width_result {
    /// The number of output columns.
    size_t width_;
    /// One beyond the last code unit used in the estimation.
    ///
    /// This limits the original output to fit in the wanted number of columns.
    Iterator last_;
};

template <std::contiguous_iterator Iterator>
column_width_result(size_t, Iterator) -> column_width_result<Iterator>;

/// Since a column width can be two it's possible that the requested column
/// width can't be achieved. Depending on the intended usage the policy can be
/// selected.
/// - When used as precision the maximum width may not be exceeded and the
///   result should be "rounded down" to the previous boundary.
/// - When used as a width we're done once the minimum is reached, but
///   exceeding is not an issue. Rounding down is an issue since that will
///   result in writing fill characters. Therefore the result needs to be
///   "rounded up".
enum class column_width_rounding { down, up };

#ifndef _LIBCPP_HAS_NO_UNICODE

namespace detail {
template <std::contiguous_iterator Iterator>
constexpr column_width_result<Iterator>
estimate_column_width_grapheme_clustering(
    Iterator first,
    Iterator last,
    size_t maximum,
    column_width_rounding rounding) noexcept
{
    using _CharT = std::iter_value_t<Iterator>;
    unicode::extended_grapheme_cluster_view<_CharT> view{first, last};

    column_width_result<Iterator> result{0, first};
    while (result.last_ != last && result.width_ <= maximum) {
        typename unicode::extended_grapheme_cluster_view<_CharT>::cluster
            cluster = view.consume();
        int width =
            width_estimation_table::estimated_width(cluster.code_point_);

        // When the next entry would exceed the maximum width the previous width
        // might be returned. For example when a width of 100 is requested the
        // returned width might be 99, since the next code point has an
        // estimated column width of 2. This depends on the rounding flag. When
        // the maximum is exceeded the loop will abort the next iteration.
        if (rounding == column_width_rounding::down &&
            result.width_ + width > maximum)
            return result;

        result.width_ += width;
        result.last_ = cluster.last_;
    }

    return result;
}

} // namespace detail

constexpr bool is_ascii(char32_t c)
{
    return c < 0x80;
}

template <
    class _CharT,
    class Iterator = typename std::basic_string_view<_CharT>::const_iterator>
constexpr column_width_result<Iterator> estimate_column_width(
    std::basic_string_view<_CharT> str,
    size_t maximum,
    column_width_rounding rounding) noexcept
{
    // The width estimation is done in two steps:
    // - Quickly process for the ASCII part. ASCII has the following properties
    //   - One code unit is one code point
    //   - Every code point has an estimated width of one
    // - When needed it will a Unicode Grapheme clustering algorithm to find
    //   the proper place for truncation.

    if (str.empty() || maximum == 0) return {0, str.begin()};

    // ASCII has one caveat; when an ASCII character is followed by a non-ASCII
    // character they might be part of an extended grapheme cluster. For
    // example:
    //   an ASCII letter and a COMBINING ACUTE ACCENT
    // The truncate should happen after the COMBINING ACUTE ACCENT. Therefore we
    // need to scan one code unit beyond the requested precision. When this code
    // unit is non-ASCII we omit the current code unit and let the Grapheme
    // clustering algorithm do its work.
    auto it = str.begin();
    if (is_ascii(*it)) {
        do {
            --maximum;
            ++it;
            if (it == str.end()) return {str.size(), str.end()};

            if (maximum == 0) {
                if (is_ascii(*it))
                    return {static_cast<size_t>(it - str.begin()), it};

                break;
            }
        } while (is_ascii(*it));
        --it;
        ++maximum;
    }

    ptrdiff_t ascii_size = it - str.begin();
    column_width_result result =
        detail::estimate_column_width_grapheme_clustering(
            it,
            str.end(),
            maximum,
            rounding);

    result.width_ += ascii_size;
    return result;
}
#else // !defined(_LIBCPP_HAS_NO_UNICODE)
template <class _CharT>
constexpr column_width_result<
    typename basic_string_view<_CharT>::const_iterator>
estimate_column_width(
    basic_string_view<_CharT> str,
    size_t maximum,
    column_width_rounding) noexcept
{
    // When Unicode isn't supported assume ASCII and every code unit is one code
    // point. In ASCII the estimated column width is always one. Thus there's no
    // need for rounding.
    size_t width = std::min(str.size(), maximum);
    return {width, str.begin() + width};
}

#endif // !defined(_LIBCPP_HAS_NO_UNICODE)

} // namespace probfd

namespace probfd {

template <class Tp>
concept fmt_pair_like =
    Specialization<Tp, std::pair> ||
    (Specialization<Tp, std::tuple> && std::tuple_size_v<Tp> == 2);

template <class _Tp, class _CharT = char>
    requires std::same_as<std::remove_cvref_t<_Tp>, _Tp> &&
             std::formattable<_Tp, _CharT>
struct range_formatter {
    constexpr void
    set_separator(std::basic_string_view<_CharT> separator) noexcept
    {
        separator_ = separator;
    }
    constexpr void set_brackets(
        std::basic_string_view<_CharT> opening_bracket,
        std::basic_string_view<_CharT> closing_bracket) noexcept
    {
        opening_bracket_ = opening_bracket;
        closing_bracket_ = closing_bracket;
    }

    constexpr std::formatter<_Tp, _CharT>& underlying() noexcept
    {
        return underlying_;
    }
    constexpr const std::formatter<_Tp, _CharT>& underlying() const noexcept
    {
        return underlying_;
    }

    template <class _ParseContext>
    constexpr typename _ParseContext::iterator parse(_ParseContext& ctx)
    {
        auto begin = parser_.parse(ctx, fields_range);
        auto end = ctx.end();
        // Note the cases where begin == end in this code only happens when the
        // replacement-field has no terminating }, or when the parse is manually
        // called with a format-spec. The former is an error and the latter
        // means using a formatter without the format functions or print.
        if (begin == end) [[unlikely]]
            return parse_empty_range_underlying_spec(ctx, begin);

        // The n field overrides a possible m type, therefore delay applying the
        // effect of n until the type has been procesed.
        parse_type(begin, end);
        if (parser_.clear_brackets_) set_brackets({}, {});
        if (begin == end) [[unlikely]]
            return parse_empty_range_underlying_spec(ctx, begin);

        bool has_range_underlying_spec = *begin == _CharT(':');
        if (has_range_underlying_spec) {
            // range-underlying-spec:
            //   :  format-spec
            ++begin;
        } else if (begin != end && *begin != _CharT('}'))
            // When there is no underlaying range the current parse should have
            // consumed the format-spec. If not, the not consumed input will be
            // processed by the underlying. For example {:-} for a range in
            // invalid, the sign field is not present. Without this check the
            // underlying_ will get -} as input which my be valid.
            throw_format_error(
                "The format specifier should consume the input or end with a "
                "'}'");

        ctx.advance_to(begin);
        begin = underlying_.parse(ctx);

        // This test should not be required if has_range_underlying_spec is
        // false. However this test makes sure the underlying formatter left the
        // parser in a valid state. (Note this is not a full protection against
        // evil parsers. For example
        //   } this is test for the next argument {}
        //   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^
        // could consume more than it should.
        if (begin != end && *begin != _CharT('}'))
            throw_format_error(
                "The format specifier should consume the input or end with a "
                "'}'");

        if (parser_.type_ != type::def) {
            // [format.range.formatter]/6
            //   If the range-type is s or ?s, then there shall be no n option
            //   and no range-underlying-spec.
            if (parser_.clear_brackets_) {
                if (parser_.type_ == type::string)
                    throw_format_error(
                        "The n option and type s can't be used together");
                throw_format_error(
                    "The n option and type ?s can't be used together");
            }
            if (has_range_underlying_spec) {
                if (parser_.type_ == type::string)
                    throw_format_error(
                        "Type s and an underlying format specification can't "
                        "be used together");
                throw_format_error(
                    "Type ?s and an underlying format specification can't be "
                    "used together");
            }
        } else if (!has_range_underlying_spec)
            set_debug_format(underlying_);

        return begin;
    }

    template <std::ranges::input_range _Rp, class _FormatContext>
        requires std::
                     formattable<std::ranges::range_reference_t<_Rp>, _CharT> &&
                 std::same_as<
                     std::remove_cvref_t<std::ranges::range_reference_t<_Rp>>,
                     _Tp>
    typename _FormatContext::iterator
    format(_Rp&& range, _FormatContext& ctx) const
    {
        parsed_specifications<_CharT> specs =
            parser_.get_parsed_std_specifications(ctx);

        if (!specs.has_width()) return format_range(range, ctx, specs);

        std::size_t capacity_hint = 0;
        if constexpr (std::ranges::sized_range<_Rp>)
            capacity_hint = 8 * std::ranges::size(range);
        retarget_buffer<_CharT> buffer{capacity_hint};
        std::basic_format_context<
            typename retarget_buffer<_CharT>::iterator,
            _CharT>
            c{buffer.make_output_iterator(), ctx};

        format_range(range, c, specs);

        return write_string_no_precision(buffer.view(), ctx.out(), specs);
    }

    template <std::ranges::input_range _Rp, class _FormatContext>
    typename _FormatContext::iterator format_range(
        _Rp&& range,
        _FormatContext& ctx,
        parsed_specifications<_CharT> specs) const
    {
        if constexpr (std::same_as<_Tp, _CharT>) {
            switch (specs.std_.type_) {
            case type::string:
            case type::debug:
                return format_as_string(
                    range,
                    ctx,
                    specs.std_.type_ == type::debug);
            default: return format_as_sequence(range, ctx);
            }
        } else
            return format_as_sequence(range, ctx);
    }

    template <std::ranges::input_range _Rp, class _FormatContext>
    typename _FormatContext::iterator
    format_as_string(_Rp&& range, _FormatContext& ctx, bool debug_format) const
    {
        // When the range is contiguous use a basic_string_view instead to avoid
        // a copy of the underlying data. The basic_string_view formatter
        // specialization is the "basic" string formatter in libc++.
        if constexpr (
            std::ranges::contiguous_range<_Rp> &&
            std::ranges::sized_range<_Rp>) {
            std::formatter<std::basic_string_view<_CharT>, _CharT> formatter;
            if (debug_format) formatter.set_debug_format();
            return formatter.format(
                std::basic_string_view<_CharT>{
                    std::ranges::data(range),
                    std::ranges::size(range),
                },
                ctx);
        } else {
            std::formatter<std::basic_string<_CharT>, _CharT> formatter;
            if (debug_format) formatter.set_debug_format();
            return formatter.format(
                std::basic_string<_CharT>{std::from_range, range},
                ctx);
        }
    }

    template <std::ranges::input_range _Rp, class _FormatContext>
    typename _FormatContext::iterator
    format_as_sequence(_Rp&& range, _FormatContext& ctx) const
    {
        ctx.advance_to(std::ranges::copy(opening_bracket_, ctx.out()).out);
        bool use_separator = false;
        for (auto&& e : range) {
            if (use_separator)
                ctx.advance_to(std::ranges::copy(separator_, ctx.out()).out);
            else
                use_separator = true;

            ctx.advance_to(underlying_.format(e, ctx));
        }

        return std::ranges::copy(closing_bracket_, ctx.out()).out;
    }

    parser<_CharT> parser_{.alignment_ = alignment::left};

private:
    template <std::contiguous_iterator Iterator>
    constexpr void parse_type(Iterator& begin, Iterator end)
    {
        switch (*begin) {
        default:
        case _CharT('m'):
            if constexpr (fmt_pair_like<_Tp>) {
                set_brackets(
                    STATICALLY_WIDEN(_CharT, "{"),
                    STATICALLY_WIDEN(_CharT, "}"));
                set_separator(STATICALLY_WIDEN(_CharT, ", "));
                ++begin;
            } else
                throw_format_error(
                    "Type m requires a pair or a tuple with two elements");
            break;

        case _CharT('s'):
            if constexpr (std::same_as<_Tp, _CharT>) {
                parser_.type_ = type::string;
                ++begin;
            } else
                throw_format_error(
                    "Type s requires character type as formatting argument");
            break;

        case _CharT('?'):
            ++begin;
            if (begin == end || *begin != _CharT('s'))
                throw_format_error(
                    "The format specifier should consume the input or end with "
                    "a '}'");
            if constexpr (std::same_as<_Tp, _CharT>) {
                parser_.type_ = type::debug;
                ++begin;
            } else
                throw_format_error(
                    "Type ?s requires character type as formatting argument");
        }
    }

    template <class _ParseContext>
    constexpr typename _ParseContext::iterator
    parse_empty_range_underlying_spec(
        _ParseContext& ctx,
        typename _ParseContext::iterator begin)
    {
        ctx.advance_to(begin);
        [[maybe_unused]]
        typename _ParseContext::iterator result = underlying_.parse(ctx);
        return begin;
    }

    std::formatter<_Tp, _CharT> underlying_;
    std::basic_string_view<_CharT> separator_ = STATICALLY_WIDEN(_CharT, ", ");
    std::basic_string_view<_CharT> opening_bracket_ =
        STATICALLY_WIDEN(_CharT, "[");
    std::basic_string_view<_CharT> closing_bracket_ =
        STATICALLY_WIDEN(_CharT, "]");
};

} // namespace probfd

namespace probfd {
template <class _Rp, class _CharT>
concept const_formattable_range =
    std::ranges::input_range<const _Rp> &&
    std::formattable<std::ranges::range_reference_t<const _Rp>, _CharT>;

template <class _Rp, class _CharT>
using fmt_maybe_const =
    std::conditional_t<const_formattable_range<_Rp, _CharT>, const _Rp, _Rp>;

// This shadows map, set, and string.
enum class range_format { disabled, map, set, sequence, string, debug_string };

// There is no definition of this struct, it's purely intended to be used to
// generate diagnostics.
template <class _Rp>
struct instantiated_the_primary_template_of_format_kind;

template <class _Rp>
constexpr range_format format_kind = [] {
    // [format.range.fmtkind]/1
    // A program that instantiates the primary template of format_kind is
    // ill-formed.
    static_assert(
        sizeof(_Rp) != sizeof(_Rp),
        "create a template specialization of format_kind for your type");
    return range_format::disabled;
}();

template <std::ranges::input_range _Rp>
    requires std::same_as<_Rp, std::remove_cvref_t<_Rp>>
inline constexpr range_format format_kind<_Rp> = [] {
    // [format.range.fmtkind]/2

    // 2.1 If same_as<remove_cvref_t<ranges::range_reference_t<R>>, R> is true,
    // Otherwise format_kind<R> is range_format::disabled.
    if constexpr (std::same_as<
                      std::remove_cvref_t<std::ranges::range_reference_t<_Rp>>,
                      _Rp>)
        return range_format::disabled;
    // 2.2 Otherwise, if the qualified-id R::key_type is valid and denotes a
    // type:
    else if constexpr (requires { typename _Rp::key_type; }) {
        // 2.2.1 If the qualified-id R::mapped_type is valid and denotes a type
        // ...
        if constexpr (
            requires { typename _Rp::mapped_type; } &&
            // 2.2.1 ... If either U is a specialization of pair or U is a
            // specialization of tuple and tuple_size_v<U> == 2
            fmt_pair_like<
                std::remove_cvref_t<std::ranges::range_reference_t<_Rp>>>)
            return range_format::map;
        else
            // 2.2.2 Otherwise format_kind<R> is range_format::set.
            return range_format::set;
    } else
        // 2.3 Otherwise, format_kind<R> is range_format::sequence.
        return range_format::sequence;
}();

template <range_format _Kp, std::ranges::input_range _Rp, class _CharT>
struct _LIBCPP_TEMPLATE_VIS range_default_formatter;

// Required specializations

template <std::ranges::input_range _Rp, class _CharT>
struct _LIBCPP_TEMPLATE_VIS
    range_default_formatter<range_format::sequence, _Rp, _CharT> {
private:
    using maybe_const_r = fmt_maybe_const<_Rp, _CharT>;
    range_formatter<
        std::remove_cvref_t<std::ranges::range_reference_t<maybe_const_r>>,
        _CharT>
        underlying_;

public:
    _LIBCPP_HIDE_FROM_ABI constexpr void
    set_separator(std::basic_string_view<_CharT> separator) noexcept
    {
        underlying_.set_separator(separator);
    }
    _LIBCPP_HIDE_FROM_ABI constexpr void set_brackets(
        std::basic_string_view<_CharT> opening_bracket,
        std::basic_string_view<_CharT> closing_bracket) noexcept
    {
        underlying_.set_brackets(opening_bracket, closing_bracket);
    }

    template <class _ParseContext>
    _LIBCPP_HIDE_FROM_ABI constexpr typename _ParseContext::iterator
    parse(_ParseContext& ctx)
    {
        return underlying_.parse(ctx);
    }

    template <class _FormatContext>
    _LIBCPP_HIDE_FROM_ABI typename _FormatContext::iterator
    format(maybe_const_r& range, _FormatContext& ctx) const
    {
        return underlying_.format(range, ctx);
    }
};

template <std::ranges::input_range _Rp, class _CharT>
struct _LIBCPP_TEMPLATE_VIS
    range_default_formatter<range_format::map, _Rp, _CharT> {
private:
    using maybe_const_map = fmt_maybe_const<_Rp, _CharT>;
    using element_type =
        std::remove_cvref_t<std::ranges::range_reference_t<maybe_const_map>>;
    range_formatter<element_type, _CharT> underlying_;

public:
    _LIBCPP_HIDE_FROM_ABI constexpr range_default_formatter()
        requires(fmt_pair_like<element_type>)
    {
        underlying_.set_brackets(
            _LIBCPP_STATICALLY_WIDEN(_CharT, "{"),
            _LIBCPP_STATICALLY_WIDEN(_CharT, "}"));
        underlying_.underlying().set_brackets({}, {});
        underlying_.underlying().set_separator(
            _LIBCPP_STATICALLY_WIDEN(_CharT, ": "));
    }

    template <class _ParseContext>
    _LIBCPP_HIDE_FROM_ABI constexpr typename _ParseContext::iterator
    parse(_ParseContext& ctx)
    {
        return underlying_.parse(ctx);
    }

    template <class _FormatContext>
    _LIBCPP_HIDE_FROM_ABI typename _FormatContext::iterator
    format(maybe_const_map& range, _FormatContext& ctx) const
    {
        return underlying_.format(range, ctx);
    }
};

template <std::ranges::input_range _Rp, class _CharT>
struct _LIBCPP_TEMPLATE_VIS
    range_default_formatter<range_format::set, _Rp, _CharT> {
private:
    using maybe_const_set = fmt_maybe_const<_Rp, _CharT>;
    using element_type =
        std::remove_cvref_t<std::ranges::range_reference_t<maybe_const_set>>;
    range_formatter<element_type, _CharT> underlying_;

public:
    _LIBCPP_HIDE_FROM_ABI constexpr range_default_formatter()
    {
        underlying_.set_brackets(
            _LIBCPP_STATICALLY_WIDEN(_CharT, "{"),
            _LIBCPP_STATICALLY_WIDEN(_CharT, "}"));
    }

    template <class _ParseContext>
    _LIBCPP_HIDE_FROM_ABI constexpr typename _ParseContext::iterator
    parse(_ParseContext& ctx)
    {
        return underlying_.parse(ctx);
    }

    template <class _FormatContext>
    _LIBCPP_HIDE_FROM_ABI typename _FormatContext::iterator
    format(maybe_const_set& range, _FormatContext& ctx) const
    {
        return underlying_.format(range, ctx);
    }
};

template <range_format _Kp, std::ranges::input_range _Rp, class _CharT>
    requires(_Kp == range_format::string || _Kp == range_format::debug_string)
struct _LIBCPP_TEMPLATE_VIS range_default_formatter<_Kp, _Rp, _CharT> {
private:
    // This deviates from the Standard, there the exposition only type is
    //   formatter<basic_string<charT>, charT> underlying_;
    // Using a string_view allows the format function to avoid a copy of the
    // input range when it is a contigious range.
    std::formatter<std::basic_string_view<_CharT>, _CharT> underlying_;

public:
    template <class _ParseContext>
    _LIBCPP_HIDE_FROM_ABI constexpr typename _ParseContext::iterator
    parse(_ParseContext& ctx)
    {
        typename _ParseContext::iterator i = underlying_.parse(ctx);
        if constexpr (_Kp == range_format::debug_string)
            underlying_.set_debug_format();
        return i;
    }

    template <class _FormatContext>
    _LIBCPP_HIDE_FROM_ABI typename _FormatContext::iterator format(
        std::
            conditional_t<std::ranges::input_range<const _Rp>, const _Rp&, _Rp&>
                range,
        _FormatContext& ctx) const
    {
        // When the range is contiguous use a basic_string_view instead to avoid
        // a copy of the underlying data. The basic_string_view formatter
        // specialization is the "basic" string formatter in libc++.
        if constexpr (
            std::ranges::contiguous_range<_Rp> && std::ranges::sized_range<_Rp>)
            return underlying_.format(
                std::basic_string_view<_CharT>{
                    std::ranges::data(range),
                    std::ranges::size(range)},
                ctx);
        else
            return underlying_.format(
                std::basic_string<_CharT>{std::from_range, range},
                ctx);
    }
};

} // namespace probfd

template <std::ranges::input_range _Rp, class _CharT>
    requires(
        std::format_kind<_Rp> != std::range_format::disabled &&
        std::formattable<std::ranges::range_reference_t<_Rp>, _CharT>)
struct std::formatter<_Rp, _CharT>
    : probfd::range_default_formatter<format_kind<_Rp>, _Rp, _CharT> {};

#endif

#endif