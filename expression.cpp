#include <vector>
#include <string>

#include "predicate.hpp"
#include "expression.hpp"


Expression::Expression (std::vector< std::string > tnames) {
    operation = INVALID;
    operand.table_names = tnames;
}

Expression::Expression (std::string oprn, std::vector< Expression* > exps) {
    if (oprn == "CARTESIAN")
        operation = CARTESIAN;
    else if (oprn == "INTERSEC")
        operation = INTERSEC;
    else if (oprn == "SETDIFF")
        operation = SETDIFF;
    else if (oprn == "UNION")
        operation = UNION;
    else if (oprn == "NATJOIN")
        operation = NATJOIN;

    operand.expressions = exps;
}

Expression::Expression (std::string oprn, std::vector< std::string > optn, std::vector< Expression* > exps) {
    if (oprn == "PROJECT") {
        operation = PROJECT;
        option.col_names = optn;
    }
    else if (oprn == "RENAME") {
        operation = RENAME;
        option.new_name.first = optn[0];
        optn.erase (optn.begin());
        option.new_name.second = optn;
    }

    operand.expressions = exps;
}

Expression::Expression (std::string oprn, Predicate p, std::vector< Expression* > exps) {
    if (oprn == "SELECT") {
        operation = SELECT;
        option.bool_exp = p;
    }

    operand.expressions = exps;
}

Expression::~Expression () {
    for (auto a : operand.expressions) {
       delete a;
    }
}

