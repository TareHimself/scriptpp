// Translated from https://www.geeksforgeeks.org/python-program-to-print-the-fibonacci-sequence/

fn fib(n){
    return when{
        n < 0 -> print("Incorrect Input");
        n == 0 -> 0;
        (n == 1 || n == 2) -> 1;
        else -> fib(n - 1) + fib(n - 2);
    };
}

print(fib(20));