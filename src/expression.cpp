#include <iostream>
#include <vector>
#include <string>

#include "table.hpp"
#include "expression.hpp"
#include "simple_ra.hpp"
#include "parser.hpp"

bool BoolExpr::eval (const Schema& schema, const Tuple& tuple) const {
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

    if (operation == EQ)
        return d1 == d2;
    else if (operation == NEQ)
        return d1 != d2;
    else if (operation == LEQ)
        return d1 <= d2;
    else if (operation == GEQ)
        return d1 >= d2;
    else if (operation == LT)
        return d1 < d2;
    else if (operation == GT)
        return d1 > d2;

    return false;
}

bool Predicate::eval (const Schema& schema, const Tuple& tuple) const {
    for (auto& a : expressions) {
        if (!a.eval (schema, tuple))
            return false;
    }

    return true;
}

Expression::Expression (const std::string& tname) {
    type = SIMPLE;
    operand.table_name = tname;
}

Expression::Expression (const Table& t) {
    type = TUPLE;
    option.tuple = t;
}

Expression::Expression (const std::string& oprn, Expression e1, Expression e2) {
    type = COMPOUND;
    operation = oprn;
    operand.expressions.push_back (e1);
    operand.expressions.push_back (e2);
}

Expression::Expression (const std::string& oprn, const std::vector< std::string >& optn, Expression e) {
    type = COMPOUND;
    operation = oprn;
    option.col_names = optn;
    operand.expressions.push_back (e);
}

Expression::Expression (const std::string& oprn, const std::vector< Aggregate >& optn, Expression e) {
    type = COMPOUND;
    operation = oprn;
    option.aggregate = optn;
    operand.expressions.push_back (e);
}

Expression::Expression (const std::string& oprn, const std::string& optn, Expression e) {
    type = COMPOUND;
    operation = oprn;
    option.table_name = optn;
    operand.expressions.push_back (e);
}

Expression::Expression (const std::string& oprn, const std::string& optn, const std::string& expr) {
    type = COMPOUND;
    operation = oprn;
    option.table_name = optn;
    option.expression = expr;
}

Expression::Expression (const std::string& oprn, const Predicate& p, Expression e) {
    type = COMPOUND;
    operation = oprn;
    option.bool_exp = p;
    operand.expressions.push_back (e);
}

void Expression::printExpr (void) const {
    std::cout << type << " " << operation << std::endl;

    for (auto& a : operand.expressions) {
        a.printExpr ();
    }

    std::cout << std::endl;
}

Table Expression::eval () const {
    Table result, temp;

    if (type == SIMPLE) {
        return database[operand.table_name];
    }
    else if (type == TUPLE) {
        return option.tuple;
    }
    else {
        if (operation == SELECT)
            return operand.expressions[0].eval ().select (option.bool_exp);

        else if (operation == PROJECT) {
            if (option.aggregate.empty ())
                return operand.expressions[0].eval ().project (option.col_names);
            else {
                temp = operand.expressions[0].eval ();

                if (!option.aggregate.empty ()) {

                    auto it = option.aggregate.begin ();

                    result = temp.aggregate (*it);
                    ++it;

                    for (; it != option.aggregate.end (); ++it) {
                        result = result * temp.aggregate (*it);
                    }
                }
            }
        }

        else if (operation == RENAME)
            return operand.expressions[0].eval ().rename (option.col_names);

        else if (operation == ASSIGN) {
            result = parseExpr (option.expression).eval ();
            database.add_view(option.table_name, option.expression);
        }
        else if (operation == STORE) {
            result = operand.expressions[0].eval ();
            database.add_table(option.table_name, result);
        }
        else if (operation == CARTESIAN)
            return operand.expressions[0].eval () * operand.expressions[1].eval ();

        else if (operation == NATJOIN) {
            Table t1 = operand.expressions[0].eval (),
                  t2 = operand.expressions[1].eval ();

            std::vector< std::string > s1, s2, common_attr, proj, rename;

            for (auto& a : t1.getSchema ()) {
                bool same = false;

                for (auto& b : t2.getSchema ()) {
                    if (a.first == b.first) {
                        common_attr.push_back (a.first);
                        s1.push_back (a.first + "1");
                        proj.push_back (a.first + "1");
                        same = true;
                    }
                }

                if (!same) {
                    s1.push_back (a.first);
                    proj.push_back (a.first);
                }

                rename.push_back (a.first);
            }

            for (auto& b : t2.getSchema ()) {
                bool same = false;

                for (auto& a : t1.getSchema ()) {
                    if (a.first == b.first) {
                        s2.push_back (a.first + "2");
                        same = true;
                    }
                }

                if (!same) {
                    s2.push_back (b.first);
                    proj.push_back (b.first);
                    rename.push_back (b.first);
                }
            }

            Predicate p;
            BoolExpr e;

            for (auto& a : common_attr) {
                e.id1 = a + "1";
                e.id2 = a + "2";
                e.operation = "=";

                p.expressions.push_back (e);
            }

            return (((t1.rename (s1) * t2.rename (s2)).select (p)).project (proj)).rename (rename);
        }

        else if (operation == INTERSEC) {
            Table t1 = operand.expressions[0].eval (),
                  t2 = operand.expressions[1].eval ();
            return t1 - (t1 - t2);
        }
        else if (operation == UNION)
            return operand.expressions[0].eval () + operand.expressions[1].eval ();

        else if (operation == SETDIFF)
            return operand.expressions[0].eval () - operand.expressions[1].eval ();
    }

    return result;
}
