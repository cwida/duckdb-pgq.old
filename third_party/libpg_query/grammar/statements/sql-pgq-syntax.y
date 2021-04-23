// // //select.y 
// //opt_clause
// insert_column_list 
// opt_target_list ColumnList
//ident is pg name 
GraphTableStmt:                 
	'(' IDENT ',' MATCH 
    PathPatternList
	where_clause
    ColumnsClauseOptional
     ')' qualified_name
    {
        PGMatchPattern *n = makeNode(PGMatchPattern);
        n->pg_name = $2;
        n->pattern = $5;
        n->where_clause = $6;
        n->columns = $7;
        
        n->name = $9;
        // n->pg_name = $6;
        // n->pattern = $9;
        // n->where_clause = $10;
        // n->columns = $11;
        
        // n->name = $13;
        $$ = (PGNode *) n;
    }
	;

// // GraphTableColumnDefinitionList:
// // 	ColumnElement                                       { $$ = list_make1($1); }                              
// // 	| GraphTableColumnDefinitionList ',' ColumnElement  { $$ = lappend($1, $3); }
// // 	;

// // ColumnsElement:
// // 	qualified_name AS IDENT
// // 	| qualified_name 
// // 	;

// // GraphPattern:
// // 	MATCH PathPatternList
// // 	GraphPatternWhereClauseOptional
// //     ColumnsClauseOptional
// // 	;

// // GraphPatternWhereClauseOptional:
// // 	WHERE a_expr                                                { $$ = $2; }
// //     | /* EMPTY */                                               { $$ = NIL; }
// // 	;

// Columns optional
ColumnsClauseOptional:
    COLUMNS '(' ColumnList ')'                                  { $$ = $3; }
    | /* EMPTY */                                              { $$ = NIL; }
    ;

ColumnList:
			insert_target  									
                { 
                    $$ = list_make1($1); 
                }
			| ColumnList ',' insert_target 	                    
                {
                     $$ = lappend($1, $3); 
                }
            ;

// ColumnElement: qualified_name
//             {
//                 $$ = $1;
//             }
//             |
//             qualified_name AS IDENT                          
//             {   
//                 $1->alias = makeAlias($3, NIL);
// 					$$ = $1;
//             }
               
//         ;         

PathPatternList:
	PathPatternNameOptional PathConcatenation                   
    { 
        $$ = list_make1(list_make2($1,$2)); 
    }
	| PathPatternList ',' PathPatternNameOptional PathConcatenation     
    { 
        $$ = lappend($1, list_make1(list_make2($3,$4)));
    }
	;

// Identifier is a Path Name.
PathPatternNameOptional:
	IDENT AS                                            {$$ = $1; }
	| /* EMPTY */                                       {$$ = NULL;}
	;

PathConcatenation:
	ElementPattern                                      {$$ = list_make1($1);}
	| PathConcatenation ElementPattern                  { $$ = lappend($1, $2);}
	;

ElementPattern:
	VertexPattern                               { $$ = $1; }
	| EdgePattern                               { $$ = $1; }
	;

VertexPattern:
	'(' GraphPatternVariableDeclarationOptional IsLabelExpressionOptional ')' 
    { 
        PGVertexPattern *n = makeNode(PGVertexPattern);
        n->variable_name = $2;
        n->alias = $3;
        $$ = (PGNode *) n;
        // $2->alias = makeAlias($3, NIL);
		// 	$$ = $2;
        }
	;

//Ident is a graph pattern variable
GraphPatternVariableDeclarationOptional:
	IDENT                                   { $$ = $1; }
	| /* EMPTY */                           { $$ = NULL; }
	;

IsLabelExpression:
    IsOrColon IDENT                         { $$ = $2;}

//Ident is a label name 
IsLabelExpressionOptional:
	IsLabelExpression                       { $$ = $1; }
	| /* EMPTY */                           { $$ = NULL; }
	;

IsOrColon:
	IS
	| ':'
	;
// //GraphPatternVariableDeclaration?? check
//ident is GraphPatternVariableDeclaration
MandatoryEdgePatternFiller:
	IDENT IsLabelExpressionOptional   
    { 
        PGVertexPattern *n = makeNode(PGVertexPattern);
        n->variable_name = $1;
        n->alias = $2;
        $$ = (PGNode *) n;
        // // $$ = $1;
        // $$ = makeNode(PGAlias);
		// $$->aliasname = $2;
            // $1->alias = makeAlias($2, NIL);
			// $$ = $1;
    }
	| 
    IsLabelExpression                                       
    { 
        // $$ = $1;
        PGVertexPattern *n = makeNode(PGVertexPattern);
        n->variable_name = $1;
        $$ = (PGNode *) n;
         // $$ = $2; 
        // $$ = makeNode(PGAlias);
		// $$->aliasname = $2;
    }
// 	;

EdgePattern:
	FullEdgePattern                                         {$$ = $1;}
	| 
    AbbreviatedEdgePattern                                {$$ = (PGNode *) makeString($1);}
	;

FullEdgePattern:
	FullEdgePointingRight                                   {$$ = $1;}
	| FullEdgePointingLeft                                  {$$ = $1;}
	| FullEdgeAnyDirection                                  {$$ = $1;}
	;

// changed [] from VariableDeclarationOptional. verify from specification
FullEdgePointingRight:
	'-' '[' MandatoryEdgePatternFiller ']' '-' '>'      { $$ = $3;}
	| '-' MandatoryEdgePatternFiller '-' '>'                   { $$ = $2;}
	;

FullEdgePointingLeft:
	'<' '-' '[' MandatoryEdgePatternFiller ']' '-'      { $$ = $4;}
	| '<' '-' MandatoryEdgePatternFiller '-'                   { $$ = $3;}
	;

FullEdgeAnyDirection:
	'-' '[' MandatoryEdgePatternFiller ']' '-'       { $$ =$3;}
	| '-' MandatoryEdgePatternFiller '-'                    { $$ = $2;}
	;

AbbreviatedEdgePattern:
	'-'                     { $$ = "-"; }
    | '-' '>'                  { $$ = "->"; }
    | '<' '-'                  { $$ = "<-"; }
    ;


// // */
// // <graph pattern quantifier> ::=
// // 	<asterisk>
// // 	| <plus sign>
// // 	| <question mark>
// // 	| <fixed quantifier>
// // 	| <general quantifier>

// // <fixed quantifier> ::=
// // 	<left brace> <unsigned integer> <right brace>

// // <general quantifier> ::=
// // 	<left brace> [ <lower bound> ] ',' [ <upper bound> ] <right brace>

// // <lower bound> ::=
// // 	<unsigned integer>

// // <upper bound> ::=
// // 	24 Property Graph Queries (SQL/PGQ)
// // 	10.2 <graph pattern>
// // 	<unsigned integer>

// // <parenthesized path pattern expression> ::=
// // 	'(' <path pattern expression>
// // 	 [ <parenthesized path pattern where clause> ] ')'
// // 	| '[' <path pattern expression>
// // 	 [ <parenthesized path pattern where clause> ] ']'

// // <parenthesized path pattern where clause> ::=
// // 	WHERE <search condition>


// // <schema element> ::=
// // 	| <property graph definition>


// // <add graph table definition> ::=
// // 	ADD VertexTablesClause [ ADD EdgeTablesClause ]
// // 	| ADD EdgeTablesClause

// // <drop graph table definition> ::=
// // 	DROP <vertex or edge> TABLES <parenthesized graph table name list> <drop behavior>



// // <table name list> ::=
// // 	<table name> [ { ',' <table name> }... ]

// // <alter graph table definition> ::=
// // 	ALTER <vertex or edge> TABLE <graph table name>
// // 	<alter graph table action>

// // <graph table name> ::=
// // 	<table name>
// // 	| <identifier>

// // <alter graph table action> ::=
// // 	<add graph table label clause>
// // 	| <drop graph table label clause>
// // 	| <alter graph table label properties>

// // <add graph table label clause> ::=
// // 	<add graph table label>...

// // <add graph table label> ::=
// // 	ADD LABEL <label name> <graph table properties clause>

// // <drop graph table label clause> ::=
// // 	<drop graph table label>...

// // <drop graph table label> ::=
// // 	DROP LABEL <label> <drop behavior>

// // <alter graph table label properties> ::=
// // 	ALTER LABEL <label> <alter label action>

// // <alter label action> ::=
// // 	<add property definition>
// // 	| <drop property definition>

// // <add property definition> ::=
// // 	ADD PROPERTIES <graph table parenthesized derived property list>

// // <drop property definition> ::=
// // 	DROP PROPERTIES <parenthesized property name list> <drop behavior>

// // <parenthesized property name list> ::=
// // 	'(' <property name list> ')'

// // <property name list> ::=
// // 	<property name> [ { ',' <property name> }... ]

// // <drop property graph statement> ::=
// // 	DROP PROPERTY GRAPH <property graph name> <drop behavior>