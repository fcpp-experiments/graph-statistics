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
    graph_statistics(CALL);
}

}

}

using namespace fcpp;

int main() {
    component::batch_graph_simulator<opt<true>>::net network{common::make_tagged_tuple<nodesinput,arcsinput>("input/outstar.nodes", "input/outstar.arcs")};
    //    component::batch_graph_simulator<opt>::net network{common::make_tagged_tuple<nodesinput,arcsinput>("input/nodes.txt", "input/arcs.txt")};
    network.run();
    return 0;
}
