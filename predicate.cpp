#include <string>

#include "cell.hpp"
#include "table.hpp"
#include "predicate.hpp"


Predicate::Predicate () {
    type = INVALID;
}

Predicate::Predicate (std::string o1, Cell o2, std::string opn) {
    type = SIMPLE;
    pred.simple_pred.operand1 = o1;
    pred.simple_pred.operation = opn;
    pred.simple_pred.constant = o2;
    pred.simple_pred.type = W_CONSTANT;
}

Predicate::Predicate (std::string o1, std::string o2, std::string opn) {
    type = SIMPLE;
    pred.simple_pred.operand1 = o1;
    pred.simple_pred.operation = opn;
    pred.simple_pred.operand2 = o2;
    pred.simple_pred.type = BINARY;
}

Predicate::Predicate (Predicate* p1, Predicate* p2, std::string opn) {
    type = COMPOSITE;
    pred.comp_pred.pred1 = p1;
    pred.comp_pred.pred2 = p2;
    pred.comp_pred.operation = opn;
}

bool Predicate::eval (Schema schema, Tuple tuple) {
    if (type == SIMPLE) {
        Cell d1, d2;

        if (pred.simple_pred.type == 0) {
            d1 = tuple[schema[pred.simple_pred.operand1].first];
            d2 = tuple[schema[pred.simple_pred.operand2].first];
        } else {
            d2 = pred.simple_pred.constant;
        }

        if (pred.simple_pred.operation ==  "=")
            return d1 == d2;
        else if (pred.simple_pred.operation == "/=")
            return d1 != d2;
        else if (pred.simple_pred.operation == ">")
            return d1 > d2;
        else if (pred.simple_pred.operation == "<")
            return d1 < d2;
        else if (pred.simple_pred.operation == "<=")
            return d1 <= d2;
        else if (pred.simple_pred.operation == ">=")
            return d1 >= d2;

        return false;
    } else {
        if (pred.comp_pred.operation == "&&")
            return pred.comp_pred.pred1 -> eval (schema, tuple) && pred.comp_pred.pred2 -> eval (schema, tuple);
        if (pred.comp_pred.operation == "||")
            return pred.comp_pred.pred1 -> eval (schema, tuple) || pred.comp_pred.pred2 -> eval (schema, tuple);
        if (pred.comp_pred.operation == "&&!")
            return pred.comp_pred.pred1 -> eval (schema, tuple) && !(pred.comp_pred.pred2 -> eval (schema, tuple));
        if (pred.comp_pred.operation == "||!")
            return pred.comp_pred.pred1 -> eval (schema, tuple) || !(pred.comp_pred.pred2 -> eval (schema, tuple));
        if (pred.comp_pred.operation == "!&&")
            return !(pred.comp_pred.pred1 -> eval (schema, tuple)) && pred.comp_pred.pred2 -> eval (schema, tuple);
        if (pred.comp_pred.operation == "!||")
            return !(pred.comp_pred.pred1 -> eval (schema, tuple)) || pred.comp_pred.pred2 -> eval (schema, tuple);
        if (pred.comp_pred.operation == "!&&!")
            return !(pred.comp_pred.pred1 -> eval (schema, tuple)) && !(pred.comp_pred.pred2 -> eval (schema, tuple));
        if (pred.comp_pred.operation == "!||!")
            return !(pred.comp_pred.pred1 -> eval (schema, tuple)) || !(pred.comp_pred.pred2 -> eval (schema, tuple));

        return false;
    }
}
