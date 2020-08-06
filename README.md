# The Bunny programming language

DISCLAIMER: This effort is a work in progress, expect it to be faulty and incomplete :)

Bunny is a programming language which makes programming and program maintenance a pleasure.
Bunny code files have the ending "bun", for example "test.bun".
The following snippet of Bunny code may give you a sense (it defines a function whose content is unknown which
calculates the sum vector c of two vectors a and b):

    vec.sum ( vec a, vec b ):( vec r ) {
      # ... see below for the function contents ;)
    }
    vec a
    vec b
    vec c
    vec.sum a b -> r:c

Using some common terminology to describe the Bunny language and type system:

- Interpreted: Bunny code is translated into an abstract syntax tree and evaluated (note, that it is not "forbidden"
  to create a bytecode-compiler based on the Bunny language spec).
- Strongly typed: All type checking is done before evaluation.
- Statically typed: Every "thing" is declared to be of an explicit type once and that notion may never change.
  Also no values are implicitly coerced from type A to type B.

## Status

Instable. Do not use.

## Why oh why

As a longtime software developer I often think of the "good old times" when I started programming with Perl, PHP and C.
I wished programming today could be as straight forward and fun like beeing able to remember 90% of the language from the top of my head
(Perl), just browsing through the function library and finding the missing puzzle piece (PHP) and moving bits and bytes
around (C). Through the years I did write down a number of language concepts but the main philosophy I was actually trying to encapsule
just emerged recently in the form of the Bunny language.

The following points form the "philosophy" of the Bunny language:

- Every step of an algorithm is a function call, picked from a public or custom function library.
- Programs describe how stuff is done, not how data is represented.

### Features worth mentioning

- What is in other languages a "function", "module" or "variable" is in Bunny just a labelled block of one or more typed values.
- Every labelled "thing" exists only once (in memory).
- All "things" the abstract syntax tree is made of also exist as builtin language types to be manipulated/evaluated.
- Types in Bunny are just static labels of values that go into a block of code and out of it. Before evaluating a piece
  of abstract syntax tree the Bunny runtime will check that the type labels of things that go into/out of a block are
  consistent with those used inside the block.
- Function parameters can be marked as "no-eval" which means the parameter value is passed as an un-evaluated piece of abstract syntax tree.
- Everytime a block is entered, the values passing in are passed-by-reference, so value going into a block is ever copied.
- By default all block inputs are marked "read-only" and only when marked "read-write" the values may be manipulated inside the block.
- Multiple named inputs and multiple named outputs for blocks. Inputs/Outputs can be optional, positional and rest (remaining values passed to a block).
- No common conditional if/else construct but decision tables to control program flow.
- No loop construct but mapping/grepping/... functions.
- No closures.
- All atomic types from C.
- Labelled blocks (aka "functions") can be re-defined just like all other labelled "stuff".

### Parts of bunny

- Language spec: This defines the syntax and semantics of the language code.
- Platform spec: How/where Bunny code/files can be used/run.

## Language spec

Here is a longer snippet of Bunny code to illustrate a few key points:

    # ? statt named params only function calls with fixed number of parameters and types:
    vec a
    vec b
    vec c
    vec.sum a b -> c
    # -> ommitted input/output parameters are marked as "."

    # call a function "sum" that has two parameters typed "vec":
    vec a        # allocates a value named "a" of type "vec"
    vec b
    !vec.0 v:a   # jump to scope "vec.0" and letting it access the input param "v" alias'ed to "a"
                  # (this will initialize the vector "a" to zeros)
    !vec.0 v:b   # jump to scope "vec.0" and letting it access the input param "v" alias'ed to "b"
    vec sum
    !vec.sum a:a b:b r:sum # jump to scope "vec.sum" and alias input/outputs beforehand
    # now value behind "sum" contains the vector sum of the vectors behind "a" and "b"

    # extend the scope "vec" by adding/redefining a new subscope "sub"
    # the scope input and output parameters are defined
    # input parameters are always read-only
    # output parameters are always read-write
    vec.sub ( vec a, vec b ):( vec r ) {
      # scope contents
      int8 i # ...
    }

    # the scope "mod.f" takes an input parameter "s" that is itself a symbol
    mod.f ( symbol s ):() {
      # ...
    }

    # how numbers and strings are actually created
    char c
    char.letter.a r:c # "char.letter.a" will write the letter a to the output parameter r
    int8 answer
    num.42 r:answer # "num.42" will write the letter 42 to the output parameter r
    string s
    string.append c:c s:s # "string.append" will append the character c to the string s (modifies s)

### Fundamental language constructs (stuff the AST is made of):

The following "things" in Bunny are the stuff that make up the AST (abstract syntax tree) when Bunny code is parsed:

- symbol
- scope (anonymous)
- ...?
-----
- input parameters: stuff that is "taken" (by name) from the scope (place/block) where the interpreter
  is jumping from (by respecting the types of the taken value!)
- output parameters (return values): stuff that is written (also by name) to the scope
  where the interpreter is jumping from
- both input and output parameter names may be alias'ed before the jump (so to be more flexible as how
  to name the in/out values and the values inside the scope of region jumping to)
- optionally labeled collection of 0+ typed values
- block definition (named or anonymous, with optional parameters and return spec)
  (blocks form a hierarchy, whereas access is strictly guarded)
- value (with access qualifier determining scoping)
- expression:
  - literal value (including a function and all language constructs)
  - call ("go to a block of name x")
  - symbol

### Predefined scopes

predefined scopes: (must be made available by all library implementations)

- ...
- ... around atomare Typen, zB alle von C plus Strings etc.
- ... around complexe Typen: Liste (heterogen), Sequence (homogene Liste), Record (feste Keys)
- ... around oder-Typ (A, B oder C etc.), entspr. tagged Union

## Platform spec

The platform architecture is organized in levels which are:

### Library level

The library level defines how the basic language constructs are organized/implemented:

- All language constructs (expressions, functions etc.) are library-language-specific types (e.g. C types).
- Library functions exist to manage the language constructs (either directly or
  by creation from parsing a string/json/xml/etc.) and compiling and/or interpreting/evaluating constructs
  (can also be used to simply parse strings into an AST and dump it as a json for example, allow easy adaptation
  in non-C library implementations).

The specific library of Bunny is organized this way:

    - spec lives on Gitub:
        README.txt
        doc/
          SPEC.txt = lang spec
          CONTRIB.txt
          LICENSE.txt
        lib/ = library level stuff
          c/ = c library
        run/ = runner level stuff
          ccmd/ = c cmdline
          cservice/ = c daemon
          js/ = js runner
        use/ = user level stuff
          webide/ = web based ide
          programs/ = actual programs

#### Specific implementation efforts

- The Bunny C library.

### Runner level

The runner level defines the interface to the C library towards a program that runs/evaluates the actual Bunny program code.

- How to embed the C library in a C/C++/etc. program.
- How to use the bindings for other languages that embed the C library.
- How to use the thin commandline wrapper around the C library (provides all functionality through commandline api).
  This can also create/compile C code that statically links the program code into a single standalone executable.
- How to use the service daemon (also wraps the C library) to run code through a HTTP api.
- How to use the JavaScript wrapper which can either use:
  - The service daemon (all code is then run by the service daemon).
  - A WASM-compiled commandline runner (all code is then run inside the browser).

#### Specific implementation efforts

- [NAME/LINK]: A C-based commandline program.
- [NAME/LINK]: A C-based service daemon.
- [NAME/LINK]: JavaScript bindings for the service daemon or in-browser WASM-C-library (compiled to WASM from the C library).

### User level

The user level defines the interface of a Bunny software towards a user of the program, including:

- How to run a program using the commandline runner
- How to run a standalone executable
- How to use the HTTP api of the service daemon to run code

#### Specific implementation efforts

- [NAME/LINK]: a web-based IDE using the Bunny JavaScript bindings.
- Example programs compiled as standalone executable. These can be found [LINK].

### Deployment level

The deployment level defines everything that is needed to distribute artefacts of Bunny software,
including:

- Distribution packages for Ubuntu, Fedora, CentOS, Debian, Suse, ...
- Plugin integrations for webservers: Apache, nginx, ...
- Automatic deployment chains to distribution package repositories
- Interpreter integrations for other languages such as Lua/Openresty

# Roadmap

- Write the language spec and implement the C library in a timely manner.
- Write the platform specs afterwards.
- Realize some of the specific platform implementations.

## Organization

### People

- maintainers: kitomer

### Ways to contribute

- Give valuable comments and inputs on the Roadmap items.
- Read through the files in this Github repository and create issues.
