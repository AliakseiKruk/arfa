Arithmetic calculator without recursion.

Supports only integers, +, -, *, /.

It was made just for fun. 

The main idea is algoritm without recursion and without deep limits for brackets.

$ gcc main.c

$ chmod +x a.out

$ echo '(((1)))' | ./a.out
1.000000

$ echo '(((22+8)))' | ./a.out
30.000000

$ echo '((22+8/2)+1)*17' | ./a.out 
459.000000

