#ifndef CELL_HPP
#define CELL_HPP

#include <string>
#include <array>


/* Type Enum
 *
 * Stores the type of the data in a cell of a table
 */
enum Type {
    INTEGER, FLOAT, STRING, INVALID
};


/* Data Union
 *
 * Stores the data for a cell of a table
 */
union Data {
    int i;
    float f;
    char s[100];
};

/* Cell Class
 *
 * Defines the behaviour of a single cell of a table
 */
class Cell {
  private:
    Data value;            // Data in the cell
    Type type;             // Type of data in cell

  public:
    Cell ();               // Ctors
    Cell (int);
    Cell (std::string);
    Cell (float);

    Type getType () const;          // Return the type
    Data getVal () const;           // Return value
    std::string show () const;      // Pretty print the value

    bool operator == (const Cell&) const;  // Comparator functions
    bool operator <= (const Cell&) const;
    bool operator > (const Cell&) const;
    bool operator < (const Cell&) const;
    bool operator != (const Cell&) const;
    bool operator >= (const Cell&) const;
};

#endif
