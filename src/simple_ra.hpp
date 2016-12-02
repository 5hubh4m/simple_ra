#ifndef RA_PRET_HPP
#define RA_PRET_HPP

#include <map>
#include <iostream>
#include <string>

#include "storage.hpp"
#include "parser.hpp"

#define DATA       ".data"
#define SCHEMA     "schema"
#define VIEW       "views"
#define SYNTAX     "syntax.ebnf"
#define PROMPT     "R-ALGEBRA>>> "
#define SEC_PROMPT "           > "

namespace RelationalAlgebra {
    extern Database database;
    
    void print_help (void);

    void rl_init(void);

    struct {
        std::vector<std::string> keywords = {
            "SELECT",
            "PROJECT",
            "RENAME",
            "AGGREGATE",
            "MIN",
            "MAX",
            "SUM",
            "AVG",
            "COUNT",
            ":quit;",
            ":drop",
            ":showall;",
            ":help;"
        };

        const char* operator [] (int idx) {
            if (idx >= 0 && idx < (int)keywords.size()) {
                return keywords[idx].c_str();
            } else {
                idx -= keywords.size();

                if (idx >= 0 && idx < (int) database.getTableList().size()) {
                    return database.getTableList()[idx].c_str();
                } else {
                    idx -=  database.getTableList().size();

                    if (idx >= 0 && idx < (int) database.getViewList().size()) {
                        auto it = database.getViewList().begin();
                        
                        for (; idx >= 0; idx--) {
                            ++it;
                        }

                        return it->first.c_str();
                    } else {
                        return nullptr;
                    }
                }
            }
        }
    } completion_names;

    char** completion(const char *, int, int);
    char* completion_generator(const char *, int);

    std::string rl_gets(void);

    void showall(void);
};

#endif
