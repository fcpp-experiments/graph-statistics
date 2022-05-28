// Copyright © 2022 Giorgio Audrito. All Rights Reserved.

#include "lib/hll_test.hpp"


//! @brief Namespace containing all the objects in the FCPP library.
namespace fcpp {
//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {
    //! @brief Main function.
    MAIN() {
        walker(CALL);
        size_counting_test(CALL);
        centrality_test(CALL, false);
    }
}
}

using namespace fcpp;

int main() {
    // The network object type.
    using net_t = component::interactive_simulator<opt<false,false>>::net;
    // The initialisation values.
    auto init_v = common::make_tagged_tuple<name, epsilon, area_min, area_max>(
        "HyperLogLog Evaluation (physical connection)",
        0.1,
        make_vec(0,0),
        make_vec(side,side)
    );
    // Construct the network object.
    net_t network{init_v};
    // Run the simulation until exit.
    network.run();
    return 0;
}
