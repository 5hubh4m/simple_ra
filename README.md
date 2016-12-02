# simple-ra
A simple implementation of Relational Algebra.

### Compiling and running

Go to the directory, and just use `make` to compile and `./simple_ra` or `make run` to run. The package `libreadline` (`libreadline-devel` or `libreadline-dev`) is required to compile and run the program. You can install it using the package manager of your operating system.

### Syntax

The syntax is totally inspired from pure RA. However, to make it easier to type using an ASCII
keyboard, some changes have been made. Below is a brief description of the syntax.

* `table_name;` : Displays whole view/table from a view/table named `table_name`

* `SELECT [predicate] (expression);` : Select using a predicate from expression. Predicate is a list of elementary boolean expressions. The following predicate expressions are valid

>`value > value`

>`value >= value`

>`value < value`

>`value <= value`

>`value = value`

>`value =/= value`

>`value > 4 && value < 3`

>`!(value > 4 && value < 3) || greeting =/= "Hello" `

Here, a `value` is either a column name or a value. Operating on two values of different `type` will result in an error.

* `PROJECT [comma_seperated_column_names] (expression);` : Project said columns from the expression.

>`PROJECT [s_id, name] (student);`

* `RENAME [comma_seperated_column_names] (expression);` : Rename said columns from the expression.

* `AGGREGATE [function : column] (expression);` : Run the aggregate function `function` on the column `column`. The following functions are built-in

>`MAX : column_name`

>`MIN : column_name`

>`SUM : column_name`, here column name should be of numeric type

>`AVG : column_name`, here column name should be of numeric type

>`COUNT : column_name`, displays the count of each value in the relation.

* `expression * expression;` : Cartesian product

* `expression + expression;` : Union

* `expression ^ expression;` : Intersection

* `expression @ expression;` : Natural join

* `expression - expression;` : Set difference

* `[view_name] ~ expression;` : Create a view with the identifier `view_name`.

* `[table_name] <- expression;` : Store the relation with name `table_name` in the database.

###Example

Some example queries have been given below.

`[student] <- RENAME [s_id, f_name, l_name, dept_id] ({1, "Shubham", "Chaudhary", 1});`

`[student] <- student ^ {2, "Shivam", "Garg", 1};`

`SELECT [s_id > 3 && f_name =/= "Shubham"] (student);`

`student @ department;`

`[marks] <- RENAME [s_id, eng, phy] ({1, 98.0, 95.0});`

`[marks] <- marks + {3, 98.1, 92.0};`

`PROJECT [AVG : eng] (marks);`

`[min_eng] ~ PROJECT [s_id] (marks @ RENAME [eng] (marks));`

__Explanation__: It then assigns the `min_eng` with the view containing the `s_id`s of the `student`s with minimum marks in `eng`.

`student @ min_eng @ department;` will give all the details of the student with minimum marks in english. The result of the query will change in accordance with the original data.

Other queries have been given in file `queries.sra`. Formal syntax is defined in EBNF form in `syntax.ebnf`.

### Helper Commands

The following utility commands are available

> `:quit;` : Quit the program

> `:help;` : Show program help

> `:drop table_ name;` : Drop the table with specified name

> `:showall;` : Show the basic information of the whole database, i.e the schema and no. of tuples of all tables and description of all views.
