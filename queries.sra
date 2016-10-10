:drop student
ASSIGN[student](RENAME[s_id, f_name, l_name, dept_id]({1, "Shubham", "Chaudhary", 1}))
ASSIGN[student]((student) U ({2, "Shivam", "Garg", 1}))
ASSIGN[student]((student) U ({3, "Manish", "Singh", 2}))
ASSIGN[student]((student) U ({4, "Sourabh", "Rajendu", 1}))
ASSIGN[student]((student) U ({5, "Bharat", "Khanna", 3}))
ASSIGN[student]((student) U ({6, "Arjun", "Malik", 4}))
ASSIGN[student]((student) U ({7, "Anant", "Dadu", 3}))
ASSIGN[student]((student) U ({8, "Atishay", "Jain", 3}))
ASSIGN[student]((student) U ({9, "Ayush", "Shrivasta", 2}))
:drop department
ASSIGN[department](RENAME[dept_id, dept_name]({1, "Computer"}))
ASSIGN[department]((department) U ({2, "Electronics"}))
ASSIGN[department]((department) U ({3, "Chemical"}))
ASSIGN[department]((department) U ({4, "Mechanical"}))
SELECT[s_id > 3](student)
(student) @ (department)
:quit
