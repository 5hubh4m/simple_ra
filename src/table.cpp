#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>
#include <set>

#include "table.hpp"
#include "expression.hpp"

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

bool Table::is_valid (Tuple t) {
    bool valid = t.size () == schema.size ();

    for (size_t i = 0; i < std::min (t.size (), schema.size ()); i++) {
        valid = valid && (t[i].getType () == schema[i].second);

        if (!valid)
            return false;
    }

    return true;
}


int Table::is_in (Tuple t) {
    if (!is_valid (t))
        throw std::runtime_error ("Incompatible Schema");

    for (size_t i = 0; i < table.size (); i++) {
        bool equal = true;
        for (size_t j = 0; j < t.size (); j++) {
            equal = equal && (t[j] == table[i][j]);

            if (!equal)
                return -1;
        }
        if (equal)
            return i;
    }

    return -1;
}

Tuple Table::operator[] (size_t idx) {
    if (idx >= table.size ())
        throw std::runtime_error ("Index size too big");

    return table[idx];
}

Table Table::operator+ (Table& t) {
    if (t.size () == 0)
        return (*this);

    if (!is_valid (t[0]))
        throw std::runtime_error ("Incompatible Schema");

    for (size_t i = 0; i < t.size (); i++) {
        if (is_in (t[i]) == -1) {
            table.push_back (t[i]);
        }
    }

    return (*this);
}

void Table::operator+= (Tuple& t) {
    if (!is_valid (t))
        throw std::runtime_error ("Incompatible Schema");

    table.push_back (t);
}

Table Table::operator- (Table& t) {
    if (t.size () == 0)
        return (*this);

    if (!is_valid(t[0]))
        throw std::runtime_error ("Incompatible Schema");

    size_t len = t[0].size ();

    for (size_t j = 0; j < t.size (); j++) {
        for (auto it = table.begin(); it != table.end ();) {
            bool equal = true;
            for (size_t k = 0; k < len; k++) {
                equal = equal && (t[j][k] == it -> operator[] (k));

                if (!equal)
                    break;
            }

            if (equal) {
                table.erase (it);
                break;
            } else {
                ++it;
            }
        }
    }

    return (*this);
}

Table Table::operator^ (Table& t) {
    if (t.size () == 0) {
        table.clear ();
    }
    else {
        //TODO: Implement this!
    }

    return (*this);
}

Table Table::operator| (Table& t) {
    if (t.size () == 0) {
        table.clear ();
    }
    else {
        //TODO: Implement this!
    }

    return (*this);
}

Table Table::operator* (Table& t) {
    if (t.size () == 0) {
        table.clear ();
        schema.clear ();
    }
    else {
        for (auto& a : schema) {
            for (auto& b : t.getSchema ()) {
                if (a.first == b.first)
                    throw std::runtime_error ("Invalid operation on tables. Column name " + a.first + " is same!");
            }
        }

        for (auto& s : t.getSchema ()) {
            schema.push_back (s);
        }

        Tuple ins, temp, tup;
        for (size_t j = 0; j <  table.size ();) {
            tup = table[j];

            ins = t[0];

            table[j].insert (table[j].end (), ins.begin (), ins.end ());

#ifdef DEBUG
            for (auto& a : table[j])
                std::cout << a.show () << " ";
            std::cout << std::endl;
#endif

            j++;

            for (size_t i = 1; i < t.size(); i++) {
                temp = tup;
                ins = t[i];

#ifdef DEBUG
                for (auto& a : temp)
                    std::cout << a.show () << " ";
                std::cout << std::endl;
#endif

                temp.insert (temp.end (), ins.begin (), ins.end ());

#ifdef DEBUG
                for (auto& a : temp)
                    std::cout << a.show () << " ";
                std::cout << std::endl;
#endif

                table.insert (table.begin () + j, temp);

#ifdef DEBUG
                std::cout << "Insert successful." << std::endl;
#endif

                j++;

#ifdef DEBUG
                std::cout << "Incremented iterator." << std::endl;
#endif
            }
        }
    }

    return (*this);
}

Table Table::select (Predicate& p) {
    for (auto it = table. begin(); it != table.end ();) {
        if (!p.eval (schema, *it)) {
            table.erase (it);
        } else {
            ++it;
        }
    }

    return (*this);
}

Table Table::project (std::vector< std::string >& col_names) {
    std::vector< size_t > keep_indices;

#ifdef DEBUG
    std::cout << "Current schema: ";
    for (auto& a: schema) {
        std::cout << a.first << " ";
    } std::cout << std::endl;

    std::cout << "Columns to project: ";
    for (auto& a: col_names) {
        std::cout << a << " ";
    } std::cout << std::endl;

#endif

    for (size_t i = 0; i < schema.size (); i++) {
        for (auto& b : col_names) {
            if (b == schema[i].first) {
                keep_indices.push_back (i);
            }
        }
    }

    for (size_t i = 0; i < keep_indices.size(); i++) {
        for (size_t j = i + 1; j < keep_indices.size(); j++) {
            if (keep_indices[i] == keep_indices[j]) {
                throw std::runtime_error ("Invalid operation. Can't project same column twice!");
            }
        }
    }

#ifdef DEBUG
    for (auto a : keep_indices) {
        std::cout << a << " ";
    }
    std::cout << std::endl;
#endif

    if (keep_indices.empty ()) {
        schema.clear ();
        table.clear ();
    }
    else {
        size_t key = 0, len = schema.size ();
        for (size_t i = 0; i < len; i++) {
            if (std::find (keep_indices.begin(), keep_indices.end(), i) == keep_indices.end ()) {
                schema.erase (schema.begin () + i - key++);
            }
        }

        for (auto& a : table) {
            size_t key = 0;
            for (size_t i = 0; i < len; i++) {
                if (std::find (keep_indices.begin(), keep_indices.end(), i) == keep_indices.end ()) {
                    a.erase (a.begin () + i - key++);
                }
            }
        }
    }

    return (*this);
}

Table Table::rename (std::vector< std::string >& new_names) {
    if (new_names.size () <= schema.size ()) {
        for (size_t i = 0 ; i < new_names.size (); i++) {
            schema[i].first = new_names[i];
        }
    } else {
        throw std::runtime_error ("Invalid number of columns for renaming.");
    }

    return (*this);
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
