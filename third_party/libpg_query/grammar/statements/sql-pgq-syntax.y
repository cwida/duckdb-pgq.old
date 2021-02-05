//select.y 
GraphTable:
	IDENT GRAPH_TABLE
	'(' GraphPattern COLUMNS '(' GraphTableColumnDefinitionList ')' ')'
	;

GraphTableColumnDefinitionList:
	qualified_name AsIdentOptional
	| GraphTableColumnDefinitionList ',' qualified_name AsIdentOptional
	;

AsIdentOptional:
	AS IDENT
	| /* EMPTY */
	;

GraphPattern:
	MATCH PathPatternList
	GraphPatternWhereClauseOptional
	;

GraphPatternWhereClauseOptional:
	WHERE a_expr
	;

PathPatternList:
	PathPatternNameOptional PathConcatenation
	| PathPatternList ',' PathPatternNameOptional PathConcatenation
	;

//Identifier is a Path Name.
PathPatternNameOptional:
	IDENT AS
	| /* EMPTY */
	;

PathConcatenation:
	ElementPattern
	| PathConcatenation ElementPattern
	;

ElementPattern:
	VertexPattern
	| EdgePattern
	;

VertexPattern:
	'(' GraphPatternVariableDeclarationOptional IsLabelExpressionOptional ')'
	;

//Ident is a graph pattern variable
GraphPatternVariableDeclarationOptional:
	IDENT
	| /* EMPTY */
	;

//Ident is a label name 
IsLabelExpressionOptional:
	IsOrColon IDENT
	| /* EMPTY */
	;

IsOrColon:
	IS
	| ':'
	;

MandatoryEdgePatternFiller:
	GraphPatternVariableDeclaration IsLabelExpressionOptional
	| IsOrColon IDENT
	;

EdgePattern:
	FullEdgePattern
	| AbbreviatedEdgePattern
	;

FullEdgePattern:
	FullEdgePointingRight
	| FullEdgePointingLeft
	| FullEdgeAnyDirection
	;

FullEdgePointingRight ::=
	'-[' GraphPatternVariableDeclarationOptional ']->'
	| '-' MandatoryEdgePatternFiller '->'
	;

FullEdgePointingLeft:
	'<-[' GraphPatternVariableDeclarationOptional ']-'
	| '<-' MandatoryEdgePatternFiller '-'
	;

FullEdgeAnyDirection:
	'-[' GraphPatternVariableDeclarationOptional ']-'
	| '-' MandatoryEdgePatternFiller '-'
	;

AbbreviatedEdgePattern:
	'->'
	| '<-'
	| '-'
	;

// <graph pattern quantifier> ::=
// 	<asterisk>
// 	| <plus sign>
// 	| <question mark>
// 	| <fixed quantifier>
// 	| <general quantifier>

// <fixed quantifier> ::=
// 	<left brace> <unsigned integer> <right brace>

// <general quantifier> ::=
// 	<left brace> [ <lower bound> ] ',' [ <upper bound> ] <right brace>

// <lower bound> ::=
// 	<unsigned integer>

// <upper bound> ::=
// 	24 Property Graph Queries (SQL/PGQ)
// 	10.2 <graph pattern>
// 	<unsigned integer>

// <parenthesized path pattern expression> ::=
// 	'(' <path pattern expression>
// 	 [ <parenthesized path pattern where clause> ] ')'
// 	| '[' <path pattern expression>
// 	 [ <parenthesized path pattern where clause> ] ']'

// <parenthesized path pattern where clause> ::=
// 	WHERE <search condition>


// <schema element> ::=
// 	| <property graph definition>

CreatePropertyGraph:
	CREATE PROPERTY GRAPH IDENT PropertyGraphContent
	;

PropertyGraphContent:
	VertexTablesClause EdgeTablesClause
	| /*EMPTY*/
	;

// NODE/VERTEX in the lexer resolve to VERTEX
VertexTablesClause:
	VERTEX TABLES '(' VertexTableDefinitionList ')'
	;
	
VertexTableDefinitionList:
	VertexTableDefinition
	| VertexTableDefinitionList ',' VertexTableDefinition
	;

VertexTableDefinition:
	qualified_name GraphTableKeyClause LabelList
	;

// name_list: column list
GraphTableKeyClause:
	KEY '(' name_list ')'
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
	EdgeTableDefinition
	| EdgeTableDefinitionList ',' EdgeTableDefinition
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

// <add graph table definition> ::=
// 	ADD VertexTablesClause [ ADD EdgeTablesClause ]
// 	| ADD EdgeTablesClause

// <drop graph table definition> ::=
// 	DROP <vertex or edge> TABLES <parenthesized graph table name list> <drop behavior>



// <table name list> ::=
// 	<table name> [ { ',' <table name> }... ]

// <alter graph table definition> ::=
// 	ALTER <vertex or edge> TABLE <graph table name>
// 	<alter graph table action>

// <graph table name> ::=
// 	<table name>
// 	| <identifier>

// <alter graph table action> ::=
// 	<add graph table label clause>
// 	| <drop graph table label clause>
// 	| <alter graph table label properties>

// <add graph table label clause> ::=
// 	<add graph table label>...

// <add graph table label> ::=
// 	ADD LABEL <label name> <graph table properties clause>

// <drop graph table label clause> ::=
// 	<drop graph table label>...

// <drop graph table label> ::=
// 	DROP LABEL <label> <drop behavior>

// <alter graph table label properties> ::=
// 	ALTER LABEL <label> <alter label action>

// <alter label action> ::=
// 	<add property definition>
// 	| <drop property definition>

// <add property definition> ::=
// 	ADD PROPERTIES <graph table parenthesized derived property list>

// <drop property definition> ::=
// 	DROP PROPERTIES <parenthesized property name list> <drop behavior>

// <parenthesized property name list> ::=
// 	'(' <property name list> ')'

// <property name list> ::=
// 	<property name> [ { ',' <property name> }... ]

// <drop property graph statement> ::=
// 	DROP PROPERTY GRAPH <property graph name> <drop behavior>
	