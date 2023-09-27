// Copyright © 2023 Giorgio Audrito. All Rights Reserved.

#include <algorithm>

#include "lib/hll_test.hpp"


//! @brief Namespace containing all the objects in the FCPP library.
namespace fcpp {
//! @brief Namespace containing the libraries of coordination routines.
namespace coordination {
    //! @brief Main function.
    MAIN() {
        centrality_test(CALL, true);
    }
}
}

using namespace fcpp;

int main(int argc, char *argv[]) {
    // The name of files containing the network information.
    const std::string file = "input/" + std::string(argc > 1 ? argv[1] : "test");
    // The network object type.
    using net_t = component::batch_graph_simulator<opt<true,true>>::net;
    // The initialisation values.
    auto init_v = common::make_tagged_tuple<nodesinput, arcsinput, max_print_len, epsilon>(
        file + ".nodes",
        file + ".arcs",
        0,
        0
    );
    // Construct the network object.
    net_t network{init_v};
    // Run the simulation until exit.
    network.run();
    // Check deviation of computed centralities with given output file.
    std::cerr << common::get<aggregator::list<all_centralities>>(network.aggregator_tuple()) << std::endl;
    auto hv = common::get<aggregator::list<harmonic_centrality>>(network.aggregator_tuple());
    auto cv = common::get<aggregator::list<closeness_centrality>>(network.aggregator_tuple());
    std::sort(hv.begin(), hv.end());
    std::sort(cv.begin(), cv.end());
    assert(hv.size() == cv.size());
    std::ifstream out(file + ".out");
    size_t i = 0;
    double h, c, dh, dc, sdh = 0, sdc = 0, qdh = 0, qdc = 0, mdh = 0, mdc = 0, ndh = 0, ndc = 0, sh = 0, sc = 0, sfh = 0, sfc = 0;
    std::vector<double> hr, cr;
    while (out >> h >> c) {
        hr.push_back(h);
        cr.push_back(c);
    }
    assert(out.eof());
    assert(hr.size() == hv.size());
    std::sort(hr.begin(), hr.end());
    std::sort(cr.begin(), cr.end());
    for (int i=0; i<hr.size(); ++i) {
        h = hr[i];
        c = cr[i];
        if (hv.size() < 20) {
            std::cout << "* " << i << ": " << hv[i] << " vs " << h << " (harmonic) - " << cv[i] << " vs " << c << " (closeness)\n";
        }
        dh = abs(h - hv[i]);
        dc = abs(c - cv[i]);
        sfh += h - hv[i];
        sfc += c - cv[i];
        sh += (h + hv[i])/2;
        sc += (c + cv[i])/2;
        sdh += dh;
        sdc += dc;
        qdh += dh*dh;
        qdc += dc*dc;
        mdh = max(mdh, dh);
        mdc = max(mdc, dc);
        ndh = max(max(h/hv[i], hv[i]/h)-1, ndh);
        ndc = max(max(c/cv[i], cv[i]/c)-1, ndc);
    }
    std::cout << "Maximum absolute error: " << mdh << " (harmonic) - " << mdc << " (closeness)\n";
    std::cout << "Maximum relative error: " << ndh << " (harmonic) - " << ndc << " (closeness)\n";
    std::cout << "Average absolute error: " << sdh/hv.size() << " (harmonic) - " << sdc/hv.size() << " (closeness)\n";
    std::cout << "Average relative error: " << sdh/sh << " (harmonic) - " << sdc/sc << " (closeness)\n";
    std::cout << "Relative deviation: " << sqrt((qdh*hv.size() - sfh*sfh)/sh/sh) << " (harmonic) - " << sqrt((qdc*hv.size() - sfc*sfc)/sc/sc) << " (closeness)\n";
    return 0;
}
