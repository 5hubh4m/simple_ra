#include <map>
#include <string>
#include <fstream>
#include <cstring>
#include <algorithm>

#include "table.hpp"
#include "storage.hpp"
#include "parser.hpp"
#include "simple_ra.hpp"

using namespace RelationalAlgebra;

Database::Database() {
    std::ifstream db_file(DATA + std::string(".") + SCHEMA, std::ios::in | std::ios::binary);

    if (db_file.is_open()) {
        size_t len;
        char temp[100];
        db_file.read((char*)&len, sizeof(size_t));

#ifdef DEBUG
        std::cout << "Read " << len << std::endl;
#endif

        for (size_t i = 0; i < len; i++) {
            db_file.read((char*)temp, sizeof(char) * 100);

#ifdef DEBUG
            std::cout << "Read " << temp << std::endl;
#endif

            table_list.push_back(std::string(temp));
        }
    }

    std::ifstream view_file(DATA + std::string(".") + VIEW, std::ios::in | std::ios::binary);

    size_t n = 0;
    char s[100], exp[1000];

    view_file.read((char*) &n, sizeof(size_t));

    for (size_t i = 0; i < n; i++) {
        view_file.read(s, sizeof(char) * 100);
        view_file.read(exp, sizeof(char) * 1000);

        views[std::string(s)] = std::string(exp);
    }
}

Table Database::operator [](const std::string& s) const {
    auto it = std::find(table_list.begin(), table_list.end(), s);
    auto jt = views.find(s);

    // If view or table not found
    if (it == table_list.end() && jt == views.end())
        throw std::runtime_error("Table/View " + s + " does not exist.");

    // If view found
    else if (jt != views.end()) {
        std::string exp = jt -> second;
        Expression* e = parse_expression(exp);
        return e->eval();
    }

    // Else if only table found.

    std::ifstream table_file(DATA + std::string(".") + s, std::ios::in | std::ios::binary);

    if (!table_file.is_open())
        throw std::runtime_error("Couldn't open table file for reading.");

    Schema sch;
    char str[100];
    size_t i, rows;
    DataType typ;
    Cell c;
    Tuple t;

    table_file.read((char*)&i, sizeof(size_t));

#ifdef DEBUG
    std::cout << "Read " << i << std::endl;
#endif

    table_file.read((char*)&rows, sizeof(size_t));

#ifdef DEBUG
    std::cout << "Read " << rows << std::endl;
#endif

    for (size_t j = 0; j < i; j++) {
        table_file.read((char*)str, sizeof(char) * 100);

#ifdef DEBUG
        std::cout << "Read " << str << std::endl;
#endif

        table_file.read((char*)&typ, sizeof(DataType));

#ifdef DEBUG
        std::cout << "Read " << typ << std::endl;
#endif

        sch.push_back(std::make_pair(std::string(str), typ));
    }

    Table result(sch);

    for (size_t k = 0; k < rows; k++) {
        t.clear();
        for (size_t j = 0; j < i; j++) {
            table_file.read((char*)&c, sizeof(Cell));

#ifdef DEBUG
            std::cout << "Read " << c.show() << std::endl;
#endif


            t.push_back(c);
        }

        result += t;
    }

    table_file.close();

    return result;
}

void Database::add_table(const std::string& s, const Table& t) {
    auto it = std::find(table_list.begin(), table_list.end(), s);

    if (it == table_list.end()) {
        table_list.push_back(s);
        std::ofstream db_file(DATA + std::string(".") + SCHEMA, std::ios::out | std::ios::trunc | std::ios::binary);

        size_t len = table_list.size();

        db_file.write((char*)&len, sizeof(size_t));

#ifdef DEBUG
        std::cout << "Wrote " << len << std::endl;
#endif

        char temp[100];
        for (auto& a : table_list) {
            size_t k;
            for (k = 0; k < a.size() && k < 99; k++) {
                temp[k] = a[k];
            }
            temp[k] = '\0';

            db_file.write((char*)temp, sizeof(char) * 100);

#ifdef DEBUG
            std::cout << "Wrote " << temp << std::endl;
#endif
        }
    }

    std::ofstream table_file(DATA + std::string(".") + s, std::ios::out | std::ios::trunc | std::ios::binary);

    if (!table_file.is_open())
        throw std::runtime_error("Couldn't open table file for writing.");

    Schema sch = t.getSchema();
    char str[100];
    size_t i = sch.size(), j = t.size();
    DataType typ;
    Cell c;

    table_file.write((char*)&i, sizeof(size_t));

#ifdef DEBUG
    std::cout << "Wrote " << i << std::endl;
#endif

    table_file.write((char*)&j, sizeof(size_t));

#ifdef DEBUG
    std::cout << "Wrote " << j << std::endl;
#endif


    for (size_t j = 0; j < i; j++) {
        size_t k;
        for (k = 0; k < sch[j].first.size() && k < 99; k++) {
            str[k] = sch[j].first[k];
        }
        str[k] = '\0';

        table_file.write((char*)str, sizeof(char) * 100);

#ifdef DEBUG
        std::cout << "Wrote " << str << std::endl;
#endif

        typ = sch[j].second;

        table_file.write((char*)&typ, sizeof(DataType));

#ifdef DEBUG
        std::cout << "Wrote " << typ << std::endl;
#endif
    }

    for (auto& row : t) {
        for (size_t j = 0; j < sch.size(); j++) {
            c = row[j];
            table_file.write((char*)&c, sizeof(Cell));

#ifdef DEBUG
            std::cout << "Wrote " << c.show() << std::endl;
#endif
        }
    }

    table_file.close();
}

void Database::remove(const std::string& s) {
    auto it = std::find(table_list.begin(), table_list.end(), s);

    if (it != table_list.end()) {
        table_list.erase(it);
        std::ofstream db_file(DATA + std::string(".") + SCHEMA, std::ios::out | std::ios::trunc | std::ios::binary);

        size_t len = table_list.size();

        db_file.write((char*)&len, sizeof(size_t));

#ifdef DEBUG
        std::cout << "Wrote " << len << std::endl;
#endif

        char temp[100];
        for (auto& a : table_list) {
            size_t k;
            for (k = 0; k < a.size() && k < 99; k++) {
                temp[k] = a[k];
            }
            temp[k] = '\0';

            db_file.write((char*)temp, sizeof(char) * 100);

#ifdef DEBUG
            std::cout << "Wrote " << temp << std::endl;
#endif
        }

        db_file.close();
        std::remove((DATA + std::string(".") + s).c_str());
    }

    if (views.find(s) != views.end()) {
        views.erase(s);
        std::ofstream view_file(DATA + std::string(".") + VIEW, std::ios::out | std::ios::trunc | std::ios::binary);

        size_t n = views.size();

        view_file.write((char*) &n, sizeof(size_t));

        for (auto& v : views) {
            view_file.write(v.first.c_str(), sizeof(char) * 100);
            view_file.write(v.second.c_str(), sizeof(char) * 1000);
        }
    }
    else
        views.erase(s);
}

void Database::add_view(const std::string& s, const std::string& expr) {
    views[s] = expr;

    std::ofstream view_file(DATA + std::string(".") + VIEW,
              std::ios::out
            | std::ios::trunc
            | std::ios::binary);

    size_t n = views.size();

    view_file.write((char*) &n, sizeof(size_t));

    for (auto& v : views) {
        view_file.write(v.first.c_str(), sizeof(char) * 100);
        view_file.write(v.second.c_str(), sizeof(char) * 1000);
    }
}

void Database::info(void) const {
    std::cout << "The following tables are registered:" << std::endl;

    if (table_list.empty())
        std::cout << "None" << std::endl;

    for (auto& a : table_list) {
        if (views.find(a) == views.end()) {
            std::cout << "    - " << a;

            std::ifstream table_file(DATA + std::string(".") + a, std::ios::in | std::ios::binary);

            if (!table_file.is_open())
                throw std::runtime_error("Couldn't open table file for IO.");

            Schema sch;
            char str[100];
            size_t i, rows;
            DataType typ;
            Cell c;
            Tuple t;

            table_file.read((char*)&i, sizeof(size_t));

#ifdef DEBUG
            std::cout << "Read " << i << std::endl;
#endif

            table_file.read((char*)&rows, sizeof(size_t));

#ifdef DEBUG
            std::cout << "Read " << rows << std::endl;
#endif

            for (size_t j = 0; j < i; j++) {
                table_file.read((char*)str, sizeof(char) * 100);

#ifdef DEBUG
                std::cout << "Read " << str << std::endl;
#endif

                table_file.read((char*)&typ, sizeof(DataType));

#ifdef DEBUG
               std::cout << "Read " << typ << std::endl;
#endif

                sch.push_back(std::make_pair(std::string(str), typ));
            }

            auto it = sch.begin();
            std::cout << "(" << it -> first;
            ++it;
            for (; it != sch.end(); ++it) {
                std::cout << ", " << it ->first;
            }
            std::cout << ") : " << rows << " tuples."<< std::endl;
        }
    }

    std::cout << "The following views are registered:" << std::endl;

    if (views.empty())
        std::cout << "None" << std::endl;

    for (auto& v : views) {
        std::cout << "    - " << v.first << " : " << v.second << std::endl;
    }
}
