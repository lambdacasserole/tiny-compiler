# tiny-compiler
Very tiny compiler for a simple arithmetic language. 

This is a tiny compiler which produces assembler code targeting [a particular very simple stack machine](https://github.com/lambdacasserole/simple-stack-machine) out of an even simpler arithmetic language. Put together for an experiment in compiler verification.

## Building
Builds a treat under Windows using the [Tiny C Compiler](http://bellard.org/tcc/) by the one and only [Fabrice Bellard](https://en.wikipedia.org/wiki/Fabrice_Bellard). 

```
tcc main.c
```

## Limitations
The language obviously isn't a serious development language. Memory currently isn't deallocated before program exit so your OS better do it for you.
