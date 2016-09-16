:drop student
ASSIGN[student](RENAME[s_roll, f_name, l_name, dept_id]({1, "Shubham", "Chaudhary", 1}))
ASSIGN[student]((student) U ({2, "Shivam", "Garg", 1}))
ASSIGN[student]((student) U ({3, "Manish", "Singh", 2}))
ASSIGN[student]((student) U ({4, "Sourabh", "Rajendu", 1}))
ASSIGN[student]((student) U ({5, "Bharat", "Khanna", 2}))
ASSIGN[student]((student) U ({6, "Arjun", "Malik", 1}))
ASSIGN[student]((student) U ({7, "Anant", "Dadu", 2}))
ASSIGN[student]((student) U ({8, "Atishay", "Jain", 1}))
ASSIGN[student]((student) U ({9, "Ayush", "Shrivasta", 2}))
ASSIGN[student]((student) U ({10, "Mehak", "Goyal", 1}))
ASSIGN[student]((student) U ({11, "Ayush", "Kedia", 2}))
ASSIGN[student]((student) U ({12, "Ishank", "Arora", 2}))
ASSIGN[student]((student) U ({13, "Divyam", "Goyal", 2}))
ASSIGN[student]((student) U ({14, "Himanshu", "Agarwal", 2}))
ASSIGN[student]((student) U ({15, "Devang", "Kulshresht", 1}))

:drop department
ASSIGN[department](RENAME[dept_no, dept_name]({1, "CSE"}))
ASSIGN[department]((department) U ({2, "ECE"}))

:quit
