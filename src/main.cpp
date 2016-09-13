#include <iostream>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>

#include "cell.hpp"
#include "table.hpp"
#include "expression.hpp"
#include "simple_parser.hpp"
#include "simple_ra.hpp"

std::string rl_gets ();

Database database;

int main (void) {
    Expression *exp;

    while (true) {
        std::string line = rl_gets ();
        if (line.empty ()) {
            continue;
        }
        if (line == QUIT) {
            return 0;
        }

        try {
            exp = parseExpr (line);

#ifdef DEBUG

            exp -> printExpr ();

#endif

            exp -> eval ().print ();
        }
        catch (std::exception &e) {
            std::cout << e.what () << std::endl;
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
