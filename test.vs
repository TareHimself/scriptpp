
let b = 20;
let a = 50 * 40 * 45;

fn foo(){
    a = 15;
    print(foo,_);
    return "ssdn";
}

let _ = fn (){
    print(foo);
};

print(when{
a == b -> a + " and " + b + " are equal";
a < b -> a + " is less than " + b;
a > b -> a + " is greater than " + b;
},"|",b,"|",a,"|",_);

foo();

print(b,"|",a);