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
    walker(CALL);
}

}

}

using namespace fcpp;

int main() {
    component::interactive_simulator<opt<false>>::net network{common::make_tagged_tuple<name,epsilon>("HyperLogLog Evaluation", 0.1)};
    network.run();
    return 0;
}
