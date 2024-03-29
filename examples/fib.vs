// Translated from https://www.geeksforgeeks.org/python-program-to-print-the-fibonacci-sequence/


// fn fib(n){
//     return when{
//         n < 0 -> throw "Incorrect input";
//         n == 0 -> 0;
//         (n == 1 || n == 2) -> 1;
//         else -> fib(n - 1) + fib(n - 2);
//     };
// }

// print(fib(20));


class NetworkPacket {
    let location = List(0,0,0);

    fn serialize(){
        let data = "";
        data = location.join("|");
        return data; // "0|0|0".split("|") // ["0","0","0"];
    }

    fn fromBytes(){

    }
};

let data = NetworkPacket().serialize();
print(data);
