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
    plot_t p;
    const std::string file = "input/cnr-2000";
    auto init_v = common::make_tagged_tuple<nodesinput, arcsinput, max_print_len, plotter>(
        file + ".nodes",
        file + ".arcs",
        0,
        &p
    );
    component::batch_graph_simulator<opt<true>>::net network{init_v};
    network.run();
    std::cerr << common::get<aggregator::list<harmonic_centrality>>(p.back()) << std::endl;
    return 0;
}
