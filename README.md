# simple-ra
A simple implementation of Relational Algebra.

### Compiling and running
Go to the directory, and just use `make` to compile and `./simple_ra` to run.

### Syntax
The syntax is totally inspired from pure RA. However, to make it easier to type using an ASCII keyboard, some changes have been made. Example queries have been given below. 

`ASSIGN[student](RENAME[s_id, f_name, l_name, dept_id]({1, "Shubham", "Chaudhary", 1}))`

`ASSIGN[student]((student) U ({2, "Shivam", "Garg", 1}))`

`ASSIGN[department](RENAME[dept_id, dept_name]({1, "Computer"}))`

`SELECT[s_id > 3](student)`

`(student) @ (department)`

Other queries have been given in file `queries.sra`.

### Helper Commands
The following utility commands are available

> `:quit` : Quit the program

> `:help` : Show program help

> `:drop <table>` : Drop the table with specified name

> `:showall` : Show the basic information of the whole database

### Errors
There have been efforts to increase the code's coverage using runtime exception handling, as a result of which the software is pretty robust. However, there might be some corner cases where the code breaks. Sorry for the inconvenience. To help debugging easier, you can enable informative outputs embedded throughout the codes by using `make debug` while compiling.
