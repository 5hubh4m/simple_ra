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
            size_t idx;
            std::string error;
        
        public:
            ParseError(size_t idx) :
                idx(idx - 1) {
                error = std::string((size_t) (6 + idx), ' ') + "^\nParsing error occured at character " + std::to_string(idx) + ".";    
            }

        const char* what() const throw();
    };

    Statement* parse_statement(const std::string&);
    Expression* parse_expression(const std::string&);    
};

#endif
