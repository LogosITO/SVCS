#include "../include/Blob.hxx"

Blob::Blob(const std::string& raw_data) : data(raw_data) {
    computeHash(serialize());
}

std::string Blob::getType() const {
    return "blob";
}

std::string Blob::serialize() const {
    return data;
}

const std::string& Blob::getData() const {
    return data;
}