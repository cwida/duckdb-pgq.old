/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

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
    COMPRESSION = 336,
    CONCURRENTLY = 337,
    CONFIGURATION = 338,
    CONFLICT = 339,
    CONNECTION = 340,
    CONSTRAINT = 341,
    CONSTRAINTS = 342,
    CONTENT_P = 343,
    CONTINUE_P = 344,
    CONVERSION_P = 345,
    COPY = 346,
    COST = 347,
    CREATE_P = 348,
    CROSS = 349,
    CSV = 350,
    CUBE = 351,
    CURRENT_P = 352,
    CURRENT_CATALOG = 353,
    CURRENT_DATE = 354,
    CURRENT_ROLE = 355,
    CURRENT_SCHEMA = 356,
    CURRENT_TIME = 357,
    CURRENT_TIMESTAMP = 358,
    CURRENT_USER = 359,
    CURSOR = 360,
    CYCLE = 361,
    DATA_P = 362,
    DATABASE = 363,
    DAY_P = 364,
    DAYS_P = 365,
    DEALLOCATE = 366,
    DEC = 367,
    DECIMAL_P = 368,
    DECLARE = 369,
    DEFAULT = 370,
    DEFAULTS = 371,
    DEFERRABLE = 372,
    DEFERRED = 373,
    DEFINER = 374,
    DELETE_P = 375,
    DELIMITER = 376,
    DELIMITERS = 377,
    DEPENDS = 378,
    DESC_P = 379,
    DESCRIBE = 380,
    DESTINATION = 381,
    DETACH = 382,
    DICTIONARY = 383,
    DISABLE_P = 384,
    DISCARD = 385,
    DISCRIMINATOR = 386,
    DISTINCT = 387,
    DO = 388,
    DOCUMENT_P = 389,
    DOMAIN_P = 390,
    DOUBLE_P = 391,
    DROP = 392,
    EACH = 393,
    EDGE = 394,
    ELSE = 395,
    ENABLE_P = 396,
    ENCODING = 397,
    ENCRYPTED = 398,
    END_P = 399,
    ENUM_P = 400,
    ESCAPE = 401,
    EVENT = 402,
    EXCEPT = 403,
    EXCLUDE = 404,
    EXCLUDING = 405,
    EXCLUSIVE = 406,
    EXECUTE = 407,
    EXISTS = 408,
    EXPLAIN = 409,
    EXPORT_P = 410,
    EXTENSION = 411,
    EXTERNAL = 412,
    EXTRACT = 413,
    FALSE_P = 414,
    FAMILY = 415,
    FETCH = 416,
    FILTER = 417,
    FIRST_P = 418,
    FLOAT_P = 419,
    FOLLOWING = 420,
    FOR = 421,
    FORCE = 422,
    FOREIGN = 423,
    FORWARD = 424,
    FREEZE = 425,
    FROM = 426,
    FULL = 427,
    FUNCTION = 428,
    FUNCTIONS = 429,
    GENERATED = 430,
    GLOB = 431,
    GLOBAL = 432,
    GRANT = 433,
    GRANTED = 434,
    GRAPH = 435,
    GRAPH_TABLE = 436,
    GROUP_P = 437,
    GROUPING = 438,
    GROUPING_ID = 439,
    HANDLER = 440,
    HAVING = 441,
    HEADER_P = 442,
    HOLD = 443,
    HOUR_P = 444,
    HOURS_P = 445,
    IDENTITY_P = 446,
    IF_P = 447,
    IGNORE_P = 448,
    ILIKE = 449,
    IMMEDIATE = 450,
    IMMUTABLE = 451,
    IMPLICIT_P = 452,
    IMPORT_P = 453,
    IN_P = 454,
    INCLUDING = 455,
    INCREMENT = 456,
    INDEX = 457,
    INDEXES = 458,
    INHERIT = 459,
    INHERITS = 460,
    INITIALLY = 461,
    INLINE_P = 462,
    INNER_P = 463,
    INOUT = 464,
    INPUT_P = 465,
    INSENSITIVE = 466,
    INSERT = 467,
    INSTALL = 468,
    INSTEAD = 469,
    INT_P = 470,
    INTEGER = 471,
    INTERSECT = 472,
    INTERVAL = 473,
    INTO = 474,
    INVOKER = 475,
    IS = 476,
    ISNULL = 477,
    ISOLATION = 478,
    JOIN = 479,
    KEY = 480,
    LABEL = 481,
    LANGUAGE = 482,
    LARGE_P = 483,
    LAST_P = 484,
    LATERAL_P = 485,
    LEADING = 486,
    LEAKPROOF = 487,
    LEFT = 488,
    LEVEL = 489,
    LIKE = 490,
    LIMIT = 491,
    LISTEN = 492,
    LOAD = 493,
    LOCAL = 494,
    LOCALTIME = 495,
    LOCALTIMESTAMP = 496,
    LOCATION = 497,
    LOCK_P = 498,
    LOCKED = 499,
    LOGGED = 500,
    MACRO = 501,
    MAP = 502,
    MAPPING = 503,
    MATCH = 504,
    MATERIALIZED = 505,
    MAXVALUE = 506,
    METHOD = 507,
    MICROSECOND_P = 508,
    MICROSECONDS_P = 509,
    MILLISECOND_P = 510,
    MILLISECONDS_P = 511,
    MINUTE_P = 512,
    MINUTES_P = 513,
    MINVALUE = 514,
    MODE = 515,
    MONTH_P = 516,
    MONTHS_P = 517,
    MOVE = 518,
    NAME_P = 519,
    NAMES = 520,
    NATIONAL = 521,
    NATURAL = 522,
    NCHAR = 523,
    NEW = 524,
    NEXT = 525,
    NO = 526,
    NODE = 527,
    NONE = 528,
    NOT = 529,
    NOTHING = 530,
    NOTIFY = 531,
    NOTNULL = 532,
    NOWAIT = 533,
    NULL_P = 534,
    NULLIF = 535,
    NULLS_P = 536,
    NUMERIC = 537,
    OBJECT_P = 538,
    OF = 539,
    OFF = 540,
    OFFSET = 541,
    OIDS = 542,
    OLD = 543,
    ON = 544,
    ONLY = 545,
    OPERATOR = 546,
    OPTION = 547,
    OPTIONS = 548,
    OR = 549,
    ORDER = 550,
    ORDINALITY = 551,
    OUT_P = 552,
    OUTER_P = 553,
    OVER = 554,
    OVERLAPS = 555,
    OVERLAY = 556,
    OVERRIDING = 557,
    OWNED = 558,
    OWNER = 559,
    PARALLEL = 560,
    PARSER = 561,
    PARTIAL = 562,
    PARTITION = 563,
    PASSING = 564,
    PASSWORD = 565,
    PERCENT = 566,
    PLACING = 567,
    PLANS = 568,
    POLICY = 569,
    POSITION = 570,
    PRAGMA_P = 571,
    PRECEDING = 572,
    PRECISION = 573,
    PREPARE = 574,
    PREPARED = 575,
    PRESERVE = 576,
    PRIMARY = 577,
    PRIOR = 578,
    PRIVILEGES = 579,
    PROCEDURAL = 580,
    PROCEDURE = 581,
    PROGRAM = 582,
    PROPERTIES = 583,
    PROPERTY = 584,
    PUBLICATION = 585,
    QUOTE = 586,
    RANGE = 587,
    READ_P = 588,
    REAL = 589,
    REASSIGN = 590,
    RECHECK = 591,
    RECURSIVE = 592,
    REF = 593,
    REFERENCES = 594,
    REFERENCING = 595,
    REFRESH = 596,
    REINDEX = 597,
    RELATIONSHIP = 598,
    RELATIVE_P = 599,
    RELEASE = 600,
    RENAME = 601,
    REPEATABLE = 602,
    REPLACE = 603,
    REPLICA = 604,
    RESET = 605,
    RESPECT_P = 606,
    RESTART = 607,
    RESTRICT = 608,
    RETURNING = 609,
    RETURNS = 610,
    REVOKE = 611,
    RIGHT = 612,
    ROLE = 613,
    ROLLBACK = 614,
    ROLLUP = 615,
    ROW = 616,
    ROWS = 617,
    RULE = 618,
    SAMPLE = 619,
    SAVEPOINT = 620,
    SCHEMA = 621,
    SCHEMAS = 622,
    SCROLL = 623,
    SEARCH = 624,
    SECOND_P = 625,
    SECONDS_P = 626,
    SECURITY = 627,
    SELECT = 628,
    SEQUENCE = 629,
    SEQUENCES = 630,
    SERIALIZABLE = 631,
    SERVER = 632,
    SESSION = 633,
    SESSION_USER = 634,
    SET = 635,
    SETOF = 636,
    SETS = 637,
    SHARE = 638,
    SHOW = 639,
    SIMILAR = 640,
    SIMPLE = 641,
    SKIP = 642,
    SMALLINT = 643,
    SNAPSHOT = 644,
    SOME = 645,
    SOURCE = 646,
    SQL_P = 647,
    STABLE = 648,
    STANDALONE_P = 649,
    START = 650,
    STATEMENT = 651,
    STATISTICS = 652,
    STDIN = 653,
    STDOUT = 654,
    STORAGE = 655,
    STRICT_P = 656,
    STRIP_P = 657,
    STRUCT = 658,
    SUBSCRIPTION = 659,
    SUBSTRING = 660,
    SUMMARIZE = 661,
    SYMMETRIC = 662,
    SYSID = 663,
    SYSTEM_P = 664,
    TABLE = 665,
    TABLES = 666,
    TABLESAMPLE = 667,
    TABLESPACE = 668,
    TEMP = 669,
    TEMPLATE = 670,
    TEMPORARY = 671,
    TEXT_P = 672,
    THEN = 673,
    TIME = 674,
    TIMESTAMP = 675,
    TO = 676,
    TRAILING = 677,
    TRANSACTION = 678,
    TRANSFORM = 679,
    TREAT = 680,
    TRIGGER = 681,
    TRIM = 682,
    TRUE_P = 683,
    TRUNCATE = 684,
    TRUSTED = 685,
    TRY_CAST = 686,
    TYPE_P = 687,
    TYPES_P = 688,
    UNBOUNDED = 689,
    UNCOMMITTED = 690,
    UNENCRYPTED = 691,
    UNION = 692,
    UNIQUE = 693,
    UNKNOWN = 694,
    UNLISTEN = 695,
    UNLOGGED = 696,
    UNTIL = 697,
    UPDATE = 698,
    USER = 699,
    USING = 700,
    VACUUM = 701,
    VALID = 702,
    VALIDATE = 703,
    VALIDATOR = 704,
    VALUE_P = 705,
    VALUES = 706,
    VARCHAR = 707,
    VARIADIC = 708,
    VARYING = 709,
    VERBOSE = 710,
    VERSION_P = 711,
    VERTEX = 712,
    VIEW = 713,
    VIEWS = 714,
    VOLATILE = 715,
    WHEN = 716,
    WHERE = 717,
    WHITESPACE_P = 718,
    WINDOW = 719,
    WITH = 720,
    WITHIN = 721,
    WITHOUT = 722,
    WORK = 723,
    WRAPPER = 724,
    WRITE_P = 725,
    XML_P = 726,
    XMLATTRIBUTES = 727,
    XMLCONCAT = 728,
    XMLELEMENT = 729,
    XMLEXISTS = 730,
    XMLFOREST = 731,
    XMLNAMESPACES = 732,
    XMLPARSE = 733,
    XMLPI = 734,
    XMLROOT = 735,
    XMLSERIALIZE = 736,
    XMLTABLE = 737,
    YEAR_P = 738,
    YEARS_P = 739,
    YES_P = 740,
    ZONE = 741,
    NOT_LA = 742,
    NULLS_LA = 743,
    WITH_LA = 744,
    POSTFIXOP = 745,
    UMINUS = 746
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "third_party/libpg_query/grammar/grammar.y"

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
	PGOnCreateConflict		oncreateconflict;
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

#line 593 "third_party/libpg_query/grammar/grammar_out.hpp"

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
