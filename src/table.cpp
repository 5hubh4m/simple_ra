#include <map>
#include <vector>
#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>

#include "table.hpp"
#include "expression.hpp"

Table::Table (const Schema& s) {
    schema = s;
}

Schema Table::getSchema () const {
    return schema;
}

size_t Table::size () const {
    return table.size ();
}

bool Table::is_valid (const Tuple& t) const {
    bool valid = t.size () == schema.size ();

    size_t i = 0;

    for (auto it = t.begin (); i < schema.size () && it != t.end (); i++, ++it) {
        valid = valid && (it -> getType () == schema[i].second);

        if (!valid)
            return false;
    }

    return true;
}

Tuple Table::operator [] (size_t idx) const {
    if (idx >= table.size ())
        throw std::runtime_error ("Index is too large.");

    auto it = table.begin ();

    while (idx--)
        ++it;

    return *it;
}

Table Table::operator + (const Table& t) {
    if (t.size () == 0)
        return (*this);

    if (!is_valid (t[0]))
        throw std::runtime_error ("Incompatible Schema");

    table.insert (t.begin (), t.end ());

    return (*this);
}

void Table::operator += (const Tuple& t) {
    if (!is_valid (t))
        throw std::runtime_error ("Incompatible Schema");

    table.insert (t);
}

Table Table::operator - (const Table& t) {
    if (t.size () == 0)
        return (*this);

    if (!is_valid(t[0]))
        throw std::runtime_error ("Incompatible Schema");

    for (auto& row : t)
        table.erase (row);

    return (*this);
}

Table Table::operator * (const Table& t) {
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

        Tuple temp;
        TableArray new_table;

        for (auto& row : table) {
            for (auto& row2 : t) {
                temp = row;
                temp.insert (temp.end (), row2.begin (), row2.end ());
                new_table.insert (temp);
            }
        }

        table = new_table;
    }

    return (*this);
}

Table Table::select (const Predicate& p) {
    TableArray new_table;

    for (auto& row : table) {
        if (p.eval (schema, row))
            new_table.insert (row);
    }

    table = new_table;

    return (*this);
}

Table Table::project (const std::vector< std::string >& col_names) {
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

        // Delete the unecessary columns from the table

        Tuple temp;
        TableArray new_table;

        for (auto& row : table) {
            temp = row;

            key = 0;
            for (size_t i = 0; i < len; i++) {
                if (std::find (keep_indices.begin(), keep_indices.end(), i) == keep_indices.end ()) {
                    temp.erase (temp.begin () + i - key++);
                }
            }

            new_table.insert (temp);
        }

        table = new_table;
    }

    return (*this);
}

Table Table::rename (const std::vector< std::string >& new_names) {
    if (new_names.size () <= schema.size ()) {
        for (size_t i = 0 ; i < new_names.size (); i++) {
            schema[i].first = new_names[i];
        }
    } else {
        throw std::runtime_error ("Invalid number of columns for renaming.");
    }

    return (*this);
}


Cell Table::min (const std::string& col) const {
    if (table.empty ())
        throw std::runtime_error ("Empty table.");

    size_t idx = 0;
    for (auto& cols : schema) {
        if (cols.first == col) {
            break;
        }

        idx++;
    }

    Cell min_elem (table.begin () -> operator [] (idx));

    for (auto& rows : table) {
        if (rows[idx] < min_elem)
            min_elem = rows[idx];
    }

    return min_elem;
}

Cell Table::max (const std::string& col) const {
    if (table.empty ())
        throw std::runtime_error ("Empty table.");

    size_t idx = 0;
    for (auto& cols : schema) {
        if (cols.first == col) {
            break;
        }

        idx++;
    }

    Cell max_elem (table.begin () -> operator [] (idx));

    for (auto& rows : table) {
        if (rows[idx] > max_elem)
            max_elem = rows[idx];
    }

    return max_elem;
}

Cell Table::sum (const std::string& col) const {
    if (table.empty ())
        throw std::runtime_error ("Empty table.");

    size_t idx = 0;
    for (auto& cols : schema) {
        if (cols.first == col) {
            break;
        }

        idx++;
    }

    Cell c;

    if (schema[idx].second == STRING)
        throw std::runtime_error ("Invalid operation on data type string.");

    else if (schema[idx].second == INTEGER) {
        c = Cell (0);

        for (auto& rows : table) {
            c = c.getVal ().i + rows[idx].getVal ().i;
        }
    }
    else if (schema[idx].second == FLOAT) {
        c = Cell (0.0f);

        for (auto& rows : table) {
            c = c.getVal ().f + rows[idx].getVal ().f;
        }
    }
    else
        throw std::runtime_error ("Unexpected type of cell.");

    return c;
}

Cell Table::avg (const std::string& col) const {
    if (table.empty ())
        throw std::runtime_error ("Empty table.");

    size_t idx = 0;
    for (auto& cols : schema) {
        if (cols.first == col) {
            break;
        }

        idx++;
    }

    Cell c;

    if (schema[idx].second == STRING)
        throw std::runtime_error ("Invalid operation on data type string.");

    else if (schema[idx].second == INTEGER) {
        c = Cell (0);

        for (auto& rows : table) {
            c = c.getVal ().i + rows[idx].getVal ().i;
        }

        c = Cell ((float)((float)c.getVal ().i / table.size ()));
    }
    else if (schema[idx].second == FLOAT) {
        c = Cell (0.0f);

        for (auto& rows : table) {
            c = c.getVal ().f + rows[idx].getVal ().f;
        }

        c = Cell ((float)(c.getVal ().f / table.size ()));
    }
    else
        throw std::runtime_error ("Unexpected type of cell.");

    return c;
}

Cell Table::count (const std::string& col, const Cell& c) const {
    size_t idx = 0;
    for (auto& cols : schema) {
        if (cols.first == col) {
            break;
        }

        idx++;
    }

    size_t count = 0;

    for (auto& rows : table) {
        if (rows[idx] == c)
            count++;
    }

    return Cell((int)count);
}

void Table::print (void) const {
    if (table.empty ()) {
        std::cout << "Empty table." << std::endl;
        return;
    }

    std::vector< size_t > column_lengths;
    size_t temp;

    size_t i = 0;
    for (auto& col : schema) {
        if (col.second == FLOAT || col.second == INTEGER)
            column_lengths.push_back (10 > col.first.length () ? 10 : col.first.length ());

        else {
            size_t len = col.first.length ();

            for (auto& row : table) {
                temp = strlen (row[i].getVal ().s);

                if (temp > len)
                    len = temp;
            }

            column_lengths.push_back (len);
        }

        ++i;
    }

    size_t row_len = 0;
    for (auto& a : column_lengths)
        row_len += a;

    row_len += column_lengths.size () + 1;


    for (size_t i = 0; i < row_len; i++)
        std::cout << "-";
    std::cout << std::endl;

    i = 0;
    std::cout << "|";
    for (auto& a : schema) {
        std::cout << std::setw (column_lengths[i]) << a.first << "|";
        ++i;
    } std::cout << std::endl;

    for (size_t i = 0; i < row_len; i++)
        std::cout << "-";
    std::cout << std::endl;


    for (auto& a : table) {
        i = 0;

        std::cout << "|";
        for (auto& b : a) {
            std::cout << std::setw (column_lengths[i]) << b.show() << "|";
            ++i;
        } std::cout << std::endl;
    }

    for (size_t i = 0; i < row_len; i++)
        std::cout << "-";
    std::cout << std::endl;
}
