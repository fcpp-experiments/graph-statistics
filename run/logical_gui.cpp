// Copyright © 2022 Giorgio Audrito. All Rights Reserved.

#include "lib/hll_test.hpp"


//! @brief Namespace containing all the objects in the FCPP library.
namespace fcpp {
//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {
    //! @brief Main function.
    MAIN() {
        disperser(CALL);
        size_counting_test(CALL);
        centrality_test(CALL, false);
    }
}
}

using namespace fcpp;

int main(int argc, char *argv[]) {
    // The name of files containing the network information.
    const std::string file = "input/" + std::string(argc > 1 ? argv[1] : "test");
    // The network object type.
    using net_t = component::interactive_graph_simulator<opt<true,false>>::net;
    // The initialisation values.
    auto init_v = common::make_tagged_tuple<name, nodesinput, arcsinput>(
        "HyperLogLog Evaluation (logical connection)",
        file + ".nodes",
        file + ".arcs"
    );
    // Construct the network object.
    net_t network{init_v};
    // Run the simulation until exit.
    network.run();
    return 0;
}
