#pragma once

#include <string>

class VcsObject {
protected:
    std::string oid;
public:
    virtual ~VcsObject() = default;
    
    virtual std::string serialize() const = 0;
    virtual std::string get_type() const = 0;

    std::string calculate_oid();
    std::string get_oid() const;
};

class Blob : public VcsObject {

};