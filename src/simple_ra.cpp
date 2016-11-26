#include <iostream>
#include <string>
#include <ctime>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "simple_ra.hpp"

using namespace RelationalAlgebra;

// The global variable for database handling.
Database RelationalAlgebra::database;

int main (void) {
    Statement *stmt;
    std::clock_t start;

    std::cout << "/***********************************************************" << std::endl
              << "*                         simple_ra                        *" << std::endl              
              << "*                                                          *" << std::endl
              << "*  The (not so simple) Relational Algebra Interpreter. See *" << std::endl
              << "*  README.md for information related to the usage of the   *" << std::endl
              << "*  software. The syntax is described in 'syntax.ebnf'.     *" << std::endl
              << "*  Enter the command ':quit' to quit and ':help' for help. *" << std::endl
              << "***********************************************************/" << std::endl;

    while (true) {
        std::string line = rl_gets();
        if (line.empty()) {
            continue;
        }
        if (line[0] == ':') {
            if (line.substr(1) == QUIT)
                exit(0);
            else if (line.substr(1) == HELP)
                print_help();
            else if (line.substr(1, std::string(DROP).length()) == DROP) {
                std::string file = line.substr(std::string(DROP).length() + 2);
                std::remove(file.c_str());

                std::cout << "Deleted relation " << file << " if exists." << std::endl;
                database.remove(file);
            }
            else if (line.substr(1, std::string(SHOWALL).length()) == SHOWALL) {
                database.info();
            }
            else
                std::cout << "Invalid command." << std::endl;
        }
        else {
            start = std::clock();
            try {
                stmt = parse_statement(line);
                stmt->exec();
                delete stmt;
            }
            catch (std::exception &e) {
                std::cout << "Error! " << e.what() << std::endl;
            }

            std::cout << "Query completed. Time taken: "
                      << double(std::clock() - start) / CLOCKS_PER_SEC
                      << " s." << std::endl;
        }
    }
}

// libreadline - readline function with error
// handling and history management
std::string RelationalAlgebra::rl_gets(void) {
    char *line_read = readline(PROMPT);
    std::string l;

    if (line_read && *line_read) {
        add_history(line_read);
        l = line_read;
    }
    else {
        l = rl_gets();
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

