let i = Thread(fn (){
    print("This is from a thread");
});

i.start();

i.join();