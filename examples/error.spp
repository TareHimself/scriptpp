fn thisWillThrow(){
    throw "YOOOOOO";
}

fn foo(){
    thisWillThrow();
}

fn bar(){
    foo();   
}

for(let i = 0; i < 3; i = i + 1){
    try{
        bar();
    }
    catch(e){
        print("Caught",e);
    }
}