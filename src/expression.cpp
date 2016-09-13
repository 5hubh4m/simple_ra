#include <iostream>
#include <vector>
#include <string>

#include "table.hpp"
#include "expression.hpp"
#include "simple_ra.hpp"

bool BoolExpr::eval (Schema schema, Tuple tuple) {
    Cell d1;

    for (size_t i = 0; i < schema.size (); i++) {
        if (schema[i].first == id)
            d1 = tuple[i];
    }

    if (d1.getType () == INVALID) {
        throw std::runtime_error ("Invalid predicate, column not found");
    }

    if (operation == "=")
        return d1 == c;
    else if (operation == "/=")
        return d1 != c;
    else if (operation == "<=")
        return d1 <= c;
    else if (operation == ">=")
        return d1 >= c;
    else if (operation == "<")
        return d1 < c;
    else if (operation == ">")
        return d1 > c;

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
    Table result;

    if (type == SIMPLE) {
        if (database.find (operand.table_name) == database.end ())
            throw std::runtime_error ("Table does not exist.");

        result = database[operand.table_name];
    }
    else if (type == TUPLE)
        result = option.tuple;
    else {
        if (operation == "SELECT") {
            Table tempT = operand.expressions[0] -> eval ();
            Schema s = tempT.getSchema ();

            result = Table(s);

            for (auto& a : tempT.getTable ()) {
                if (option.bool_exp.eval (s, a))
                    result += a;
            }
        }
        else if (operation == "PROJECT") {
            Table tempT = operand.expressions[0] -> eval ();
            Schema s = tempT.getSchema ();

            std::vector< size_t > indices;

            for (size_t i = 0; i < s.size (); i++) {
                bool found = false;
                for (auto& a : option.col_names) {
                    if (s[i].first == a) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    indices.push_back (i);
                }
            }

            size_t keep = 0;
            for (auto& i : indices) {
                s.erase (s.begin () + i - keep++);
            }

#ifdef DEBUG

            std::cout << "Erasing columns ";
            for (auto& a : indices) {
                std::cout << a << " ";
            } std::cout << std::endl;

#endif
            result = Table (s);

            for (auto a : tempT.getTable ()) {
                size_t keep = 0;
                for (auto& i : indices) {
                    a.erase (a.begin () + i - keep++);
                }

                result += a;
            }
        }
        else if (operation == "RENAME") {
            result = operand.expressions[0] -> eval ();

            result.rename (option.col_names);
        }
        else if (operation == "X") {}
        else if (operation == "@") {}
        else if (operation == "v") {}
        else if (operation == "U") {
            result = operand.expressions[0] -> eval ();
            Table temp = operand.expressions[1] -> eval ();

            for (auto& a : temp.getTable ()) {
                result += a;
            }
        }
        else if (operation == "-") {
            result = operand.expressions[0] -> eval ();
            Table temp = operand.expressions[1] -> eval ();

            for (auto& a : temp.getTable ()) {
                result -= a;
            }
        }
    }

    return result;
}
