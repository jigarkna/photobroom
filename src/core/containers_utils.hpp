
#ifndef CONTAINERS_UTILS
#define CONTAINERS_UTILS

#include "generic_concepts.hpp"

#ifndef CONCEPTS_SUPPORTED
#define Container typename
#endif

template<Container T>
const typename T::value_type& front(const T& container)
{
    return *container.begin();
}

template<Container T>
const typename T::value_type& back(const T& container)
{
    return *container.rbegin();
}


template<typename MapT,
            typename RemovedT,
            typename ChangedT,
            typename AddedT>
void compare(const MapT& lhs, const MapT& rhs,
                RemovedT rem_inserter,
                ChangedT chg_inserter,
                AddedT add_inserter)
{
    typedef typename MapT::value_type Pair;
    typedef std::tuple<typename MapT::key_type,
                        typename MapT::mapped_type,
                        typename MapT::mapped_type> Changed;

    // find *keys* which exist only in lhs
    std::set_difference(lhs.cbegin(), lhs.cend(),
                        rhs.cbegin(), rhs.cend(),
                        rem_inserter, [](const Pair& l, const Pair& r)
    {
        return l.first < r.first;
    });

    // find *keys* which exist only in rhs
    std::set_difference(rhs.cbegin(), rhs.cend(),
                        lhs.cbegin(), lhs.cend(),
                        add_inserter, [](const Pair& l, const Pair& r)
    {
        return l.first < r.first;
    });

    // find *keys* which exist in both collections
    std::vector<Pair> intersection;
    std::set_intersection(lhs.cbegin(), lhs.cend(),
                            rhs.cbegin(), rhs.cend(),
                            std::back_inserter(intersection), [](const Pair& l, const Pair& r)
    {
        return l.first < r.first;
    });

    for(const Pair& lhs_data: intersection)
    {
        auto it = rhs.find(lhs_data.first);
        assert(it != rhs.cend());

        if (lhs_data.second != it->second)
        {
            Changed changed(lhs_data.first, lhs_data.second, it->second);

            chg_inserter = changed;
            chg_inserter++;
        }
    }

}

#ifndef CONCEPTS_SUPPORTED
#undef Container
#endif

#endif
