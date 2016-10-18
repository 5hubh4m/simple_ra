#include <iostream>
#include <string>
#include <ctime>

#include <readline/readline.h>
#include <readline/history.h>

#include "simple_ra.hpp"
#include "cell.hpp"
#include "table.hpp"
#include "expression.hpp"
#include "parser.hpp"
#include "storage.hpp"

// The global variable for database handling.
Database database;

int main (void) {
    Expression exp;
    std::clock_t start;

    //std::cout << "/**********************************************************" << std::endl
    //          << "* This program is in beta stages. Invalid expressions may *" << std::endl
    //          << "* result in irrecoverable errors. Please use the grammar  *" << std::endl
    //          << "* carefully. It is formally described in EBNF form in the *" << std::endl
    //          << "* file \"syntax.ebnf\". Thank you!                          *" << std::endl
    //          << "**********************************************************/" << std::endl;

    while (true) {
        std::string line = rl_gets ();
        if (line.empty ()) {
            continue;
        }
        if (line[0] == ':') {
            if (line.substr (1) == QUIT)
                exit (0);
            else if (line.substr (1) == HELP)
                print_help ();
            else if (line.substr (1, std::string (DROP).length ()) == DROP) {
                std::string file = line.substr (std::string (DROP).length () + 2);
                std::remove (file.c_str ());

                std::cout << "Deleted relation " << file << " if exists." << std::endl;
                database.remove (file);
            }
            else if (line.substr (1, std::string (SHOWALL).length ()) == SHOWALL) {
                database.info ();
            }
            else
                std::cout << "Invalid command." << std::endl;
        }
        else {
            start = std::clock ();
            try {
                exp = parseExpr (line);
                exp.eval ().print ();
            }
            catch (std::exception &e) {
                std::cout << "Error! " << e.what () << std::endl;
            }

            std::cout << "Query completed. Time taken: "
                << double(std::clock () - start) / CLOCKS_PER_SEC
                << " s." << std::endl;
        }
    }
}

// libreadline - readline function with error
// handling and history management
std::string rl_gets () {
    char *line_read = readline (PROMPT);
    std::string l;

    if (line_read && *line_read) {
        add_history (line_read);
        l = line_read;
    }
    else
        l = std::string (":") + QUIT;

    return l;
}

void print_help (void) {
    std::cout << "/************************" << std::endl
              << "* WELCOME TO simple_ra! *" << std::endl
              << "************************/" << std::endl << std::endl

              << "simple_ra is a basic implementation of formal relational algebra type quer"
              << "y language. It contains support for all of the basic RA operations. The fo"
              << "mal syntax is defined below in EBNF." << std::endl << std::endl
              << "**********************************************************" << std::endl
              << std::endl;

    std::string temp;
    std::ifstream syntax (SYNTAX, std::ios::in);
    if (syntax.is_open ()) {
        while (syntax) {
            std::getline (syntax, temp);
            std::cout << temp << std::endl;
        }
    }

    std::cout << "**********************************************************" << std::endl
              << std::endl
              << "Hope this helped you! For more details, have a look at the source code." << std::endl;

    syntax.close ();
}
