#include <iostream>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>

#include "simple_ra.hpp"

#include "cell.hpp"
#include "table.hpp"
#include "expression.hpp"
#include "simple_parser.hpp"

std::string rl_gets ();

Database database;

int main (void) {
    Expression *exp;

    std::cout << "/**********************************************************" << std::endl
              << "* This program is in beta stages. Invalid expressions may *" << std::endl
              << "* result in irrecoverable errors. Please use the grammar  *" << std::endl
              << "* carefully. It is formally described in EBNF form in the *" << std::endl
              << "* \"syntax.cf\". Thank you!                                 *" << std::endl
              << "**********************************************************/" << std::endl;

    while (true) {
        std::string line = rl_gets ();
        if (line.empty ()) {
            continue;
        }
        if (line[0] == ':') {
            if (line.substr (1) == QUIT)
                exit (0);
            else
                std::cout << "Invalid command." << std::endl;
        }
        else {
            try {
                exp = parseExpr (line);
                exp -> eval ().print ();
            }
            catch (std::exception &e) {
                std::cout << e.what () << std::endl;
            }
            delete exp;
        }
    }
}

std::string rl_gets () {
    char *line_read = readline (PROMPT);
    std::string l;

    if (line_read && *line_read) {
        add_history (line_read);
        l = line_read;
    }

    return l;
}
