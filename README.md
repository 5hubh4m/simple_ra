# simple-ra
A simple implementation of Relational Algebra.

### Compiling and running
Go to the directory, and just use `make` to compile and `./simple_ra` or `make run` to run. The package `libreadline` (`libreadline-devel` or `libreadline-dev`) is required to compile and run the program. You can install it using the package manager of your operating system.

### Syntax
The syntax is totally inspired from pure RA. However, to make it easier to type using an ASCII
keyboard, some changes have been made. Below is a brief description of the syntax.

* `table_name` : Displays whole view/table from a view/table named `table_name`

* `SELECT [Predicate] (Expression)` : Select using a predicate from expression. Predicate is a list of elementary boolean expressions. The following predcate expressiona are valid

>`value > value`

>`value >= value`

>`value < value`

>`value <= value`

>`value = value`

>`value /= value`

Here, a `value` is either a column name or a value. Operating on 2 values of different `type` will result in an error.

* `PROJECT [List of columns / Aggregate functions] (Expression)` : Project said columns from the expression, including the following aggregate functions

>`MAX : column name`

>`MIN : column name`

>`SUM : column name`, here column name should be of numeric type

>`AVG : column name`, here column name should be of numeric type

>`COUNT : column name : value`

* `ASSIGN [View name] (Expression)` : Create a view with the given name

* `STORE [Table name] (Expression)` : Store the relation with the given name in the database

* `(Expression) X (Expression)` : Cartesian product

* `(Expression) U (Expression)` : Union

* `(Expression) ^ (Expression)` : Intersection

* `(Expression) @ (Expression)` : Natural join

* `(Expression) - (Expression)` : Set difference

###Example

Some example queries have been given below.

`ASSIGN[student](RENAME[s_id, f_name, l_name, dept_id]({1, "Shubham", "Chaudhary", 1}))`

`ASSIGN[student]((student) U ({2, "Shivam", "Garg", 1}))`

`ASSIGN[department](RENAME[dept_id, dept_name]({1, "Computer"}))`

`SELECT[s_id > 3, f_name /= "Shubham"](student)`

`(student) @ (department)`

`STORE[marks](RENAME[s_id, eng, phy]({1, 98.0, 95.0}))`

`STORE[marks]((marks) U ({2, 89.0, 99.0}))`

`STORE[marks]((marks) U ({3, 98.1, 92.0}))`

`PROJECT[MIN : eng](marks)`

`ASSIGN[min_eng](PROJECT[s_id]((marks) @ (RENAME[s_id, eng]((PROJECT[s_id](marks)) X (PROJECT[MIN : eng](marks))))))`

__Explanation__: It selects the `s_id` of the student with minimum marks in `english`. It then takes it's cartesian product with a column of `s_id`s and natural joins it with marks table to obtain the `s_id` of student with minimum marks in english. It then assigns the `min_eng` with the view.

`((student) @ (min_eng)) @ (department)` will give all the details of the student with minimum marks in english. The result of the query will change in accordance with the original data.

Other queries have been given in file `queries.sra`. Formal syntax is defined in EBNF form in `syntax.ebnf`.

### Helper Commands
The following utility commands are available

> `:quit` : Quit the program

> `:help` : Show program help

> `:drop <table>` : Drop the table with specified name

> `:showall` : Show the basic information of the whole database, i.e the schema and no. of tuples of all tables and description of all views.

### Precautions
There have been efforts to increase the code's coverage using runtime exception handling, as a result of which the software is pretty robust. However, there might be some corner cases where the code breaks. Sorry for the inconvenience. To help debugging easier, you can enable informative outputs embedded throughout the codes by using `make debug` while compiling.

Still, here is a list of identified causes of errors:
* Using `ASSIGN` or `STORE` inside itself, and referring to itself. Like `ASSIGN[hello](PROJECT[id](ASSIGN[hello](hi)))` may cause errors. But they will be caught and reported.

### Caveats
* The maximum length of any `string`(`identifier` or `value`) is `100 bytes`.
* Because of the bad parser function, `string`s cannot have spaces. `Hello, world.` will become `Hello, world.`. Also, there are no escape characters.
* `null` value is parsed but other functionality like evaluation or aggregate query handling is not implemented.
