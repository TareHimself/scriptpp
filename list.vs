
let x = List(1,2,3,4,5);

let z = x.map(fn (a,i,arr){
    return arr[arr.size() - (i + 1)];
});

let f = z.filter(fn (a){
    return a >= 3;
});

fn filterFn(a){
    return a >= 3;
}

let g = z.filter(filterFn);

print(z,f,g);
for(let i = 0; i < z.size(); i = i + 1){
    print(x[i],z[i]);
}


