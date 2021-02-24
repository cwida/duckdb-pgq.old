/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_BASE_YY_THIRD_PARTY_LIBPG_QUERY_GRAMMAR_GRAMMAR_OUT_HPP_INCLUDED
# define YY_BASE_YY_THIRD_PARTY_LIBPG_QUERY_GRAMMAR_GRAMMAR_OUT_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int base_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    IDENT = 258,
    FCONST = 259,
    SCONST = 260,
    BCONST = 261,
    XCONST = 262,
    Op = 263,
    ICONST = 264,
    PARAM = 265,
    TYPECAST = 266,
    DOT_DOT = 267,
    COLON_EQUALS = 268,
    EQUALS_GREATER = 269,
    LAMBDA_ARROW = 270,
    LESS_EQUALS = 271,
    GREATER_EQUALS = 272,
    NOT_EQUALS = 273,
    ABORT_P = 274,
    ABSOLUTE_P = 275,
    ACCESS = 276,
    ACTION = 277,
    ADD_P = 278,
    ADMIN = 279,
    AFTER = 280,
    AGGREGATE = 281,
    ALL = 282,
    ALSO = 283,
    ALTER = 284,
    ALWAYS = 285,
    ANALYSE = 286,
    ANALYZE = 287,
    AND = 288,
    ANY = 289,
    ARRAY = 290,
    AS = 291,
    ASC_P = 292,
    ASSERTION = 293,
    ASSIGNMENT = 294,
    ASYMMETRIC = 295,
    AT = 296,
    ATTACH = 297,
    ATTRIBUTE = 298,
    AUTHORIZATION = 299,
    BACKWARD = 300,
    BEFORE = 301,
    BEGIN_P = 302,
    BETWEEN = 303,
    BIGINT = 304,
    BINARY = 305,
    BIT = 306,
    BOOLEAN_P = 307,
    BOTH = 308,
    BY = 309,
    CACHE = 310,
    CALL_P = 311,
    CALLED = 312,
    CASCADE = 313,
    CASCADED = 314,
    CASE = 315,
    CAST = 316,
    CATALOG_P = 317,
    CHAIN = 318,
    CHAR_P = 319,
    CHARACTER = 320,
    CHARACTERISTICS = 321,
    CHECK_P = 322,
    CHECKPOINT = 323,
    CLASS = 324,
    CLOSE = 325,
    CLUSTER = 326,
    COALESCE = 327,
    COLLATE = 328,
    COLLATION = 329,
    COLUMN = 330,
    COLUMNS = 331,
    COMMENT = 332,
    COMMENTS = 333,
    COMMIT = 334,
    COMMITTED = 335,
    CONCURRENTLY = 336,
    CONFIGURATION = 337,
    CONFLICT = 338,
    CONNECTION = 339,
    CONSTRAINT = 340,
    CONSTRAINTS = 341,
    CONTENT_P = 342,
    CONTINUE_P = 343,
    CONVERSION_P = 344,
    COPY = 345,
    COST = 346,
    CREATE_P = 347,
    CROSS = 348,
    CSV = 349,
    CUBE = 350,
    CURRENT_P = 351,
    CURRENT_CATALOG = 352,
    CURRENT_DATE = 353,
    CURRENT_ROLE = 354,
    CURRENT_SCHEMA = 355,
    CURRENT_TIME = 356,
    CURRENT_TIMESTAMP = 357,
    CURRENT_USER = 358,
    CURSOR = 359,
    CYCLE = 360,
    DATA_P = 361,
    DATABASE = 362,
    DAY_P = 363,
    DAYS_P = 364,
    DEALLOCATE = 365,
    DEC = 366,
    DECIMAL_P = 367,
    DECLARE = 368,
    DEFAULT = 369,
    DEFAULTS = 370,
    DEFERRABLE = 371,
    DEFERRED = 372,
    DEFINER = 373,
    DELETE_P = 374,
    DELIMITER = 375,
    DELIMITERS = 376,
    DEPENDS = 377,
    DESC_P = 378,
    DESCRIBE = 379,
    DESTINATION = 380,
    DETACH = 381,
    DICTIONARY = 382,
    DISABLE_P = 383,
    DISCARD = 384,
    DISTINCT = 385,
    DO = 386,
    DOCUMENT_P = 387,
    DOMAIN_P = 388,
    DOUBLE_P = 389,
    DROP = 390,
    EACH = 391,
    EDGE = 392,
    ELSE = 393,
    ENABLE_P = 394,
    ENCODING = 395,
    ENCRYPTED = 396,
    END_P = 397,
    ENUM_P = 398,
    ESCAPE = 399,
    EVENT = 400,
    EXCEPT = 401,
    EXCLUDE = 402,
    EXCLUDING = 403,
    EXCLUSIVE = 404,
    EXECUTE = 405,
    EXISTS = 406,
    EXPLAIN = 407,
    EXPORT_P = 408,
    EXTENSION = 409,
    EXTERNAL = 410,
    EXTRACT = 411,
    FALSE_P = 412,
    FAMILY = 413,
    FETCH = 414,
    FILTER = 415,
    FIRST_P = 416,
    FLOAT_P = 417,
    FOLLOWING = 418,
    FOR = 419,
    FORCE = 420,
    FOREIGN = 421,
    FORWARD = 422,
    FREEZE = 423,
    FROM = 424,
    FULL = 425,
    FUNCTION = 426,
    FUNCTIONS = 427,
    GENERATED = 428,
    GLOB = 429,
    GLOBAL = 430,
    GRANT = 431,
    GRANTED = 432,
    GRAPH = 433,
    GRAPH_TABLE = 434,
    GROUP_P = 435,
    GROUPING = 436,
    HANDLER = 437,
    HAVING = 438,
    HEADER_P = 439,
    HOLD = 440,
    HOUR_P = 441,
    HOURS_P = 442,
    IDENTITY_P = 443,
    IF_P = 444,
    ILIKE = 445,
    IMMEDIATE = 446,
    IMMUTABLE = 447,
    IMPLICIT_P = 448,
    IMPORT_P = 449,
    IN_P = 450,
    INCLUDING = 451,
    INCREMENT = 452,
    INDEX = 453,
    INDEXES = 454,
    INHERIT = 455,
    INHERITS = 456,
    INITIALLY = 457,
    INLINE_P = 458,
    INNER_P = 459,
    INOUT = 460,
    INPUT_P = 461,
    INSENSITIVE = 462,
    INSERT = 463,
    INSTEAD = 464,
    INT_P = 465,
    INTEGER = 466,
    INTERSECT = 467,
    INTERVAL = 468,
    INTO = 469,
    INVOKER = 470,
    IS = 471,
    ISNULL = 472,
    ISOLATION = 473,
    JOIN = 474,
    KEY = 475,
    LABEL = 476,
    LANGUAGE = 477,
    LARGE_P = 478,
    LAST_P = 479,
    LATERAL_P = 480,
    LEADING = 481,
    LEAKPROOF = 482,
    LEFT = 483,
    LEVEL = 484,
    LIKE = 485,
    LIMIT = 486,
    LISTEN = 487,
    LOAD = 488,
    LOCAL = 489,
    LOCALTIME = 490,
    LOCALTIMESTAMP = 491,
    LOCATION = 492,
    LOCK_P = 493,
    LOCKED = 494,
    LOGGED = 495,
    MACRO = 496,
    MAPPING = 497,
    MATCH = 498,
    MATERIALIZED = 499,
    MAXVALUE = 500,
    METHOD = 501,
    MICROSECOND_P = 502,
    MICROSECONDS_P = 503,
    MILLISECOND_P = 504,
    MILLISECONDS_P = 505,
    MINUTE_P = 506,
    MINUTES_P = 507,
    MINVALUE = 508,
    MODE = 509,
    MONTH_P = 510,
    MONTHS_P = 511,
    MOVE = 512,
    NAME_P = 513,
    NAMES = 514,
    NATIONAL = 515,
    NATURAL = 516,
    NCHAR = 517,
    NEW = 518,
    NEXT = 519,
    NO = 520,
    NODE = 521,
    NONE = 522,
    NOT = 523,
    NOTHING = 524,
    NOTIFY = 525,
    NOTNULL = 526,
    NOWAIT = 527,
    NULL_P = 528,
    NULLIF = 529,
    NULLS_P = 530,
    NUMERIC = 531,
    OBJECT_P = 532,
    OF = 533,
    OFF = 534,
    OFFSET = 535,
    OIDS = 536,
    OLD = 537,
    ON = 538,
    ONLY = 539,
    OPERATOR = 540,
    OPTION = 541,
    OPTIONS = 542,
    OR = 543,
    ORDER = 544,
    ORDINALITY = 545,
    OUT_P = 546,
    OUTER_P = 547,
    OVER = 548,
    OVERLAPS = 549,
    OVERLAY = 550,
    OVERRIDING = 551,
    OWNED = 552,
    OWNER = 553,
    PARALLEL = 554,
    PARSER = 555,
    PARTIAL = 556,
    PARTITION = 557,
    PASSING = 558,
    PASSWORD = 559,
    PERCENT = 560,
    PLACING = 561,
    PLANS = 562,
    POLICY = 563,
    POSITION = 564,
    PRAGMA_P = 565,
    PRECEDING = 566,
    PRECISION = 567,
    PREPARE = 568,
    PREPARED = 569,
    PRESERVE = 570,
    PRIMARY = 571,
    PRIOR = 572,
    PRIVILEGES = 573,
    PROCEDURAL = 574,
    PROCEDURE = 575,
    PROGRAM = 576,
    PROPERTIES = 577,
    PROPERTY = 578,
    PUBLICATION = 579,
    QUOTE = 580,
    RANGE = 581,
    READ_P = 582,
    REAL = 583,
    REASSIGN = 584,
    RECHECK = 585,
    RECURSIVE = 586,
    REF = 587,
    REFERENCES = 588,
    REFERENCING = 589,
    REFRESH = 590,
    REINDEX = 591,
    RELATIONSHIP = 592,
    RELATIVE_P = 593,
    RELEASE = 594,
    RENAME = 595,
    REPEATABLE = 596,
    REPLACE = 597,
    REPLICA = 598,
    RESET = 599,
    RESTART = 600,
    RESTRICT = 601,
    RETURNING = 602,
    RETURNS = 603,
    REVOKE = 604,
    RIGHT = 605,
    ROLE = 606,
    ROLLBACK = 607,
    ROLLUP = 608,
    ROW = 609,
    ROWS = 610,
    RULE = 611,
    SAMPLE = 612,
    SAVEPOINT = 613,
    SCHEMA = 614,
    SCHEMAS = 615,
    SCROLL = 616,
    SEARCH = 617,
    SECOND_P = 618,
    SECONDS_P = 619,
    SECURITY = 620,
    SELECT = 621,
    SEQUENCE = 622,
    SEQUENCES = 623,
    SERIALIZABLE = 624,
    SERVER = 625,
    SESSION = 626,
    SESSION_USER = 627,
    SET = 628,
    SETOF = 629,
    SETS = 630,
    SHARE = 631,
    SHOW = 632,
    SIMILAR = 633,
    SIMPLE = 634,
    SKIP = 635,
    SMALLINT = 636,
    SNAPSHOT = 637,
    SOME = 638,
    SOURCE = 639,
    SQL_P = 640,
    STABLE = 641,
    STANDALONE_P = 642,
    START = 643,
    STATEMENT = 644,
    STATISTICS = 645,
    STDIN = 646,
    STDOUT = 647,
    STORAGE = 648,
    STRICT_P = 649,
    STRIP_P = 650,
    STRUCT = 651,
    SUBSCRIPTION = 652,
    SUBSTRING = 653,
    SYMMETRIC = 654,
    SYSID = 655,
    SYSTEM_P = 656,
    TABLE = 657,
    TABLES = 658,
    TABLESAMPLE = 659,
    TABLESPACE = 660,
    TEMP = 661,
    TEMPLATE = 662,
    TEMPORARY = 663,
    TEXT_P = 664,
    THEN = 665,
    TIME = 666,
    TIMESTAMP = 667,
    TO = 668,
    TRAILING = 669,
    TRANSACTION = 670,
    TRANSFORM = 671,
    TREAT = 672,
    TRIGGER = 673,
    TRIM = 674,
    TRUE_P = 675,
    TRUNCATE = 676,
    TRUSTED = 677,
    TYPE_P = 678,
    TYPES_P = 679,
    UNBOUNDED = 680,
    UNCOMMITTED = 681,
    UNENCRYPTED = 682,
    UNION = 683,
    UNIQUE = 684,
    UNKNOWN = 685,
    UNLISTEN = 686,
    UNLOGGED = 687,
    UNTIL = 688,
    UPDATE = 689,
    USER = 690,
    USING = 691,
    VACUUM = 692,
    VALID = 693,
    VALIDATE = 694,
    VALIDATOR = 695,
    VALUE_P = 696,
    VALUES = 697,
    VARCHAR = 698,
    VARIADIC = 699,
    VARYING = 700,
    VERBOSE = 701,
    VERSION_P = 702,
    VERTEX = 703,
    VIEW = 704,
    VIEWS = 705,
    VOLATILE = 706,
    WHEN = 707,
    WHERE = 708,
    WHITESPACE_P = 709,
    WINDOW = 710,
    WITH = 711,
    WITHIN = 712,
    WITHOUT = 713,
    WORK = 714,
    WRAPPER = 715,
    WRITE_P = 716,
    XML_P = 717,
    XMLATTRIBUTES = 718,
    XMLCONCAT = 719,
    XMLELEMENT = 720,
    XMLEXISTS = 721,
    XMLFOREST = 722,
    XMLNAMESPACES = 723,
    XMLPARSE = 724,
    XMLPI = 725,
    XMLROOT = 726,
    XMLSERIALIZE = 727,
    XMLTABLE = 728,
    YEAR_P = 729,
    YEARS_P = 730,
    YES_P = 731,
    ZONE = 732,
    NOT_LA = 733,
    NULLS_LA = 734,
    WITH_LA = 735,
    POSTFIXOP = 736,
    UMINUS = 737
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 14 "third_party/libpg_query/grammar/grammar.y" /* yacc.c:1909  */

	core_YYSTYPE		core_yystype;
	/* these fields must match core_YYSTYPE: */
	int					ival;
	char				*str;
	const char			*keyword;
	const char          *conststr;

	char				chr;
	bool				boolean;
	PGJoinType			jtype;
	PGDropBehavior		dbehavior;
	PGOnCommitAction		oncommit;
	PGList				*list;
	PGNode				*node;
	PGValue				*value;
	PGObjectType			objtype;
	PGTypeName			*typnam;
	PGObjectWithArgs		*objwithargs;
	PGDefElem				*defelt;
	PGSortBy				*sortby;
	PGWindowDef			*windef;
	PGJoinExpr			*jexpr;
	PGIndexElem			*ielem;
	PGAlias				*alias;
	PGRangeVar			*range;
	PGIntoClause			*into;
	PGWithClause			*with;
	PGInferClause			*infer;
	PGOnConflictClause	*onconflict;
	PGAIndices			*aind;
	PGResTarget			*target;
	PGInsertStmt			*istmt;
	PGVariableSetStmt		*vsetstmt;
	PGOverridingKind       override;
	PGSortByDir            sortorder;
	PGSortByNulls          nullorder;
	PGLockClauseStrength lockstrength;
	PGLockWaitPolicy lockwaitpolicy;
	PGSubLinkType subquerytype;
	PGViewCheckOption viewcheckoption;

#line 580 "third_party/libpg_query/grammar/grammar_out.hpp" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int base_yyparse (core_yyscan_t yyscanner);

#endif /* !YY_BASE_YY_THIRD_PARTY_LIBPG_QUERY_GRAMMAR_GRAMMAR_OUT_HPP_INCLUDED  */
