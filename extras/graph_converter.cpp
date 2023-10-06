#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <string>
#include <vector>

//using fin_tuple_type = common::tagged_tuple_t<>

void parserow(const std::string &srow, std::vector<int> &vrow) {
    unsigned int idx;
    std::istringstream istrrow(srow);

    while (istrrow >> idx) {
        vrow.push_back(idx);
    }

    return;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "graph_converter <file>" << std::endl;
        return 1;
    }

    std::string finname(std::string(argv[1]) + ".graph-txt");
    std::string flabsname(std::string(argv[1]) + ".urls");
    std::string fnodesname(std::string(argv[1]) + ".nodes");
    std::string farcsname(std::string(argv[1]) + ".arcs");

    bool labels = true;

    std::ifstream fin(finname);
    if (!fin) {
        std::cout << "could not open input graph " << finname << std::endl;
        return 1;
    }
    std::ifstream flabs(flabsname);
    if (!flabs) {
        labels = false;
        std::cout << "conversion with dummy labels " << std::endl;
    }

    std::ofstream fnodes(fnodesname);
    if (!fnodes) {
        std::cout << "could not open output graph nodes " << fnodesname << std::endl;
        return 1;
    }
    std::ofstream farcs(farcsname);
    if (!farcs) {
        std::cout << "could not open output graph nodes " << farcsname << std::endl;
        return 1;
    }

    unsigned int idxnode = 0;
    std::string srow;
    std::string slab;
    unsigned int numnodes;

    // first row is number of nodes
    getline(fin,srow);
    numnodes = atoi(srow.c_str());

    while (getline(fin,srow)) {
        // write node
        slab = "dummy";
        if (labels) {
            getline(flabs,slab);
            // beware of labels with spaces
            size_t found = slab.find(' ');
            if (found!=std::string::npos) {
                slab = slab.substr(0,found);
            }
        }
        fnodes << slab << "\t";
        fnodes << idxnode+1;
        fnodes << std::endl;

        // write arcs
        std::vector<int> vrow;
        parserow(srow, vrow);
        for (auto succ : vrow) {
            farcs << idxnode+1 << "\t" << succ+1 << std::endl;
        }

        idxnode++;
    }


    return 0;
}
