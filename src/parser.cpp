#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <cmath>

#include "expression.hpp"
#include "parser.hpp"

#define Bool(T) std::pair<bool, T>
#define Pair(x, y) std::make_pair(x, y)

using namespace RelationalAlgebra;

const char* ParseError::what() const throw() {
    return "Parsing error.";
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

Bool(int) digit(std::istream&);
Bool(int) number(std::istream&);
Bool(char) character(std::istream&);
Bool(char) smallalpha_score(std::istream&);
Bool(char) alpha_score_digit(std::istream&);
Bool(std::string) string(std::istream&);
Bool(std::string) identifier(std::istream&);
Bool(std::string) comma_identifier(std::istream&);

Bool(Cell) cell(std::istream&);
Bool(Cell) comma_cell(std::istream&);
Bool(Tuple) tuple(std::istream& in);

Bool(ComparisionOperation) comparision(std::istream&);
Bool(BinaryOperation) binary(std::istream&);
Bool(BooleanOperation) boolean(std::istream&);
Bool(AggregateOperation) aggregate_func(std::istream&);
Bool(StatementOperation) operation(std::istream&);

Bool(BasicPredicate*) comp_exp(std::istream&);
Bool(Predicate*) predicate(std::istream&);
Bool(Expression*) unary_expression(std::istream&);
Bool(Expression*) expression(std::istream&);
Bool(Statement*) statement(std::istream&);

// End of declarations

// The main parsing methods exposed to outside of this file

Statement* RelationalAlgebra::parse_statement(const std::string& input) {
    std::stringstream in(input, std::ios_base::in);

    Bool(Statement*) s = statement(in);
    
    if (!s.first) {
        delete s.second;
        
        throw ParseError();
    } else {
        return s.second;
    }
}

Expression* RelationalAlgebra::parse_expression(const std::string& input) {
    std::stringstream in(input, std::ios_base::in);

    Bool(Expression*) s = expression(in);
    
    if (!s.first) {
        delete s.second;
        
        throw ParseError();
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
    space(in);
    
    if (in.eof()) {
        return true;
    } else {
        return false;
    }
}

Bool(int) digit(std::istream& in) {
    if (isdigit(in.peek())) {
        char s = in.get();

        return Pair(true, s - '0');
    } else {
        return Pair(false, 0);
    }
}

Bool(int) number(std::istream& in) {
    Bool(int) a;
    std::string num;

    if ((a = digit(in)).first) {
        num += (a.second + '0');

        while ((a = digit(in)).first) {
            num += (a.second + '0');
        }

        return Pair(true, std::stoi(num));
    } else {
        return Pair(false, 0);
    }
}

Bool(char) character(std::istream& in) {
    char c;

    if (in.peek() == '\\') {
        in.get();

        if (in.peek() == '\\') {
            in.get();

            return Pair(true, '\\');
        } else if (in.peek() == '\"') {
            in.get();

            return Pair(true, '\"');
        } else if (in.peek() == ';') {
            in.get();
            
            return Pair(true, ';');
        } else {
            throw ParseError();
        }
    } else if (in.peek() != '\"') {
        c = in.get();

        return Pair(true, c);
    } else {
        return Pair(false, 0);
    }
}

Bool(std::string) string(std::istream& in) {
    Bool(char) c;
    std::string s;

    if (in.peek() == '\"') {
        in.get();

        if ((c = character(in)).first) {
            s.push_back(c.second);

            while ((c = character(in)).first) {
                s.push_back(c.second);
            }

            if (in.get() == '\"') {
                return Pair(true, s);
            } else {
                throw ParseError();
            }
        } else {
            throw ParseError();
        }
    } else {
        return Pair(false, std::string());
    }
}

Bool(Cell) cell(std::istream& in) {
    Bool(std::string) s;
    Bool(int) i;
    Bool(float) f;
    Cell c;

    if ((s = string(in)).first) {
        return Pair(true, Cell(s.second));
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

                    return Pair(true, Cell(neg ? -std::stof(a) : std::stof(a)));
                } else {
                    throw ParseError();
                }
            } else {
                return Pair(true, Cell(neg ? -i.second : i.second));
            }
        } else {
            if (issigned) {
                throw ParseError();
            } else {
                return Pair(false, Cell());
            }
        }
    }
}

Bool(Cell) comma_cell(std::istream& in) {
    Bool(Cell) c;

    if (in.peek() == ',') {
        in.get();

        space(in);

        if ((c = cell(in)).first) {
            return c;
        } else {
            throw ParseError();
        }
    } else {
        return Pair(false, Cell());
    }
}

Bool(Tuple) tuple(std::istream& in) {
    Bool(Cell) c;
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
                return Pair(true, tup);
            } else {
                throw ParseError();
            }
        } else {
            throw ParseError();
        }
    } else {
        return Pair(false, Tuple());
    }
}

Bool(char) smallalpha_score(std::istream& in) {
    char c;

    if ((in.peek() >= 'a'  && in.peek() <= 'z') || in.peek() == '_') {
        c = in.get();

        return Pair(true, c);
    } else {
        return Pair(false, 0);
    }
}

Bool(char) alpha_score_digit(std::istream& in) {
    char c;

    if (isalpha(in.peek()) || isdigit(in.peek()) || in.peek() == '_') {
        c = in.get();

        return Pair(true, c);
    } else {
        return Pair(false, 0);
    }
}

Bool(std::string) identifier(std::istream& in) {
    Bool(char) c;
    std::string s;


    if ((c = smallalpha_score(in)).first) {
        s.push_back(c.second);

        while ((c = alpha_score_digit(in)).first) {
            s.push_back(c.second);
        }

        return Pair(true, s);
    } else {
        return Pair(false, std::string());
    }
}

Bool(ComparisionOperation) comparision(std::istream& in) {
    if (in.peek() == '=') {
        in.get();

        if (in.peek() == '/') {
            in.get();

            if (in.get() == '=') {
                return Pair(true, ComparisionOperation::NotEqual);
            } else {
                throw ParseError();
            }
        }

        return Pair(true, ComparisionOperation::Equal);
    } else if (in.peek() == '<' || in.peek() == '>') {
        char c = in.get();

        if (in.peek() == '=') {
            in.get();

            return Pair(true, (c == '<' ? ComparisionOperation::LessThanEqual
                                        : ComparisionOperation::GreaterThanEqual));
        }

        return Pair(true, (c == '<' ? ComparisionOperation::LessThan
                                    : ComparisionOperation::GreaterThan));
    } else {
        return Pair(false, ComparisionOperation::NotEqual);
    }
}

Bool(BinaryOperation) binary(std::istream& in) {
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

        return Pair(true, op);
    } else {
        return Pair(false, BinaryOperation::Union);
    }
}

Bool(BooleanOperation) boolean(std::istream& in) {
    if (in.peek() == '&') {
        in.get();

        if (in.get() == '&') {
            return Pair(true, BooleanOperation::And);
        } else {
            throw ParseError();
        }
    } else if (in.peek() == '|') {
        in.get();

        if (in.get() == '|') {
            return Pair(true, BooleanOperation::Or);
        } else {
            throw ParseError();
        }
    } else {
        return Pair(false, BooleanOperation::Or);
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

Bool(BasicPredicate*) comp_exp(std::istream& in) {
    Bool(std::string) i1, i2;
    Bool(Cell) c1, c2;
    Bool(ComparisionOperation) op;

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

                return Pair(true, pred);
            } else {
                throw ParseError();
            }
        } else {
            throw ParseError();
        }
    } else {
        return Pair(false, nullptr);
    }
}

Bool(Predicate*) predicate(std::istream& in) {
    Bool(Predicate*) p, p2;
    Bool(BooleanOperation) op;

    if (negate(in)) {
        space(in);

        if ((p = predicate(in)).first) {
            return Pair(true, new NotPredicate(p.second));
        } else {
            throw ParseError();
        }
    } else if (in.peek() == '(') {
        in.get();
        space(in);

        if ((p = predicate(in)).first) {
            space(in);

            if (in.get() == ')') {
                space(in);
                
                if ((op = boolean(in)).first) {
                    space(in);
                    
                    if ((p2 = predicate(in)).first) {
                        return Pair(true, new BinaryPredicate(p.second, op.second, p2.second));
                    } else {
                        delete p.second;
                        
                        throw ParseError();
                    }
                } else {
                    return p;
                }
            } else {
                delete p.second;
                
                throw ParseError();
            }
        } else {
            throw ParseError();
        }
    } else if ((p = comp_exp(in)).first) {
        space(in);

        if ((op = boolean(in)).first) {
            space(in);

            if ((p2 = predicate(in)).first) {
                return Pair(true, new BinaryPredicate(p.second, op.second, p2.second));
            } else {
                delete p.second;
                
                throw ParseError();
            }
        } else {
            return p;
        }
    } else {
        return Pair(false, nullptr);
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
            throw ParseError();
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
            throw ParseError();
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
            throw ParseError();
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
            throw ParseError();
        }
    } else {
        return false;
    }
}

Bool(AggregateOperation) aggregatefunc(std::istream& in) {
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
            throw ParseError();
        }

        return Pair(true, op);
    } else if (in.peek() == 'C') {
        std::string s;
        for (int i = 0; i < 5; i++) {
            s.push_back(in.get());
        }

        if (s == "COUNT") {
            return Pair(true, AggregateOperation::Count);
        } else {
            throw ParseError();
        }
    } else {
        return Pair(false, op);
    }
}

Bool(std::string) comma_identifier(std::istream& in) {
    Bool(std::string) s;

    if (in.peek() == ',') {
        in.get();

        space(in);

        if ((s = identifier(in)).first) {
            return s;
        } else {
            throw ParseError();
        }
    } else {
        return Pair(false, std::string());
    }
}

Bool(Expression*) unary_expression(std::istream& in) {
    Bool(Tuple) t;
    Bool(std::string) s;
    Bool(Predicate*) p;
    Bool(Expression*) e1;
    Bool(AggregateOperation) agop;
    Bool(BinaryOperation) binop;
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

        return Pair(true, new TupleExpression(tbl));
    } else if ((s = identifier(in)).first) {
        return Pair(true, new TableExpression(s.second));
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
                                return Pair(true, new SelectExpression(p.second, e1.second));
                            } else {
                                delete e1.second;
                                delete p.second;
                                
                                throw ParseError();
                            }
                        } else {
                            delete p.second;

                            throw ParseError();
                        }
                    } else {
                        delete p.second;

                        throw ParseError();
                    }
                } else {
                    delete p.second;

                    throw ParseError();
                }
            } else {
                throw ParseError();
            }
        } else {
            throw ParseError();
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
                                
                                return Pair(true, e);
                            } else {
                                delete e1.second;
                                
                                throw ParseError();
                            }
                        } else {
                            throw ParseError();
                        }
                    } else {
                        throw ParseError();
                    }
                } else {
                    throw ParseError();
                }
            } else {
                throw ParseError();
            }
        } else {
            throw ParseError();
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
                                        return Pair(true, new AggregateExpression(agop.second, s.second, e1.second));
                                    } else {
                                        delete e1.second;
                                        
                                        throw ParseError();
                                    }
                                } else {
                                    throw ParseError();
                                }
                            } else {
                                throw ParseError();
                            }
                        } else {
                            throw ParseError();
                        }
                    } else {
                        throw ParseError();
                    }
                } else {
                    throw ParseError();
                }
            } else {
                throw ParseError();
            }
        } else {
            throw ParseError();
        }
    } else {
        return Pair(false, nullptr);
    }
}

Bool(Expression*) expression(std::istream& in) {
    Bool(Tuple) t;
    Bool(std::string) s;
    Bool(Predicate*) p;
    Bool(Expression*) e1, e2;
    Bool(AggregateOperation) agop;
    Bool(BinaryOperation) binop;
    std::vector<std::string> cols;

    if (in.peek() == '(') {
        in.get();

        space(in);

        if ((e1 = expression(in)).first) {
            space(in);

            if (in.get() == ')') {
                space(in);

                if ((binop = binary(in)).first) {
                    space(in);

                    if ((e2 = expression(in)).first) {
                        return Pair(true, new BinaryExpression(e1.second, binop.second, e2.second));
                    } else {
                        delete e1.second;

                        throw ParseError();
                    }
                } else {
                    return e1;
                }
            } else {
                delete e1.second;
                
                throw ParseError();
            }
        } else {
            throw ParseError();
        }
    } else if ((e1 = unary_expression(in)).first) {
        space(in);

        if ((binop = binary(in)).first) {
            space(in);

            if ((e2 = expression(in)).first) {
                return Pair(true, new BinaryExpression(e1.second, binop.second, e2.second));
            } else {
                delete e1.second;
                
                throw ParseError();
            }
        } else {
            return e1;
        }
    } else {
        return Pair(false, nullptr);
    }
}

Bool(StatementOperation) operation(std::istream& in) {
    if (in.peek() == '~') {
        in.get();

        return Pair(true, StatementOperation::CreateView);
    } else if (in.peek() == '<') {
        in.get();

        if (in.get() == '-') {
            return Pair(true, StatementOperation::StoreTable);
        } else {
            throw ParseError();
        }
    } else {
        return Pair(false, StatementOperation::CreateView);
    }
}

Bool(Statement*) statement(std::istream& in) {
    Bool(Expression*) e;
    Bool(std::string) i;
    Bool(StatementOperation) op;

    if (in.peek() == '[') {
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
                                return Pair(true, new StoreStatement(i.second, e.second));
                            case StatementOperation::CreateView:
                                return Pair(true, new AssignStatement(i.second, exp));
                            }
                        } else {
                            delete e.second;
                            throw ParseError();
                        }
                    } else {
                        throw ParseError();
                    }
                } else {
                    throw ParseError();
                }
            } else {
                throw ParseError();
            }
        } else {
            throw ParseError();
        }
    } else if ((e = expression(in)).first) {
        if (end(in)) {
            return Pair(true, new ExpressionStatement(e.second));
        } else {
            delete e.second;
            
            throw ParseError();
        }
    } else {
        return Pair(false, nullptr);
    }
}
