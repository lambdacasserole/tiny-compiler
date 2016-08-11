# Tiny Compiler
Very tiny compiler for a simple arithmetic language. 

This is a tiny compiler which produces assembler code targeting [a particular very simple stack machine](http://www.staff.science.uu.nl/~dijks106/SSM/) out of an even simpler arithmetic language. Put together for an experiment in compiler verification.

## Building
Builds a treat under Windows using the [Tiny C Compiler](http://bellard.org/tcc/) by the one and only [Fabrice Bellard](https://en.wikipedia.org/wiki/Fabrice_Bellard).

```
tcc tiny.c
```

## Source Language
The source language consists of an arbitrarily deep tree of expressions. An expression may take the form of a number or three-member tuple containing an operator followed by exactly two operands which must each themselves be expressions. In EBNF:

```
digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9".
number = digit { digit }.
operator = "add" | "sub" | "mul" | "div".
expression = number | ( "(" operator expression expression ")" ).
```

This grammar is written to be compatible with [a piece of visualization software](http://dotnet.jku.at/applications/visualizer/) developed by a team at the University of Linz. To save you downloading it and rendering the grammar, here it is visually:

![Grammar](https://github.com/lambdacasserole/tiny-compiler/raw/master/grammar/grammar.png)

## Target Language
The target for this compiler is the [Simple Stack Machine](http://www.staff.science.uu.nl/~dijks106/SSM/). The source code:

```
(add 1 2)
```

Will produce the following result:

```
ldc 1
ldc 2
add
```

This will push two constants onto the stack (using the `ldc` command) and then add them together, pushing the result back on to the stack.

## Limitations
This 'compiler' obviously has a few limitations:

* The language obviously isn't a serious development language. 
* Memory currently isn't deallocated before program exit so your OS better do it for you. This may be addressed in future.
