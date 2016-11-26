#include <string>

#include "cell.hpp"

using namespace RelationalAlgebra;

const DataType& Cell::getType() const {
    return type;
}

const Data& Cell::getVal() const {
    return value;
}

std::string Cell::show() const {
    switch(type) {
    case DataType::Integer:
        return std::to_string(value.i);

    case DataType::String:
        return std::to_string(value.f);
    
    case DataType::Float:
        return std::string(value.s);
    
    case DataType::Null:
        return std::string("Null");
    }
}

bool Cell::operator == (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
        case DataType::Integer:
            return value.i == d.getVal().i;
        
        case DataType::Float:
            return value.f == d.getVal().f;
        
        case DataType::String:
            return std::string(value.s) == std::string(d.getVal().s);
        
        case DataType::Null:
            return false;
        }
    } else if (type == DataType::Null || d.getType() == DataType::Null) {
        return false;
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return false;
}

bool Cell::operator <= (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
        case DataType::Integer:
            return value.i == d.getVal().i || value.i < d.getVal().i;
        
        case DataType::Float:
            return value.f == d.getVal().f || value.f < d.getVal().f;
        
        case DataType::String:
            return std::string(value.s) == std::string(d.getVal().s) || std::string(value.s) < std::string(d.getVal().s);
        
        case DataType::Null:
            return false;
        }
    } else if (type == DataType::Null || d.getType() == DataType::Null) {
        return false;
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return false;
}

bool Cell::operator > (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
        case DataType::Integer:
            return value.i > d.getVal().i;
        
        case DataType::Float:
            return value.f > d.getVal().f;
        
        case DataType::String:
            return std::string(value.s) > std::string(d.getVal().s);
        
        case DataType::Null:
            return false;
        }
    } else if (type == DataType::Null || d.getType() == DataType::Null) {
        return false;
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return false;
}

bool Cell::operator < (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
        case DataType::Integer:
            return value.i < d.getVal().i;
        
        case DataType::Float:
            return value.f < d.getVal().f;
        
        case DataType::String:
            return std::string(value.s) < std::string(d.getVal().s);
        
        case DataType::Null:
            return false;
        }
    } else if (type == DataType::Null || d.getType() == DataType::Null) {
        return false;
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return false;
}

bool Cell::operator != (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
        case DataType::Integer:
            return value.i != d.getVal().i;
        
        case DataType::Float:
            return value.f != d.getVal().f;
        
        case DataType::String:
            return std::string(value.s) != std::string(d.getVal().s);
        
        case DataType::Null:
            return true;
        }
    } else if (type == DataType::Null || d.getType() == DataType::Null) {
        return true;
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return true;
}

bool Cell::operator >= (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
        case DataType::Integer:
            return value.i == d.getVal().i || value.i > d.getVal().i;
        
        case DataType::Float:
            return value.f == d.getVal().f || value.f > d.getVal().f;
        
        case DataType::String:
            return std::string(value.s) == std::string(d.getVal().s) || std::string(value.s) > std::string(d.getVal().s);
        
        case DataType::Null:
            return false;
        }
    } else if (type == DataType::Null || d.getType() == DataType::Null) {
        return false;
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return false;
}
