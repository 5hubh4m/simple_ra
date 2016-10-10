#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <iostream>
#include <vector>
#include <string>

#include "cell.hpp"
#include "table.hpp"

struct BoolExpr {
    std::string id1, operation, id2;

    bool eval (const Schema&, const Tuple&) const;
};

struct Predicate {
    std::vector< BoolExpr > expressions;

    bool eval (const Schema&, const Tuple&) const;
};

struct Aggregate {
    std::string operation, col_name;
    Cell value;
};

class Expression {
  private:
    enum {
        TUPLE, SIMPLE, COMPOUND
    } type;
    std::string operation;
    struct {
        std::vector< Expression* > expressions;
        std::string table_name;
    } operand;
    struct {
        Predicate bool_exp;
        std::vector< std::string > col_names;
        std::vector< Aggregate > aggregate;
        std::string table_name;
        Table tuple;
    } option;

  public:
    Expression (const std::string& tname);
    Expression (const Table&);
    Expression (const std::string&, const std::string&, Expression*);
    Expression (const std::string&, Expression*, Expression*);
    Expression (const std::string&, const std::vector< std::string >&, Expression*);
    Expression (const std::string&, const std::vector< Aggregate >&, Expression*);
    Expression (const std::string&, const Predicate&, Expression*);

    void printExpr (void) const;
    Table eval () const;

    ~Expression () {
        for (auto& a : operand.expressions) {
            if (a != nullptr)
                delete a;
        }
    }
};

#endif
