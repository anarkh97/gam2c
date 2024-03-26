%{
#include <math.h>
#include <stdio.h>
#include <string.h>
extern int  yyCmdflex(void);
extern void yyCmdferror(const char  *);
#include "Assigner.h"
#include "Dictionary.h"
#include "ResizeArray.h"
#include "CustomArray.h"

ResizeArray<Assigner *> underVar(0);
int underLevel = 0;

%}

%code requires {
#include <set>
}

%union
{
 int                       ival;
 double                    dval;
 int	                  token;
 char                     *sval;
 Assigner                 *asgn;
 std::set<int>           *ilist;
 std::set<double>        *dlist;
 std::set<std::string>   *slist;
}

%token UNDER EoF IntConstant DblConstant Symbol String END BOGUS

%left '+' '-'
%left '*' '/'
%left '^'

%type <ival> IntConstant IntExpr
%type <dval> DblConstant DblExpr
%type <ilist> IntSetExpr IntSetLoop
%type <dlist> DblSetExpr DblSetLoop
%type <slist> StrSetExpr StrSetLoop
%type <token> Symbol
%type <asgn>	Assignable
%type <sval>	String

/* Rules */
%%

File: ValidInput EoF { return 0; }
	| ValidInput END
	{ return 0; }

ValidInput: 
	| ValidInput Assignment
	| ValidInput GroupInput

Assignment: Assignable '=' Symbol ';'
	{ $1->assignToken($3); }
	| Assignable '=' IntExpr ';'
	{ $1->assignInt($3); }
	| Assignable '=' DblExpr ';'
	{ $1->assignDouble($3); }
  	| Assignable '=' IntSetExpr ';'
  	{ $1->assignSet<int>($3); }
  	| Assignable '=' DblSetExpr ';'
  	{ $1->assignSet<double>($3); }
  	| Assignable '=' StrSetExpr ';'
  	{ $1->assignSet<std::string>($3); }
	| Assignable '=' String ';'
	{ $1->assignString(strdup($3)); }

GroupInput: UNDER Assignable '{'
	{
    underVar[underLevel] = $2; underLevel++;
  }
	 ValidInput '}'
	{ underLevel--; }


Assignable:
	Symbol
	{ 
    $$ = 0;
    for(int i = underLevel; i-- ; ) {
      $$ = underVar[i]->findSubToken($1);
      if($$ != 0)  
        break; 
    } 
    if($$ == 0)
      $$ = findSysObj($1); 
  }
	| Assignable '.' Symbol
	{ 
    $$ = $1->findSubToken($3); 
    if($$ == 0) {
      fprintf(stderr, "ERROR: Structure element not found: %s\n",
      dictionary->word($3).c_str());
      exit(-1);
    }
  }
  | Assignable '[' IntExpr ']'
  { $$ = $1->findIndexObject($3); 
    if($$ == 0) {
      fprintf(stderr, "ERROR: Object is not an array\n");
      exit(-1);
    }
  }
        


IntExpr:
	IntConstant
	| '(' IntExpr ')'
	{ $$ = $2; }
	| IntExpr '+' IntExpr
	{ $$ = $1 + $3; }
	| IntExpr '-' IntExpr
	{ $$ = $1 - $3; }
	| IntExpr '*' IntExpr
	{ $$ = $1 * $3; }
	| IntExpr '/' IntExpr
	{ $$ = $1 / $3; }

DblExpr:
	DblConstant
	| '(' DblExpr ')'
  { $$ = $2; }
  | DblExpr '+' DblExpr
  { $$ = $1 + $3; }
  | DblExpr '+' IntExpr
	{ $$ = $1 + $3; }
  | IntExpr '+' DblExpr
	{ $$ = $1 + $3; }
  | DblExpr '-' DblExpr
  { $$ = $1 - $3; }
  | DblExpr '-' IntExpr
  { $$ = $1 - $3; }
  | IntExpr '-' DblExpr
  { $$ = $1 - $3; }
  | DblExpr '*' DblExpr
  { $$ = $1 * $3; }
  | IntExpr '*' DblExpr
  { $$ = $1 * $3; }
  | DblExpr '*' IntExpr
  { $$ = $1 * $3; }
  | DblExpr '/' DblExpr
  { $$ = $1 / $3; }
  | IntExpr '/' DblExpr
  { $$ = $1 / $3; }
  | DblExpr '/' IntExpr
  { $$ = $1 / $3; }

IntSetExpr: 
  '(' IntSetLoop ')'
  {
    $$ = $2;
  }

IntSetLoop: 
  IntExpr
  { 
    $$ = new std::set<int>();
    $$->insert($1);
  }
  | IntSetLoop ',' IntExpr
  {
    $1->insert($3);
    $$ = $1;
  }

DblSetExpr: 
  '(' DblSetLoop ')'
  {
    $$ = $2;
  }

DblSetLoop: 
  DblExpr
  { 
    $$ = new std::set<double>();
    $$->insert($1);
  }
  | DblSetLoop ',' DblExpr
  {
    $1->insert($3);
    $$ = $1;
  }

StrSetExpr: 
  '(' StrSetLoop ')'
  {
    $$ = $2;
  }

StrSetLoop: 
  String
  { 
    $$ = new std::set<std::string>();
    $$->insert($1);
  }
  | StrSetLoop ',' String
  {
    $1->insert($3);
    $$ = $1;
  }

%%
