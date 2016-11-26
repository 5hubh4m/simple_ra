:drop student

[student] <- RENAME[s_id, name, dept_id]({1, "Shubham Chaudhary", 1})
[student] <- student + {2, "Shivam Garg", 1}
[student] <- student + {3, "Manish Singh", 2}
[student] <- student + {4, "Sourabh Rajendu", 1}
[student] <- student + {5, "Bharat Khanna", 3}
[student] <- student + {6, "Arjun Malik", 4}
[student] <- student + {7, "Anant Dadu", 3}
[student] <- student + {8, "Atishay Jain", 3}
[student] <- student + {9, "Ayush Shrivastava", 2}

:drop department

[department] <- RENAME[dept_id, dept_name]({1, "Computer Science & Engg."})
[department] <- department + {2, "Electronics Engg."}
[department] <- department + {3, "Chemical Engg."}
[department] <- department + {4, "Mechanical Engg."}

:drop marks

[marks] <- RENAME[s_id, eng, phy]({1, 98.0, 95.0})
[marks] <- marks + {2, 89.0, 99.0}
[marks] <- marks + {3, 98.1, 92.0}
[marks] <- marks + {4, 67.0, 100.0}
[marks] <- marks + {5, 89.0, 99.0}

[min_eng] ~ PROJECT [s_id] (marks @ RENAME [eng] (AGGREGATE [MIN :eng] (marks)))

SELECT[(s_id > 3 && name =/= "Shubham Chaudhary") || s_id <= 3](student)

student @ department
(min_eng @ student) @ department

:quit
