// Copyright © 2022 Giorgio Audrito. All Rights Reserved.

#include "lib/hll_test.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {

//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {

//! @brief Main function.
MAIN() {
    if (node.uid == 0)
        node.position() = make_vec(side,side)/2;
    else
        rectangle_walk(CALL, make_vec(0,0), make_vec(side,side), comm/5, 1);
    node.storage(tags::true_count{}) = devices;
    node.storage(tags::hll_count{}) = hll_size_count(CALL);
    node.storage(tags::wmp_count{}) = wmp_size_count(CALL);
    real_t harmonic_centrality = get<1>(hyperANF(CALL, (2*side+height)/comm, true));
    node.storage(tags::harmonic_centrality{}) = harmonic_centrality;
    node.storage(tags::centrality_c{}) = color::hsva(harmonic_centrality*3.6, 1, 1);
}

}

}

using namespace fcpp;

int main() {
    component::interactive_simulator<opt>::net network{common::make_tagged_tuple<name,epsilon>("HyperLogLog Evaluation", 0.1)};
    network.run();
    return 0;
}
