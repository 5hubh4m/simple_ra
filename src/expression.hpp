#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <vector>
#include <string>

#include "table.hpp"

struct BoolExpr {
    std::string id, operation;
    Cell c;

    bool eval (Schema, Tuple);
};

struct Predicate {
    std::vector< BoolExpr > expressions;

    bool eval (Schema, Tuple tuple);
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
        Table tuple;
    } option;

  public:
    Expression (std::string tname);
    Expression (Table);
    Expression (std::string, Expression*, Expression*);
    Expression (std::string, std::vector< std::string >, Expression*);
    Expression (std::string, Predicate p, Expression*);

    void printExpr (void);
    Table eval ();

    ~Expression () {
        for (auto& a : operand.expressions) {
            delete a;
        }
    }
};

#endif
