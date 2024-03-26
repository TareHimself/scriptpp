

fn subOne(value){
    return value - 1;
}

let f = List(1,2).map(subOne).map(subOne).map(subOne);

let x = null;
x = f.map(subOne);

print(f,x);