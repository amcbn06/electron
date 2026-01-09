#pragma once
#include <string>
#include <vector>

#include "Component.hpp"

namespace SaveManager {
    // Saves the current state of components and wires to a text file
    void saveCircuit(const std::string& filename, const std::vector<Component>& components, const std::vector<Wire>& wires);

    // Clears the current vectors and repopulates them from the file
    void loadCircuit(const std::string& filename, std::vector<Component>& components, std::vector<Wire>& wires);    
}