#ifndef PROBFD_FOLD_H
#define PROBFD_FOLD_H

#include <algorithm>

namespace probfd::views {

#ifndef __cpp_lib_ranges_fold

#include <iterator>

namespace detail {
template <typename Fp>
class flipped {
    Fp M_f;

public:
    template <typename Tp, typename Up>
        requires std::invocable<Fp&, Up, Tp>
    std::invoke_result_t<Fp&, Up, Tp>
    operator()(Tp&&, Up&&); // not defined
};

template <typename Fp, typename Tp, typename Iter, typename Up>
concept indirectly_binary_left_foldable_impl =
    std::movable<Tp> && std::movable<Up> && std::convertible_to<Tp, Up> &&
    std::invocable<Fp&, Up, std::iter_reference_t<Iter>> &&
    std::assignable_from<
        Up&,
        std::invoke_result_t<Fp&, Up, std::iter_reference_t<Iter>>>;

template <typename Fp, typename Tp, typename Iter>
concept indirectly_binary_left_foldable =
    std::copy_constructible<Fp> && std::indirectly_readable<Iter> &&
    std::invocable<Fp&, Tp, std::iter_reference_t<Iter>> &&
    std::convertible_to<
        std::invoke_result_t<Fp&, Tp, std::iter_reference_t<Iter>>,
        std::decay_t<
            std::invoke_result_t<Fp&, Tp, std::iter_reference_t<Iter>>>> &&
    indirectly_binary_left_foldable_impl<
        Fp,
        Tp,
        Iter,
        std::decay_t<
            std::invoke_result_t<Fp&, Tp, std::iter_reference_t<Iter>>>>;

template <typename Fp, typename Tp, typename Iter>
concept indirectly_binary_right_foldable =
    indirectly_binary_left_foldable<flipped<Fp>, Tp, Iter>;
} // namespace detail

struct fold_left_fn {
    template <
        std::input_iterator I,
        std::sentinel_for<I> S,
        class T = std::iter_value_t<I>,
        detail::indirectly_binary_left_foldable<T, I> F>
    constexpr auto operator()(I first, S last, T init, F f) const
    {
        using U =
            std::decay_t<std::invoke_result_t<F&, T, std::iter_reference_t<I>>>;
        if (first == last) return U(std::move(init));
        U accum = std::invoke(f, std::move(init), *first);
        for (++first; first != last; ++first)
            accum = std::invoke(f, std::move(accum), *first);
        return std::move(accum);
    }

    template <
        std::ranges::input_range R,
        class T = std::ranges::range_value_t<R>,
        detail::indirectly_binary_left_foldable<T, std::ranges::iterator_t<R>>
            F>
    constexpr auto operator()(R&& r, T init, F f) const
    {
        return (*this)(
            std::ranges::begin(r),
            std::ranges::end(r),
            std::move(init),
            std::ref(f));
    }
};

inline constexpr fold_left_fn fold_left;

#else

inline constexpr auto fold_left = std::ranges::fold_left;

#endif

} // namespace probfd

#endif // PROBFD_FOLD_H
