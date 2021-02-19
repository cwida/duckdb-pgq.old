/*****************************************************************************
 *
 *		QUERY :
 *				CREATE PROPERTY GRAPH graphname
                VERTEX TABLES ()
                EDGE TABLES ()
 *
 *****************************************************************************/


 CreatePropertyGraphStmt:
			CREATE_P macro_alias qualified_name param_list AS a_expr
				{
					CreatePropertyGraphStmt *n = makeNode(CreatePropertyGraphStmt);
					n->name = $3;
					n->params = $4;
					n->function = $6;
					$$ = (PGNode *)n;
				}
 		;

macro_alias:
		PROPERTY GRAPH
	;

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
	CREATE_P PROPERTY GRAPH IDENT PropertyGraphContent
	{
		CreatePropertyGraphStmt *n = makeNode(CreatePropertyGraphStmt);
		n->name = $4;
		n->property_graph_content = $5;
		$$ = (PGNode *)n;
	}
	;

PropertyGraphContent:
	VertexTablesClause EdgeTablesClause					{ $$ = $1 $2;}
	| /*EMPTY*/											{ $$ = NIL; }
	;

// NODE/VERTEX in the lexer resolve to VERTEX
VertexTablesClause:
	VERTEX TABLES '(' VertexTableDefinitionList ')'  	{ $$= $4;}
	;
	
VertexTableDefinitionList:
	VertexTableDefinition							{ $$ = list_make1($1); }
	| VertexTableDefinitionList ',' VertexTableDefinition { $$ = lappend($1, $3); }
	;

VertexTableDefinition:
	qualified_name GraphTableKeyClause LabelList	{ $$ = $2 $3;}
	;

// name_list: column list
GraphTableKeyClause:
	KEY '(' name_list ')'				{ $$ = $3; }
	;

LabelList:
	LABEL LabelEnd
	| LabelList LABEL IDENT
	;

LabelEnd:
	qualified_name
	| DEFAULT
	;

// EDGE/RELATIONSHIP will be replaced by the lexer with EDGE
EdgeTablesClause:
	EDGE TABLES EdgeTableDefinitionList
	| /*EMPTY*/
	;

EdgeTableDefinitionList:
	EdgeTableDefinition									{ $$ = list_make1($1); }
	| EdgeTableDefinitionList ',' EdgeTableDefinition 	{ $$ = lappend($1, $3); }
	;

EdgeTableDefinition:
	qualified_name GraphTableKeyClause
	SourceVertexTable
	DestinationVertexTable
	LabelList
	;

SourceVertexTable:
	SOURCE GraphTableKeyClause REFERENCES qualified_name
	;

DestinationVertexTable:
	DESTINATION GraphTableKeyClause REFERENCES qualified_name
	;
