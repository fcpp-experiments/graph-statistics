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
    node.storage(tags::wmp_count{}) = wmp_size_count(CALL);
    disperser(CALL);
}

}

}

using namespace fcpp;

std::string twonodes =
    "1000notes.com	0\n"
    "100500.tv	1";

std::string onearc = "0	1";

int main() {
    std::stringstream ssnodes(twonodes);
    std::stringstream ssarcs(onearc);
    //! @brief The network object type.
    using net_t = component::interactive_graph_simulator<opt<true>>::net;
    //! @brief The initialisation values.
    auto init_v = common::make_tagged_tuple<name, nodesinput, arcsinput>(
        "HyperLogLog Evaluation",
        "input/nodes.txt",
        "input/arcs.txt"
    );
    //! @brief Construct the network object.
    net_t network{init_v};
    //! @brief Run the simulation until exit.
    network.run();
    return 0;
}
