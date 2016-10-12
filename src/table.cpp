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
#include "simple_ra.hpp"

Table::Table (const Schema& s) {
    schema = s;
}

const Schema& Table::getSchema () const {
    return schema;
}

size_t Table::size () const {
    return table.size ();
}

bool Table::is_union_compatible (const Tuple& t) const {
    bool valid = t.size () == schema.size ();

    auto it = t.begin ();
    auto jt = schema.begin ();

    for (; jt != schema.end () && it != t.end (); ++jt, ++it) {
        valid = valid && (it -> getType () == jt -> second);

        if (!valid)
            return false;
    }

    return true;
}

bool Table::is_union_compatible (const Table& t) const {
    return is_union_compatible (t.getSchema ());
}

bool Table::is_union_compatible (const Schema& s) const {
    bool valid = s.size () == schema.size ();

    for (auto it = s.begin (), jt = schema.begin (); jt != schema.end () && it != s.end (); ++jt, ++it) {
        valid = valid && (it -> second == jt -> second);

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

    if (!is_union_compatible (t))
        throw std::runtime_error ("Incompatible Schema");

    table.insert (t.begin (), t.end ());

    return (*this);
}

void Table::operator += (const Tuple& t) {
    if (!is_union_compatible (t))
        throw std::runtime_error ("Incompatible Schema");

    table.insert (t);
}

Table Table::operator - (const Table& t) {
    if (t.size () == 0)
        return (*this);

    if (!is_union_compatible(t))
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
    for (size_t i = 0 ; i < new_names.size () && i < schema.size (); i++)
        schema[i].first = new_names[i];

    return (*this);
}

Table Table::min (const std::string& col) const {
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

    Schema s;
    Tuple t;
    t.push_back (min_elem);
    s.push_back (std::make_pair ("MIN (" + col + ")", min_elem.getType ()));

    Table result (s);
    result += t;

    return result;
}

Table Table::max (const std::string& col) const {
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

    Schema s;
    Tuple t;
    t.push_back (max_elem);
    s.push_back (std::make_pair ("MAX (" + col + ")", max_elem.getType ()));

    Table result (s);
    result += t;

    return result;
}

Table Table::sum (const std::string& col) const {
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

    Schema s;
    Tuple t;
    t.push_back (c);
    s.push_back (std::make_pair ("SUM (" + col + ")", c.getType ()));

    Table result (s);
    result += t;

    return result;

}

Table Table::avg (const std::string& col) const {
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

    Schema s;
    Tuple t;
    t.push_back (c);
    s.push_back (std::make_pair ("AVG (" + col + ")", c.getType ()));

    Table result (s);
    result += t;

    return result;

}

Table Table::aggregate (const Aggregate& a) const {
    if (a.operation == MIN)
        return min (a.col_name);

    else if (a.operation == MAX)
        return max (a.col_name);

    else if (a.operation == SUM)
        return sum (a.col_name);

    else if (a.operation == AVG)
        return avg (a.col_name);

    else if (a.operation == COUNT)
        return count (a.col_name, a.value);

    else
        throw std::runtime_error ("Unexpected and unidentified aggregate operation type.");

}

Table Table::count (const std::string& col, const Cell& c) const {
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

    Schema s;
    Tuple t;
    t.push_back (Cell ((int) count));
    s.push_back (std::make_pair ("COUNT (" + col + ", " + c.show () + ")", INTEGER));

    Table result (s);
    result += t;

    return result;
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
        if (col.second == INTEGER)
            column_lengths.push_back (5 > col.first.length () ? 5 : col.first.length ());

        else if (col.second == FLOAT)
            column_lengths.push_back (12 > col.first.length () ? 12 : col.first.length ());

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
