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
digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";
number = digit, { digit };
operator = "add" | "sub" | "mul" | "div";
expression = number | ( "(", operator, expression, expression, ")" );
```

## Limitations
This 'compiler' obviously has a few limitations:

* The language obviously isn't a serious development language. 
* Memory currently isn't deallocated before program exit so your OS better do it for you. This may be addressed in future.
