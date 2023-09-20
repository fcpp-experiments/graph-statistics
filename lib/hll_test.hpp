// Copyright © 2022 Giorgio Audrito. All Rights Reserved.

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

template <typename O>
O& operator<<(O& o, hll_t const& c) {
    o << "hll(" << c.size() << ")";
    return o;
}

//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {

namespace tags {
    //! @brief HLL-estimated diameter.
    struct diameter {};
    //! @brief Number of nodes.
    struct true_count {};
    //! @brief Number of nodes through HLL gossiping.
    struct hll_count {};
    //! @brief Number of nodes through MP collection.
    struct mpc_count {};
    //! @brief Number of nodes through WMP collection.
    struct wmp_count {};
    //! @brief Degree centrality.
    struct degree_centrality {};
    //! @brief PageRank centrality.
    struct pagerank_centrality {};
    //! @brief Closeness centrality.
    struct closeness_centrality {};
    //! @brief Harmonic centrality.
    struct harmonic_centrality {};
    //! @brief Distance of the current node.
    struct my_distance {};
    //! @brief Color representing the distance of the current node.
    struct distance_c {};
    //! @brief Color representing the centrality estimate.
    struct centrality_c {};
    //! @brief Size of the current node.
    struct node_size {};
    //! @brief Shape of the current node.
    struct node_shape {};
}


// NETWORK SIZE COUNTING TEST

FUN real_t mpc_size_count(ARGS) { CODE
    device_t source_id = 0;
    bool is_source = node.uid == source_id;
    real_t dist = abf_hops(CALL, is_source);
    real_t count = mp_collection(CALL, dist, real_t(1), real_t(0), [](real_t x, real_t y){
        return x+y;
    }, [](real_t x, size_t n){
        return x/n;
    });
    return broadcast(CALL, dist, count);
}
FUN_EXPORT mpc_size_count_t = export_list<abf_hops_t, mp_collection_t<real_t, real_t>, broadcast_t<real_t,real_t>>;

FUN real_t wmp_size_count(ARGS) { CODE
    device_t source_id = 0;
    bool is_source = node.uid == source_id;
    node.storage(tags::node_size{}) = is_source ? 20 : 10;
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
FUN_EXPORT wmp_size_count_t = export_list<bis_distance_t, wmp_collection_t<real_t>, broadcast_t<real_t,real_t>>;

FUN real_t hll_size_count(ARGS) { CODE
    return nbr(CALL, hll_t(node.uid), [&](field<hll_t> x){
        hll_t c(self(CALL, x));
        fold_hood(CALL, [&](hll_t const& d, nullptr_t){
            c.insert(d);
            return nullptr;
        }, x, nullptr);
        return c;
    }).size();
}
FUN_EXPORT hll_size_count_t = export_list<hll_t>;

FUN void size_counting_test(ARGS) { CODE
    node.storage(tags::true_count{}) = devices;
    node.storage(tags::mpc_count{}) = mpc_size_count(CALL);
    node.storage(tags::wmp_count{}) = wmp_size_count(CALL);
    node.storage(tags::hll_count{}) = hll_size_count(CALL);
}
FUN_EXPORT size_counting_test_t = export_list<mpc_size_count_t, wmp_size_count_t, hll_size_count_t>;


// NETWORK CENTRALITY TEST

FUN real_t degree(ARGS) { CODE
    return node.size()-1;
}
FUN_EXPORT degree_t = export_list<>;

FUN real_t page_rank(ARGS) { CODE
    return nbr(CALL, real_t(1), [&](field<real_t> nr){
        return sum_hood(CALL, real_t(0.85) * nr / nbr(CALL, max(degree(CALL), real_t(1))), real_t(0.15));
    });
}
FUN_EXPORT page_rank_t = export_list<real_t, degree_t>;

FUN tuple<real_t, real_t, bool> hyperANF(ARGS) { CODE
    tuple<real_t, real_t, bool> t{0, 0, true};
    real_t& h = get<0>(t); // harmonic counts
    real_t& c = get<1>(t); // closeness counts
    bool&   u = get<2>(t); // results are unchanged?
    hll_t l(node.uid); // running HLL counter
    real_t ps = 0; // size at the previous depth
    real_t cs = l.size(); // size at the current depth
    LOOP(depth, 1);
    for (; depth < 10 or ps < cs; ++depth) {
        ps = cs;
        field<hll_t> nl = nbr(CALL, hll_t{}, l);
        u = self(CALL, nl) == l and u;
        fold_hood(CALL, [&](hll_t const& d, nullptr_t){
            hll_t ol(l);
            l.insert(d);
            return nullptr;
        }, std::move(nl), nullptr);
        cs = l.size();
        h += (cs-ps)/depth;
        c += (cs-ps)*depth;
    }
    node.storage(tags::diameter{}) = depth-1;
    u = old(CALL, hll_t{}, l) == l and u;
    return t;
}
FUN_EXPORT hyperANF_t = export_list<hll_t>;

FUN void centrality_test(ARGS, bool reactive) { CODE
    node.storage(tags::degree_centrality{}) = degree(CALL);
    if (not reactive)
        node.storage(tags::pagerank_centrality{}) = page_rank(CALL);
    real_t h, c;
    bool u;
    fcpp::tie(h, c, u) = hyperANF(CALL);
    if (reactive and u) node.disable_send();
    node.storage(tags::harmonic_centrality{}) = h;
    node.storage(tags::closeness_centrality{}) = 1/c;
    node.storage(tags::centrality_c{}) = color::hsva(h*3.6, 1, 1);
}
FUN_EXPORT centrality_test_t = export_list<degree_t, page_rank_t, hyperANF_t, hll_t>;


// MOVEMENT PATTERNS

FUN void disperser(ARGS) { CODE
    node.velocity() = neighbour_elastic_force(CALL, 300, 0.03) + point_elastic_force(CALL, make_vec(side,side)/2, 0, 0.005);
}
FUN_EXPORT disperser_t = export_list<neighbour_elastic_force_t, point_elastic_force_t>;

FUN void walker(ARGS) { CODE
    if (node.uid == 0)
        node.position() = make_vec(side,side)/2;
    else
        rectangle_walk(CALL, make_vec(0,0), make_vec(side,side), comm/5, 1);
}
FUN_EXPORT walker_t = export_list<rectangle_walk_t<2>>;


//! @brief Declaration of the main program.
struct main;
template <bool reactive>
FUN_EXPORT main_t = std::conditional_t<
    reactive,
    centrality_test_t,
    export_list<size_counting_test_t, centrality_test_t, disperser_t, walker_t>
>;

} // end coordination

using namespace component::tags;
using namespace coordination::tags;

template <bool sync, bool reactive>
using round_s = std::conditional_t<
    sync,
    std::conditional_t<
        reactive,
        sequence::never,
        sequence::periodic_n<1, 1, 1, endtime+1>
    >,
    sequence::periodic<
        distribution::interval_n<times_t, 0, 1>,
        distribution::weibull_n<times_t, 10, 1, 10>,
        distribution::constant_n<times_t, endtime+2>
    >
>;

template <bool reactive>
using log_s = std::conditional_t<
    reactive,
    sequence::periodic_n<1, 1, 2, 2>,
    sequence::periodic_n<1, 0, 1, endtime>
>;

using rectangle_d = distribution::rect_n<1, 0, 0, side, side>;
constexpr size_t dim = 2;

template <typename T>
using count_aggregator = aggregator::combine<aggregator::min<T>, aggregator::mean<T>, aggregator::max<T>>;

template <typename T>
using centrality_aggregator = aggregator::combine<aggregator::min<T>, aggregator::mean<T>, aggregator::max<T>, aggregator::list<T>>;

using plot_t = plot::last_rows<
    aggregators<
        plot::time,                             times_t,
        aggregator::list<harmonic_centrality>,  std::vector<double>,
        aggregator::list<closeness_centrality>, std::vector<double>
    >, void, 1
>;

template <bool sync, bool reactive>
DECLARE_OPTIONS(opt,
    parallel<true>,
    synchronised<sync>,
    program<coordination::main>,
    round_schedule<round_s<sync, reactive>>,
    dimension<dim>,
    exports<coordination::main_t<reactive>>,
    retain<std::conditional_t<
        reactive,
        metric::always,
        metric::retain<3>
    >>,
    reactive_delay<std::conditional_t<reactive, distribution::constant_n<times_t, 1, 1024>, sequence::never>>,
    send_delay<distribution::constant_n<times_t, 1, 1024>>,
    log_schedule<log_s<reactive>>,
    node_attributes<
        url,                std::string,
        uid,                device_t
    >,
    tuple_store<
        url,                    std::string,
        diameter,               size_t,
        true_count,             double,
        hll_count,          	double,
        mpc_count,              double,
        wmp_count,              double,
        degree_centrality,      double,
        pagerank_centrality,    double,
        closeness_centrality,   double,
        harmonic_centrality,    double,
        my_distance,            double,
        distance_c,             color,
        centrality_c,           color,
        node_shape,         	shape,
        node_size,          	double
    >,
    aggregators<
        diameter,               count_aggregator<double>,
        true_count,             aggregator::mean<double>,
        hll_count,              count_aggregator<double>,
        mpc_count,              count_aggregator<double>,
        wmp_count,              count_aggregator<double>,
        degree_centrality,      centrality_aggregator<double>,
        pagerank_centrality,    centrality_aggregator<double>,
        closeness_centrality,   centrality_aggregator<double>,
        harmonic_centrality,    centrality_aggregator<double>
    >,
    plot_type<plot_t>,
    spawn_schedule<sequence::multiple_n<devices, 0>>,
    init<x, rectangle_d>,
    connector<connect::fixed<comm, 1, dim>>,
    symmetric<false>,
    shape_tag<node_shape>,
    size_tag<node_size>,
    color_tag<distance_c,centrality_c>
);


}
