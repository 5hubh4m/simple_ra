#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <iostream>
#include <vector>
#include <string>

#include "cell.hpp"
#include "table.hpp"

/* BoolExpr - Boolean Expression
 *
 * Defines the struct for an elementary boolean expression
 */
struct BoolExpr {
    std::string id1, operation, id2;

    // Eval function, given a schema and a tuple, evaluate the
    // value of the boolean expression
    bool eval (const Schema&, const Tuple&) const;
};

/* Predicate struct
 *
 * Defines the behaviour of a collection of BoolExprs,
 * all of them must be true for the predicate to be true.
 */
struct Predicate {
    std::vector< BoolExpr > expressions;

    bool eval (const Schema&, const Tuple&) const;
};

/* Aggregate struct
 *
 * Stores info for aggregate queries
 */
struct Aggregate {
    std::string operation, col_name;
    Cell value;
};

/* Expression class
 *
 * Represents the node of the abstract syntax tree of the
 * language.
 */
class Expression {
  private:
    // Define the type of node
    enum {
        TUPLE, SIMPLE, COMPOUND
    } type;

    std::string operation;          // Operation

    // Define the operands
    struct {
        std::vector< Expression > expressions;
        std::string table_name;
    } operand;

    // Option for the operation
    struct {
        Predicate bool_exp;
        std::vector< std::string > col_names;
        std::vector< Aggregate > aggregate;
        std::string table_name, expression;
        Table tuple;
    } option;

  public:
    // Constructors
    Expression () {}
    Expression (const std::string& tname);
    Expression (const Table&);
    Expression (const std::string&, const std::string&, Expression);
    Expression (const std::string&, const std::string&, const std::string&);
    Expression (const std::string&, Expression, Expression);
    Expression (const std::string&, const std::vector< std::string >&, Expression);
    Expression (const std::string&, const std::vector< Aggregate >&, Expression);
    Expression (const std::string&, const Predicate&, Expression);

    // Print expression for debugging purposes
    void printExpr (void) const;

    // Evaluate an expression
    Table eval () const;
};

#endif
