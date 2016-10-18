:drop student
STORE[student](RENAME[s_id, f_name, l_name, dept_id]({1, "Shubham", "Chaudhary", 1}))
STORE[student]((student) U ({2, "Shivam", "Garg", 1}))
STORE[student]((student) U ({3, "Manish", "Singh", 2}))
STORE[student]((student) U ({4, "Sourabh", "Rajendu", 1}))
STORE[student]((student) U ({5, "Bharat", "Khanna", 3}))
STORE[student]((student) U ({6, "Arjun", "Malik", 4}))
STORE[student]((student) U ({7, "Anant", "Dadu", 3}))
STORE[student]((student) U ({8, "Atishay", "Jain", 3}))
STORE[student]((student) U ({9, "Ayush", "Shrivastava", 2}))
:drop department
STORE[department](RENAME[dept_id, dept_name]({1, "Computer"}))
STORE[department]((department) U ({2, "Electronics"}))
STORE[department]((department) U ({3, "Chemical"}))
STORE[department]((department) U ({4, "Mechanical"}))
:drop marks
STORE[marks](RENAME[s_id, eng, phy]({1, 98.0, 95.0}))
STORE[marks]((marks) U ({2, 89.0, 99.0}))
STORE[marks]((marks) U ({3, 98.1, 92.0}))
STORE[marks]((marks) U ({4, 67.0, 100.0}))
STORE[marks]((marks) U ({5, 89.0, 99.0}))
ASSIGN[min_eng](PROJECT[s_id]((marks) @ (RENAME[s_id, eng]((PROJECT[s_id](marks)) X (PROJECT[MIN : eng](marks))))))
SELECT[s_id > 3](student)
(student) @ (department)
min_eng
:quit