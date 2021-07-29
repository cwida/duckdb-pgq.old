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
    DISCRIMINATOR = 385,
    DISTINCT = 386,
    DO = 387,
    DOCUMENT_P = 388,
    DOMAIN_P = 389,
    DOUBLE_P = 390,
    DROP = 391,
    EACH = 392,
    EDGE = 393,
    ELSE = 394,
    ENABLE_P = 395,
    ENCODING = 396,
    ENCRYPTED = 397,
    END_P = 398,
    ENUM_P = 399,
    ESCAPE = 400,
    EVENT = 401,
    EXCEPT = 402,
    EXCLUDE = 403,
    EXCLUDING = 404,
    EXCLUSIVE = 405,
    EXECUTE = 406,
    EXISTS = 407,
    EXPLAIN = 408,
    EXPORT_P = 409,
    EXTENSION = 410,
    EXTERNAL = 411,
    EXTRACT = 412,
    FALSE_P = 413,
    FAMILY = 414,
    FETCH = 415,
    FILTER = 416,
    FIRST_P = 417,
    FLOAT_P = 418,
    FOLLOWING = 419,
    FOR = 420,
    FORCE = 421,
    FOREIGN = 422,
    FORWARD = 423,
    FREEZE = 424,
    FROM = 425,
    FULL = 426,
    FUNCTION = 427,
    FUNCTIONS = 428,
    GENERATED = 429,
    GLOB = 430,
    GLOBAL = 431,
    GRANT = 432,
    GRANTED = 433,
    GRAPH = 434,
    GRAPH_TABLE = 435,
    GROUP_P = 436,
    GROUPING = 437,
    HANDLER = 438,
    HAVING = 439,
    HEADER_P = 440,
    HOLD = 441,
    HOUR_P = 442,
    HOURS_P = 443,
    IDENTITY_P = 444,
    IF_P = 445,
    ILIKE = 446,
    IMMEDIATE = 447,
    IMMUTABLE = 448,
    IMPLICIT_P = 449,
    IMPORT_P = 450,
    IN_P = 451,
    INCLUDING = 452,
    INCREMENT = 453,
    INDEX = 454,
    INDEXES = 455,
    INHERIT = 456,
    INHERITS = 457,
    INITIALLY = 458,
    INLINE_P = 459,
    INNER_P = 460,
    INOUT = 461,
    INPUT_P = 462,
    INSENSITIVE = 463,
    INSERT = 464,
    INSTEAD = 465,
    INT_P = 466,
    INTEGER = 467,
    INTERSECT = 468,
    INTERVAL = 469,
    INTO = 470,
    INVOKER = 471,
    IS = 472,
    ISNULL = 473,
    ISOLATION = 474,
    JOIN = 475,
    KEY = 476,
    LABEL = 477,
    LANGUAGE = 478,
    LARGE_P = 479,
    LAST_P = 480,
    LATERAL_P = 481,
    LEADING = 482,
    LEAKPROOF = 483,
    LEFT = 484,
    LEVEL = 485,
    LIKE = 486,
    LIMIT = 487,
    LISTEN = 488,
    LOAD = 489,
    LOCAL = 490,
    LOCALTIME = 491,
    LOCALTIMESTAMP = 492,
    LOCATION = 493,
    LOCK_P = 494,
    LOCKED = 495,
    LOGGED = 496,
    MACRO = 497,
    MAPPING = 498,
    MATCH = 499,
    MATERIALIZED = 500,
    MAXVALUE = 501,
    METHOD = 502,
    MICROSECOND_P = 503,
    MICROSECONDS_P = 504,
    MILLISECOND_P = 505,
    MILLISECONDS_P = 506,
    MINUTE_P = 507,
    MINUTES_P = 508,
    MINVALUE = 509,
    MODE = 510,
    MONTH_P = 511,
    MONTHS_P = 512,
    MOVE = 513,
    NAME_P = 514,
    NAMES = 515,
    NATIONAL = 516,
    NATURAL = 517,
    NCHAR = 518,
    NEW = 519,
    NEXT = 520,
    NO = 521,
    NODE = 522,
    NONE = 523,
    NOT = 524,
    NOTHING = 525,
    NOTIFY = 526,
    NOTNULL = 527,
    NOWAIT = 528,
    NULL_P = 529,
    NULLIF = 530,
    NULLS_P = 531,
    NUMERIC = 532,
    OBJECT_P = 533,
    OF = 534,
    OFF = 535,
    OFFSET = 536,
    OIDS = 537,
    OLD = 538,
    ON = 539,
    ONLY = 540,
    OPERATOR = 541,
    OPTION = 542,
    OPTIONS = 543,
    OR = 544,
    ORDER = 545,
    ORDINALITY = 546,
    OUT_P = 547,
    OUTER_P = 548,
    OVER = 549,
    OVERLAPS = 550,
    OVERLAY = 551,
    OVERRIDING = 552,
    OWNED = 553,
    OWNER = 554,
    PARALLEL = 555,
    PARSER = 556,
    PARTIAL = 557,
    PARTITION = 558,
    PASSING = 559,
    PASSWORD = 560,
    PERCENT = 561,
    PLACING = 562,
    PLANS = 563,
    POLICY = 564,
    POSITION = 565,
    PRAGMA_P = 566,
    PRECEDING = 567,
    PRECISION = 568,
    PREPARE = 569,
    PREPARED = 570,
    PRESERVE = 571,
    PRIMARY = 572,
    PRIOR = 573,
    PRIVILEGES = 574,
    PROCEDURAL = 575,
    PROCEDURE = 576,
    PROGRAM = 577,
    PROPERTIES = 578,
    PROPERTY = 579,
    PUBLICATION = 580,
    QUOTE = 581,
    RANGE = 582,
    READ_P = 583,
    REAL = 584,
    REASSIGN = 585,
    RECHECK = 586,
    RECURSIVE = 587,
    REF = 588,
    REFERENCES = 589,
    REFERENCING = 590,
    REFRESH = 591,
    REINDEX = 592,
    RELATIONSHIP = 593,
    RELATIVE_P = 594,
    RELEASE = 595,
    RENAME = 596,
    REPEATABLE = 597,
    REPLACE = 598,
    REPLICA = 599,
    RESET = 600,
    RESTART = 601,
    RESTRICT = 602,
    RETURNING = 603,
    RETURNS = 604,
    REVOKE = 605,
    RIGHT = 606,
    ROLE = 607,
    ROLLBACK = 608,
    ROLLUP = 609,
    ROW = 610,
    ROWS = 611,
    RULE = 612,
    SAMPLE = 613,
    SAVEPOINT = 614,
    SCHEMA = 615,
    SCHEMAS = 616,
    SCROLL = 617,
    SEARCH = 618,
    SECOND_P = 619,
    SECONDS_P = 620,
    SECURITY = 621,
    SELECT = 622,
    SEQUENCE = 623,
    SEQUENCES = 624,
    SERIALIZABLE = 625,
    SERVER = 626,
    SESSION = 627,
    SESSION_USER = 628,
    SET = 629,
    SETOF = 630,
    SETS = 631,
    SHARE = 632,
    SHOW = 633,
    SIMILAR = 634,
    SIMPLE = 635,
    SKIP = 636,
    SMALLINT = 637,
    SNAPSHOT = 638,
    SOME = 639,
    SOURCE = 640,
    SQL_P = 641,
    STABLE = 642,
    STANDALONE_P = 643,
    START = 644,
    STATEMENT = 645,
    STATISTICS = 646,
    STDIN = 647,
    STDOUT = 648,
    STORAGE = 649,
    STRICT_P = 650,
    STRIP_P = 651,
    STRUCT = 652,
    SUBSCRIPTION = 653,
    SUBSTRING = 654,
    SYMMETRIC = 655,
    SYSID = 656,
    SYSTEM_P = 657,
    TABLE = 658,
    TABLES = 659,
    TABLESAMPLE = 660,
    TABLESPACE = 661,
    TEMP = 662,
    TEMPLATE = 663,
    TEMPORARY = 664,
    TEXT_P = 665,
    THEN = 666,
    TIME = 667,
    TIMESTAMP = 668,
    TO = 669,
    TRAILING = 670,
    TRANSACTION = 671,
    TRANSFORM = 672,
    TREAT = 673,
    TRIGGER = 674,
    TRIM = 675,
    TRUE_P = 676,
    TRUNCATE = 677,
    TRUSTED = 678,
    TYPE_P = 679,
    TYPES_P = 680,
    UNBOUNDED = 681,
    UNCOMMITTED = 682,
    UNENCRYPTED = 683,
    UNION = 684,
    UNIQUE = 685,
    UNKNOWN = 686,
    UNLISTEN = 687,
    UNLOGGED = 688,
    UNTIL = 689,
    UPDATE = 690,
    USER = 691,
    USING = 692,
    VACUUM = 693,
    VALID = 694,
    VALIDATE = 695,
    VALIDATOR = 696,
    VALUE_P = 697,
    VALUES = 698,
    VARCHAR = 699,
    VARIADIC = 700,
    VARYING = 701,
    VERBOSE = 702,
    VERSION_P = 703,
    VERTEX = 704,
    VIEW = 705,
    VIEWS = 706,
    VOLATILE = 707,
    WHEN = 708,
    WHERE = 709,
    WHITESPACE_P = 710,
    WINDOW = 711,
    WITH = 712,
    WITHIN = 713,
    WITHOUT = 714,
    WORK = 715,
    WRAPPER = 716,
    WRITE_P = 717,
    XML_P = 718,
    XMLATTRIBUTES = 719,
    XMLCONCAT = 720,
    XMLELEMENT = 721,
    XMLEXISTS = 722,
    XMLFOREST = 723,
    XMLNAMESPACES = 724,
    XMLPARSE = 725,
    XMLPI = 726,
    XMLROOT = 727,
    XMLSERIALIZE = 728,
    XMLTABLE = 729,
    YEAR_P = 730,
    YEARS_P = 731,
    YES_P = 732,
    ZONE = 733,
    NOT_LA = 734,
    NULLS_LA = 735,
    WITH_LA = 736,
    POSTFIXOP = 737,
    UMINUS = 738
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

#line 584 "third_party/libpg_query/grammar/grammar_out.hpp"

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
