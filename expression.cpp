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

Table Expression::eval () {
    Table result;
    std::vector <size_t> temp;
    Table temp_table;
    Schema s;
    Tuple t;

    switch (operation) {
        case SELECT:
            break;
        case PROJECT:
            temp.clear ();
            temp_table = operand.expressions[0] -> eval ();
            s = temp_table.getSchema ();

            for (auto a : option.col_names) {
                temp.push_back (s[a].first);
            }

            for (auto a : temp_table.getTable ()) {
                t.clear ();
                for (auto i : temp) {
                    t.push_back (a[i]);
                }

                result += t;
            }
            break;
        case RENAME:
            result = operand.expressions[0] -> eval ();

            result.rename (option.new_name.second);
            break;
        case CARTESIAN:
            break;
        case NATJOIN:
            break;
        case INTERSEC:
            break;
        case UNION:
            break;
        case SETDIFF:
            break;
        case INVALID:
            result = database[operand.table_names[0]];
            break;
    }

    return result;
}

Expression::~Expression () {
    for (auto a : operand.expressions) {
       delete a;
    }
}

