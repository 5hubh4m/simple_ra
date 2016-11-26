#ifndef PARSER_HPP
#define PARSER_HPP

#include <exception>
#include <string>
#include <vector>
#include <queue>

#include "expression.hpp"

namespace RelationalAlgebra {
    // A simple exception class
    class ParseError : public std::exception {
        const char* what() const throw();
    };

    Statement* parse_statement(const std::string&);
    Expression* parse_expression(const std::string&);    
};

#endif
