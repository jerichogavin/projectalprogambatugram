#ifndef USER_HPP
#define USER_HPP

#include <string>

class User {
public:
    std::string id;
    std::string name;
    std::string role;
    std::string rfidId;

    User(std::string id, std::string name, std::string role, std::string rfidId)
        : id(id), name(name), role(role), rfidId(rfidId) {}
};

#endif // USER_HPP