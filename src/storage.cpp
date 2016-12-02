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
    std::ifstream db_file(DATA + std::string(".") + SCHEMA, std::ios::in | std::ios::binary),
                  view_file(DATA + std::string(".") + VIEW, std::ios::in | std::ios::binary);
        
    if (db_file.is_open()) {
        size_t len = 0, n = 0;
        db_file.read((char*) &len, sizeof(size_t));

        for (size_t i = 0; i < len; i++) {
            db_file.read((char*) &n, sizeof(size_t));

            char *temp = new char[n];
            db_file.read(temp, sizeof(char) * n);
            
            table_list.push_back(std::string(temp));

            delete[] temp;
        }

        db_file.close();
    }

    if (view_file.is_open()) {
        size_t len = 0, n = 0;
        view_file.read((char*) &len, sizeof(size_t));
 
        for (size_t i = 0; i < len; i++) {
            view_file.read((char*) &n, sizeof(size_t));

            char *temp1 = new char[n];
            view_file.read(temp1, sizeof(char) * n);

            view_file.read((char*) &n, sizeof(size_t));
            char *temp2 = new char[n];
            view_file.read(temp2, sizeof(char) * n);
 
            views[std::string(temp1)] = std::string(temp2);

            delete[] temp1;
            delete[] temp2;
        }

        view_file.close();
    }
}

Table Database::operator [] (const std::string& s) const {
    auto it = std::find(table_list.begin(), table_list.end(), s);
    auto jt = views.find(s);

    if (it == table_list.end() && jt == views.end()) {                          // If neither table nor view found by the name
        throw std::runtime_error("Table/View " + s + " does not exist.");
    } else if (jt != views.end()) {                                             // If a view was found by the name
        std::string exp = jt -> second;
        Expression* e = parse_expression(exp);
        return e->eval();
    } else {                                                                    // Otherwise, if a table was found by the name
        std::ifstream table_file(DATA + std::string(".") + s, std::ios::in | std::ios::binary);

        if (table_file) {
            Schema sch;
            size_t cols = 0, rows = 0;
            table_file.read((char*) &cols, sizeof(size_t));
            table_file.read((char*) &rows, sizeof(size_t));

            for (size_t i = 0; i < cols; i++) {
                size_t n = 0;
                DataType typ;
                table_file.read((char*) &n, sizeof(size_t));                

                char *str = new char[n];
                table_file.read(str, sizeof(char) * n);

                table_file.read((char*) &typ, sizeof(DataType));

                sch.push_back(std::make_pair(std::string(str), typ));
            }

            Table result(sch);

            for (size_t i = 0; i < rows; i++) {
                Tuple t;
                
                for (size_t j = 0; j < cols; j++) {
                    Cell c;

                    switch(sch[j].second) {
                    case DataType::Null:
                        DataType typ;
                        table_file.read((char*) &typ, sizeof(DataType));
                        
                        break;
                    case DataType::Integer:
                        int i;
                        table_file.read((char*) &i, sizeof(int));
                        
                        c = Cell(i);

                        break;
                    case DataType::Float:
                        float f;
                        table_file.read((char*) &f, sizeof(float));
                        
                        c = Cell(f);

                        break;
                    case DataType::String:
                        size_t n = 0;
                        table_file.read((char*) &n, sizeof(size_t));

                        char *str = new char[n];
                        table_file.read(str, sizeof(char) * n);

                        c = Cell(std::string(str));

                        delete[] str;
                        
                        break;
                    }

                    t.push_back(c);
                }

                result += t;
            }

            table_file.close();

            return result;
        } else {
            throw std::runtime_error("Couldn't open file for reading.");
        }
    }
}

void Database::add_table(const std::string& s, const Table& t) {
    auto it = std::find(table_list.begin(), table_list.end(), s);

    if (it == table_list.end()) {
        table_list.push_back(s);
        std::ofstream db_file(DATA + std::string(".") + SCHEMA, std::ios::out | std::ios::trunc | std::ios::binary);

        size_t len = table_list.size();

        db_file.write((char*) &len, sizeof(size_t));

        for (auto& a : table_list) {
            size_t n = a.size() + 1;
            db_file.write((char*) &n, sizeof(size_t));
            
            db_file.write(a.c_str(), sizeof(char) * n);
        }

        db_file.close();
    }

    std::ofstream table_file(DATA + std::string(".") + s, std::ios::out | std::ios::trunc | std::ios::binary);

    if (table_file) {
        Schema sch = t.getSchema();
        size_t rows = t.size(), cols = sch.size();

        table_file.write((char*) &cols, sizeof(size_t));

        table_file.write((char*) &rows, sizeof(size_t));


        for (auto& a : sch) {
            size_t n = a.first.size() + 1;
            table_file.write((char*) &n, sizeof(size_t));            

            table_file.write(a.first.c_str(), sizeof(char) * n);

            DataType typ = a.second;
            table_file.write((char*)&typ, sizeof(DataType));
        }

        for (auto& row : t) {
            for (auto& c : row) {
                if (c.getType() == DataType::Null) {
                        DataType typ = DataType::Null;
                        table_file.write((char*) &typ, sizeof(DataType));
                } else if (c.getType() == DataType::Integer) {
                        int i = c.getVal().i;
                        table_file.write((char*) &i, sizeof(int));
                } else if (c.getType() == DataType::Float) {
                        float f = c.getVal().f;
                        table_file.write((char*) &f, sizeof(float));

                } else {
                        size_t n = c.getVal().s.size() + 1;
                        table_file.write((char*) &n, sizeof(size_t));

                        table_file.write(c.getVal().s.c_str(), sizeof(char) * n);
                }
            }
        }
        
        table_file.close();
    } else {
        throw std::runtime_error("Couldn't open table file for writing.");        
    }
}

void Database::remove(const std::string& s) {
    auto it = std::find(table_list.begin(), table_list.end(), s);

    if (it != table_list.end()) {
        std::ofstream db_file(DATA + std::string(".") + SCHEMA, std::ios::out | std::ios::trunc | std::ios::binary);


        if (db_file.good()) {
            table_list.erase(it);
            
            size_t len = table_list.size();
            db_file.write((char*) &len, sizeof(size_t));

            for (auto& a : table_list) {
                size_t n = a.size() + 1;
                db_file.write((char*) &n, sizeof(size_t));                

                db_file.write(a.c_str(), sizeof(char) * n);
            }

            db_file.close();
            
            std::remove((DATA + std::string(".") + s).c_str());
        } else {
            throw std::runtime_error("Couldn't open file for writing.");        
        }
    }

    if (views.find(s) != views.end()) {
        views.erase(s);
        std::ofstream view_file(DATA + std::string(".") + VIEW, std::ios::out | std::ios::trunc | std::ios::binary);

        size_t n = views.size();

        view_file.write((char*) &n, sizeof(size_t));

        for (auto& v : views) {
            size_t n = v.first.size() + 1;
            view_file.write((char*) &n, sizeof(size_t));
            
            view_file.write(v.first.c_str(), sizeof(char) * n);
            
            n = v.second.size() + 1;
            view_file.write((char*) &n, sizeof(size_t));
            
            view_file.write(v.second.c_str(), sizeof(char) * n);
        }
    }
}

void Database::add_view(const std::string& s, const std::string& expr) {
    std::ofstream view_file(DATA + std::string(".") + VIEW, std::ios::out
                                                          | std::ios::trunc
                                                          | std::ios::binary);

    if (view_file.good() && find(table_list.begin(), table_list.end(), s) == table_list.end()) {
        views[s] = expr;        
        size_t n = views.size();

        view_file.write((char*) &n, sizeof(size_t));

        for (auto& v : views) {
            size_t n = v.first.size() + 1;
            view_file.write((char*) &n, sizeof(size_t));            

            view_file.write(v.first.c_str(), sizeof(char) * n);

            n = v.second.size() + 1;
            view_file.write((char*) &n, sizeof(size_t));

            view_file.write(v.second.c_str(), sizeof(char) * n);
        }

        view_file.clear();
    } else if (!view_file.good()) {
        throw std::runtime_error("Couldn't open file for writing.");        
    } else {
        throw std::runtime_error("A relation already exists with that name.");        
    }
}

void Database::info(void) const {
    std::cout << "The following tables are registered:" << std::endl;

    if (table_list.empty()) {
        std::cout << "None" << std::endl;
    } else {
        for (auto& a : table_list) {
            std::cout << "    - " << a;

            std::ifstream table_file(DATA + std::string(".") + a, std::ios::in | std::ios::binary);

            if (table_file.good()) {            
                Schema sch;
                size_t cols, rows;

                table_file.read((char*) &cols, sizeof(size_t));

                table_file.read((char*) &rows, sizeof(size_t));

                for (size_t i = 0; i < cols; i++) {
                    size_t n = 0;
                    table_file.read((char*) &n, sizeof(size_t));

                    char *str = new char[n];
                    table_file.read(str, sizeof(char) * n);

                    DataType typ;
                    table_file.read((char*) &typ, sizeof(DataType));

                    sch.push_back(std::make_pair(std::string(str), typ));
                }

                auto it = sch.begin();
                std::cout << "(" << it -> first;
                ++it;
                for (; it != sch.end(); ++it) {
                    std::cout << ", " << it ->first;
                }
                
                std::cout << ") : " << rows << " tuple(s)."<< std::endl;

                table_file.close();
            } else {
                throw std::runtime_error("Couldn't open table file for IO.");
            }
        }
    }

    std::cout << "The following views are registered:" << std::endl;

    if (views.empty()) {
        std::cout << "None" << std::endl;
    } else {
        for (auto& v : views) {
            std::cout << "    - " << v.first << " : " << v.second << std::endl;
        }
    }
}
