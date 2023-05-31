## LOX

LOX is a C++ version of jlox which was implemented in Bob Nystrom’s great [Crafting Interpreters](http://craftinginterpreters.com/) book.

## build

```
$ git clone git@github.com:wy-ei/lox.git
$ cd lox
$ mkdir build && cd build
& cmake ..
& make
```

## run

run in REPL mode:

```
$ ./lox 
> var a = 123;
> var b = 234;
> b + a;
357
> quit
```

run with lox script:

```sh
$ ./lox ./example/sum.lox
5050
```