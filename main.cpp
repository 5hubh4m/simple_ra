
#include <readline/readline.h>
#include <readline/history.h>

#include "cell.hpp"
#include "predicate.hpp"
#include "table.hpp"
#include "expression.hpp"
#include "parser.hpp"
#include "ra_pret.hpp"


std::string rl_gets ();


int main (void) {
    std::string line;

    while (true) {
        line = rl_gets ();
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
