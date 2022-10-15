#ifndef __RANGE_PROXY_H__
#define __RANGE_PROXY_H__

namespace utils {

template <typename IteratorT, typename SentinelT = IteratorT>
struct RangeProxy {
    IteratorT b;
    SentinelT e;

    RangeProxy() = default;

    RangeProxy(IteratorT b, SentinelT e)
        : b(b)
        , e(e)
    {
    }

    auto begin() { return b; }
    auto end() { return e; }
};

template <typename IteratorT, typename SentinelT = IteratorT>
auto make_range(IteratorT b, SentinelT e)
{
    return RangeProxy<IteratorT, SentinelT>(b, e);
}

} // namespace utils

#endif // __RANGE_PROXY_H__