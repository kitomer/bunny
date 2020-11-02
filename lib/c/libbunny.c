
/*

              - neue Syntax fuer:
		            - Aliase
		            - Shortcuts
		            - Syntax: wenn "=" am Zeilenende, dann geht es in der naechsten Zeile weiter!
		            - Klammern bei Fkt.-Def optional!

  typedef ParserState, der zB so was wie number-of-open-parens beinhaltet
		und an jede Parser-Fkt. uebergeben wird...
  
	parsing function call:
		- global to count OPEN function calls
		- whenever a “)" is parsed:
			- if the current function call has a “(”: it is done (decrement open parens)
			- else: function done (NO decrement open parens!)

						- Bunny:
              - Typen sind nur Aliase auf Builtin-Typen (inkl. Listen etc. aber immer speziiziert und mit gesetzten Optionen zB min/max)
              - Zugriff auf Werte von Typen geschieht mit generischen speziellen Builtin-Fkt., die Typ-spezifisch verfuegbar gemacht werden
              - Fkt. wie die Builtin-Fkt., die mit div. Typen gleichzeitig funktionieren koennen NICHT in Bunny direkt definiert
                werden sondern nur ueber Erweiterung des Interpreters (einfach moeglich da embedeable)
              - It is currently not possible to define a TYPE to be used for multiple variables, which would
                be useful (ability to define multiple vars with same characteristics, ability to define deep complex data)
                -> this should be possible!
                -> the structure of a type must be known DOWN TO THE atomic level with builtin types!
                -> function parameter types are just type-names (no complex type spec/options!)
                -> Syntax for type definition:

                    <space> (can be between (mostly) anything) :=
                        when parenthesis groups are open (any kind, the KIND is known though!):
                          <space> := { <whitespace> | <eol> | <comment> }
                        else:
                          <space> := <whitespace> [ <comment> ]

                    <comment> := "#" <anything-not-eol>
                        !! note: a comment DOES NOT INCLUDE the final <eol> !
                    <eol> := "\r\n" | "\n" | "\r"
                    <whitespace> := { ( " " | "\t" ) }

                    <program> := { <program-statement> }
                    <program-statement> := <function> | <type>

                    // f( num a, num b -> num c ){ ... }
                    <function> := <name> "(" [ <function-inputs> ] [ "->" <function-outputs> ] ")" "{" { <function-statement> } "}"
                      <function-inputs> := { <function-parameter> } [ <function-rest-parameter> ] 
                      <function-outputs> := { <function-parameter> }
                      <function-parameter> := <name> <name>
                      <function-rest-parameter> := <name> <name> ".."
                    
                    <function-statement> := <variable> | <assignment> | <function-call>

                    // example: month base:num min:1 max:12 step:1
                    <type> := <name> { ( <name> ":" <literal> ) }

                    <variable> := "@" <name> <name> [ "=" <literal> ]
                    <assignment> := <name> "=" <literal>
                    <function-call> := <name> ( <function-call-multi-line> | <function-call-single-line> )
                    <function-call-multi-line> := 
                    <function-call-single-line> := { <function-call-argument> } <eol>
                    <expression> := <function-call> | <name> | <literal>
                    
                    <literal> := ( <name> | <string> | <decimal-number> | <hexdecimal-number> | <binary-number> | <list> | <record> | <map> ) [ "~" <name> ]
                      <name> :=  <symbol> { "." <symbol> }
                      <symbol> := <letter> { ( <letter> | <digit> | "-" | "_" ) }
                      <letter> := 
                      <digit> := 
                      <string> := 
                      <decimal-number> :=
                      <hexadecimal-number> :=
                      <binary-number> := 
                      <list> := 
                      <record> := 
                      <map> := 

                    
                    only allowed in function scope (statements):
                      @x month
                      @x month = 12 # with literal value
                      x = 11 # plain assignment
                      f( a:42 43 -> r )
                      f a b
                      
                    ? access to elements of compound values ??
                      ...
                    
              - the compound types when used for parameters always have to be specified
                down to the element-level ("list" only is _not_ allowed)
              - It is therefor not _easy_ to implement generic list functions etc.
                  ?? Maybe make it possible to 
                  
            - Bunny.c: nur noch _eine_ global g_parserState vom Typ BunParserState_t:
                typedef struct {
                  // Memory for AST nodes (allocated in chunks)
                  /*
                    - how avoid creating unnessessary AST node structs during peek-stage while parsing (usually certain stuff is TRIED
                      and if the parsing does not conclude the resulting AST nodes are destroyed...):
                      -> have a static array of AST nodes + a way of extending that one in big chunks
                      -> have alloc_ast_node(), free_ast_node() functions
                  */
                  BunASTNode_t * astNodes; // memory for AST nodes
                  uint32_t astNodesSize;
                  uint32_t astNodesUsedSize;

                  // current filename and position
                  char currentFilename[512];
                  char *currentString;
                  uint64_t currentStringLength; // excluding any trailing \0 byte
                  FilePos_t currentPosition;

                  // number of open parenthesises
                  uint32_t numOpenParenthesises;
                }
                BunParserState_t;
                
            - Bunny.c: 
/*
// function call
//    <funcname> ["("] [ <function-call-argument-list> ] [ "->" <function-call-argument-list> ] [")"]
// examples:
//    vec.0 b 2 # second parameter "len" is also 0 as in previous example
//    vec.sum a b -> c # jump to block "vec.sum" and pass locals "a" and "b" into the block, block result "r" is written to local "c"
//    vec.sum( a b -> c ) # same just uses parenthesises
//    vec.sum a:a b:b -> r:c # same but explicitly sets input- and outputs via their name
//    vec.sum a . -> c # almost the same but second param "b" is ommitted (the block treats as a zero vector)
//    vec.sum -> c # same as previous line
*/
uint8_t _parse_function_call( BunASTNode_t *node )
{
  // TODO

		- whenever a ")" is parsed:
			- if the current function call has a Ã¢ÂÂ(Ã¢ÂÂ: it is done (decrement open parens)
			- else: function done (NO decrement open parens!)
  
  
  // parse <name>

  // if parse "(": this call is continued until the closing ")" even over eol(s)

  // optional parse <function-call-argument-list>

  // optional parse "->" and <function-call-argument-list>

  //

  // while 1:
  //   optional parse <name> and ":"
  //   parse <expression> or "."
  //   if eol and no-parenthesis: return

  // if parse "->":
  //   while 1:
  //     optional parse <name> and ":"
  //     parse <expression> or "."
  //     if eol and no-parenthesis: return

  return 0;
}
			
*/

//------------- HEADER START

// type that represents a position inside a file
typedef struct {
  uint32_t line;
  uint32_t column;
  uint32_t offset; // character offset from string start
}
FilePos_t;

typedef enum {
   BN_ASTNODETYPE_NOTYPE = 0
  ,BN_ASTNODETYPE_SINGLE_CHARACTER
  ,BN_ASTNODETYPE_CHARACTER_SEQUENCE
  ,BN_ASTNODETYPE_SPACE
  ,BN_ASTNODETYPE_COMMENT
  ,BN_ASTNODETYPE_ANYTHING_TO_EOL
  ,BN_ASTNODETYPE_VARIABLE
  ,BN_ASTNODETYPE_NAME
  ,BN_ASTNODETYPE_SYMBOL
  ,BN_ASTNODETYPE_TYPE_OPTIONS
  ,BN_ASTNODETYPE_LITERAL
  ,BN_ASTNODETYPE_BLOCK
  ,BN_ASTNODETYPE_FUNCTION
  ,BN_ASTNODETYPE_FUNCTION_CALL
  ,BN_ASTNODETYPE_PROGRAM
}
BunASTNodeType_t;

typedef struct BunASTNode_s {
  BunASTNodeType_t type;
  FilePos_t start;
  uint64_t length;
  //struct BunASTNode_s * parent;
  union BunASTNodeInfo_u {
    uint8_t notype;
    struct {
        BunASTNode_t *name;
        BunASTNode_t *typename;
        BunASTNode_t *typeoptions;
      }
      variable;
    char single_character;
    struct {
        BunASTNode_t **children;
        uint32_t children_length;
        uint32_t num_children;
      }
      program;
    // ...
  }
  value;
}
BunASTNode_t;

typedef enum {
   BN_INTERPRETER_ERROR_FINE = 0
  ,BN_INTERPRETER_ERROR_GENERIC
  // ...
}
BunInterpreterError_t;

typedef enum {
   BN_PARSER_ERROR_FINE = 0
  ,BN_PARSER_ERROR_GENERIC
  // ...
}
BunParserError_t;

BunASTNode_t *bn_program_string_to_ast( char *str );
uint32_t bn_interpret_program_ast( BunASTNode_t *ast );

//------------- HEADER END

// ring buffer
typedef struct BunMessage_s {
  uint32_t flag; // for assigning a type/kind to the message
  uint8_t *data;
  uint64_t datalen;
  struct BunMessage_s *prev;
  struct BunMessage_s *next;
}
BunMessage_t;
typedef struct BunMessageQueue_s {
  BunMessage_t *next;
  uint32_t total; // total number of messages in ring buffer
}
BunMessageQueue_t;

// Memory for AST nodes (allocated in chunks)
/*
  - how avoid creating unnessessary AST node structs during peek-stage while parsing (usually certain stuff is TRIED
    and if the parsing does not conclude the resulting AST nodes are destroyed...):
    -> have a static array of AST nodes + a way of extending that one in big chunks
    -> have alloc_ast_node(), free_ast_node() functions
*/
BunASTNode_t * g_ASTNodes = 0; // memory for AST nodes
uint32_t g_ASTNodesSize = 0;
uint32_t g_ASTNodesUsedSize = 0;

BunASTNode_t *_alloc_astnode()
{
  if( g_ASTNodesUsedSize == g_ASTNodesSize ) {
    // scale up
    g_ASTNodesSize += 1024;
    g_ASTNodes = realloc( g_ASTNodes, sizeof(BunASTNode_t) * g_ASTNodesSize );
  }
  g_ASTNodesUsedSize ++;
  return ( g_ASTNodes + g_ASTNodesUsedSize - 1 )
}

void _free_astnode( BunASTNode_t ** node )
{
  // nothing needs to be freed actually, maybe later reuse freed nodes
  *node = 0;
}

char g_currentFilename[512] = { 0 };
char *g_currentString = 0;
uint64_t g_currentStringLength = 0; // excluding any trailing \0 byte
FilePos_t g_currentPosition = { 0 };

/*
    - generic functions to
      - set current string buffer
      - inc/dec char offset inside the current buffer (keeps track of line/columns!)
      - get n chars from current offset
*/
void _set_current_string( char *filename, char *str, uint64_t len )
{
  if( filename ) {
    size_t namelen = strlen(filename);
    memcpy( g_currentFilename, filename, namelen) );
    g_currentFilename[ namelen ] = 0;
  }
  g_currentString = str;
  g_currentStringLength = len;
}

FilePos_t _get_pos()
{
  return g_currentPosition;
}

void _set_pos( FilePos_t pos )
{
  g_currentPosition = pos;
}

void _dec_pos()
{
  g_currentPosition.offset --;
}

void _dec_pos_by( uint32_t n )
{
  g_currentPosition.offset -= n;
}

char _next_char()
{
  if( g_currentStringLength > 0 ) {
    // get character under current cursor
		uint32_t i = g_currentPosition.offset;
    char ch = g_currentString[ i ];
    // set cursor to next character if possible
    if( i + 1 < g_currentStringLength ) {
      g_currentPosition.offset ++;
    }
		// count up line number
		// newline characters:
		//   \n   unix
		//   \r\n win
		//   \r   mac
    if( ch == '\n' ) {
			_inc_line();
		}
		else if( ch == '\r' ) {
			// check the next character: if NOT a \n, increment the line as well
			if( i + 1 < g_currentStringLength ) {
				char ch2 = g_currentString[ i + 1 ];
				if( ch2 != '\n' ) {
					_inc_line();
				}
			}
		}
    return ch;
  }
  return 0;
}

void _inc_line()
{
  g_currentPosition.line ++;
  g_currentPosition.column = 0;
}

uint8_t _end_of_string()
{
  return ( g_currentPosition.offset + 1 < g_currentStringLength ? 0 : 1 );
}

//=============================================================================
// reader functions (try to read a certain kind of stuff)

// detect non-ascii (utf-8) characters this way:
// s. https://www.instructables.com/Programming--how-to-detect-and-read-UTF-8-charact/
// chars ascii 0-127 https://www.ascii-code.com/

uint8_t _parser_error( BunParserError error, char *details )
{
  printf("Parser error: %s\n", details);
  return 0;
}

// tries to read one of a set of given characters
// note: wrong line counting occurs, when \n or \r are in the character set
uint8_t _parse_single_character_from_set( char *set, BunASTNode_t *node )
{
	// TODO
	// ! error of _parse_character_sequence() or _parse_single_character_of_set()
	//	 is passed a “\r” or “\n” !! (this will not work!)
	
  FilePos_t start = _get_pos();
  char next = _next_char();
  if( next && set ) {
    size_t i = 0;
    for( i = 0; i < strlen(set); i++ ) {
      if( set[i] == next ) { // match
        if( node ) {
          // collect
          node->type = BN_ASTNODETYPE_SINGLE_CHARACTER;
          node->start = start;
          node->length = 1;
          node->value->single_character = next;
        }
        return 1;
      }
    }
    // no match, go back 1 character
    _dec_pos();
  }
  return 0;
}

// tries to read a character sequence
// note: wrong line counting occurs, when \n or \r are in the character sequence
uint8_t _parse_character_sequence( char *str, BunASTNode_t *node )
{
	// TODO
	// ! error of _parse_character_sequence() or _parse_single_character_of_set()
	//	 is passed a “\r” or “\n” !! (this will not work!)
	
  FilePos_t start = _get_pos();
  size_t i = 0;
  for( i = 0; i < strlen(str); i++ ) {
    char next = _next_char();
    if( ! next || next != str[i] ) {
      _dec_pos_by( i );
      return 0;
    }
  }
  if( node ) {
    // collect
    node->type = BN_ASTNODETYPE_CHARACTER_SEQUENCE;
    node->start = start;
    node->length = strlen(str);
    // ?? store the string itself
  }
  return 1;
}

uint8_t _parse_space( BunASTNode_t *node )
{
  FilePos_t start = _get_pos();
  uint32_t len = 0;
  while( _parse_single_character_from_set( " \t", 0 ) ) {
    len ++;
  }
  if( node ) {
    // collect
    node->type = BN_ASTNODETYPE_SPACE;
    node->start = start;
    node->length = len;
  }
}

/*
    line endings
      unix and os x: \n (decimal charcode 10)
      windows: \r\n
      classic mac: \r (decimal charcode 13)
 */
uint8_t _parse_anything_to_eol( BunASTNode_t *node )
{
  FilePos_t start = _get_pos();
  uint32_t len = 0;
  while( 1 ) {
    char next = _next_char();
    if( ! next ) {
      break;
    }
    else if( next == '\r' ) {
      // peek to next char after that, if \n, consume as well
      char next2 = _next_char();
      if( ! next2 ) {
        break;
      }
      else if( next2 == '\n' ) {
        break;
      }
      else {
        // anything that is not \n
        _dec_pos();
        break;
      }
    }
    else if( next == '\n' ) {
      break;
    }
    len ++;
  }
  if( len > 0 ) {
    if( node ) {
      // collect
      node->type = BN_ASTNODETYPE_ANYTHING_TO_EOL;
      node->start = start;
      node->length = len;
    }
    return 1;
  }
  _set_pos( pos ); // reset position to before trying this non-terminal
  return 0;
}

#define SPACE _parse_space( 0 )
#define ANYTHING_TO_EOL _parse_anything_to_eol( 0 ) ) {

/*
// comment (ignored)
// ... # ...
*/
uint8_t _parse_comment( BunASTNode_t *node )
{
  FilePos_t start = _get_pos();
  if( SPACE && _parse_character_sequence( "#", 0 ) && ANYTHING_TO_EOL ) {
    if( node ) {
      node->type = BN_ASTNODETYPE_COMMENT;
      node->start = start;
      node->length = length;
    }
    return 1;
  }
  _set_pos( start ); // reset position to before trying this non-terminal
  return 0;
}

uint8_t _parse_type_options( BunASTNode_t *node )
{
  // TODO
  return 0;
}

/*
// variable
//    "@" <name> <typename> <typeoptions>
// examples:
//    vec a b c # declare some vectors
*/
int _parse_variable( BunASTNode_t *node )
{
  FilePos_t start = _get_pos();
  BunASTNode_t *name = _alloc_astnode();
  BunASTNode_t *typename = _alloc_astnode();
  BunASTNode_t *typeoptions = _alloc_astnode();
  if( SPACE &&
      _parse_character_sequence( "@", 0 ) && SPACE &&
      _parse_name( name ) && SPACE &&
      _parse_typename( typename ) && SPACE &&
      _parse_typeoptions( typeoptions ) && ANYTHING_TO_EOL ) {
    if( node ) {
      // collect parsed nodes
      node->type = BN_ASTNODETYPE_VARIABLE;
      node->start = start;
      node->length = _get_pos().offset - start.offset;
      node->value->variable->name = name;
      node->value->variable->typename = typename;
      node->value->variable->typeoptions = typeoptions;
    }
    return 1;
  }
  _set_pos( start ); // reset position to before trying this non-terminal
  // you could also just leave it at that (the nodes are not referenced from anywhere...)
  _free_astnode( &name );
  _free_astnode( &typename );
  _free_astnode( &typeoptions );
  return 0;
}

/*
// expression
//   <name> | <function-call> | <literal>
//   	! always be greedy, e.g. TRY to parse a function call (f()) before a pure name (f)
// examples
//   ...
*/
uint8_t _parse_expression( BunASTNode_t *node )
{
  // TODO
  return 0;
}

/*
// function call argument
//   [ <name> ":" ] <expression>
// examples
//   ...
*/
uint8_t _parse_function_call_argument( BunASTNode_t *node )
{
  // TODO
  return 0;
}

/* function call argument list
//   { <function-call-argument> }
*/
uint8_t _parse_function_call_argument_list( BunASTNode_t *node )
{
  // TODO
  return 0;
}

/*
// function call
//    <funcname> ["("] [ <function-call-argument-list> ] [ "->" <function-call-argument-list> ] [")"]
// examples:
//    vec.0 b 2 # second parameter "len" is also 0 as in previous example
//    vec.sum a b -> c # jump to block "vec.sum" and pass locals "a" and "b" into the block, block result "r" is written to local "c"
//    vec.sum( a b -> c ) # same just uses parenthesises
//    vec.sum a:a b:b -> r:c # same but explicitly sets input- and outputs via their name
//    vec.sum a . -> c # almost the same but second param "b" is ommitted (the block treats as a zero vector)
//    vec.sum -> c # same as previous line
*/
uint8_t _parse_function_call( BunASTNode_t *node )
{
  // TODO

  // parse <name>

  // if parse "(": this call is continued until the closing ")" even over eol(s)

  // optional parse <function-call-argument-list>

  // optional parse "->" and <function-call-argument-list>

  //

  // while 1:
  //   optional parse <name> and ":"
  //   parse <expression> or "."
  //   if eol and no-parenthesis: return

  // if parse "->":
  //   while 1:
  //     optional parse <name> and ":"
  //     parse <expression> or "."
  //     if eol and no-parenthesis: return

  return 0;
}

/*
// name
//    <symbol> { "." <symbol> }
*/
uint8_t _parse_name( BunASTNode_t *node )
{
  // TODO
  return 0;
}

/*
// symbol
//    <letter> { <letter>|<digit>|"-"|"_" }
*/
uint8_t _parse_symbol( BunASTNode_t *node )
{
  // TODO
  return 0;
}

/*
// literal
//    <dec-number> | <hex-number> | <bin-number> [ "~" <name> ]
*/
uint8_t _parse_literal( BunASTNode_t *node )
{
  // TODO
  return 0;
}

/*
// block
//    "{" ( <function-call> | <variable> ) "}"
*/
uint8_t _parse_block( BunASTNode_t *node )
{
  // TODO

  // parse "{"

  // while 1: parse <function-call> or <variable>

  // parse "}"

  return 0;
}

/*
// function parameters
//    ...
// examples:
//    num x
//    -> num r
//    num x -> num r
//    num x, num y ..
*/
uint8_t _parse_function_parameters( BunASTNode_t *node )
{
  // TODO
  return 0;
}

/*
// function
//    "@" <name> "(" <function-parameters> ")" ( <block> | <literal> )
// examples:
//    @f 42
//    @f {} # does take neither input nor output parameters
//    @f( num x ) {} # only takes an input parameter "x" of type "num"
//    @f( -> num r ) {} # no inputs, only one output "r" of type "num"
//    @f( num x -> num r ) {} # input and output
//    @f( num x, num y .. ) {} # first param "x" and all the remaining params must be of type "int" and will be available as type "list"
*/
uint8_t _parse_function( BunASTNode_t *node )
{
  // TODO

  // parse "@"

  // parse <name>

  // parse "("

  // parse <function-parameters>

  // parse ")"

  // parse <block> or <literal>

  // parse to eol

  return 1;
}

/*
// program
//   { <function> }
*/
uint8_t _parse_program( BunASTNode_t *node )
{
  FilePos_t start = _get_pos();
  if( node ) {
    node->type = BN_ASTNODETYPE_PROGRAM;
    node->start = start;
  }
  while( ! _end_of_string() ) {
    BunASTNode_t *func = _alloc_astnode();
    if( SPACE && _parse_function( func ) && ANYTHING_TO_EOL ) {
      // collect function into program children
      if( node ) {
        if( node->value->program->num_children == node->value->program->children_length ) {
          node->value->program->children_length += 32;
          node->value->program->children = realloc( node->value->program->children, sizeof(BunASTNode_t*) * node->value->program->children_length );
        }
        node->value->program->children[ node->value->program->num_children ] = func;
        node->value->program->num_children ++;
      }
    }
    else {
      if( node && node->value->program->children ) {
        node->value->program->children_length = 0;
        free( node->value->program->children );
      }
      _free_astnode( &func );
      _set_pos( start ); // reset position to before trying this non-terminal
      return _parser_error( BN_PARSER_ERROR_GENERIC, "Failed to parse program." );
    }
  }
  if( node ) {
    node->length = _get_pos().offset - start.offset;
  }
  return 1;
}

//-----------------------------------------------------------------------------
// language-specific AST node interpreters

// ...

uint8_t _interpreter_error( BunInterpreterError error, char *details, BunASTNode_t *ast, BunMessageQueue_t *inQueue, BunMessageQueue_t *outQueue )
{
  printf("Interpreter error: %s\n", details);
  return 0;
}

uint8_t _interpret_program( BunASTNode_t *ast, BunMessageQueue_t *inQueue, BunMessageQueue_t *outQueue )
{
  if( ast && ast->type == BN_ASTNODETYPE_PROGRAM ) {
    // for each stuff: interpret it
    // TODO
  }
  return _interpreter_error( BN_INTERPRETER_ERROR_GENERIC, "Cannot interpret a non-program AST at this piont", ast, inQueue, outQueue );
}

//=============================================================================
// public api

BunASTNode_t *bn_program_string_to_ast( char *str )
{
  FilePos_t start = { 0 };
  _set_pos( start );
  _set_current_string( 0, str, strlen(str) );
  BunASTNode_t *ast = _alloc_astnode();
  if( _parse_program( ast ) ) {
    return ast;
  }
  else {
    return 0;
  }
}

uint32_t bn_interpret_program_ast( BunASTNode_t *ast )
{
  // setup input message queue
  BunMessageQueue_t inQueue = { 0 };

  // setup output message queue (includes stuff like stdout/stderr messages)
  BunMessageQueue_t outQueue = { 0 };

  return _interpret_program( ast, &inQueue, &outQueue );
}

