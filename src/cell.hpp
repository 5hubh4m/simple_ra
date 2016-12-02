#ifndef CELL_HPP
#define CELL_HPP

#include <string>
#include <array>

namespace RelationalAlgebra {
    /* Type Enum
     *
     * Stores the type of the data in a cell of a table
     */
    enum class DataType {
        Integer, Float, String, Null
    };


    /* Data Union
     *
     * Stores the data for a cell of a table
     */
    struct Data {
        int i;
        float f;
        std::string s;

        Data() {}
        Data(int i) : i(i) {}
        Data(float f) : f(f) {}
        Data(const std::string& str) : s(str) {} 
    };

    /* Cell Class
     *
     * Defines the behaviour of a single cell of a table
     */
    class Cell {
      private:
        DataType type;         // Type of data in cell
        Data value;            // Data in the cell

      public:
        // Constructors
        Cell() : type(DataType::Null) {}
        Cell(int i) : type(DataType::Integer), value(i) {}
        Cell(std::string s) : type(DataType::String), value(s) {}
        Cell(float f) : type(DataType::Float), value(f) {} 

        const DataType& getType() const;      // Return the type
        const Data& getVal() const;           // Return value
        std::string show() const;             // Pretty print the value

        bool operator == (const Cell&) const;  // Comparator functions
        bool operator <= (const Cell&) const;
        bool operator > (const Cell&) const;
        bool operator < (const Cell&) const;
        bool operator != (const Cell&) const;
        bool operator >= (const Cell&) const;
    };
};

#endif
