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
    CHEAPEST = 322,
    CHECK_P = 323,
    CHECKPOINT = 324,
    CLASS = 325,
    CLOSE = 326,
    CLUSTER = 327,
    COALESCE = 328,
    COLLATE = 329,
    COLLATION = 330,
    COLUMN = 331,
    COLUMNS = 332,
    COMMENT = 333,
    COMMENTS = 334,
    COMMIT = 335,
    COMMITTED = 336,
    COMPRESSION = 337,
    CONCURRENTLY = 338,
    CONFIGURATION = 339,
    CONFLICT = 340,
    CONNECTION = 341,
    CONSTRAINT = 342,
    CONSTRAINTS = 343,
    CONTENT_P = 344,
    CONTINUE_P = 345,
    CONVERSION_P = 346,
    COPY = 347,
    COST = 348,
    CREATE_P = 349,
    CROSS = 350,
    CSV = 351,
    CUBE = 352,
    CURRENT_P = 353,
    CURRENT_CATALOG = 354,
    CURRENT_DATE = 355,
    CURRENT_ROLE = 356,
    CURRENT_SCHEMA = 357,
    CURRENT_TIME = 358,
    CURRENT_TIMESTAMP = 359,
    CURRENT_USER = 360,
    CURSOR = 361,
    CYCLE = 362,
    DATA_P = 363,
    DATABASE = 364,
    DAY_P = 365,
    DAYS_P = 366,
    DEALLOCATE = 367,
    DEC = 368,
    DECIMAL_P = 369,
    DECLARE = 370,
    DEFAULT = 371,
    DEFAULTS = 372,
    DEFERRABLE = 373,
    DEFERRED = 374,
    DEFINER = 375,
    DELETE_P = 376,
    DELIMITER = 377,
    DELIMITERS = 378,
    DEPENDS = 379,
    DESC_P = 380,
    DESCRIBE = 381,
    DESTINATION = 382,
    DETACH = 383,
    DICTIONARY = 384,
    DISABLE_P = 385,
    DISCARD = 386,
    DISCRIMINATOR = 387,
    DISTINCT = 388,
    DO = 389,
    DOCUMENT_P = 390,
    DOMAIN_P = 391,
    DOUBLE_P = 392,
    DROP = 393,
    EACH = 394,
    EDGE = 395,
    ELSE = 396,
    ENABLE_P = 397,
    ENCODING = 398,
    ENCRYPTED = 399,
    END_P = 400,
    ENUM_P = 401,
    ESCAPE = 402,
    EVENT = 403,
    EXCEPT = 404,
    EXCLUDE = 405,
    EXCLUDING = 406,
    EXCLUSIVE = 407,
    EXECUTE = 408,
    EXISTS = 409,
    EXPLAIN = 410,
    EXPORT_P = 411,
    EXTENSION = 412,
    EXTERNAL = 413,
    EXTRACT = 414,
    FALSE_P = 415,
    FAMILY = 416,
    FETCH = 417,
    FILTER = 418,
    FIRST_P = 419,
    FLOAT_P = 420,
    FOLLOWING = 421,
    FOR = 422,
    FORCE = 423,
    FOREIGN = 424,
    FORWARD = 425,
    FREEZE = 426,
    FROM = 427,
    FULL = 428,
    FUNCTION = 429,
    FUNCTIONS = 430,
    GENERATED = 431,
    GLOB = 432,
    GLOBAL = 433,
    GRANT = 434,
    GRANTED = 435,
    GRAPH = 436,
    GRAPH_TABLE = 437,
    GROUP_P = 438,
    GROUPING = 439,
    GROUPING_ID = 440,
    HANDLER = 441,
    HAVING = 442,
    HEADER_P = 443,
    HOLD = 444,
    HOUR_P = 445,
    HOURS_P = 446,
    IDENTITY_P = 447,
    IF_P = 448,
    IGNORE_P = 449,
    ILIKE = 450,
    IMMEDIATE = 451,
    IMMUTABLE = 452,
    IMPLICIT_P = 453,
    IMPORT_P = 454,
    IN_P = 455,
    INCLUDING = 456,
    INCREMENT = 457,
    INDEX = 458,
    INDEXES = 459,
    INHERIT = 460,
    INHERITS = 461,
    INITIALLY = 462,
    INLINE_P = 463,
    INNER_P = 464,
    INOUT = 465,
    INPUT_P = 466,
    INSENSITIVE = 467,
    INSERT = 468,
    INSTALL = 469,
    INSTEAD = 470,
    INT_P = 471,
    INTEGER = 472,
    INTERSECT = 473,
    INTERVAL = 474,
    INTO = 475,
    INVOKER = 476,
    IS = 477,
    ISNULL = 478,
    ISOLATION = 479,
    JOIN = 480,
    KEY = 481,
    LABEL = 482,
    LANGUAGE = 483,
    LARGE_P = 484,
    LAST_P = 485,
    LATERAL_P = 486,
    LEADING = 487,
    LEAKPROOF = 488,
    LEFT = 489,
    LEVEL = 490,
    LIKE = 491,
    LIMIT = 492,
    LISTEN = 493,
    LOAD = 494,
    LOCAL = 495,
    LOCALTIME = 496,
    LOCALTIMESTAMP = 497,
    LOCATION = 498,
    LOCK_P = 499,
    LOCKED = 500,
    LOGGED = 501,
    MACRO = 502,
    MAP = 503,
    MAPPING = 504,
    MATCH = 505,
    MATERIALIZED = 506,
    MAXVALUE = 507,
    METHOD = 508,
    MICROSECOND_P = 509,
    MICROSECONDS_P = 510,
    MILLISECOND_P = 511,
    MILLISECONDS_P = 512,
    MINUTE_P = 513,
    MINUTES_P = 514,
    MINVALUE = 515,
    MODE = 516,
    MONTH_P = 517,
    MONTHS_P = 518,
    MOVE = 519,
    NAME_P = 520,
    NAMES = 521,
    NATIONAL = 522,
    NATURAL = 523,
    NCHAR = 524,
    NEW = 525,
    NEXT = 526,
    NO = 527,
    NODE = 528,
    NONE = 529,
    NOT = 530,
    NOTHING = 531,
    NOTIFY = 532,
    NOTNULL = 533,
    NOWAIT = 534,
    NULL_P = 535,
    NULLIF = 536,
    NULLS_P = 537,
    NUMERIC = 538,
    OBJECT_P = 539,
    OF = 540,
    OFF = 541,
    OFFSET = 542,
    OIDS = 543,
    OLD = 544,
    ON = 545,
    ONLY = 546,
    OPERATOR = 547,
    OPTION = 548,
    OPTIONS = 549,
    OR = 550,
    ORDER = 551,
    ORDINALITY = 552,
    OUT_P = 553,
    OUTER_P = 554,
    OVER = 555,
    OVERLAPS = 556,
    OVERLAY = 557,
    OVERRIDING = 558,
    OWNED = 559,
    OWNER = 560,
    PARALLEL = 561,
    PARSER = 562,
    PARTIAL = 563,
    PARTITION = 564,
    PASSING = 565,
    PASSWORD = 566,
    PERCENT = 567,
    PLACING = 568,
    PLANS = 569,
    POLICY = 570,
    POSITION = 571,
    PRAGMA_P = 572,
    PRECEDING = 573,
    PRECISION = 574,
    PREPARE = 575,
    PREPARED = 576,
    PRESERVE = 577,
    PRIMARY = 578,
    PRIOR = 579,
    PRIVILEGES = 580,
    PROCEDURAL = 581,
    PROCEDURE = 582,
    PROGRAM = 583,
    PROPERTIES = 584,
    PROPERTY = 585,
    PUBLICATION = 586,
    QUOTE = 587,
    RANGE = 588,
    READ_P = 589,
    REAL = 590,
    REASSIGN = 591,
    RECHECK = 592,
    RECURSIVE = 593,
    REF = 594,
    REFERENCES = 595,
    REFERENCING = 596,
    REFRESH = 597,
    REINDEX = 598,
    RELATIONSHIP = 599,
    RELATIVE_P = 600,
    RELEASE = 601,
    RENAME = 602,
    REPEATABLE = 603,
    REPLACE = 604,
    REPLICA = 605,
    RESET = 606,
    RESPECT_P = 607,
    RESTART = 608,
    RESTRICT = 609,
    RETURNING = 610,
    RETURNS = 611,
    REVOKE = 612,
    RIGHT = 613,
    ROLE = 614,
    ROLLBACK = 615,
    ROLLUP = 616,
    ROW = 617,
    ROWS = 618,
    RULE = 619,
    SAMPLE = 620,
    SAVEPOINT = 621,
    SCHEMA = 622,
    SCHEMAS = 623,
    SCROLL = 624,
    SEARCH = 625,
    SECOND_P = 626,
    SECONDS_P = 627,
    SECURITY = 628,
    SELECT = 629,
    SEQUENCE = 630,
    SEQUENCES = 631,
    SERIALIZABLE = 632,
    SERVER = 633,
    SESSION = 634,
    SESSION_USER = 635,
    SET = 636,
    SETOF = 637,
    SETS = 638,
    SHARE = 639,
    SHORTEST = 640,
    SHOW = 641,
    SIMILAR = 642,
    SIMPLE = 643,
    SKIP = 644,
    SMALLINT = 645,
    SNAPSHOT = 646,
    SOME = 647,
    SOURCE = 648,
    SQL_P = 649,
    STABLE = 650,
    STANDALONE_P = 651,
    START = 652,
    STATEMENT = 653,
    STATISTICS = 654,
    STDIN = 655,
    STDOUT = 656,
    STORAGE = 657,
    STRICT_P = 658,
    STRIP_P = 659,
    STRUCT = 660,
    SUBSCRIPTION = 661,
    SUBSTRING = 662,
    SUMMARIZE = 663,
    SYMMETRIC = 664,
    SYSID = 665,
    SYSTEM_P = 666,
    TABLE = 667,
    TABLES = 668,
    TABLESAMPLE = 669,
    TABLESPACE = 670,
    TEMP = 671,
    TEMPLATE = 672,
    TEMPORARY = 673,
    TEXT_P = 674,
    THEN = 675,
    TIME = 676,
    TIMESTAMP = 677,
    TO = 678,
    TRAILING = 679,
    TRANSACTION = 680,
    TRANSFORM = 681,
    TREAT = 682,
    TRIGGER = 683,
    TRIM = 684,
    TRUE_P = 685,
    TRUNCATE = 686,
    TRUSTED = 687,
    TRY_CAST = 688,
    TYPE_P = 689,
    TYPES_P = 690,
    UNBOUNDED = 691,
    UNCOMMITTED = 692,
    UNENCRYPTED = 693,
    UNION = 694,
    UNIQUE = 695,
    UNKNOWN = 696,
    UNLISTEN = 697,
    UNLOGGED = 698,
    UNTIL = 699,
    UPDATE = 700,
    USER = 701,
    USING = 702,
    VACUUM = 703,
    VALID = 704,
    VALIDATE = 705,
    VALIDATOR = 706,
    VALUE_P = 707,
    VALUES = 708,
    VARCHAR = 709,
    VARIADIC = 710,
    VARYING = 711,
    VERBOSE = 712,
    VERSION_P = 713,
    VERTEX = 714,
    VIEW = 715,
    VIEWS = 716,
    VOLATILE = 717,
    WHEN = 718,
    WHERE = 719,
    WHITESPACE_P = 720,
    WINDOW = 721,
    WITH = 722,
    WITHIN = 723,
    WITHOUT = 724,
    WORK = 725,
    WRAPPER = 726,
    WRITE_P = 727,
    XML_P = 728,
    XMLATTRIBUTES = 729,
    XMLCONCAT = 730,
    XMLELEMENT = 731,
    XMLEXISTS = 732,
    XMLFOREST = 733,
    XMLNAMESPACES = 734,
    XMLPARSE = 735,
    XMLPI = 736,
    XMLROOT = 737,
    XMLSERIALIZE = 738,
    XMLTABLE = 739,
    YEAR_P = 740,
    YEARS_P = 741,
    YES_P = 742,
    ZONE = 743,
    NOT_LA = 744,
    NULLS_LA = 745,
    WITH_LA = 746,
    POSTFIXOP = 747,
    UMINUS = 748
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
	PGDropBehavior			dbehavior;
	PGOnCommitAction		oncommit;
	PGOnCreateConflict		oncreateconflict;
	PGList				*list;
	PGNode				*node;
	PGValue				*value;
	PGObjectType			objtype;
	PGTypeName			*typnam;
	PGObjectWithArgs		*objwithargs;
	PGDefElem			*defelt;
	PGSortBy			*sortby;
	PGWindowDef			*windef;
	PGJoinExpr			*jexpr;
	PGIndexElem			*ielem;
	PGAlias				*alias;
	PGRangeVar			*range;
	PGIntoClause			*into;
	PGWithClause			*with;
	PGInferClause			*infer;
	PGOnConflictClause		*onconflict;
	PGAIndices			*aind;
	PGResTarget			*target;
	PGInsertStmt			*istmt;
	PGVariableSetStmt		*vsetstmt;
	PGOverridingKind       		override;
	PGSortByDir            		sortorder;
	PGSortByNulls          		nullorder;
	PGLockClauseStrength 		lockstrength;
	PGLockWaitPolicy 		lockwaitpolicy;
	PGSubLinkType 			subquerytype;
	PGViewCheckOption 		viewcheckoption;

#line 595 "third_party/libpg_query/grammar/grammar_out.hpp"

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
