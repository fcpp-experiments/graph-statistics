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
    node.velocity() = neighbour_elastic_force(CALL, 300, 0.03) + point_elastic_force(CALL, make_vec(side,side)/2, 0, 0.005);
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

std::string twonodes =
    "1000notes.com	0\n"
    "100500.tv	1";

std::string onearc = "0	1";

int main() {
    std::stringstream ssnodes(twonodes);
    std::stringstream ssarcs(onearc);
    //! @brief The network object type.
    using net_t = component::interactive_graph_simulator<opt>::net;
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
