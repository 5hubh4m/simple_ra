# simple-ra
A simple implementation of Relational Algebra.

### Compiling and running
Go to the directory, and just use `make` to compile and `./simple_ra` to run.

### Syntax
The syntax is totally inspired from pure RA. However, to make it easier to type using an ASCII
keyboard, some changes have been made. Below is a brief description of the syntax.

* `SELECT [Predicate] (Expression)` : Select using a predicate from expression

* `PROJECT [List of columns / Aggregate functions] (Expression)` : Project said columns from the expression

* `ASSIGN [View name] (Expression)` : Create a view with the given name

* `STORE [Table name] (Expression)` : Store the relation with the given name in the database

* `(Expression) X (Expression)` : Cartesian product

* `(Expression) U (Expression)` : Union

* `(Expression) ^ (Expression)` : Intersection

* `(Expression) @ (Expression)` : Natural join

* `(Expression) - (Expression)` : Set difference

Some example queries have been given below. 

`ASSIGN[student](RENAME[s_id, f_name, l_name, dept_id]({1, "Shubham", "Chaudhary", 1}))`

`ASSIGN[student]((student) U ({2, "Shivam", "Garg", 1}))`

`ASSIGN[department](RENAME[dept_id, dept_name]({1, "Computer"}))`

`SELECT[s_id > 3](student)`

`(student) @ (department)`

`STORE[marks](RENAME[s_id, eng, phy]({1, 98.0, 95.0}))`
`STORE[marks]((marks) U ({2, 89.0, 99.0}))`
`STORE[marks]((marks) U ({3, 98.1, 92.0}))`

`PROJECT[MIN : eng](marks)`

`ASSIGN[min_eng](PROJECT[s_id]((marks) @ (RENAME[s_id, eng]((PROJECT[s_id](marks)) X (PROJECT[MIN : eng](marks))))))`

__Explanation__: It selects the `s_id` of the student with minimum marks in `english`. It then takes it's cartesian product with a column of `s_id`s and natural joins it with marks table to obtain the `s_id` of student with minimum marks in english. It then assigns the `min_eng` with the view.

`((student) @ (min_eng)) @ (department)` will give all the details of the student with minimum marks in english. 

Other queries have been given in file `queries.sra`. Formal syntax is defined in EBNF form in `syntax.ebnnf`.

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
