#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include <iostream>
#include <vector>
#include <string>

#include "cell.hpp"
#include "table.hpp"

namespace RelationalAlgebra {
    enum class BinaryOperation {
        Union, Intersection, SetDifference, Cartesian, NaturalJoin
    };

    enum class BooleanOperation {
        And, Or
    };

    enum class ComparisionOperation {
        Equal, NotEqual, LessThan, GreaterThan, LessThanEqual, GreaterThanEqual
    };

    enum class AggregateOperation {
        Max, Min, Avg, Sum, Count
    };

    enum class StatementOperation {
        StoreTable, CreateView
    };

    class Predicate {
    public:
        virtual bool eval(const Schema&, const Tuple&) const = 0;

        virtual ~Predicate() {}
    };

    class BasicPredicate : public Predicate {
        ComparisionOperation operation;
        std::string operand1, operand2;
        Cell value1, value2;

    public:
        BasicPredicate(const std::string& c1, const ComparisionOperation op, const std::string& c2) :
            operation(op), operand1(c1), operand2(c2) {}

        BasicPredicate(const Cell& c1, const ComparisionOperation op, const Cell& c2) :
            operation(op), value1(c1), value2(c2) {}

        BasicPredicate(const std::string& c1, const ComparisionOperation op, const Cell& c2) :
            operation(op), operand1(c1), value2(c2) {}

        BasicPredicate(const Cell& c1, const ComparisionOperation op, const std::string& c2) :
            operation(op), operand2(c2), value1(c1) {}

        bool eval(const Schema&, const Tuple&) const override;

        ~BasicPredicate() override {}
    };

    class NotPredicate : public Predicate {
        std::unique_ptr<Predicate> pred;

    public:
        NotPredicate(Predicate* p) :
            pred(p) {}

        bool eval(const Schema&, const Tuple&) const override;

        ~NotPredicate() override {}
    };

    class BinaryPredicate : public Predicate {
        BooleanOperation operation;
        std::unique_ptr<Predicate> pred1, pred2;

    public:
        BinaryPredicate(Predicate* p1, const BooleanOperation& op, Predicate* p2) :
            operation(op), pred1(p1), pred2(p2) {}

        bool eval(const Schema&, const Tuple&) const override;

        ~BinaryPredicate() override {}
    };

    class Expression {
    public:
        virtual Table eval(void) const = 0;

        virtual ~Expression() {}
    };

    class TupleExpression : public Expression {
        Table tuple;

    public:
        TupleExpression(const Table& tup) :
            tuple(tup) {}

        Table eval(void) const override;

        ~TupleExpression() override {}
    };

    class TableExpression : public Expression {
        std::string table_name;

    public:
        TableExpression(const std::string& name) :
            table_name(name) {}

        Table eval(void) const override;

        ~TableExpression() override {}
    };

    class BinaryExpression : public Expression {
        BinaryOperation operation;
        std::unique_ptr<Expression> expression1, expression2;

    public:
        BinaryExpression(Expression* e1, const BinaryOperation& op, Expression* e2):
            operation(op), expression1(e1), expression2(e2) {}

        Table eval(void) const override;

        ~BinaryExpression() override {}
    };

    class SelectExpression : public Expression {
        std::unique_ptr<Predicate> predicate;
        std::unique_ptr<Expression> expression;

    public:
        SelectExpression(Predicate* p, Expression* e) :
            predicate(p), expression(e) {};

        Table eval(void) const override;

        ~SelectExpression() override {}
    };

    class ProjectExpression : public Expression {
        std::vector<std::string> columns;
        std::unique_ptr<Expression> expression;

    public:
        ProjectExpression(const std::vector<std::string>& cols, Expression* e) :
            columns(cols), expression(e) {};

        Table eval(void) const override;

        ~ProjectExpression() override {}
    };

    class AggregateExpression : public Expression {
        AggregateOperation operation;
        std::string column;
        std::unique_ptr<Expression> expression;

    public:
        AggregateExpression(const AggregateOperation& op, const std::string& col, Expression* e) :
            operation(op), column(col), expression(e) {};

        Table eval(void) const override;

         ~AggregateExpression() override {}
    };

    class RenameExpression : public Expression {
        std::vector<std::string> new_names;
        std::unique_ptr<Expression> expression;

    public:
        RenameExpression(const std::vector<std::string>& names, Expression* e) :
            new_names(names), expression(e) {}

        Table eval(void) const override;

        ~RenameExpression() override {}
    };

    class Statement {
    public:
        virtual void exec(void) const = 0;

        virtual ~Statement() {}
    };

    class ExpressionStatement : public Statement {
        std::unique_ptr<Expression> expression;

    public:
        ExpressionStatement(Expression* exp) :
            expression(exp) {}

        void exec(void) const override;

        ~ExpressionStatement() override {}
    };

    class AssignStatement : public Statement {
        std::string expression, name;

    public:
        AssignStatement(const std::string& n, const std::string& exp) :
            expression(exp), name(n) {}

        void exec(void) const override;

        ~AssignStatement() override {}
    };

    class StoreStatement : public Statement {
        std::unique_ptr<Expression> expression;
        std::string name;

    public:
        StoreStatement(const std::string& n, Expression* exp) :
            expression(exp), name(n) {}

        void exec(void) const override;

        ~StoreStatement() override {}
    };
};

#endif
