# The Bunny programming language

DISCLAIMER: This effort is a work in progress, expect it to be faulty and incomplete :)

Bunny is a programming language which makes programming and program maintenance a pleasure.
Bunny code files have the ending "bun", for example "test.bun".
The following snippet of Bunny code may give you a sense (it defines a function whose content is unknown which
calculates the sum vector c of two vectors a and b):

    @vec.sum( vec a, vec b -> vec r ) {
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

    #### basic vector arithmetic

    # the following blocks are assumed to exist:
    # - initialize a vector to a given length with zeros: @vec.0( vec v, int len -> vec r )
    # - calculate vector sum of two vectors: @vec.sum( vec a, vec b -> vec r )

    vec a, b, c # declare some vectors
    vec.0 a len:2
    vec.0 b 2 # second parameter "len" is also 0 as in previous example
    vec.sum a b -> c # jump to block "vec.sum" and pass locals "a" and "b" into the block, block result "r" is written to local "c"
    vec.sum( a b -> c ) # same just uses parenthesises
    vec.sum a:a b:b -> r:c # same but explicitly sets input- and outputs via their name
    vec.sum a . -> c # almost the same but second param "b" is ommitted (the block treats as a zero vector)
    vec.sum a -> c # same as previous line
    vec.sum a # almost the same as previous line but the result param "r" is not used
    vec.sum . b -> c # another example of ommitting a parameter, this time input param "a" is ommitted
    vec.sum . . -> c # b is ommitted as well
    vec.sum -> c # same as previous line

    #### block definition examples

    # blocks are actually just top-level locations that can be jumped-to (they are not like go-to's though)
    # a block has 0 or more input and output parameters
    # input parameters are always read-only (pass-by-reference) while outputs are read-write (also passed by reference from caller scope)

    # the block "f" is stored in the scope "" (this is a legal scope name)
    # in this example the block does not take any input or output parameters
    @f {} # does take neither input nor output parameters
    @f( num x ) {} # only takes an input parameter "x" of type "num"
    @f( -> num r ) {} # no inputs, only one output "r" of type "num"
    @f( num x -> num r ) {} # input and output
    @f( num x, num y .. ) {} # first param "x" and all the remaining params must be of type "int" and will be available as type "list"

    # using the output parameter of a block as an input parameter for a another block
    # (without having to store the output parameter in some local variable and then using the variable in the other block)
    # assumes the following blocks:
    #   - block "0" which writes zero to its result defined as: @0( -> num r )
    #   - block "num.sum" defined as: @num.sum( num a, num b -> num r )
    num r
    num.sum \0 \0 -> r
    # this can actually semantically similar be written as:
    num r
    num zero
    0 -> zero
    num.sum zero zero -> r

    #### block implementation examples

    # this block takes two single parameters and then a number of more parameters
    @f( num x, num y, num rest .. -> num r ) {
      print "x is " .str(x) "\n"
      print "y is " .str(y) "\n"
      # this shows how to access rest input parameters
      map rest as:z idx:i {
        print "rest[" .str(i) "] is " .str(z)
      }
    }
    f 42 43 44 45 56

    @vec.sum( vec a, vec b -> vec r ) {
      # ensure vec a is larger than vec b:
      and num.is_larger_than( a:vec.size(a), b:vec.size(b) ) -> any.swap( a b )

      # for each element of vec a:
      #   if vec b has an element: write sum of elem from a and b to same pos in vec r
      #   else: write elem from a to vec r
      multi-map l:vec.as-list( a )
    }
    vec a, b, c
    vec.sum a b -> c

    #### conditional execution

    # the basic trick are the standard blocks ".and", ".not" and ".or" that take any number of inputs
    # and based on their boolean meaning will evaluate their first result parameter
    # the blocks definition are (the type "ast" is the type of any piece of unevaluated abstract syntax tree):
    #   @and( bool value .. -> ast action )
    #   @or( bool value .. -> ast action )
    #   @not( bool value -> ast action )

    # a decision table works as follows:
    # - one or more conditions are evaluated (in any order) and the results are stored in local variables
    # - based on one or more rules (a rule is a combination of condition results) certain outcomes are evaluated (in pre-defined order)
    # the following "hand-codes" a decision table:
    @outcome1 {}
    @outcome2 {}
    @outcome3 {}
    {
      bool c1, c2, c3
      f1 -> c1
      f2 -> c2
      f3 -> c3
      and c1 c2 c3 -> outcome1
      and c1 c2 \not(c3) -> outcome2
      not c3 -> outcome3
    }

    #### fun with un-evaluated pieces of abstract syntax tree

    # to evaluate any piece of abstract syntax tree, just use the block @eval, defined as: @eval( -> ast r )
    eval -> 42
    eval -> f()
    eval -> {}

    #### literals

    # the code examples above already illustrated a few literals, but there are some more
    print 42 # literal of type "num.int"
    print 42.4 # literal of type "num.real"
    # ...

    #### defined hierarchy of type names

    # numeric types
    #   num : any kind of numeric value
    #   num.int : whole numbers, neg. and pos.
    #   num.int0 : pos. whole numbers including 0
    #   num.int1 : pos. whole numbers without 0
    #   num.real : real whole numbers
    #   ...

    # C types
    #   c.sint8 : C type 8 bit signed int
    #   c.uint8 : C type 8 bit unsigned int
    #   ...

    # compound types
    #   list : heterogenous list
    #   seq : homogenous list
    #   record : map with fixed keys
    #   map : map with variable keys

    # union/or/attged-union types (if bunch is only one other type this is essentially an alias-type)
    #   any : can be any one of a bunch of other types






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
