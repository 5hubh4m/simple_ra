#include <iostream>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#include "expression.hpp"
#include "cell.hpp"
#include "table.hpp"
#include "parser.hpp"
#include "simple_ra.hpp"

int is_val (std::string a) {
    a.erase (std::remove (a.begin (), a.end (), ' '), a.end ());

    bool is_null = (a == "null");

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
    if (is_null)
        return 4;

    return 0;
}

size_t find_unary(const std::string& s) {
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

#ifdef DEBUG
        std::cout << "Parsing for operation: " << s << std::endl;
#endif

    size_t i;
    for (size_t z = 0; z < operations.size (); z++) {
#ifdef DEBUG
        std::cout << "Checking for operation: " << operations[z] << std::endl;
#endif

        i = s.find (operations[z]);

        if (i != std::string::npos) {
#ifdef DEBUG
        std::cout << "Found operation: " << operations[z] << std::endl;
#endif

            p.first = i;
            p.second = z;
            break;
        }
    }

    if (p.first == std::string::npos && p.second == std::string::npos) {
        throw std::runtime_error ("No comparision operator found.");
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
        default:
            return Cell ();
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
    std::vector < std::string > tempSch;

    s.erase (std::remove (s.begin (), s.end (), ' '), s.end ());

    if (!(s[0] == '{' && s[s.length () - 1] == '}')) {
        throw std::runtime_error ("Invalid tuple definition!");
    }
    s = s.substr (1, s.length () - 2);

    size_t j = 0;
    while (s.length () > 0) {
        j = s.find (",");

        if (j == std::string::npos) {
            tempSch.push_back (s);
            s.clear ();
        } else {
            tempSch.push_back (s.substr (0, j));
            s = s.substr (j + 1);
        }
    }

    srand (time (nullptr));

    for (auto& a : tempSch) {
        tmp = parseCell (a);
        t.push_back (tmp);
        sch.push_back(std::make_pair (std::to_string (rand ()), tmp.getType()));
    }

    Table table (sch);
    table += t;

    return table;
}

bool hasAgg (const std::vector< std::string >& cols) {
#ifdef DEBUG
    for (auto& a : cols)
        std::cout << "Parsing columns: " << a << std::endl;
#endif

    size_t num_agg = 0;
    for (auto& b : cols) {
        for (auto& a : aggregate) {
            if (b.substr (0, a.length ()) == a) {
                num_agg++;
                break;
            }
        }
    }

    if (num_agg > 0 && num_agg < cols.size ())
        std::runtime_error ("Invalid expression, can't mix aggregate and non-aggregate in project.");

    if (num_agg > 0)
        return true;

    return false;
}

Aggregate parseAgg (std::string s) {
#ifdef DEBUG
    std::cout << "Parsing: " << s << std::endl;
#endif

    Aggregate agg;

    for (auto& a : aggregate) {
        if (s.substr (0, a.length ()) == a) {
            agg.operation = a;
            break;
        }
    }

    s = s.substr (agg.operation.length () + 1);

    if (agg.operation == COUNT) {
        size_t idx = s.find (":");
        agg.col_name = s.substr (0, idx);
        agg.value = parseCell (s.substr (idx + 1));
    }
    else {
        agg.col_name = s;
    }

#ifdef DEBUG
    std::cout << "Parsed Aggregate: " << agg.operation << " " << agg.col_name;

    if (agg.operation == COUNT)
        std::cout << " " << agg.value.show ();

    std::cout << std::endl;

#endif

    return agg;
}

Predicate parsePredicate (std::string s) {
    Predicate p;
    BoolExpr e;

    s.erase (std::remove (s.begin (), s.end (), ' '), s.end ());

#ifdef DEBUG
    std::cout << "Parsing: " << s << std::endl;
#endif

    size_t i = 0, k = 0;
    while (s.length () > 0) {
        i = s.find (",");
        auto x = find_operation(s);

        k = x.first + operations[x.second].length ();
        e.id1 = s.substr(0, x.first);
        e.operation = operations[x.second];
        if (i == std::string::npos) {
            e.id2 = s.substr (k);
            s.clear ();
        }
        else {
            e.id2 = s.substr (k, i - k);
            s = s.substr (i + 1);
        }

#ifdef DEBUG
        std::cout << "Parsed Predicate: "
            << e.id1 << " " << e.operation << " " << e.id2 << std::endl;
#endif

        p.expressions.push_back (e);
    }

    return p;
}

Expression parseExpr (std::string s) {
#ifdef DEBUG
    std::cout << "Parsing: " << s << std::endl;
#endif
    Expression e;

    s.erase (std::remove (s.begin (), s.end (), ' '), s.end ());

    size_t i = 0;

    if (s[0] == '{') {
        e = Expression (parseTuple (s));
    }
    else if (std::islower(s[0])) {
        e = Expression (s);
    }
    else if ((i = find_unary(s)) != std::string::npos) {
        size_t j = s.find ("]"),
               k = s.find ("[");

        Expression temp = parseExpr (s.substr (j + 2, s.length () - j - 3));

        if (unary[i] == SELECT) {
            Predicate p = parsePredicate (s.substr (k + 1, j - k - 1));
            e = Expression (SELECT, p, temp);
        }
        else if (unary[i] == ASSIGN) {
            e = Expression (ASSIGN, s.substr (k + 1, j - k - 1), temp);
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
                std::cout << a << " ";
            } std::cout << std::endl;
#endif

            // If project and aggregate expression is encountered
            if (unary[i] == PROJECT && hasAgg (cols)) {
                std::vector< Aggregate > aggregate_options;

                for (auto& a : cols) {
                    aggregate_options.push_back (parseAgg (a));
                }

                e = Expression (unary[i], aggregate_options, temp);
            }
            else {
                e = Expression (unary[i], cols, temp);
            }
        }
    }
    else {
        std::vector< char > st;
        st.push_back (s[0]);
        size_t i = 1;
        while (st.size () > 0 && i < s.length ()) {
            if (s[i] == '(')
                st.push_back ('(');
            else if (s[i] == ')')
                if (st.back () == '(')
                    st.pop_back ();
            i++;
        }

        if (st.size () > 0)
            throw std::runtime_error ("Ill formed query. Use proper brackets for expressions.");

        bool found = false;
        for (auto& c : binary) {
            if (s.substr (i, 1) == c) {
                found = true;
                break;
            }
        }

        if (!found) {
            throw std::runtime_error ("Invalid operator " + s.substr(i, 1) + ".");
        }
        else {
            Expression temp1 = parseExpr (s.substr (1, i - 2)),
                       temp2 = parseExpr (s.substr (i + 2, s.length () - i - 3));

            e = Expression (s.substr(i, 1), temp1, temp2);
        }
    }

#ifdef DEBUG

    e.printExpr ();

#endif

    return e;
}
