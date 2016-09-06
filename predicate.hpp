#ifndef PREDICATE_HPP
#define PREDICATE_HPP

#include <string>

#include "cell.hpp"
#include "table.hpp"

enum SimPredType {
    BINARY, W_CONSTANT
};

class Predicate {
  private:
    enum {
        SIMPLE, COMPOSITE, INVALID
    } type;
    struct {
        struct {
            std::string operand1, operand2, operation;
            Cell constant;
            SimPredType type;
        } simple_pred;
        struct {
            Predicate *pred1, *pred2;
            std::string operation;
        } comp_pred;
    } pred;

  public:
    Predicate ();
    Predicate (std::string o1, Cell o2, std::string opn);
    Predicate (std::string o1, std::string o2, std::string opn);
    Predicate (Predicate* p1, Predicate* p2, std::string opn);

    bool eval (Schema schema, Tuple tuple);
};

#endif
