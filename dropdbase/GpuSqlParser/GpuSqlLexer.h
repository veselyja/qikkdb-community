
// Generated from C:/GPU-DB/dropdbase/GpuSqlParser\GpuSqlLexer.g4 by ANTLR 4.7.2

#pragma once


#include "antlr4-runtime.h"




class  GpuSqlLexer : public antlr4::Lexer {
public:
  enum {
    DATETIMELIT = 1, LF = 2, CR = 3, CRLF = 4, WS = 5, SEMICOL = 6, SQOUTE = 7, 
    DQOUTE = 8, UNDERSCORE = 9, COLON = 10, COMMA = 11, DOT = 12, LSQR_BRC = 13, 
    RSQR_BRC = 14, STRING = 15, DELIMID = 16, DATELIT = 17, TIMELIT = 18, 
    POINT = 19, MULTIPOINT = 20, LINESTRING = 21, MULTILINESTRING = 22, 
    POLYGON = 23, MULTIPOLYGON = 24, INTTYPE = 25, LONGTYPE = 26, DATETYPE = 27, 
    DETETIMETYPE = 28, FLOATTYPE = 29, DOUBLETYPE = 30, STRINGTYPE = 31, 
    BOOLEANTYPE = 32, POINTTYPE = 33, POLYTYPE = 34, INSERTINTO = 35, CREATEDB = 36, 
    DROPDB = 37, CREATETABLE = 38, DROPTABLE = 39, ALTERTABLE = 40, ALTERDATABASE = 41, 
    DROPCOLUMN = 42, ALTERCOLUMN = 43, RENAMECOLUMN = 44, RENAMETO = 45, 
    CREATEINDEX = 46, INDEX = 47, UNIQUE = 48, PRIMARYKEY = 49, CREATE = 50, 
    ADD = 51, DROP = 52, ALTER = 53, RENAME = 54, DATABASE = 55, TABLE = 56, 
    COLUMN = 57, VALUES = 58, SELECT = 59, FROM = 60, JOIN = 61, WHERE = 62, 
    GROUPBY = 63, AS = 64, IN = 65, TO = 66, ISNULL = 67, ISNOTNULL = 68, 
    NOTNULL = 69, BETWEEN = 70, ON = 71, ORDERBY = 72, DIR = 73, LIMIT = 74, 
    OFFSET = 75, INNER = 76, FULLOUTER = 77, SHOWDB = 78, SHOWTB = 79, SHOWCL = 80, 
    SHOWQTYPES = 81, SHOWCONSTRAINTS = 82, AVG_AGG = 83, SUM_AGG = 84, MIN_AGG = 85, 
    MAX_AGG = 86, COUNT_AGG = 87, YEAR = 88, MONTH = 89, DAY = 90, HOUR = 91, 
    MINUTE = 92, SECOND = 93, NOW = 94, PI = 95, ABS = 96, SIN = 97, COS = 98, 
    TAN = 99, COT = 100, ASIN = 101, ACOS = 102, ATAN = 103, ATAN2 = 104, 
    LOG10 = 105, LOG = 106, EXP = 107, POW = 108, SQRT = 109, SQUARE = 110, 
    SIGN = 111, ROOT = 112, ROUND = 113, CEIL = 114, FLOOR = 115, LTRIM = 116, 
    RTRIM = 117, LOWER = 118, UPPER = 119, REVERSE = 120, LEN = 121, LEFT = 122, 
    RIGHT = 123, CONCAT = 124, CAST = 125, GEO_CONTAINS = 126, GEO_INTERSECT = 127, 
    GEO_UNION = 128, PLUS = 129, MINUS = 130, ASTERISK = 131, DIVISION = 132, 
    MODULO = 133, XOR = 134, EQUALS = 135, NOTEQUALS = 136, NOTEQUALS_GT_LT = 137, 
    LPAREN = 138, RPAREN = 139, GREATER = 140, LESS = 141, GREATEREQ = 142, 
    LESSEQ = 143, LOGICAL_NOT = 144, OR = 145, AND = 146, BIT_OR = 147, 
    BIT_AND = 148, L_SHIFT = 149, R_SHIFT = 150, BOOLEANLIT = 151, TRUE = 152, 
    FALSE = 153, FLOATLIT = 154, INTLIT = 155, NULLLIT = 156, ID = 157
  };

  GpuSqlLexer(antlr4::CharStream *input);
  ~GpuSqlLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

