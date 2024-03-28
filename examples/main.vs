fn main(){
    let x = "Let my people go"; // YO
    

    print(x.size(),x.split().map(fn (str){
        return "Nested splitting".split();
    }));
    for(let i = 0; i < 10; i = i + 1){
        print("Let my people go",i);
        continue;
    }
}

main();
 