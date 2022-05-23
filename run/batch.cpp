// Copyright © 2021 Giorgio Audrito. All Rights Reserved.

#include "lib/hll_test.hpp"


/**
 * @brief Namespace containing all the objects in the FCPP library.
 */
namespace fcpp {

//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {

//! @brief Main function.
MAIN() {
    node.storage(tags::true_count{}) = devices;
    node.storage(tags::hll_count{}) = hll_size_count(CALL);
    real_t centrality = get<1>(hyperANF(CALL, estimated_diameter, true));
    node.storage(tags::centrality{}) = centrality;
    node.storage(tags::centrality_c{}) = color::hsva(centrality*3.6, 1, 1);
}

}

}

using namespace fcpp;

int main() {
    component::batch_graph_simulator<opt>::net network{common::make_tagged_tuple<nodesinput,arcsinput>("input/outstar.nodes", "input/outstar.arcs")};
    //    component::batch_graph_simulator<opt>::net network{common::make_tagged_tuple<nodesinput,arcsinput>("input/nodes.txt", "input/arcs.txt")};
    network.run();
    return 0;
}
