// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

#include "lib/fcpp.hpp"

//! @brief Minimum number whose square is at least n.
constexpr size_t discrete_sqrt(size_t n) {
    size_t lo = 0, hi = n, mid = 0;
    while (lo < hi) {
        mid = (lo + hi)/2;
        if (mid*mid < n) lo = mid+1;
        else hi = mid;
    }
    return lo;
}

constexpr size_t devices = 1000;
constexpr size_t comm = 100;
constexpr size_t side = discrete_sqrt(devices * 2000);
constexpr size_t height = 100;
constexpr size_t endtime = 10;
constexpr size_t estimated_diameter = 20;

constexpr float hue_scale = 360.0f/(side+height);


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {

//! @brief HyperLogLog counter type with 128 registers.
using hll_t = hyperloglog_counter<128>;

//! @brief Namespace for all FCPP components.
namespace component {
namespace tags {
    //! @brief URL of a node.
    struct url {};
}
}

//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {

namespace tags {
    //! @brief Number of nodes.
    struct true_count {};
    //! @brief Number of nodes through HLL gossiping.
    struct hll_count {};
    //! @brief Number of nodes through WMP collection.
    struct wmp_count {};
    //! @brief Harmonic centrality.
    struct centrality {};
    //! @brief Distance of the current node.
    struct my_distance {};
    //! @brief Color representing the distance of the current node.
    struct distance_c {};
    //! @brief Color representing the collection estimate.
    struct centrality_c {};
    //! @brief Size of the current node.
    struct size {};
    //! @brief Shape of the current node.
    struct node_shape {};
}

FUN real_t hll_size_count(ARGS) { CODE
    return nbr(CALL, hll_t(), [&](field<hll_t> x){
        hll_t c;
        c.insert(node.uid);
        fold_hood(CALL, [&](hll_t const& d, nullptr_t){
            c.insert(d);
            return nullptr;
        }, x, nullptr);
        return c;
    }).size();
}

FUN real_t wmp_size_count(ARGS) { CODE
    device_t source_id = 0;
    bool is_source = node.uid == source_id;
    node.storage(tags::size{}) = is_source ? 20 : 10;
    node.storage(tags::node_shape{}) = is_source ? shape::cube : shape::sphere;

    real_t dist = bis_distance(CALL, is_source, 1, 0.7*comm);
    node.storage(tags::my_distance{}) = dist;
    node.storage(tags::distance_c{}) = color::hsva(dist*hue_scale, 1, 1);

    real_t count = wmp_collection(CALL, dist, comm, real_t(1), [](real_t x, real_t y){
        return x+y;
    }, [](real_t x, real_t k){
        return x*k;
    });
    return broadcast(CALL, dist, count);
}

FUN tuple<hll_t, real_t> hyperANF(ARGS, size_t depth, bool source) { CODE
    if (depth == 0) {
        hll_t c;
        c.insert(node.uid);
        return {c, 0};
    } else {
        hll_t rc;
        real_t rr;
        fcpp::tie(rc, rr) = hyperANF(CALL, depth-1, source);
        hll_t nc;
        fold_hood(CALL, [&](hll_t const& d, nullptr_t){
            nc.insert(d);
            return nullptr;
        }, nbr(CALL, rc), nullptr);
        return {nc, rr + (nc.size() - rc.size())/depth};
    }
}

//! @brief Declaration of the main program.
struct main;

} // end coordination

using namespace component::tags;
using namespace coordination::tags;

using round_s = sequence::periodic<
    distribution::interval_n<times_t, 0, 1>,
    distribution::weibull_n<times_t, 10, 1, 10>,
    distribution::constant_n<times_t, endtime+1>
>;

using rectangle_d = distribution::rect_n<1, 0, 0, side, side>;
constexpr size_t dim = 2;

template <typename T>
using multi_aggregator = aggregator::combine<aggregator::min<T>, aggregator::mean<T>, aggregator::max<T>>;

DECLARE_OPTIONS(opt,
    parallel<false>,
    synchronised<false>,
    program<coordination::main>,
    round_schedule<round_s>,
    dimension<dim>,
    exports<vec<dim>, real_t, field<real_t>, tuple<real_t, times_t>, hll_t>,
    log_schedule<sequence::periodic_n<1, 0, 1, endtime>>,
    node_attributes<
                //        url,                std::string,
        uid,                device_t
    >,
    tuple_store<
        url,                std::string,
        true_count,         double,
        hll_count,          double,
        wmp_count,          double,
        centrality,         double,
        my_distance,        double,
        distance_c,         color,
        centrality_c,       color,
        node_shape,         shape,
        size,               double
    >,
    aggregators<
        true_count,         aggregator::mean<double>,
        hll_count,          multi_aggregator<double>,
        wmp_count,          multi_aggregator<double>,
                //        centrality,         multi_aggregator<double>
        centrality,         aggregator::list<double>
    >,
    spawn_schedule<sequence::multiple_n<devices, 0>>,
    init<x, rectangle_d>,
    connector<connect::fixed<comm, 1, dim>>,
    symmetric<false>,
    shape_tag<node_shape>,
    size_tag<size>,
    color_tag<distance_c,centrality_c>
);


}
