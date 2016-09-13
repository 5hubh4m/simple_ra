#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>

#include "table.hpp"

Table::Table (Schema s) {
    schema = s;
}

Schema Table::getSchema () {
    return schema;
}

TableArray Table::getTable () {
    return table;
}

size_t Table::size () {
    return table.size ();
}

Tuple Table::operator[] (size_t idx) {
    return table[idx];
}

void Table::operator+= (Tuple t) {
    bool valid = t.size () == schema.size ();

    for (size_t i = 0; i < t.size (); i++) {
        if (t[i].getType () != schema[i].second) {
            valid = false;
            break;
        }
    }

    if (valid) {
        auto it = table.begin ();
        for (; it != table.end (); ++it) {
            bool equal = true;
            for (size_t i = 0; i < t.size (); i++) {
                if (it -> operator[] (i) != t[i]) {
                    equal = false;
                    break;
                }
            }

            if (equal) {
                return;
            }
        }

        table.push_back (t);
    } else {
        throw std::runtime_error ("Incompatible Schema");
    }
}

void Table::operator-= (Tuple t) {
    bool valid = t.size () == schema.size ();

    for (size_t i = 0; i < t.size (); i++) {
        if (t[i].getType () != schema[i].second) {
            valid = false;
            break;
        }
    }

    if (valid) {
        auto it = table.begin ();
        for (; it != table.end (); ++it) {
            bool equal = true;
            for (size_t i = 0; i < t.size (); i++) {
                if (it->operator[](i) != t[i]) {
                    equal = false;
                    break;
                }
            }

            if (equal) {
                table.erase (it);
            }
        }
    } else {
        throw std::runtime_error ("Incompatible Schema");
    }
}

void Table::rename (std::vector< std::string > new_names) {
    if (new_names.size () <= schema.size ()) {
        for (size_t i = 0 ; i < new_names.size (); i++) {
            schema[i].first = new_names[i];
        }
    } else {
        throw std::runtime_error ("Incompatible Schema.");
    }
}

void Table::print (void) {
    if (table.empty ()) {
        std::cout << "Empty table." << std::endl;
        return;
    }


    for (size_t i = 0; i < schema.size () * 13; i++)
        std::cout << "-";
    std::cout << std::endl;

    std::cout << "|";
    for (auto& a : schema) {
        std::cout << std::setw (10) << a.first << " | ";
    } std::cout << std::endl;

    for (size_t i = 0; i < schema.size () * 13; i++)
        std::cout << "-";
    std::cout << std::endl;


    for (auto& a : table) {
        std::cout << "|";
        for (auto& b : a) {
            std::cout << std::setw (10) << b.show() << " | ";
        } std::cout << std::endl;
    }

    for (size_t i = 0; i < schema.size () * 13; i++)
        std::cout << "-";
    std::cout << std::endl;
}
