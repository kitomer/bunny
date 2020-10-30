
# The Bunny programming language

DISCLAIMER: This effort is a work in progress, expect it to be faulty and incomplete :)

Bunny is a programming language that focusses on simplicity and maintainability.
To give you a first impression of the language, see the following code snippet that
sums up two numbers of a user-defined type:

    # define a type based on the whole number builtin type
    type whole = num( min = 0, max = 1000, step = 1 )
    
    # main function that sums up two values of the 
    sub main() {
      whole x = 40
      whole y = 2
      whole s
      sum x y -> s
      say s
    }

## Status

Instable. Do not use.

## Table of Contents

* [Motivation](#mot)
* [Features worth mentioning](#feat)
* [Parts of bunny](#parts)
* [Language spec](#lang)
* [A Bunny program](#prog)
* [Files](*files)
* [Type declarations](#types)
* [Function definitions](#funcs)
* [Typed values: variables](#vars)
* [Expressions: Function calls, symbols and literals](#expr)
* [More syntax](#syntax)
* [Builtin functions with lazy evaluation](#lazy)
* [Library level](#lib)
* [Runner level](#run)
* [User level](#use)
* [Process level](#proc)
* [Deployment level](#depl)
* [Roadmap](#road)
* [Organization](#org)

<a name="mot"/>

## Motivation

Bunny tries to combine three major characteristics of other programming languages I heavily used with pleasure into a single new language.
The first language is Perl with its very well defined collection of standard functions that makes it easy to remember 90% of the whole
toolset. The second language is PHP (version 4, 5 not so much) with is remarkable low barrier of entry for programmers of all levels of 
experience due its basic concept that nearly all you do is write functions and most of the time you simply can pick a function from the vast
builtin collection. The third language is C which gives you the ability to write high performant code and the possibility to
go down to the bit level.

The following points form the "philosophy" of the Bunny language:

- In general a Bunny program should be easy to read, understand and reason about and everything should be well-defined
  so that there are no surprises (for example overflows, automatic coersion of types).
- Every step of an algorithm is a function call, picked from a public or custom function library.
- Programs describe how stuff is done, not how data is represented.
- Programs should specify in great detail the boundaries in which code is supposed to function, and the
  interpreter will check those boundaries by static analysis.
    
Using some common terminology to describe the Bunny language and type system:

- Interpreted: Bunny code is translated into an abstract syntax tree and evaluated (note, that it is not "forbidden"
  to create a bytecode-compiler based on the Bunny language spec).
- Strongly typed: All type checking is done before evaluation.
- Statically typed: Every "thing" is declared to be of an explicit type once and that notion may never change.
  Also no values are implicitly coerced from type A to type B.

<a name="feat"/>

## Features worth mentioning

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
- Static analysis that ensures a function works on the full range of its specified parameter values in order
  to create a good foundation to writing safety-critical programs.

<a name="parts"/>

## Parts of bunny

- Language spec: This defines the syntax and semantics of the language code.
- Platform spec: How/where Bunny code/files can be used/run.

<a name="lang"/>

## Language spec

<a name="prog"/>

### A Bunny program

A Bunny program is a collection of type declarations and function definitions. Here is an example program:

    type whole = num( min = 0, max = 10000, step = 1 )
    sub sum( whole a, whole b -> whole c ) {
      sum a b -> c
    }
    sub main() {
      whole x = 1
      whole y = 2
      whole s
      sum x y -> s
      say s
    }

The previous snippet demonstrated the basic concepts of Bunny: How to define a custom type "whole" that
is a subset of values of the builtin type "num". Furthermore it shows a definition of the function "sum"
that takes two parameters and generates a result parameter, all of the type previously defined type "whole".
At last an entry point for the interpreter is defined as the function "main" which uses the defined
type, calls the defined function and generates a side effect, the printing of the calculated sum to
the standard output of the program process.

<a name="files"/>

### Files

The Bunny language does not concern itself with the concept of a "file", but by convention Bunny code comes 
in files that have the ending "bun", for example "hello-world.bun". The implementation will usually accept
Bunny code in the form of files or strings.

Since the language itself is not aware that its code (most often) resides inside files there is no way
to "include" or "load" code from within a Bunny program. (Note: If you really wanted to you could of course
extend the interpreter with a Bunny builtin function that would call the very interpreter itself and that
way create a kind of "eval" function ... which completely destroys most compile-time type-checking ;))

<a name="types"/>

### Type declarations

Types are defined by simply using them for function input or output parameters. Their internal layout though
must be known before interpretation down to the builtin types. All user-defined types are therefor aliases
or compounds of other user-defined and/or builtin types.

A type cannot be redefined

#### Builtin types

The builtin types are all without name prefix (to give them short names) and can be organized into 
single-value (or "simple" value), complex (or multi-value) and union-types. When creating values
of a type (any type) there are certain options that need to be specified which vary depending on
the base builtin type the actual type is based on.

As a general rule the builtin types cannot be used for values directly, except when there are
default values for all the options, like for example the C-based numeric types do.

##### Simple types

Bit type: A single bit has the type "bit".

Numeric types: They came on two "flavours". The first is based on the C types and thus usually
calculations on them are done using the CPU-supported operations. The other ones are more based
on the mathematical concept of numbers, e.g. whole numbers etc.

The usual type options for numeric types:

    min (same type as numeric)
    max (same type as numeric)
    step (same type as numeric)
    init (initial value)
    overflow behaviour (tbd)

###### C-based numeric types

    sint8 : C type 8 bit signed int
    uint8 : C type 8 bit unsigned int
    ...

When the options, s.a., are ommitted, the appropriate
limits are derived from the limits dictated by the CPU-supported operations.

Overflow behaviour is specified as an option to the type, s.a.
If overflow is allowed to happen it must be handled explicitly, which is ensured by the interpreter before interpretation.

###### Mathematical numeric types

Mathematical types have no default values for the options, s.a., like minimum/maximum, so it
is mandatory that these are defined when deriving a new type from a mathematical type.
The performance of calculations thus depends on the defined limits because at any time
the interpreter chooses the CPU-supported operation that fits those limits and sometimes
has to make multiple calculations to do a simple addition. For real numbers the precision
must also be set which also impacts performance.

    num : whole numbers, neg. and pos.
    real : real whole numbers
    ...

Options for "real" type:

    precision
            
##### Compound types

Compound types usually encapsulate multiple values. As you might noticed it is not possible to
define deep compound types in one sweep (for example "list of list of numbers"), which is
considered a feature as it requires to give each "stuff" inside a type a distinct name which
hopefully clarifies its purpose.

###### List type

The "list" type is an ordered sequence of unlimited length with same-typed values.
The following options exist for the "list" type:

    of (typename of element type)
    minlen (num or c-based whole-number type)
    maxlen (same)
    <all options from element type>

Example list-based type:

    ...
    
###### Record type

The "record" type is a heterogenous collection of values with a fixed size where each value
has a unique key. The keys are always symbols. The following options exist for the "record" type:

    key-<keyname> = The type name for key named <keyname>
    default-<keyname> = The default value for key, using the type

Example record-based type:

    type day = num( min = 1, max = 31, step = 1 )
    type month = num( min = 1, max = 12, step = 1 )
    type year = num( min = -100000, max = 10000, step = 1 )
    type date =
      record(
        key-day = day
        key-month = month
        key-year = year
      )
    date d( day = 1, month = 2, year = 2020 )

###### Map type

The "map" type is conceptually an collection of values with unlimited size, where each value has a unique key.
There is one type for the keys and one for the values. The options of the "list" type:
              
    key-type = The type name for all keys
    value-type = The type name for all values
    max-keys = The maximum number of keys (the type of that option is "num")

Note that heterogenous lists are not supported as a separate type but instead can be defined
by creating a "map" type with numeric keys and "any" typed values.
          
As with all types, in order to be able to create values of that type its structure has to
be known to the interpreter down to level where only builtin types remain.

##### Union types

Union types (aka "tagged-union") are types that are either one (or maybe even more, in other languages?)
of a given set of types. If the set only contains a single type then this is the "same" as a type alias
(formally the semantics are different but practically they behave very similar).

    any : can be any one of a bunch of other types

##### Symbol type

The symbol type plays a special role as literals can be of that type but it cannot be used as the
base for custom types. Also a few builtin functions take parameters of type "symbol". Furthermore
values of certain type options are of type "symbol". 
    
<a name="funcs"/>

### Function definitions

    sub f {} # does take neither input nor output parameters
    sub f( num x ) {} # only takes an input parameter "x" of type "num"
    sub f( -> num r ) {} # no inputs, only one output "r" of type "num"
    sub f( num x -> num r ) {} # input and output
    sub f( num x, num y .. ) {} # first param "x" and all the remaining params must be of type "int" and will be available as type "list"

#### Redefinition and overloading

A function with the same (full) name is allowed to exist, as long as the signature (argument types and/or return types) differs
between the same-named functions, sometimes called function overloading or multiple dispatch. A redefinition of a
function with the same full name and same signature will not be accepted by the implementation at compile time.
Probably obvious, but two functions with the same name (not full name) and same signature can exist within different
name-prefixes, s.b. (because then they do not have the same full name).

Functions may be used before they are known to the parser.
    
<a name="vars"/>

### Typed values: variables

Variables can be declared in function-scope and always have a specific fixed type (that may never change).
For example:

    sub main() {
      sint32 i
      uint64 j = 42
      io.say "i = " i ", j = " j
    }

<a name="expr"/>

### Expressions: Function calls, symbols and literals

#### Symbols

...

#### Function calls

A function is called either as a statement (in the function scope) or as an expression (for example in a literal or surrounding
function call). A call of a function can be visualized as a "jump" (that is what inspired the name "Bunny") after evaluating
all function arguments (eager evaluation). For builtin functions that is not always so, s.b.

Examples of function calls:

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

Raw notes to be intergrated into this paragraph:
    
    - Ergebnis eines Function calls ist immer der erste result parameter, zusaetzlich koennen alle result parameter explizit
      an locals des AUFRUF-Scopes zugewiesen werden
      -> Wenn eine Fkt. aufgerufen wird, werden deren locals allokiert und bei Beenden der Fkt. freigegeben
      -> Result parameter gehen immer von AUSSEN rein!
      -> wird eine Fkt. aufgerufen ohne dass die/der Result parameter explizit zugewiesen wird, wird
        im AUFRUF-Scope eine implizite Variable allokiert und zugewiesen
    - Wenn kein expliziter Result parameter vorh., wird Result parameter "void r" implizit verwendet
      -> ? ggf. meckern, so dass er explizit angegeben wird!
    - Eine Fkt. entscheidet darueber, ob ihre Parameter evaluiert oder unevaluiert uebergeben werden
      -> alle Parametertypen von ast.* sind autom. NICHT evaluiert (keine spezielle Markierung der Parameter notwendig!)
      -> es gibt Typen zB ast.name etc. aber auch ast.type.TYPE fuer jeden Custom-Typ, so
        dass man einfach un-evaluierte aber typ-gecheckte Werte erwarten kann

#### Literals

In general every value has a type and literals provides a syntax for writing any value of any type
directly into the source. This is useful for initialization of values, defaults for function parameters
and for passing values to functions without using an intermediate variable. Often it leads to more compact
code. The main *problem* with typed literals is that a rather generic syntax needs to useable for all kinds
of types, builtin and custom ones. In Bunny all types are eventually derived from the builtin ones anderefor each of
the builtin "kinds" of types there is a syntax for noting their corresponding literal.

A literal can optionally be appended with a specific type name but if that is ommitted, the interpreter tries
to determine the type from the context in which the literal is used (for example function call or variable
declaration). If the interpreter cannot match the literal to the specific type or the context, it will deny the
interpretation of the whole program. The generic syntax for literals:

    <literal>               : let the interpreter decide the type (generates warning for most coercions!)
    <literal>~<typename>    : explicitly define the target type (generates warning on slightest mismatch)

##### Literals for values based on simple types:
    
    42    : numeric types
    42.3  : same
    1/3   : ?
    ...

##### Literals for values based on compound types:
    
    [ x, y, z ] : list
    { 0 = x, 1 = y, abc = z } : record
    < 0 = x, 1 = y, 2 = z > : map

#### Coercion of values between types
    
This does never happen implicitly but needs to be done by the user.
    
<a name="syntax"/>

### More syntax
    
#### Comments and documentation

Raw notes to be integrated into this document:

    - integrate ways of elaborate documentation that is actually _part_ of the language (meaning it is parsed and
      can be extracted/analyzed)
      -> may replace/help issue (bug/todo/note) tracking
      -> may be used to generate readable documentation (which may include some code)
      -> get some hints on how to do that from Literate Programming
      --- rules
      - so-called notes can be added to sources
      - there are certain predefined kinds of notes
      - a note always refers to the direct ast-thing after itself
      - a note looks similar to a comment
    
#### Line continuation: newlines and parenthesises

...

#### Symbol shortcuts ("scopes")

Raw notes to be integrated into this document:

    - ! [JA] shortcut for defining functions and types within a scope:
        date {
          sub sum( .date a, .date b -> .date c ){ }
          sub diff( .date a, .date b -> .date c ){ }
          type .date = # ... the â.â says âprepend current shortcutâ
        }
        date {
          subdate {	# can be nested at will...
            # ...
          }
        }
        # same as
        sub date.sum( date.date a, date.date b -> date.date c ){ }
        sub date.diff( date.date a, date.date b -> date.date c ){ }
        type date.date = # ...

#### Symbol aliasing

Sometimes it is useful to be able to create a function or type alias in order
to give it a more domain-specific name or to define a more abstract program api.
Also it can be used to shorten names when using other functions/types that
have long full names. With function aliases it is important to know that the
alias is an alias for the *name* which includes all the different actual
functions with that name (with different signature).

    sub f = date.sum
    type t = date.date

<a name="lazy"/>

### Builtin functions with lazy evaluation

user-defined functions will always eagerly evaluate their arguments before entering the function's
body. Some builtin functions though will behave only evaluate certain arguments under certain
conditions, which is needed in order to be able to create multiple control flow branches.

    - ? or have a builtin function with a very flexible signature that allows to notate
      a decision table as a simple function call
      
        decide(
          0101 = f1
          0111 = f2
          1001 = f3( 42 )
          x_xx = outcome1( bla )
          x___ = outcome2
          __xx = outcome3
        )
        
      The 
      
    - ? or have special syntax for decision tables (n conditions, m pairs of boolean-combinations + outcome)
      -> not really nessessary as the function-call syntax is sufficient, s.a.

In order for this to work formally some builtin functions will have parameters of type "expression"
which is what is says: (unevaluated) expression. It is ensured that when evaluating such expression
it is always evaluated in the context of its notation.

#### Conditional execution

Use the "decice" builtin function for conditional execution.

Raw notes to be integrated into this document:

    - ? need more functions than "decide" ?
        maybe(
          f1()
          f2
          f3
          all-true: outcome1()
          all-false: outcome2
          anyone-true: outcome3
        )
        sub and( bool value .. -> expression action )
        sub or( bool value .. -> ast action )
        sub not( bool value -> ast action )

#### Processing of multiple values

...

Raw notes to be integrated into this document:

    - @map( list a, function f -> list r )
    - @grep( list a, function f -> list r )
    - @concat( ... )
    - @unique( ... )
    - @sort( ... )
    - @reduce
    --- more s.b.
    - https://timelydataflow.github.io/differential-dataflow/introduction.html
      The map operator applies a supplied function to each element of a collection, the results 
        of which are accumulated into a new collection.
      The filter operator applies a supplied predicate to each element of a collection, and retains 
        only those for which the predicate returns true.
      The concat operator takes two collections whose element have the same type, and produces the 
        collection in which the counts of each element are added together.
      The consolidate operator takes an input collection, and does nothing other than possibly 
        changing its physical representation. It leaves the same sets of elements at the same times with the same logical counts.
        What consolidate does do is ensure that each element at each time has at most one physical tuple.
        Generally, we might have multiple updates to the same element at the same time, expressed as 
        independent updates. The consolidate operator adds all of these updates together before moving the update along.
      The join operator takes two input collections, each of which must have records with a (key, value) 
        structure, and must have the same type of key. For each pair of elements with matching key, one from 
        each input, the join operator produces the output (key, (value1, value2)).
      The reduce operator takes an input collection whose records have a (key, value) structure, and it 
        applies a user-supplied reduction closure to each group of values with the same key.
      The iterate operator takes a starting input collection and a closure to repeatedly apply to this input 
        collection. The output of the iterate operator is the collection that results from an unbounded number 
        of applications of this closure to the input. Ideally this process converges, as otherwise the computation will run forever!
      The arrange operator is a massively important operator that we will discuss in more detail in the 
        chapter on Arrangements. Arrange controls and coordinates how data are stored, indexed, and maintained, 
        and forms the basis of efficient data sharing.
        Operators like consolidate, join, and reduce all use arrange internally, and there can be substantial 
        benefit to exposing this use. We will discuss this further.

### Ideas that need to be though about
    
    - implement units (e.g. metric, physical) into Bunny program:
      - implement a unit as a separate data type (internally represented by a numeric atomic type)
        and impl. conversion function based on the type
        
In the future it might be useful to specify the Bunny language in the form of
type inference rules (s. https://www.cs.princeton.edu/courses/archive/spring12/cos320/typing.pdf) in order to
make it possible to apply mathematical methods for reasoning about Bunnys type system.

## Platform spec

The platform architecture is organized in levels which are:

<a name="lib"/>

### Library level

The library level defines how the basic language constructs are organized/implemented:

- All language constructs (expressions, functions etc.) are library-language-specific types (e.g. C types).
- Library functions exist to manage the language constructs (either directly or
  by creation from parsing a string/json/xml/etc.) and compiling and/or interpreting/evaluating constructs
  (can also be used to simply parse strings into an AST and dump it as a json for example, allow easy adaptation
  in non-C library implementations).
- a Bunny program is conceptually always single-thread/single-process, as lightweight as possible
  (also mostly statically allocated memory)
- memory model:
  - no global variables, only function-local variables
  - by default each function variable is implemented as an ARRAY so to have one data value
    per function call, but this can be overriden to allow "static" variables, which access the same value
    for every time the function is called (jumped to)

Raw notes to be integrated into this document:

     - auto-merging sw code non-semantically is not really robust!
      -> Bunny INCLUDES functionality to COMBINE two segments of CODE semantically
      -> this can be used in source-code-management tools to manage Bunny source files or inject
        changes from other people
      -> the Bunny cmdline tool extends this to work with MULTIPLE files and DEEP directories!
 
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

Raw notes to be integrated into this document:

    - library level https://slembcke.github.io/2020/10/12/CustomAllocators.html
    - every lexical variable is implemented as a statically allocated array of values
      (plus a current counter (= current length of array)), each array-value beeing used inside the callstack
      -> if the interpreter runs out of memory, it will stop OR allocate more using dynamic malloc
      -> dynamic memory allocation can be turned off
    - each function is brute-force tested given its parameter value range (apply "all" combination of allowed values)
      (in sensible limits)
    - software quality measurement detection (function length, complexity etc.)
    - static analysis of code
    - ability to limit the stack size (how deep the function call tree may be at any time)
    - when the value of a variable is checked, ALL values of that type need to be checked
      -> may be simplified by checking ranges of values!
      -> also for or-types (e.g. error results: all kinds of errors must be checked explicitly)
    - Variablen sind immer read-write
      - eine Variable ist ein typisierter Eintrag im aktuellen Scope
      - die Syntax fuer den Scope ist { ... }
      - ?? Var.-Definition analog zu JUMP-Def.? Bsp.
          @vec a        # defines var a
          @vec.sum() {} # defines jump vec.sum
      - eine Variablen-Deklaration ist ebenfalls ein Function call: @TYPE( ast.name name .. )
          vec a b c
      - Konstanten koennen zB durch JUMPs erzeugt werden, die immer denselben Result-Parameter haben
    - alle "aeusseren" Einfluesse auf ein Bunny-Programm werden auf Funktions-Aufrufe gemapped
      - die API eines Programms (= Kommandozeile Parameter/Argumente) ist als Funktion im Bunny Programm abbildbar;
        - die embedded lib mapped die Befehlszeile auf eine entspr. Bunny-Fkt. ab
        - ebenso eine URL (zB wenn man eine Webserver-URL/Path auf ein Bunny-Programm abbilden moechte)
      - OS-Interrupts
      - HID-Events
    - parallel processing:
      - the embedded Bunny interpreter (aka the function that evaluates an ast typed value) is able to
        evaluate multiple ast's in parallel
      - parallel running evaluations can send messages to each using message queues
      - the priority of an evaluation can be set along with the parameters to "eval"
    - there are mechanisms builtin to
      - sync n processes at a given point in code and be able to control all of the processes
      - send messages between n processes: each process has a message queue that is to be
        processed (the "main loop" has to be implemented in the program manually)
      - share resources: actually no resource is ever shared, instead what is done:
          resource is copied -> changes are made -> the changed copy is appended to the resources history

<a name="run"/>

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

- all external events/triggers/messages are put into a queue that can be accessed by a Bunny program
- other Bunny program runners can be contacted/monitored (local and remote) using a few builtin functions
- inter-runner messages can have a few meta infos attached, e.g. sender runner ID
- the inter-runner communication protocol is specified based on various technologies for local and remote runners:
  - HTTP(s) based (for communicating)
  - local ??
- it is not possible to start a remote runner, so it is advised (in distributed runner setups) that one runner
  is started on system startup which then starts the actual runners (possibly after receiving remote messages to do so)
- there is a builtin function that instructs the runner to restart itself making it possible to
  switch to a new version of a Bunny program if the sources changed since the previous runner start
- the runner can be instructed to submit a message should the Bunny program crash in order to signal its
  crash to other Bunny programs (maybe the parent)
- the usual philosophy is to be very liberal about crashing programs (let them crash) but also let them be
  easily monitored and restart. the runner can be instructed to restart a Bunny program on failure
- ? the runner spawns by default making it a default-to-demon program that controls the execution of the
  Bunny code it runs

#### Specific implementation efforts

- [NAME/LINK]: A C-based commandline program.
  - is able to modify sources as to record semantic changes to existing code, e.g.
    "rename function x to y" instead of just modifying the source files!
- [NAME/LINK]: A C-based service daemon.
- [NAME/LINK]: JavaScript bindings for the service daemon or in-browser WASM-C-library (compiled to WASM from the C library).

<a name="use"/>

### User level

The user level defines the interface of a Bunny software towards a user of the program, including:

- How to run a program using the commandline runner
- How to run a standalone executable
- How to use the HTTP api of the service daemon to run code

<a name="proc"/>

### Process level

The process level defines a software development process that leads towards a working Bunny program.
The specified process is scalable from a single developer to a multi-maintainer team.

For ideas on that topic, s. https://www.fastcompany.com/28121/they-write-right-stuff

#### Process goals and priorities

The three goals, time, cost and quality are priotized in this order:

1. Quality (the higher the better)
2. Time (the shorter the better)
3. Cost (the cheaper the better)

The reasoning behind that is:

- Quality is the highest priority because a malfunctioning system will backfire in the future.
- Time and cost are closely related as it is often possible to reduce time while increasing cost.
  In case you are able to choose between time and cost, time should be reduced as much as
  possible to yield results faster as they can be used sooner.
- Cost always has a limit so at any time, all the money should be awarded into
  reaching a high-quality result as fast as possible. In the lucky case when you can choose between two
  equal solutions, choose the cheaper one because choosing the more expensive one would not gain anything.

#### Main principles of the process:

- Be as simple as possible:
  - Have as few different roles as possible.
  - Have as few different activities as possible.
  - Have as few different artefacts as possible.
- Be as automatable as possible.
- Be as strict and exact as possible.
- All activity is documented.
- Every purpose of activity is documented.
- Based on computer-readable definitions (not documents/diagrams/checklists)
	so as to easily automate parts of the process.
- Clear takeover chain in cases people are missing.
- no standups (fixed regular meetings)
- only work on stuff that matters = stuff that is visible AND must be done
- live-deployable code without downtime is the standard!
- there are no "projects" (because it is always a unproductive waste of time to discuss what is
  a project, what is a module etc.), there is just the "thing" everyone is working on
- there is no formal chain of steps that need to be run through, a work package is simply
  handed from person to person that does certain things (skill/action-based steps) where
  each step can be done by multiple people (separately or simultaneously) as well as multiple times

#### Process activities:

- Specification: The act of defining the goal of an artefact (e.g. a (piece of) program).
- Development: The act of writing/modifying an artefact (e.g. a piece of code) towards a specification.
- Validation: The act of matching the specification with the developed artefact (e.g. program) to ensure consistency.
- Deployment: ...

#### Targets for activities:

- Program code.
- Process definition.
- Additional artefacts, like documents, websites etc.

#### Process implementation on GitHub

All artefacts, specifications and validation results are kept inside a GitHub repository.

##### Specification:

Form: In the form of a directory in the repositories

Contains:

- A description behind the purpose of the specification.
- A description of the specification as exact/formal as possible.

? how ensure that the specification matches its purpose ?
  -> have a machine-readable description of the specification items (semantically)

##### Development:

- A reference to one or more specifications that define the goal of the development.
- A description of the changes made for implementing the referenced specifications.

? how ensure that all stuff that was specified was developed ?
  -> have a machine-readable description of the changes (semantically) referencing the specification

##### Validation:

- A reference to one or more developments that are beeing validated.
- A detailed result of the validation that lists for every specification item whether it
  is consistent with the developed result.

? how ensure that all stuff that was specified was developed ?
? how ensure that all stuff that is beeing developed was validated ?
  -> have a machine-readable description of the validation result
  -> match spec with dev and val. results
  
#### Specific implementation efforts

- [NAME/LINK]: a web-based IDE using the Bunny JavaScript bindings.
- Example programs compiled as standalone executable. These can be found [LINK].

<a name="depl"/>

### Deployment level

The deployment level defines everything that is needed to distribute artefacts of Bunny software,
including:

- Distribution packages for Ubuntu, Fedora, CentOS, Debian, Suse, ...
- Plugin integrations for webservers: Apache, nginx, ...
- Automatic deployment chains to distribution package repositories
- Interpreter integrations for other languages such as Lua/Openresty

<a name="road"/>

## Roadmap

- Write the language spec and implement the C library in a timely manner.
  1. Write c file with typedefs for lang constructs and parse() func to turn char* into language construct
  2. Write func to run language construct
  3. Compile as c library
  4. Small c cmdline prog link lib, read input file, compile program as standalone exe
  5. Small c http server (libmicrohttpd) embed c lib, run code via http
  6. Compile lib to js with WASM compiler
  7. Small js wrapper to use WASM lib to run code in browser or using c server
- Write the platform specs afterwards.
- Realize some of the specific platform implementations.

<a name="org"/>

## Organization

### People

- maintainers: kitomer

### Ways to contribute

- Give valuable comments and inputs on the Roadmap items.
- Read through the files in this Github repository and create issues.
