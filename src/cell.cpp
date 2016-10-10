#include <string>

#include "cell.hpp"

Cell::Cell () {
    type = INVALID;
}

Cell::Cell (int a) {
    value.i = a;
    type = INTEGER;
}

Cell::Cell (std::string a) {
    size_t i;
    for (i = 0; i < 99 && i < a.length (); i++) {
        value.s[i] = a[i];
    }

    value.s[i] = '\0';
    type = STRING;
}

Cell::Cell (float a) {
    value.f = a;
    type = FLOAT;
}

Type Cell::getType () const {
    return type;
}

Data Cell::getVal () const {
    return value;
}

std::string Cell::show () const {
    switch(type) {
        case INTEGER:
            return std::to_string (value.i);
        case FLOAT:
            return std::to_string (value.f);
        case STRING:
            return std::string (value.s);
        case INVALID:
            return std::string("null");
    }
}

bool Cell::operator == (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i == d.getVal().i;
            case FLOAT:
                return value.f == d.getVal().f;
            case STRING:
                return std::string (value.s) == std::string (d.getVal().s);
            case INVALID:
                return false;
        }
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return false;
}

bool Cell::operator <= (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i < d.getVal().i || value.i == d.getVal().i;
            case FLOAT:
                return value.f < d.getVal().f || value.f == d.getVal().f;
            case STRING:
                return std::string (value.s) < std::string (d.getVal().s) || std::string (value.s) == std::string (d.getVal().s);
            case INVALID:
                return false;
       }
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return false;
}

bool Cell::operator > (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i > d.getVal().i;
            case FLOAT:
                return value.f > d.getVal().f;
            case STRING:
                return std::string (value.s) > std::string (d.getVal().s);
            case INVALID:
                return false;
       }
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return false;
}

bool Cell::operator < (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i < d.getVal().i;
            case FLOAT:
                return value.f < d.getVal().f;
            case STRING:
                return std::string (value.s) < std::string (d.getVal().s);
            case INVALID:
                return false;
       }
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return false;
}

bool Cell::operator != (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i != d.getVal().i;
            case FLOAT:
                return value.f != d.getVal().f;
            case STRING:
                return std::string (value.s) != std::string (d.getVal().s);
            case INVALID:
                return false;
      }
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return true;
}

bool Cell::operator >= (const Cell& d) const {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i > d.getVal().i || value.i == d.getVal().i;
            case FLOAT:
                return value.f > d.getVal().f || value.f == d.getVal().f;
            case STRING:
                return std::string (value.s) > std::string (d.getVal().s) || std::string (value.s) == std::string (d.getVal().s);
            case INVALID:
                return false;
       }
    } else {
        throw std::runtime_error ("Invalid operation. Type mismatch.");
    }

    return false;
}
