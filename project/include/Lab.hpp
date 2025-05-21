#ifndef LAB_HPP
#define LAB_HPP

#include <string>

class Lab {
public:
    std::string id;
    std::string name;
    std::string location;
    int capacity;
    int currentOccupancy;

    Lab(std::string id, std::string name, std::string location, int capacity)
        : id(id), name(name), location(location), capacity(capacity), currentOccupancy(0) {}
};

#endif // LAB_HPP