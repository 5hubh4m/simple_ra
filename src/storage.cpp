#include <map>
#include <string>
#include <fstream>
#include <cstring>

#include "table.hpp"
#include "storage.hpp"
#include "simple_ra.hpp"

Database::Database () {
    std::ifstream db_file (DATA + std::string (".") + SCHEMA, std::ios::in | std::ios::binary);

    if (db_file.is_open ()) {
        size_t len;
        char temp[100];
        db_file.read ((char*)&len, sizeof (size_t));

#ifdef DEBUG
        std::cout << "Read " << len << std::endl;
#endif

        for (size_t i = 0; i < len; i++) {
            db_file.read ((char*)temp, sizeof (char) * 100);

#ifdef DEBUG
            std::cout << "Read " << temp << std::endl;
#endif

            table_list.push_back (std::string(temp));
        }
    }
}

Table Database::operator[] (const std::string& s) const {
    auto it = std::find (table_list.begin (), table_list.end (), s);

    if (it == table_list.end ())
        throw std::runtime_error ("Table " + s + " does not exist.");

    std::ifstream table_file (DATA + std::string (".") + s, std::ios::in | std::ios::binary);

    if (!table_file.is_open ())
        throw std::runtime_error ("Couldn't open table file for reading.");

    Schema sch;
    char str[100];
    size_t i, rows;
    Type typ;
    Cell c;
    Tuple t;

    table_file.read ((char*)&i, sizeof(size_t));

#ifdef DEBUG
    std::cout << "Read " << i << std::endl;
#endif

    table_file.read ((char*)&rows, sizeof(size_t));

#ifdef DEBUG
    std::cout << "Read " << rows << std::endl;
#endif

    for (size_t j = 0; j < i; j++) {
        table_file.read ((char*)str, sizeof(char) * 100);

#ifdef DEBUG
        std::cout << "Read " << str << std::endl;
#endif

        table_file.read ((char*)&typ, sizeof(Type));

#ifdef DEBUG
        std::cout << "Read " << typ << std::endl;
#endif

        sch.push_back (std::make_pair (std::string (str), typ));
    }

    Table result (sch);

    for (size_t k = 0; k < rows; k++) {
        t.clear ();
        for (size_t j = 0; j < i; j++) {
            table_file.read ((char*)&c, sizeof(Cell));

#ifdef DEBUG
            std::cout << "Read " << c.show () << std::endl;
#endif


            t.push_back (c);
        }

        result += t;
    }

    table_file.close ();

    return result;
}

void Database::add_table (const std::string& s, const Table& t) {
    auto it = std::find (table_list.begin (), table_list.end (), s);

    if (it == table_list.end ()) {
        table_list.push_back (s);
        std::ofstream db_file (DATA + std::string (".") + SCHEMA, std::ios::out | std::ios::trunc | std::ios::binary);

        size_t len = table_list.size ();

        db_file.write ((char*)&len, sizeof (size_t));

#ifdef DEBUG
        std::cout << "Wrote " << len << std::endl;
#endif

        char temp[100];
        for (auto& a : table_list) {
            size_t k;
            for (k = 0; k < a.size () && k < 99; k++) {
                temp[k] = a[k];
            }
            temp[k] = '\0';

            db_file.write ((char*)temp, sizeof (char) * 100);

#ifdef DEBUG
            std::cout << "Wrote " << temp << std::endl;
#endif
        }
    }

    std::ofstream table_file (DATA + std::string (".") + s, std::ios::out | std::ios::trunc | std::ios::binary);

    if (!table_file.is_open ())
        throw std::runtime_error ("Couldn't open table file for writing.");

    Schema sch = t.getSchema ();
    char str[100];
    size_t i = sch.size (), j = t.size ();
    Type typ;
    Cell c;

    table_file.write ((char*)&i, sizeof (size_t));

#ifdef DEBUG
    std::cout << "Wrote " << i << std::endl;
#endif

    table_file.write ((char*)&j, sizeof (size_t));

#ifdef DEBUG
    std::cout << "Wrote " << j << std::endl;
#endif


    for (size_t j = 0; j < i; j++) {
        size_t k;
        for (k = 0; k < sch[j].first.size () && k < 99; k++) {
            str[k] = sch[j].first[k];
        }
        str[k] = '\0';

        table_file.write ((char*)str, sizeof (char) * 100);

#ifdef DEBUG
        std::cout << "Wrote " << str << std::endl;
#endif

        typ = sch[j].second;

        table_file.write ((char*)&typ, sizeof (Type));

#ifdef DEBUG
        std::cout << "Wrote " << typ << std::endl;
#endif
    }

    for (auto& row : t) {
        for (size_t j = 0; j < sch.size (); j++) {
            c = row[j];
            table_file.write ((char*)&c, sizeof (Cell));

#ifdef DEBUG
            std::cout << "Wrote " << c.show () << std::endl;
#endif
        }
    }

    table_file.close ();
}

void Database::remove (const std::string& s) {
    auto it = std::find (table_list.begin (), table_list.end (), s);

    if (it != table_list.end ()) {
        table_list.erase (it);
        std::ofstream db_file (DATA + std::string (".") + SCHEMA, std::ios::out | std::ios::trunc | std::ios::binary);

        size_t len = table_list.size ();

        db_file.write ((char*)&len, sizeof (size_t));

#ifdef DEBUG
        std::cout << "Wrote " << len << std::endl;
#endif

        char temp[100];
        for (auto& a : table_list) {
            size_t k;
            for (k = 0; k < a.size () && k < 99; k++) {
                temp[k] = a[k];
            }
            temp[k] = '\0';

            db_file.write ((char*)temp, sizeof (char) * 100);

#ifdef DEBUG
            std::cout << "Wrote " << temp << std::endl;
#endif
        }
    }

    std::remove ((DATA + std::string (".") + s).c_str ());
}

void Database::info (void) const {
    std::cout << "The following tables are registered:" << std::endl;

    for (auto& a : table_list) {
        std::cout << "    - " << a;

        std::ifstream table_file (DATA + std::string (".") + a, std::ios::in | std::ios::binary);

        if (!table_file.is_open ())
            throw std::runtime_error ("Couldn't open table file for IO.");

        Schema sch;
        char str[100];
        size_t i, rows;
        Type typ;
        Cell c;
        Tuple t;

        table_file.read ((char*)&i, sizeof(size_t));

#ifdef DEBUG
        std::cout << "Read " << i << std::endl;
#endif

        table_file.read ((char*)&rows, sizeof(size_t));

#ifdef DEBUG
        std::cout << "Read " << rows << std::endl;
#endif

        for (size_t j = 0; j < i; j++) {
            table_file.read ((char*)str, sizeof(char) * 100);

#ifdef DEBUG
            std::cout << "Read " << str << std::endl;
#endif

            table_file.read ((char*)&typ, sizeof(Type));

#ifdef DEBUG
            std::cout << "Read " << typ << std::endl;
#endif

            sch.push_back (std::make_pair (std::string (str), typ));
        }

        auto it = sch.begin ();
        std::cout << " (" << it -> first;
        ++it;
        for (; it != sch.end (); ++it) {
            std::cout << ", " << it ->first;
        }
        std::cout << ") : " << rows << " tuples."<< std::endl;
    }
}
