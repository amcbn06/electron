#pragma once
#include <string>
#include <vector>

#include "Component.hpp"

namespace SaveManager {
    
    void saveCircuit(const std::string& filename, const std::vector<Component>& components, const std::vector<Wire>& wires);

    
    void loadCircuit(const std::string& filename, std::vector<Component>& components, std::vector<Wire>& wires);    
}