#include <iostream>
#include <string>
#include <memory>
#include <cctype>

#include <readline/readline.h>
#include <readline/history.h>

#include "simple_ra.hpp"

using namespace RelationalAlgebra;

// The global variable for database handling.
Database RelationalAlgebra::database;

int main (void) {
    std::unique_ptr<Statement> stmt;

    std::cout << "************************************************************" << std::endl
              << "*                         simple_ra                        *" << std::endl              
              << "*                                                          *" << std::endl
              << "*  The (not so simple) Relational Algebra Interpreter. See *" << std::endl
              << "*  README.md for information related to the usage of the   *" << std::endl
              << "*  software. The syntax is described in 'syntax.ebnf'.     *" << std::endl
              << "*  Enter the command ':quit' to quit and ':help' for help. *" << std::endl
              << "************************************************************" << std::endl;

    while (true) {
        std::string line = rl_gets();
        if (line.empty()) {
            continue;
        }

        try {
            stmt = std::unique_ptr<Statement>(parse_statement(line));
            stmt->exec();
        } catch (std::exception& e) {
            std::cout << "Error! " << e.what() << std::endl;
        }
    }
}

// libreadline - readline function with error
// handling and history management
std::string RelationalAlgebra::rl_gets(void) {
    char *line_read = nullptr;
    std::string l;
    size_t count = 0;
    bool first = true;

    line_read = readline(PROMPT);

    while (true) {
        if (line_read) {
            if(*line_read) {
                add_history(line_read);
            } else if (first) {
                return std::string();
            }
            
            first = false;
            l += line_read;

            for (size_t i = l.size(); i > 0; i--) {
                if (isspace(l[i - 1])) {
                    count++;
                } else {
                    break;
                }
            }

            l = l.substr(0, l.size() - count);

            if (l.back() == ';') {
                return l;
            }
        } else {
            exit(0);
        }

        line_read = readline(SEC_PROMPT);
    }

    return l;
}

void RelationalAlgebra::print_help(void) {
    std::cout << "/************************" << std::endl
              << "* WELCOME TO simple_ra! *" << std::endl
              << "************************/" << std::endl
              << std::endl
              << "simple_ra is a basic implementation of formal relational algebra type quer"
              << "y language. It contains support for all of the basic RA operations. The fo"
              << "mal syntax is defined below in EBNF." << std::endl << std::endl
              << "**********************************************************" << std::endl
              << std::endl;

    std::string temp;
    std::ifstream syntax(SYNTAX, std::ios::in);
    if (syntax.is_open()) {
        while (syntax) {
            std::getline(syntax, temp);
            std::cout << temp << std::endl;
        }
    }

    std::cout << "**********************************************************" << std::endl
              << std::endl
              << "Hope this helped you! For more details, have a look at the source code." << std::endl;

    syntax.close();
}

void RelationalAlgebra::showall(void) {
    database.info();
}
