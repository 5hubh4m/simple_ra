#include <iostream>
#include <vector>
#include <ctime>
#include <string>

#include "cell.hpp"
#include "table.hpp"
#include "expression.hpp"
#include "simple_ra.hpp"
#include "parser.hpp"

using namespace RelationalAlgebra;

bool BasicPredicate::eval (const Schema& schema, const Tuple& tuple) const {
    Cell d1, d2;

    for (size_t i = 0; i < schema.size (); i++) {
        if (schema[i].first == operand1)
            d1 = tuple[i];
        if (schema[i].first == operand2)
            d2 = tuple[i];
    }

    if (d1.getType () == DataType::Null) {
        d1 = value1;
    }

    if (d2.getType () == DataType::Null) {
        d2 = value2;
    }

    switch(operation) {
    case ComparisionOperation::Equal:
        return d1 == d2;

    case ComparisionOperation::NotEqual:
        return d1 != d2;

    case ComparisionOperation::LessThanEqual:
        return d1 <= d2;

    case ComparisionOperation::GreaterThanEqual:
        return d1 >= d2;

    case ComparisionOperation::LessThan:
        return d1 < d2;

    case ComparisionOperation::GreaterThan:
        return d1 > d2;
    }
}

bool NotPredicate::eval (const Schema& schema, const Tuple& tuple) const {
    return !pred->eval(schema, tuple);
}

bool BinaryPredicate::eval (const Schema& schema, const Tuple& tuple) const {
    switch(operation) {
    case BooleanOperation::And:
        return pred1->eval(schema, tuple) && pred2->eval(schema, tuple);

    case BooleanOperation::Or:
        return pred1->eval(schema, tuple) || pred2->eval(schema, tuple);
    }
}

Table TableExpression::eval(void) const {
    return database[table_name];
}

Table TupleExpression::eval(void) const {
    return tuple;
}

Table SelectExpression::eval(void) const {
    return expression->eval().select(predicate.get());
}

Table ProjectExpression::eval(void) const {
    return expression->eval().project(columns);
}

Table RenameExpression::eval(void) const {
    return expression->eval().rename(new_names);
}

Table AggregateExpression::eval(void) const {
    return expression->eval().aggregate(operation, column);
}

Table BinaryExpression::eval(void) const {
    Table t1, t2;

    switch(operation) {
    case BinaryOperation::Intersection:
        t1 = expression1->eval();
        t2 = expression2->eval();
        return t1 - (t1 - t2);

    case BinaryOperation::Union:
        return expression1->eval() + expression2->eval();

    case BinaryOperation::SetDifference:
        return expression1->eval() - expression2->eval();

    case BinaryOperation::Cartesian:
        return expression1->eval() * expression2->eval();

    case BinaryOperation::NaturalJoin:
        t1 = expression1->eval();
        t2 = expression2->eval();

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

        if (common_attr.empty()) {
            return t1 * t2;
        } else {
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

            Predicate* p = new BasicPredicate(common_attr[0] + "1", ComparisionOperation::Equal, common_attr[0] + "2");

            for (size_t i = 1; i < common_attr.size(); i++) {
                BasicPredicate e(common_attr[i] + "1", ComparisionOperation::Equal, common_attr[i] + "2");

                p = new BinaryPredicate(&e, BooleanOperation::And, p);
            }

            return (((t1.rename(s1) * t2.rename(s2)).select(p)).project(proj)).rename(rename);
        }
    }
}

void ExpressionStatement::exec(void) const {
    std::clock_t start = std::clock();
    
    expression->eval().print();

    std::cout << "Query completed. Time taken: "
              << double(std::clock() - start) / CLOCKS_PER_SEC
              << " s." << std::endl;
}

void StoreStatement::exec(void) const {
    std::clock_t start = std::clock();
    
    database.add_table(name, expression->eval());

    std::cout << "Query completed. Time taken: "
              << double(std::clock() - start) / CLOCKS_PER_SEC
              << " s." << std::endl;
}

void AssignStatement::exec(void) const {
    std::clock_t start = std::clock();
    
    database.add_view(name, expression);
    
    std::cout << "Query completed. Time taken: "
              << double(std::clock() - start) / CLOCKS_PER_SEC
              << " s." << std::endl;
}

void CommandStatement::exec(void) const {
    switch(command) {
    case Command::Help:
        print_help();
        break;
    case Command::ShowAll:
        showall();
        break;
    case Command::Quit:
        exit(0);
        break;
    }
}

void DropStatement::exec(void) const {
    database.remove(table_name.c_str());

    std::cout << "Removed Table/View " << table_name << " from database (if existed)." << std::endl;
}
