
<delimiter token> ::=
	| <bracket right arrow>
	| <left arrow>
	| <left arrow bracket>
	| <minus left bracket>
	| <right bracket minus>
	| <multiset alternation operator>

<bracket right arrow> ::=
	]->

<left arrow> ::=
	<-

<left arrow bracket> ::=
	<-[

<minus left bracket> ::=
	-[

<right bracket minus> ::=
	]-

<multiset alternation operator> ::=
	|+|

<reserved word> ::=
	| GRAPH
	| GRAPH_TABLE

<non-reserved word> ::=
	| DESTINATION
	| EDGE
	| LABEL
	| NODE
	| PROPERTY
	| PROPERTIES
	| RELATIONSHIP
	| TABLES
	| VERTEX

<destination vertex table name> ::=
	<table name>

<destination vertex table name short> ::=
	<identifier>

<edge table name> ::=
	<table name>

<edge table name short> ::=
	<identifier>

<graph name> ::=
	<schema qualified name>

<graph pattern variable> ::=
	<identifier>

<label> ::=
	<identifier>

<label name> ::=
	<identifier>

<path name> ::=
	<identifier>

<property graph name> ::=
	<schema qualified name>

<property name> ::=
	<identifier>

<source vertex table name> ::=
	<table name>

<source vertex table name short> ::=
	<identifier>

<vertex table name> ::=
	<table name>

<vertex table name short> ::=
	<identifier>

<non-parenthesized value expression primary> ::=
	| <property reference>

<table primary> ::=
	| <graph table> <correlation or recognition>

<graph table> ::=
	<graph reference> GRAPH_TABLE
	'(' <graph pattern> <graph table shape> ')'

<graph table shape> ::=
	<graph table columns clause>

<graph table columns clause> ::=
	COLUMNS '(' <graph table column definition>
	 [ { ',' <graph table column definition> }... ] ')'

<graph table column definition> ::=
	<value expression> [ AS <column name> ]

<graph reference> ::=
	<graph name>

<graph pattern> ::=
	MATCH <path pattern list>
	 [ <graph pattern where clause> ]

<path pattern list> ::=
	<path pattern> [ { ',' <path pattern> }... ]

<path pattern> ::=
	[ <path name> AS ] <path pattern expression>

<path pattern expression> ::=
	<path term>
	| <path multiset alternation>
	| <path set disjunction>

<path multiset alternation> ::=
	<path term> <multiset alternation operator> <path term>
	 [ { <multiset alternation operator> <path term> }... ]

<path set disjunction> ::=
	<path term> <vertical bar> <path term> [ { <vertical bar> <path term> }... ]

<path term> ::=
	<path factor>
	| <path concatenation>

<path concatenation> ::=
	<path term> <path factor>

<path factor> ::=
	<path primary>
	| <quantified path primary>

<quantified path primary> ::=
	<path primary> <graph pattern quantifier>

<path primary> ::=
	<element pattern>
	| <parenthesized path pattern expression>

<element pattern> ::=
	<vertex pattern>
	| <edge pattern>

<vertex pattern> ::=
	'(' <optional element pattern filler> ')'

<optional element pattern filler> ::=
	[ <graph pattern variable declaration> ]
	[ <is label expression> ]
	[ <element pattern where clause> ]

<graph pattern variable declaration> ::=
	<graph pattern variable>

<is label expression> ::=
	<is or colon> <label expression>

<is or colon> ::=
	IS
	| <colon>

<element pattern where clause> ::=
	WHERE <search condition>

<mandatory edge pattern filler> ::=
	<graph pattern variable declaration> [ <is label expression> ]
	| <is label expression>

<edge pattern> ::=
	<full edge pattern>
	| <abbreviated edge pattern>

<full edge pattern> ::=
	<full edge pointing right>
	| <full edge pointing left>
	| <full edge: any direction>

<full edge pointing right> ::=
	<minus left bracket> <optional element pattern filler> <bracket right arrow>
	| <minus sign> <mandatory edge pattern filler> <right arrow>

<full edge pointing left> ::=
	<left arrow bracket> <optional element pattern filler> <right bracket minus>
	| <left arrow> <mandatory edge pattern filler> <minus sign>

<full edge: any direction> ::=
	<minus left bracket> <optional element pattern filler> <right bracket minus>
	| <minus sign> <mandatory edge pattern filler> <minus sign>

<abbreviated edge pattern> ::=
	<right arrow>
	| <left arrow>
	| <minus sign>

<graph pattern quantifier> ::=
	<asterisk>
	| <plus sign>
	| <question mark>
	| <fixed quantifier>
	| <general quantifier>

<fixed quantifier> ::=
	<left brace> <unsigned integer> <right brace>

<general quantifier> ::=
	<left brace> [ <lower bound> ] ',' [ <upper bound> ] <right brace>

<lower bound> ::=
	<unsigned integer>

<upper bound> ::=
	24 Property Graph Queries (SQL/PGQ)
	10.2 <graph pattern>
	<unsigned integer>

<parenthesized path pattern expression> ::=
	'(' <path pattern expression>
	 [ <parenthesized path pattern where clause> ] ')'
	| <left bracket> <path pattern expression>
	 [ <parenthesized path pattern where clause> ] <right bracket>

<parenthesized path pattern where clause> ::=
	WHERE <search condition>

<graph pattern where clause> ::=
	WHERE <search condition>

<label expression> ::=
	<label term>
	| <label disjunction>

<label disjunction> ::=
	<label expression> <vertical bar> <label term>

<label term> ::=
	<label factor>
	| <label conjunction>

<label conjunction> ::=
	<label term> <ampersand> <label factor>

<label factor> ::=
	<label primary>
	| <label negation>

<label negation> ::=
	<exclamation mark> <label primary>

<label primary> ::=
	<label>
	| <parenthesized label expression>

<parenthesized label expression> ::=
	'(' <label expression> ')'
	| <left bracket> <label expression> <right bracket>

<schema element> ::=
	| <property graph definition>

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
	qualified_name GraphTableKeyClauseOptional LabelList
	;

GraphTableKeyClauseOptional:
	GraphTableKeyClause
	| /*EMPTY*/
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
	EDGE TABLES <parenthesized edge table list>
	| /*EMPTY*/
	;


<parenthesized edge table list> ::=
	<property graph definition>
	'(' <edge table definition> [ { ',' <edge table definition> }... ] ')'

<edge table definition> ::=
	qualified_name GraphTableKeyClauseOptional
	<source vertex table>
	<destination vertex table>
	GraphTableLabelAndPropertiesClause
	;

<source vertex table> ::=
	SOURCE SourceVertexTableKeyClauseReferencesOptional <source vertex reference>

SourceVertexTableKeyClauseReferencesOptional:
	GraphTableKeyClause REFERENCES
	| /*EMPTY*/
	;


<source vertex reference> ::=
	qualified_name [ '(' <referenced source column list> ')' ]

<referenced source column list> ::=
	<column name list>

<destination vertex table> ::=
	DESTINATION [ <destination vertex table key clause> REFERENCES ]
	<destination vertex reference>

<destination vertex table key clause> ::=
	GraphTableKeyClause

<destination vertex reference> ::=
	<destination vertex table alternative>
	 [ '(' <referenced destination column list> ')' ]

<destination vertex table alternative> ::=
	<destination vertex table name>
	| <destination vertex table name short>

<referenced destination column list> ::=
	<column name list>

<alter property graph statement> ::=
	ALTER PROPERTY GRAPH <property graph name>
	<alter property graph action>

<alter property graph action> ::=
	<add graph table definition>
	| <drop graph table definition>
	| <alter graph table definition>

<add graph table definition> ::=
	ADD VertexTablesClause [ ADD EdgeTablesClause ]
	| ADD EdgeTablesClause

<drop graph table definition> ::=
	DROP <vertex or edge> TABLES <parenthesized graph table name list> <drop behavior>

<vertex or edge> ::=
	<vertex synonym>
	| <edge synonym>

<parenthesized graph table name list> ::=
	<parenthesized table name list>

<parenthesized table name list> ::=
	'(' <table name list> ')'

<table name list> ::=
	<table name> [ { ',' <table name> }... ]

<alter graph table definition> ::=
	ALTER <vertex or edge> TABLE <graph table name>
	<alter graph table action>

<graph table name> ::=
	<table name>
	| <identifier>

<alter graph table action> ::=
	<add graph table label clause>
	| <drop graph table label clause>
	| <alter graph table label properties>

<add graph table label clause> ::=
	<add graph table label>...

<add graph table label> ::=
	ADD LABEL <label name> <graph table properties clause>

<drop graph table label clause> ::=
	<drop graph table label>...

<drop graph table label> ::=
	DROP LABEL <label> <drop behavior>

<alter graph table label properties> ::=
	ALTER LABEL <label> <alter label action>

<alter label action> ::=
	<add property definition>
	| <drop property definition>

<add property definition> ::=
	ADD PROPERTIES <graph table parenthesized derived property list>

<drop property definition> ::=
	DROP PROPERTIES <parenthesized property name list> <drop behavior>

<parenthesized property name list> ::=
	'(' <property name list> ')'

<property name list> ::=
	<property name> [ { ',' <property name> }... ]

<drop property graph statement> ::=
	DROP PROPERTY GRAPH <property graph name> <drop behavior>
	