#include "SaveManager.hpp"

#include <fstream>
#include <iostream>

namespace SaveManager {

    void saveCircuit(const std::string& filename, const std::vector<Component>& components, const std::vector<Wire>& wires) {
        std::string path = "saves/" + filename;
        std::ofstream out(path);
        if (!out.is_open()) {
            std::cerr << "[Error] Failed to create save file: " << filename << std::endl;
            return;
        }

        
        out << components.size() << "\n";
        for (const auto& comp : components) {
            out << comp.type << " " 
                << comp.position.x << " " 
                << comp.position.y << " " 
                << comp.rotation << " " 
                << comp.value << " "
                << comp.scale << " "
                << comp.sters << "\n";
        }

        
        out << wires.size() << "\n";
        for (const auto& wire : wires) {
            out << wire.startComponentIndex << " " 
                << wire.startPinIndex << " " 
                << wire.endComponentIndex << " " 
                << wire.endPinIndex << "\n";
        }

        out.flush();

        std::cout << "[Success] Circuit saved to " << filename << std::endl;
    }

    void loadCircuit(const std::string& filename, std::vector<Component>& components, std::vector<Wire>& wires) {
        std::string path = "saves/" + filename;
        std::ifstream in(path);
        if (!in.is_open()) {
            std::cerr << "[Error] File not found: " << filename << std::endl;
            return;
        }

        
        components.clear();
        wires.clear();

        
        size_t compCount;
        if (!(in >> compCount)) return;

        for (size_t i = 0; i < compCount; ++i) {
            std::string type;
            float x, y, rot, val, scl;
            bool sters;
            
            in >> type >> x >> y >> rot >> val >> scl >> sters;

            
            
            Component c(x, y, type); 
            c.rotation = rot;
            c.value = val;
            c.scale = scl;
            c.sters = sters;
            
            components.push_back(c);
        }

        
        size_t wireCount;
        if (in >> wireCount) {
            for (size_t i = 0; i < wireCount; ++i) {
                int sIdx, sPin, eIdx, ePin;
                in >> sIdx >> sPin >> eIdx >> ePin;
                
                
                if (sIdx < components.size() && eIdx < components.size()) {
                    wires.push_back({sIdx, sPin, eIdx, ePin});
                }
            }
        }

        std::cout << "[Success] Circuit loaded from " << filename << std::endl;
    }
}