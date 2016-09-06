#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <vector>
#include <string>

#include "predicate.hpp"
#include "ra_pret.hpp"

class Expression {
  private:
    enum {
        SELECT, PROJECT, RENAME, CARTESIAN, NATJOIN, INTERSEC, UNION, SETDIFF, INVALID
    } operation;
    struct {
        std::vector< Expression* > expressions;
        std::vector< std::string > table_names;
    } operand;
    struct {
        Predicate bool_exp;
        std::vector< std::string > col_names;
        std::pair< std::string, std::vector< std::string > > new_name;
    } option;

  public:
    Expression (std::vector< std::string > tnames);
    Expression (std::string oprn, std::vector< Expression* > exps);
    Expression (std::string oprn, std::vector< std::string > optn, std::vector< Expression* > exps);
    Expression (std::string oprn, Predicate p, std::vector< Expression* > exps);

    Table eval ();

    ~Expression ();
};

#endif
