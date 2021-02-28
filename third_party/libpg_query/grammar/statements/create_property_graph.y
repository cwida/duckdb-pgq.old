/*****************************************************************************
 *
 *		QUERY :
 *				CREATE PROPERTY GRAPH graphname
                VERTEX TABLES ()
                EDGE TABLES ()
 *
 *****************************************************************************/


//  CreatePropertyGraphStmt:
// 			CREATE_P macro_alias qualified_name param_list AS a_expr
// 				{
// 					CreatePropertyGraphStmt *n = makeNode(CreatePropertyGraphStmt);
// 					n->name = $3;
// 					n->params = $4;
// 					n->function = $6;
// 					$$ = (PGNode *)n;
// 				}
//  		;

// macro_alias:
// 		PROPERTY GRAPH
// 	;
// 
// param_list:
// 		'(' ')'
// 			{
// 				$$ = NIL;
// 			}
// 		| '(' func_arg_list ')'
// 			{
// 				$$ = $2;
// 			}
// 	;
 

CreatePropertyGraphStmt:
	CREATE_P PROPERTY GRAPH IDENT 
	vertex_alias TABLES '(' VertexTableDefinitionList ')' 
	edge_alias TABLES '(' EdgeTableDefinitionList ')'
	// 
	{
		// PGCreatePropertyGraphStmt *n = $5; //PGList ? 
		PGCreatePropertyGraphStmt *n = makeNode(PGCreatePropertyGraphStmt);
		n->name = $4;
		n->vertex_tables = 	$8;
		n->edge_tables = $13;
		// n->property_graph_content = $5;
		$$ = (PGNode *)n;
		// n = $5;
	}
	| CREATE_P PROPERTY GRAPH IDENT 
	{
		PGCreatePropertyGraphStmt *n = makeNode(PGCreatePropertyGraphStmt);
		n->name = $4;
		$$ = (PGNode *)n;
	}

	;

// PropertyGraphContent:
// 	vertex_alias TABLES '(' VertexTableDefinitionList ')' edge_alias TABLES '(' EdgeTableDefinitionList					
// 	{ 	
// 		PGCreatePropertyGraphStmt *n = makeNode(PGCreatePropertyGraphStmt);
// 		n->vertex_tables = 	$4;
// 		n->edge_tables = $8;
// 		$$ = (PGNode *)n;
// 	}
// 	| /*EMPTY*/											{ $$ = NIL; }
// 	;

vertex_alias:
		VERTEX
		| NODE
	;

edge_alias:
		EDGE
		| RELATIONSHIP
	;
// NODE/VERTEX in the lexer resolve to VERTEX
// VertexTablesClause:
// 	VERTEX TABLES '(' VertexTableDefinitionList ')'  	{ $$= $4;}
// 	;
	
VertexTableDefinitionList:
	VertexTableDefinition			{ $$ = list_make1($1); }
	| VertexTableDefinitionList ',' VertexTableDefinition { $$ = lappend($1, $3); }
	;

VertexTableDefinition:
	qualified_name GraphTableKeyClause LabelList
	// qualified_name KEY '(' name_list ')' LabelList		
	{
		PGPropertyGraphTable *n = makeNode(PGPropertyGraphTable);
		n->name = $1;
		n->labels = $3;
		n->keys = $2;
	}
	;

// name_list: column list
GraphTableKeyClause:
	KEY '(' name_list ')'				{ $$ = $3; }
	;

LabelList:
	LABEL LabelEnd					{ $$ = list_make1($2); }
	| LabelList LABEL IDENT			{ $$ = lappend($1, $3); }
	;

LabelEnd:
	IDENT				{ $$ = $1; }
	// | /* Empty */		{ $$ = NIL; }
	// | DEFAULT					{ $$ = $1; }	//all columns??
	;

// EDGE/RELATIONSHIP will be replaced by the lexer with EDGE
// EdgeTablesClause:
// 	EDGE TABLES EdgeTableDefinitionList 	
// 	| /*EMPTY*/
// 	;

EdgeTableDefinitionList:
	EdgeTableDefinition									{ $$ = list_make1($1); }
	| EdgeTableDefinitionList ',' EdgeTableDefinition 	{ $$ = lappend($1, $3); }
	;

EdgeTableDefinition:
	// qualified_name GraphTableKeyClause
	// SourceVertexTable
	// DestinationVertexTable
	// LabelList
	qualified_name GraphTableKeyClause
	SOURCE GraphTableKeyClause REFERENCES qualified_name
	DESTINATION GraphTableKeyClause REFERENCES qualified_name
	LabelList
	{
		PGPropertyGraphTable *n = makeNode(PGPropertyGraphTable);
		n->name = $1;
		n->keys = $2;
		// n->labels = $10;
		// n->source_key = $3;
		// n->source_key_reference = $5;
		// n->destination_key = $7;
		// n->destination_key_reference = $9;

		n->source_key = $4;
		n->source_key_reference = $6;
		n->destination_key = $8;
		n->destination_key_reference = $10;
		n->labels = $11; 
		//Do I need a struct for Source/Dest Vertex Table
	}
	;

// SourceVertexTable:
// 	SOURCE GraphTableKeyClause REFERENCES qualified_name
// 	{
// 		$$ = $2;
// 	}
// 	;

// DestinationVertexTable:
// 	DESTINATION GraphTableKeyClause REFERENCES qualified_name
// 	{
// 		$$ = $2;
// 	}
// 	;