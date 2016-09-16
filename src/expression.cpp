#include <iostream>
#include <vector>
#include <string>

#include "table.hpp"
#include "expression.hpp"
#include "simple_ra.hpp"
#include "parser.hpp"

bool BoolExpr::eval (Schema schema, Tuple tuple) {
#ifdef DEBUG

    std::cout << id1 << " " << operation << " " << id2 << std::endl;

#endif

    Cell d1, d2;

    for (size_t i = 0; i < schema.size (); i++) {
        if (schema[i].first == id1)
            d1 = tuple[i];
        if (schema[i].first == id2)
            d2 = tuple[i];
    }

    if (d1.getType () == INVALID) {
        d1 = parseCell (id1);
    }

    if (d2.getType () == INVALID) {
        d2 = parseCell (id2);
    }

    if (operation == "=")
        return d1 == d2;
    else if (operation == "/=")
        return d1 != d2;
    else if (operation == "<=")
        return d1 <= d2;
    else if (operation == ">=")
        return d1 >= d2;
    else if (operation == "<")
        return d1 < d2;
    else if (operation == ">")
        return d1 > d2;

    return false;
}

bool Predicate::eval (Schema schema, Tuple tuple) {
    for (auto& a : expressions) {
        if (!a.eval (schema, tuple))
            return false;
    }

    return true;
}

Expression::Expression (std::string tname) {
    type = SIMPLE;
    operand.table_name = tname;
}

Expression::Expression (Table t) {
    type = TUPLE;
    option.tuple = t;
}

Expression::Expression (std::string oprn, Expression* e1, Expression* e2) {
    type = COMPOUND;
    operation = oprn;
    operand.expressions.push_back (e1);
    operand.expressions.push_back (e2);
}

Expression::Expression (std::string oprn, std::vector< std::string > optn, Expression* e) {
    type = COMPOUND;
    operation = oprn;
    option.col_names = optn;
    operand.expressions.push_back (e);
}

Expression::Expression (std::string oprn, std::string optn, Expression* e) {
    type = COMPOUND;
    operation = oprn;
    option.table_name = optn;
    operand.expressions.push_back (e);
}


Expression::Expression (std::string oprn, Predicate p, Expression* e) {
    type = COMPOUND;
    operation = oprn;
    option.bool_exp = p;
    operand.expressions.push_back (e);
}

void Expression::printExpr (void) {
    std::cout << type << " " << operation << std::endl;

    for (auto& a : operand.expressions) {
        a -> printExpr ();
    }

    std::cout << std::endl;
}

Table Expression::eval () {
    Table result, temp;

    if (type == SIMPLE) {
        return database[operand.table_name];
    }
    else if (type == TUPLE) {
        return option.tuple;
    }
    else {
        if (operation == SELECT) {
            return operand.expressions[0] -> eval ().select (option.bool_exp);
        }
        else if (operation == PROJECT) {
            return operand.expressions[0] -> eval ().project (option.col_names);
        }
        else if (operation == RENAME) {
            return operand.expressions[0] -> eval ().rename (option.col_names);
        }
        else if (operation == ASSIGN) {
            result = operand.expressions[0] -> eval ();
            database.add_table(option.table_name, result);
        }
        else if (operation == CARTESIAN) {
            result = operand.expressions[0] -> eval ();
            temp = operand.expressions[1] -> eval ();

            return result * temp;
        }
        else if (operation == NATJOIN) {
            result = operand.expressions[0] -> eval ();
            temp = operand.expressions[1] -> eval ();

            return result | temp;
       }
        else if (operation == INTERSEC) {
            result = operand.expressions[0] -> eval ();
            temp = operand.expressions[1] -> eval ();

            return result ^ temp;
        }
        else if (operation == UNION) {
            result = operand.expressions[0] -> eval ();
            temp = operand.expressions[1] -> eval ();

            return result + temp;
       }
        else if (operation == SETDIFF) {
            result = operand.expressions[0] -> eval ();
            temp = operand.expressions[1] -> eval ();

            return result - temp;
       }
    }

    return result;
}
