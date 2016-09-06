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
    value.s = a;
    type = STRING;
}

Cell::Cell (float a) {
    value.f = a;
    type = FLOAT;
}

Type Cell::getType () {
    return type;
}

Data Cell::getVal () {
    return value;
}

std::string Cell::show () {
    switch(type) {
        case INTEGER:
            return std::to_string(value.i);
        case FLOAT:
            return std::to_string(value.f);
        case STRING:
            return value.s;
        case INVALID:
            return std::string();
    }
}

bool Cell::operator== (Cell d) {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i == d.getVal().i;
            case FLOAT:
                return value.f == d.getVal().f;
            case STRING:
                return value.s == d.getVal().s;
            case INVALID:
                return false;
        }
    }

    return false;
}

bool Cell::operator<= (Cell d) {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i < d.getVal().i || value.i == d.getVal().i;
            case FLOAT:
                return value.f < d.getVal().f || value.i == d.getVal().i;
            case STRING:
                return value.s < d.getVal().s || value.i == d.getVal().i;
            case INVALID:
                return false;
       }
    }

    return false;
}

bool Cell::operator> (Cell d) {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i > d.getVal().i;
            case FLOAT:
                return value.f > d.getVal().f;
            case STRING:
                return value.s > d.getVal().s;
            case INVALID:
                return false;
       }
    }

    return false;
}

bool Cell::operator< (Cell d) {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i < d.getVal().i;
            case FLOAT:
                return value.f < d.getVal().f;
            case STRING:
                return value.s < d.getVal().s;
            case INVALID:
                return false;
       }
    }

    return false;
}

bool Cell::operator!= (Cell d) {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i != d.getVal().i;
            case FLOAT:
                return value.f != d.getVal().f;
            case STRING:
                return value.s != d.getVal().s;
            case INVALID:
                return false;
      }
    }

    return true;
}

bool Cell::operator>= (Cell d) {
    if (type == d.getType()) {
        switch (type) {
            case INTEGER:
                return value.i > d.getVal().i || value.i == d.getVal().i;
            case FLOAT:
                return value.f > d.getVal().f || value.i == d.getVal().i;
            case STRING:
                return value.s > d.getVal().s || value.i == d.getVal().i;
            case INVALID:
                return false;
       }
    }

    return false;
}
