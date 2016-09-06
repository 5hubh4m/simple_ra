#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>

#include "table.hpp"

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

void Table::print (void) {
    std::vector< std::string > temp;

    for (size_t i = 0; i < schema.size(); i++)
        temp.push_back (std::string (""));

    for (auto a : schema) {
        temp[a.second.first] = a.first;
    }

    for (auto a : temp) {
        std::cout << std::setw (10) << a << " | ";
    } std::cout << std::endl;

    for (size_t i = 0; i < schema.size () * 13; i++)
        std::cout << "-";
    std::cout << std::endl;

    for (auto a : table) {
        for (auto b : a) {
            std::cout << std::setw (10) << b.show() << " | ";
        } std::cout << std::endl;
    }

    for (size_t i = 0; i < schema.size () * 13; i++)
        std::cout << "-";
    std::cout << std::endl;
}
