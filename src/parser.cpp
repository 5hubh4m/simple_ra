#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <cmath>

#include "expression.hpp"
#include "parser.hpp"

using namespace RelationalAlgebra;

const char* ParseError::what() const throw() {
    return error.c_str();
}

// Parsing functions for LL(1) recursive descent.
//
// We do not need to expose these functions outside
// the library. They are automatically called through
// the RelationalAlgebra::parse() function.

void space(std::istream&);

bool end(std::istream&);
bool aggregate(std::istream&);
bool select(std::istream&);
bool project(std::istream&);
bool rename(std::istream&);
bool negate(std::istream&);

std::pair<bool, int>         digit(std::istream&);
std::pair<bool, int>         number(std::istream&);
std::pair<bool, char>        character(std::istream&);
std::pair<bool, char>        smallalpha_score(std::istream&);
std::pair<bool, char>        alpha_score_digit(std::istream&);
std::pair<bool, std::string> string(std::istream&);
std::pair<bool, std::string> identifier(std::istream&);
std::pair<bool, std::string> comma_identifier(std::istream&);

std::pair<bool, Cell>  cell(std::istream&);
std::pair<bool, Cell>  comma_cell(std::istream&);
std::pair<bool, Tuple> tuple(std::istream& in);

std::pair<bool, ComparisionOperation> comparision(std::istream&);
std::pair<bool, BinaryOperation>      binary(std::istream&);
std::pair<bool, BooleanOperation>     boolean(std::istream&);
std::pair<bool, AggregateOperation>   aggregate_func(std::istream&);
std::pair<bool, StatementOperation>   operation(std::istream&);
std::pair<bool, Command>              command(std::istream&);

std::pair<bool, Predicate*>                              comp_exp(std::istream&);
std::pair<bool, Predicate*>                              basepred(std::istream&);
std::pair<bool, Predicate*>                              frontpred(std::istream&);
std::pair<bool, std::pair<BooleanOperation, Predicate*>> backpred(std::istream&);
std::pair<bool, Predicate*>                              predicate(std::istream&);
std::pair<bool, Expression*>                             unary_expression(std::istream&);
std::pair<bool, Expression*>                             frontexp(std::istream&);
std::pair<bool, std::pair<BinaryOperation, Expression*>> backexp(std::istream&);
std::pair<bool, Expression*>                             expression(std::istream&);
std::pair<bool, Statement*>                              statement(std::istream&);

// End of declarations

// The main parsing methods exposed to outside of this file

Statement* RelationalAlgebra::parse_statement(const std::string& input) {
    std::stringstream in(input, std::ios_base::in);

    std::pair<bool, Statement*> s = statement(in);
    
    if (!s.first) {
        delete s.second;
        
        throw ParseError(in.tellg());
    } else {
        return s.second;
    }
}

Expression* RelationalAlgebra::parse_expression(const std::string& input) {
    std::stringstream in(input, std::ios_base::in);

    std::pair<bool, Expression*> s = expression(in);
    
    if (!s.first) {
        delete s.second;
        
        throw ParseError(in.tellg());
    } else {
        return s.second;
    }
}

// End of parsing function interface


// Definition of mutually-recursive parsing methods
//
// The production of the grammar parsed by each function is
// self-explanatory. Otherwise, parsing is done using simple
// LL(1) resursive descent while building the Abstract Syntax Tree
// along the way. Pointer to the value parsed and a success
// value (bool) is returned by each function.

void space(std::istream& in) {
    while (isspace(in.peek())) {
        in.get();
    }
}

bool end(std::istream& in) {
    if (in.peek() == ';') {
        in.get();
        
        return true;
    } else {
        return false;
    }
}

std::pair<bool, int> digit(std::istream& in) {
    if (isdigit(in.peek())) {
        char s = in.get();

        return std::make_pair(true, s - '0');
    } else {
        return std::make_pair(false, 0);
    }
}

std::pair<bool, int> number(std::istream& in) {
    std::pair<bool, int> a;
    std::string num;

    if ((a = digit(in)).first) {
        num += (a.second + '0');

        while ((a = digit(in)).first) {
            num += (a.second + '0');
        }

        return std::make_pair(true, std::stoi(num));
    } else {
        return std::make_pair(false, 0);
    }
}

std::pair<bool, char> character(std::istream& in) {
    char c;

    if (in.peek() == '\\') {
        in.get();

        if (in.peek() == '\\') {
            in.get();

            return std::make_pair(true, '\\');
        } else if (in.peek() == '\"') {
            in.get();

            return std::make_pair(true, '\"');
        } else if (in.peek() == ';') {
            in.get();
            
            return std::make_pair(true, ';');
        } else {
            throw ParseError(in.tellg());
        }
    } else if (in.peek() != '\"') {
        c = in.get();

        return std::make_pair(true, c);
    } else {
        return std::make_pair(false, 0);
    }
}

std::pair<bool, std::string> string(std::istream& in) {
    std::pair<bool, char> c;
    std::string s;

    if (in.peek() == '\"') {
        in.get();

        if ((c = character(in)).first) {
            s.push_back(c.second);

            while ((c = character(in)).first) {
                s.push_back(c.second);
            }

            if (in.get() == '\"') {
                return std::make_pair(true, s);
            } else {
                throw ParseError(in.tellg());
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, std::string());
    }
}

std::pair<bool, Cell> cell(std::istream& in) {
    std::pair<bool, std::string> s;
    std::pair<bool, int> i;
    std::pair<bool, float> f;
    Cell c;

    if ((s = string(in)).first) {
        return std::make_pair(true, Cell(s.second));
    } else {
        bool issigned = false, neg = false;

        if (in.peek() == '+' || in.peek() == '-'){
            if (in.get() == '-') {
                neg = false;
            }
            issigned = true;
        }

        if ((i = number(in)).first) {
            if (in.peek() == '.') {
                in.get();

                std::string a = std::to_string(i.second);

                a.push_back('.');

                if ((i = number(in)).first) {
                    a += std::to_string(i.second);

                    return std::make_pair(true, Cell(neg ? -std::stof(a) : std::stof(a)));
                } else {
                    throw ParseError(in.tellg());
                }
            } else {
                return std::make_pair(true, Cell(neg ? -i.second : i.second));
            }
        } else {
            if (issigned) {
                throw ParseError(in.tellg());
            } else {
                return std::make_pair(false, Cell());
            }
        }
    }
}

std::pair<bool, Cell> comma_cell(std::istream& in) {
    std::pair<bool, Cell> c;

    if (in.peek() == ',') {
        in.get();

        space(in);

        if ((c = cell(in)).first) {
            return c;
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, Cell());
    }
}

std::pair<bool, Tuple> tuple(std::istream& in) {
    std::pair<bool, Cell> c;
    Tuple tup;

    if (in.peek() == '{') {
        in.get();

        space(in);

        if ((c = cell(in)).first) {
            tup.push_back(c.second);

            space(in);

            while ((c = comma_cell(in)).first) {
                space(in);

                tup.push_back(c.second);
            }

            if (in.get() == '}') {
                return std::make_pair(true, tup);
            } else {
                throw ParseError(in.tellg());
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, Tuple());
    }
}

std::pair<bool, char> smallalpha_score(std::istream& in) {
    char c;

    if ((in.peek() >= 'a'  && in.peek() <= 'z') || in.peek() == '_') {
        c = in.get();

        return std::make_pair(true, c);
    } else {
        return std::make_pair(false, 0);
    }
}

std::pair<bool, char> alpha_score_digit(std::istream& in) {
    char c;

    if (isalpha(in.peek()) || isdigit(in.peek()) || in.peek() == '_') {
        c = in.get();

        return std::make_pair(true, c);
    } else {
        return std::make_pair(false, 0);
    }
}

std::pair<bool, std::string> identifier(std::istream& in) {
    std::pair<bool, char> c;
    std::string s;


    if ((c = smallalpha_score(in)).first) {
        s.push_back(c.second);

        while ((c = alpha_score_digit(in)).first) {
            s.push_back(c.second);
        }

        return std::make_pair(true, s);
    } else {
        return std::make_pair(false, std::string());
    }
}

std::pair<bool, ComparisionOperation> comparision(std::istream& in) {
    if (in.peek() == '=') {
        in.get();

        if (in.peek() == '/') {
            in.get();

            if (in.get() == '=') {
                return std::make_pair(true, ComparisionOperation::NotEqual);
            } else {
                throw ParseError(in.tellg());
            }
        }

        return std::make_pair(true, ComparisionOperation::Equal);
    } else if (in.peek() == '<' || in.peek() == '>') {
        char c = in.get();

        if (in.peek() == '=') {
            in.get();

            return std::make_pair(true, (c == '<' ? ComparisionOperation::LessThanEqual
                                        : ComparisionOperation::GreaterThanEqual));
        }

        return std::make_pair(true, (c == '<' ? ComparisionOperation::LessThan
                                    : ComparisionOperation::GreaterThan));
    } else {
        return std::make_pair(false, ComparisionOperation::NotEqual);
    }
}

std::pair<bool, BinaryOperation> binary(std::istream& in) {
    if (in.peek() == '+' ||
        in.peek() == '*' ||
        in.peek() == '^' ||
        in.peek() == '-' ||
        in.peek() == '@') {
        char c = in.get();
        BinaryOperation op;


        switch (c) {
        case '+':
            op = BinaryOperation::Union;
            break;
        case '^':
            op = BinaryOperation::Intersection;
            break;
        case '*':
            op = BinaryOperation::Cartesian;
            break;
        case '-':
            op = BinaryOperation::SetDifference;
            break;
        case '@':
            op = BinaryOperation::NaturalJoin;
            break;
        }

        return std::make_pair(true, op);
    } else {
        return std::make_pair(false, BinaryOperation::Union);
    }
}

std::pair<bool, BooleanOperation> boolean(std::istream& in) {
    if (in.peek() == '&') {
        in.get();

        if (in.get() == '&') {
            return std::make_pair(true, BooleanOperation::And);
        } else {
            throw ParseError(in.tellg());
        }
    } else if (in.peek() == '|') {
        in.get();

        if (in.get() == '|') {
            return std::make_pair(true, BooleanOperation::Or);
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, BooleanOperation::Or);
    }
}

bool negate(std::istream& in) {
    if (in.peek() == '!') {
        in.get();

        return true;
    } else {
        return false;
    }
}

std::pair<bool, Predicate*> comp_exp(std::istream& in) {
    std::pair<bool, std::string> i1, i2;
    std::pair<bool, Cell> c1, c2;
    std::pair<bool, ComparisionOperation> op;

    if ((i1 = identifier(in)).first || (c1 = cell(in)).first) {
        space(in);

        if ((op = comparision(in)).first) {
            space(in);

            if ((i2 = identifier(in)).first || (c2 = cell(in)).first) {
                BasicPredicate *pred;

                if (i1.first && i2.first)
                    pred = new BasicPredicate(i1.second, op.second, i2.second);

                if (c1.first && c1.first)
                    pred = new BasicPredicate(c1.second, op.second, c2.second);

                if (c1.first && i2.first)
                    pred = new BasicPredicate(c1.second, op.second, i2.second);

                if (i1.first && c2.first)
                    pred = new BasicPredicate(i1.second, op.second, c2.second);

                return std::make_pair(true, pred);
            } else {
                throw ParseError(in.tellg());
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, nullptr);
    }
}

std::pair<bool, Predicate*> basepred(std::istream& in) {
    std::pair<bool, Predicate*> p;

    if (in.peek() == '(') {
        in.get();
        space(in);

        if ((p = predicate(in)).first) {
            space(in);

            if (in.get() == ')') {
                space(in);
                
                return p;
            } else {
                delete p.second;
                
                throw ParseError(in.tellg());
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else if ((p = comp_exp(in)).second) {
        return p;
    } else {
        return std::make_pair(false, nullptr);
    }
}

std::pair<bool, Predicate*> frontpred(std::istream& in) {
    std::pair<bool, Predicate*> p;
    
    if (negate(in)) {
        space(in);

        if ((p = basepred(in)).second) {
            return std::make_pair(true, new NotPredicate(p.second));
        } else {
            throw ParseError(in.tellg());
        }
    } else if ((p = basepred(in)).second) {
        return p;
    } else {
        return std::make_pair(false, nullptr);
    }
}

std::pair<bool, std::pair<BooleanOperation, Predicate*>> backpred(std::istream& in) {
    std::pair<bool, Predicate*> p;
    std::pair<bool, std::pair<BooleanOperation, Predicate*>> bp;
    std::pair<bool, BooleanOperation> op;

    if ((op = boolean(in)).first) {
        space(in);

        if ((p = frontpred(in)).second) {
            space(in);

            if ((bp = backpred(in)).first) {
                return std::make_pair(true, std::make_pair(op.second, new BinaryPredicate(p.second, bp.second.first, bp.second.second)));
            } else {
                return std::make_pair(true, std::make_pair(op.second, p.second));
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, std::make_pair(BooleanOperation::And, nullptr));
    }
}

std::pair<bool, Predicate*> predicate(std::istream& in) {
    std::pair<bool, Predicate*> p;
    std::pair<bool, std::pair<BooleanOperation, Predicate*>> bp;

    if ((p = frontpred(in)).first) {
        space(in);

        if ((bp = backpred(in)).first) {
            return std::make_pair(true, new BinaryPredicate(p.second, bp.second.first, bp.second.second));
        } else {
            return p;
        }
    } else {
        return std::make_pair(false, nullptr);
    }
}

bool select(std::istream& in) {
    if (in.peek() == 'S') {
        std::string s;
        for (int i = 0; i < 6; i++) {
            s.push_back(in.get());
        }

        if (s == "SELECT") {
            return true;
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return false;
    }
}

bool project(std::istream& in) {
    if (in.peek() == 'P') {
        std::string s;
        for (int i = 0; i < 7; i++) {
            s.push_back(in.get());
        }

        if (s == "PROJECT") {
            return true;
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return false;
    }
}

bool rename(std::istream& in) {
    if (in.peek() == 'R') {
        std::string s;
        for (int i = 0; i < 6; i++) {
            s.push_back(in.get());
        }

        if (s == "RENAME") {
            return true;
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return false;
    }
}

bool aggregate(std::istream& in) {
    if (in.peek() == 'A') {
        std::string s;
        for (int i = 0; i < 9; i++) {
            s.push_back(in.get());
        }

        if (s == "AGGREGATE") {
            return true;
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return false;
    }
}

std::pair<bool, AggregateOperation> aggregatefunc(std::istream& in) {
    AggregateOperation op;

    if (in.peek() == 'M' || in.peek() == 'A' || in.peek() == 'S') {
        std::string s;
        for (int i = 0; i < 3; i++) {
            s.push_back(in.get());
        }

        if (s == "MIN") {
            op = AggregateOperation::Min;
        } else if (s == "MAX") {
            op = AggregateOperation::Max;
        } else if (s == "AVG") {
            op = AggregateOperation::Avg;
        } else if (s == "SUM") {
            op = AggregateOperation::Count;
        } else {
            throw ParseError(in.tellg());
        }

        return std::make_pair(true, op);
    } else if (in.peek() == 'C') {
        std::string s;
        for (int i = 0; i < 5; i++) {
            s.push_back(in.get());
        }

        if (s == "COUNT") {
            return std::make_pair(true, AggregateOperation::Count);
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, op);
    }
}

std::pair<bool, std::string> comma_identifier(std::istream& in) {
    std::pair<bool, std::string> s;

    if (in.peek() == ',') {
        in.get();

        space(in);

        if ((s = identifier(in)).first) {
            return s;
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, std::string());
    }
}

std::pair<bool, Expression*> unary_expression(std::istream& in) {
    std::pair<bool, Tuple> t;
    std::pair<bool, std::string> s;
    std::pair<bool, Predicate*> p;
    std::pair<bool, Expression*> e1;
    std::pair<bool, AggregateOperation> agop;
    std::pair<bool, BinaryOperation> binop;
    std::vector<std::string> cols;
    bool proj = false, ren = false;
    Expression* e;

    if ((t = tuple(in)).first) {
        Schema temp;

        for (auto& c : t.second) {
            temp.push_back(std::make_pair(std::string(""), c.getType()));
        }

        Table tbl(temp);

        tbl += t.second;

        return std::make_pair(true, new TupleExpression(tbl));
    } else if ((s = identifier(in)).first) {
        return std::make_pair(true, new TableExpression(s.second));
    } else if (select(in)) {
        space(in);

        if (in.get() == '[') {
            space(in);

            if ((p = predicate(in)).first) {
                space(in);

                if (in.get() == ']') {
                    space(in);

                    if (in.get() == '(') {
                        space(in);

                        if ((e1 = expression(in)).first) {
                            space(in);

                            if (in.get() == ')') {
                                return std::make_pair(true, new SelectExpression(p.second, e1.second));
                            } else {
                                delete e1.second;
                                delete p.second;
                                
                                throw ParseError(in.tellg());
                            }
                        } else {
                            delete p.second;

                            throw ParseError(in.tellg());
                        }
                    } else {
                        delete p.second;

                        throw ParseError(in.tellg());
                    }
                } else {
                    delete p.second;

                    throw ParseError(in.tellg());
                }
            } else {
                throw ParseError(in.tellg());
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else if ((proj = project(in)) || (ren = rename(in))) {
        space(in);

        if (in.get() == '[') {
            space(in);

            if ((s = identifier(in)).first) {
                cols.push_back(s.second);

                space(in);

                while ((s = comma_identifier(in)).first) {
                    cols.push_back(s.second);

                    space(in);
                }

                if (in.get() == ']') {
                    space(in);

                    if (in.get() == '(') {
                        space(in);

                        if ((e1 = expression(in)).first) {
                            space(in);

                            if (in.get() == ')') {
                                if (proj) {
                                    e = new ProjectExpression(cols, e1.second);
                                }
                                
                                if (ren) {
                                    e = new RenameExpression(cols, e1.second);
                                }
                                
                                return std::make_pair(true, e);
                            } else {
                                delete e1.second;
                                
                                throw ParseError(in.tellg());
                            }
                        } else {
                            throw ParseError(in.tellg());
                        }
                    } else {
                        throw ParseError(in.tellg());
                    }
                } else {
                    throw ParseError(in.tellg());
                }
            } else {
                throw ParseError(in.tellg());
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else if (aggregate(in)) {
        space(in);

        if (in.get() == '[') {
            space(in);

            if ((agop = aggregatefunc(in)).first) {
                space(in);

                if (in.get () == ':') {
                    space(in);

                    if ((s = identifier(in)).first) {
                        space(in);

                        if (in.get() == ']') {
                            space(in);

                            if (in.get() == '(') {
                                space(in);

                                if ((e1 = expression(in)).first) {
                                    space(in);

                                    if (in.get() == ')') {
                                        return std::make_pair(true, new AggregateExpression(agop.second, s.second, e1.second));
                                    } else {
                                        delete e1.second;
                                        
                                        throw ParseError(in.tellg());
                                    }
                                } else {
                                    throw ParseError(in.tellg());
                                }
                            } else {
                                throw ParseError(in.tellg());
                            }
                        } else {
                            throw ParseError(in.tellg());
                        }
                    } else {
                        throw ParseError(in.tellg());
                    }
                } else {
                    throw ParseError(in.tellg());
                }
            } else {
                throw ParseError(in.tellg());
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, nullptr);
    }
}


std::pair<bool, Expression*> frontexp(std::istream& in) {
    std::pair<bool, Expression*> e;

    if (in.peek() == '(') {
        in.get();

        space(in);

        if ((e = expression(in)).first) {
            space(in);

            if (in.get() == ')') {
                space(in);

                return e;
            } else {
                delete e.second;
                
                throw ParseError(in.tellg());
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else if ((e = unary_expression(in)).first) {
        space(in);

        return e;
    } else {
        return std::make_pair(false, nullptr);
    }
}

std::pair<bool, std::pair<BinaryOperation, Expression*>> backexp(std::istream& in) {
    std::pair<bool, Expression*> e;
    std::pair<bool, BinaryOperation> binop;
    std::pair<bool, std::pair<BinaryOperation, Expression*>> bexp;

    if ((binop = binary(in)).first) {
        space(in);

        if ((e = frontexp(in)).first) {
            space(in);

            if ((bexp = backexp(in)).first) {
                return std::make_pair(true, std::make_pair(binop.second, new BinaryExpression(e.second, bexp.second.first, bexp.second.second)));
            } else {
                return std::make_pair(true, std::make_pair(binop.second, e.second));
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, std::make_pair(BinaryOperation::Union, nullptr));
    }
}


std::pair<bool, Expression*> expression(std::istream& in) {
    std::pair<bool, Expression*> e;
    std::pair<bool, std::pair<BinaryOperation, Expression*>> bexp;

    if ((e = frontexp(in)).first) {
        space(in);

        if ((bexp = backexp(in)).first) {
            return std::make_pair(true, new BinaryExpression(e.second, bexp.second.first, bexp.second.second));
        } else {
            return e;
        }
    } else {
        return std::make_pair(false, nullptr);
    }
}

std::pair<bool, StatementOperation> operation(std::istream& in) {
    if (in.peek() == '~') {
        in.get();

        return std::make_pair(true, StatementOperation::CreateView);
    } else if (in.peek() == '<') {
        in.get();

        if (in.get() == '-') {
            return std::make_pair(true, StatementOperation::StoreTable);
        } else {
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, StatementOperation::CreateView);
    }
}

std::pair<bool, Command> command(std::istream& in) {
    std::string s;

    if (in.peek() == 'q' || in.peek() == 'h') { 
        for (size_t i = 0; i < 4; i++) {
            s += in.get();
        }

        if (s == "quit") {
            return std::make_pair(true, Command::Quit);
        } else if (s == "help") {
            return std::make_pair(true, Command::Help);
        } else {
            throw ParseError(in.tellg());
        }
    } else if (in.peek() == 's') {
        for (size_t i = 0; i <7; i++) {
            s += in.get();
        }

        if (s == "showall") {
            return std::make_pair(true, Command::ShowAll);
        } else {
            throw ParseError(in.tellg());
        }

    } else {
        return std::make_pair(false, Command::Quit);
    }
}

std::pair<bool, Statement*> statement(std::istream& in) {
    std::pair<bool, Expression*> e;
    std::pair<bool, std::string> i;
    std::pair<bool, StatementOperation> op;
    std::pair<bool, Command> c;

    if (in.peek() == ':') {
        in.get();

        space(in);

        if ((c = command(in)).first) {
            space(in);

            return std::make_pair(true, new CommandStatement(c.second));
        } else if (in.peek() == 'd') {
            std::string s;
            
            for (size_t i = 0; i < 4; i++) {
                s += in.get();
            }

            if (s == "drop") {
                space(in);

                if ((i = identifier(in)).first) {
                    space(in);

                    if (end(in)) {
                        return std::make_pair(true, new DropStatement(i.second));
                    } else {
                        throw ParseError(in.tellg());
                    }
                } else {
                    throw ParseError(in.tellg());
                }
            } else {
                throw ParseError(in.tellg());
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else if (in.peek() == '[') {
        in.get();

        space(in);

        if ((i = identifier(in)).first) {
            space(in);

            if (in.get() == ']') {
                space(in);

                if ((op = operation(in)).first) {
                    space(in);
                    
                    std::streampos idx = in.tellg();

                    std::string exp, s;

                    while (in.good()) {
                        std::getline(in, s);
                        exp += s;
                    }

                    in.seekg(idx);

                    if ((e = expression(in)).first) {
                        if (end(in)) {
                            switch(op.second) {
                            case StatementOperation::StoreTable:
                                return std::make_pair(true, new StoreStatement(i.second, e.second));
                            case StatementOperation::CreateView:
                                return std::make_pair(true, new AssignStatement(i.second, exp));
                            }
                        } else {
                            delete e.second;
                            throw ParseError(in.tellg());
                        }
                    } else {
                        throw ParseError(in.tellg());
                    }
                } else {
                    throw ParseError(in.tellg());
                }
            } else {
                throw ParseError(in.tellg());
            }
        } else {
            throw ParseError(in.tellg());
        }
    } else if ((e = expression(in)).first) {
        if (end(in)) {
            return std::make_pair(true, new ExpressionStatement(e.second));
        } else {
            delete e.second;
            
            throw ParseError(in.tellg());
        }
    } else {
        return std::make_pair(false, nullptr);
    }
}
