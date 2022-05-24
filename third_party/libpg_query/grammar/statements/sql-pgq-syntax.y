// // //select.y 
// //opt_clause
// insert_column_list 
// opt_target_list ColumnList
//ident is pg name 
GraphTableStmt:                 
	'(' IDENT ',' MATCH SHORTEST PathPatternList
	where_clause
    ColumnsClauseOptional
     ')' qualified_name
    {
        PGMatchPattern *n = makeNode(PGMatchPattern);
        n->pg_name = $2;
        n->distance_type = PG_DISTANCE_TYPE_SHORTEST;
        n->pattern = $6;
        n->where_clause = $7;
        n->columns = $8;
        n->name = $10;
        $$ = (PGNode *) n;
    }
	|
	'(' IDENT ',' MATCH CHEAPEST PathPatternList
        	where_clause
            ColumnsClauseOptional
             ')' qualified_name
            {
                PGMatchPattern *n = makeNode(PGMatchPattern);
                n->pg_name = $2;
                n->distance_type = PG_DISTANCE_TYPE_CHEAPEST;
                n->pattern = $6;
                n->where_clause = $7;
                n->columns = $8;
                n->name = $10;
                $$ = (PGNode *) n;
            }
            |
	'(' IDENT ',' MATCH PathPatternList
		where_clause
		ColumnsClauseOptional
		 ')' qualified_name
		{
		    PGMatchPattern *n = makeNode(PGMatchPattern);
		    n->pg_name = $2;
		    n->distance_type = PG_DISTANCE_TYPE_SHORTEST;
		    n->pattern = $5;
		    n->where_clause = $6;
		    n->columns = $7;
		    n->name = $9;
		    $$ = (PGNode *) n;
		}
;


//OptionalShortestOrCheapest:
//	SHORTEST { $$ = PG_DISTANCE_TYPE_SHORTEST; }
//	| CHEAPEST { $$ = PG_DISTANCE_TYPE_CHEAPEST; }
//	|  /* EMPTY */ { $$ = NULL; }
//	;


// Columns optional
ColumnsClauseOptional:
    COLUMNS '(' ColumnList ')'                               { $$ = $3; }
    | /* EMPTY */                                              { $$ = NIL; }
    ;

ColumnList:
			target_el  									
                { 
                    $$ = list_make1($1); 
                }
			| ColumnList ',' target_el 	                    
                {
                     $$ = lappend($1, $3); 
                }
            ;

//DistancePathPatternList:
//	SHORTEST PathPatternList {
//		PGDistancePattern *n = makeNode(PGDistancePattern);
//		n->distance_type = PG_DISTANCE_TYPE_SHORTEST;
//		n->pattern = $2;
//		$$ = (PGNode *) n;
//	}
//	| CHEAPEST PathPatternList {
//		PGDistancePattern *n = makeNode(PGDistancePattern);
//		n->distance_type = PG_DISTANCE_TYPE_CHEAPEST;
//		n->pattern = $2;
//		$$ = (PGNode *) n;
//	}
//	| PathPatternList {
//		PGDistancePattern *n = makeNode(PGDistancePattern);
//		n->distance_type = PG_DISTANCE_TYPE_SHORTEST;
//		n->pattern = $1;
//		$$ = (PGNode *) n;
//}

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


ElementPatternFillerOptional:
            GraphPatternVariableDeclarationOptional IsLabelExpressionOptional
            {
                PGGraphVariablePattern *n = makeNode(PGGraphVariablePattern);
                n->alias_name = $1;
                n->label_name = $2;
                $$ = (PGNode *) n;
            }
VertexPatternFiller: 
            IDENT IsLabelExpressionOptional
            {
                PGGraphVariablePattern *n = makeNode(PGGraphVariablePattern);
                n->alias_name = $1;
                n->label_name = $2;
                $$ = (PGNode *) n;
            }
VertexPattern:
            '(' VertexPatternFiller ')' 
            { 
                PGGraphElementPattern *n = makeNode(PGGraphElementPattern);
                n->pattern_clause = $2;
                // n->label_name = $3;
                n->is_vertex_pattern = true;
                $$ = (PGNode *) n;
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

EdgeCostPattern:
	'(' FullEdgePattern COST

//ident is GraphPatternVariableDeclaration
MandatoryEdgePatternFiller:
            IDENT IsLabelExpressionOptional  
            { 
                PGGraphVariablePattern *n = makeNode(PGGraphVariablePattern);
                n->alias_name = $1;
                n->label_name = $2;
                $$ = (PGNode *) n;
            }
            | IsLabelExpression                                       
            { 
                PGGraphVariablePattern *n = makeNode(PGGraphVariablePattern);
                n->alias_name = $1;
                $$ = (PGNode *) n;
            }
	    ;

EdgePattern:
            FullEdgePattern                                         {$$ = $1;}
            // | 
            // AbbreviatedEdgePattern                                
            // {
	    // how to handle this for conversion in tranform
            //     $$ = (PGNode *) makeString($1);
            // }
        ;

FullEdgePattern:
            FullEdgePointingRight                                   {$$ = $1;}
            | FullEdgePointingLeft                                  {$$ = $1;}
            | FullEdgeAnyDirection                                  {$$ = $1;}
        ;

// changed after verifying from specification
//LAMBDA_ARROW -> is a special token defined in grammar. Directly parsing does not work. 
FullEdgePointingRight:
        '-' '[' ElementPatternFillerOptional ']' LAMBDA_ARROW      
            {
                PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                n->pattern_clause = $3;
                n->direction = PG_MATCH_DIR_RIGHT;
                n->is_vertex_pattern = false;
                n->star_pattern = PG_STAR_NONE;
                $$ = (PGNode *) n;
            }
        | '-' '[' ElementPatternFillerOptional '*' ']' LAMBDA_ARROW      
            {
                PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                n->pattern_clause = $3;
                n->direction = PG_MATCH_DIR_RIGHT;
                n->is_vertex_pattern = false;
                n->star_pattern = PG_STAR_ALL;
                // $$ = $3;
                $$ = (PGNode *) n;
            }
        | '-' '[' ElementPatternFillerOptional '*' ICONST DOT_DOT ICONST ']' LAMBDA_ARROW      
            {
                PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                n->pattern_clause = $3;
                n->direction = PG_MATCH_DIR_RIGHT;
                n->is_vertex_pattern = false;
                n->star_pattern = PG_STAR_BOUNDED;
                n->lower_bound = $5;
                n->upper_bound = $7;
                $$ = (PGNode *) n;
            }
        | '-' MandatoryEdgePatternFiller LAMBDA_ARROW                   
            { 
                PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                n->pattern_clause = $2;
                n->direction = PG_MATCH_DIR_RIGHT;
                n->is_vertex_pattern = false;
                n->star_pattern = PG_STAR_NONE;
                $$ = (PGNode *) n;
            }
        ;

FullEdgePointingLeft:
            '<' '-' '[' ElementPatternFillerOptional ']' '-'                
                { 
                    PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                    n->pattern_clause = $4;
                    n->direction = PG_MATCH_DIR_LEFT;
                    n->is_vertex_pattern = false;
                    n->star_pattern = PG_STAR_NONE;
                    $$ = (PGNode *) n;
                }
            |
            '<' '-' '[' ElementPatternFillerOptional '*' ']' '-'                
                { 
                    PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                    n->pattern_clause = $4;
                    n->direction = PG_MATCH_DIR_LEFT;
                    n->is_vertex_pattern = false;
                    n->star_pattern = PG_STAR_ALL;
                    $$ = (PGNode *) n;
                }
            |
            '<' '-' '[' ElementPatternFillerOptional '*' ICONST DOT_DOT ICONST ']' '-'                
                { 
                    PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                    n->pattern_clause = $4;
                    n->direction = PG_MATCH_DIR_LEFT;
                    n->is_vertex_pattern = false;
                    n->star_pattern = PG_STAR_BOUNDED;
                    n->lower_bound = $6;
                    n->upper_bound = $8;
                    $$ = (PGNode *) n;
                }
            | '<' '-' MandatoryEdgePatternFiller '-'                        
                { 
                    
                    PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                    n->pattern_clause = $3;
                    n->direction = PG_MATCH_DIR_LEFT;
                    n->is_vertex_pattern = false;
                    n->star_pattern = PG_STAR_NONE;
                    $$ = (PGNode *) n;
                }
            ;

FullEdgeAnyDirection:
            '-' '[' ElementPatternFillerOptional ']' '-'       
                { 
                    PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                    n->pattern_clause = $3;
                    n->direction = PG_MATCH_DIR_ANY;
                    n->is_vertex_pattern = false;
                    n->star_pattern = PG_STAR_NONE;
                    $$ = (PGNode *) n;
                }
            | '-' '[' ElementPatternFillerOptional '*' ']' '-'       
                { 
                    PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                    n->pattern_clause = $3;
                    n->direction = PG_MATCH_DIR_ANY;
                    n->is_vertex_pattern = false;
                    n->star_pattern = PG_STAR_ALL;
                    $$ = (PGNode *) n;
                }
            | '-' '[' ElementPatternFillerOptional '*' ICONST DOT_DOT ICONST ']' '-'       
                { 
                    PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                    n->pattern_clause = $3;
                    n->direction = PG_MATCH_DIR_ANY;
                    n->is_vertex_pattern = false;
                    n->star_pattern = PG_STAR_BOUNDED;
                    n->lower_bound = $5;
                    n->upper_bound = $7;
                    $$ = (PGNode *) n;
                }
            
            | '-' MandatoryEdgePatternFiller '-'                    
                { 
                    PGGraphElementPattern *n = makeNode(PGGraphElementPattern); 
                    n->pattern_clause = $2;
                    n->direction = PG_MATCH_DIR_ANY;
                    n->is_vertex_pattern = false;
                    n->star_pattern = PG_STAR_NONE;
                    $$ = (PGNode *) n;
                }

	    | '-' '[' ElementPatternFillerOptional COST ColId']' '-'
		{
		    PGGraphElementPattern *n = makeNode(PGGraphElementPattern);
		    n->pattern_clause = $3;
		    n->direction = PG_MATCH_DIR_ANY;
		    n->is_vertex_pattern = false;
		    n->star_pattern = PG_STAR_NONE;
		    n->cost_pattern = $5;
		    $$ = (PGNode *) n;
		}
            ;

//avoiding unions can scope out
//scoped out
// AbbreviatedEdgePattern:
//             '-'                     { $$ = "-"; }
//             |  LAMBDA_ARROW                  { $$ = "->"; }
//             | '<' '-'                  { $$ = "<-"; }
//         ;


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