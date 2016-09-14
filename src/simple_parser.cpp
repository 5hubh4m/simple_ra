#include <iostream>

#include <string>
#include <vector>
#include <cctype>

#include "expression.hpp"
#include "cell.hpp"
#include "table.hpp"
#include "simple_parser.hpp"

int is_val (std::string a) {
    a.erase (std::remove (a.begin (), a.end (), ' '), a.end ());

    bool is_int = a[0] == '+' || a[0] == '-' || std::isdigit (a[0]);
    for (size_t i = 1; i < a.length (); i++) {
        if (!std::isdigit (a[i])) {
            is_int = false;
            break;
        }
    }

    bool is_float = a[0] == '+' || a[0] == '-' || std::isdigit (a[0]);
    bool decimal = false;
    for (size_t i = 1; i < a.length (); i++) {
        if (!std::isdigit (a[i])) {
            if (a[i] == '.' && !decimal) {
                decimal = true;
            }
            else {
                is_float = false;
                break;
            }
        }
    }

    bool is_string = a[0] == '"' && a[a.length () - 1] == '"';

    if (is_int)
        return 1;
    if (is_float)
        return 2;
    if (is_string)
        return 3;

    return 0;
}

size_t find_unary(std::string s) {
    for (size_t i = 0; i < unary.size (); i++) {
        if (s.substr(0, unary[i].length ()) == unary[i]) {
            return i;
        }
    }

    return std::string::npos;
}

std::pair< size_t, size_t > find_operation (std::string s) {
    std::pair< size_t, size_t > p = std::make_pair (std::string::npos,
            std::string::npos);

    size_t i;
    for (size_t z = 0; z < operations.size (); z++) {
        i = s.find (operations[z]);

        if (i != std::string::npos) {
            p.first = i;
            p.second = z;
        }
    }

    return p;
}

Cell parseCell (std::string s) {
    s.erase (std::remove (s.begin (), s.end (), ' '), s.end ());

    switch (is_val (s)) {
        case 1:
            return Cell (std::stoi (s));
        case 2:
            return Cell (std::stof (s));
        case 3:
            return Cell (s.substr (1, s.length () - 2));
    }

    return Cell ();
}

Table parseTuple(std::string s) {
#ifdef DEBUG

    std::cout << "Parsing: " << s << std::endl;

#endif

    Schema sch;
    Tuple t;
    Cell tmp;
    std::vector < std::pair< std::string, std::string > > tempSch;

    s.erase (std::remove (s.begin (), s.end (), ' '), s.end ());

    if (!(s[0] == '{' && s[s.length () - 1] == '}')) {
        throw std::runtime_error ("Invalid tuple definition!");
    }
    s = s.substr (1, s.length () - 2);

    size_t j = 0, k = 0;
    while (s.length () > 0) {
        j = s.find (",");
        k = s.find (":");

        if (j == std::string::npos) {
            tempSch.push_back (std::make_pair (s.substr (0, k), s.substr (k + 1)));
            s.clear ();
        } else {
            tempSch.push_back (std::make_pair (s.substr (0, k), s.substr (k + 1, j - k - 1)));
            s = s.substr (j + 1);
        }
    }

    for (auto& a : tempSch) {
        tmp = parseCell (a.second);
        t.push_back (tmp);
        sch.push_back(std::make_pair (a.first, tmp.getType()));
    }

    Table table (sch);
    table += t;

    return table;
}

Predicate parsePredicate (std::string s) {

    std::cout << "Parsing: " << s << std::endl;


    Predicate p;
    BoolExpr e;

    s.erase (std::remove (s.begin (), s.end (), ' '), s.end ());

    size_t i = 0, k = 0;
    while (s.length () > 0) {
        i = s.find (",");
        auto x = find_operation(s);

        k = x.first + operations[x.second].length ();
        e.id = s.substr(0, x.first);
        e.operation = operations[x.second];
        if (i == std::string::npos) {
            e.c = parseCell (s.substr (k));
            s.clear ();
        }
        else {
            e.c = parseCell (s.substr (k, i - k));
            s = s.substr (i + 1);
        }
        p.expressions.push_back (e);
    }

    return p;
}

Expression* parseExpr (std::string s) {
#ifdef DEBUG

    std::cout << "Parsing: " << s << std::endl;

#endif

    s.erase (std::remove (s.begin (), s.end (), ' '), s.end ());

    size_t i = 0;
    Expression *e = nullptr;

    if (s[0] == '{') {
        e = new Expression (parseTuple (s));
    }
    else if (std::islower(s[0])) {
        e = new Expression (s);
    }
    else if ((i = find_unary(s)) != std::string::npos) {
        size_t j = s.find ("]"),
               k = s.find ("[");

        Expression *temp = parseExpr (s.substr (j + 2, s.length () - j - 3));

        if (unary[i] == "SELECT") {
            Predicate p = parsePredicate (s.substr (k + 1, j - k - 1));
            e = new Expression ("SELECT", p, temp);
        }
        else if (unary[i] == "ASSIGN") {
            e = new Expression ("ASSIGN", s.substr (k + 1, j - k - 1), temp);
        }
        else {
            std::vector< std::string > cols;
            std::string temp_str = s.substr (k + 1, j - k - 1);

            size_t idx = 0;
            while (temp_str.length () > 0) {
                idx = temp_str.find (",");
                if (idx == std::string::npos) {
                    cols.push_back (temp_str);
                    temp_str.clear ();
                }
                else {
                    cols.push_back (temp_str.substr (0, idx));
                    temp_str = temp_str.substr (idx + 1);
                }
            }

#ifdef DEBUG

            std::cout << unary[i] << std::endl;

            for (auto& a : cols) {
                std::cout << a;
            } std::cout << std::endl;

#endif

            e = new Expression (unary[i], cols, temp);
        }
    }
    else {

    }

    if (e == nullptr)
        throw std::runtime_error ("Could not parse expression.");

    return e;
}
