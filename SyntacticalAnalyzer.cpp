/*******************************************************************************
 * Assignment: Project 2 - Syntactic Analyzer for Scheme to C++ Translator      *
 * Author: Elias Mote, Chase Watson                                             *
 * D`ate: Spring 2018                                                            *
 * File: SyntacticalAnalyzer.cpp                                                *
 *                                                                              *
 * Description: This file contains the                                          *
 *******************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <set>
#include "SyntacticalAnalyzer.h"

using namespace std;

/*******************************************************************************
 * Function: SyntacticalAnalyzer Constructor                                    *
 *                                                                              *
 * Parameters:                                                                  *
 * Return value:                                                                *
 * Description: This function will                                              *
 *******************************************************************************/
SyntacticalAnalyzer::SyntacticalAnalyzer (char * filename)
{
  lex = new LexicalAnalyzer (filename);
  string name = filename;
  string p2name = name.substr (0, name.length()-3) + ".p2";
  p2file.open (p2name.c_str());
  token = lex->GetToken();
  code = new CodeGen(name, lex);
  isMain = false;
  buf = "";
  tabs = 0;
  Program ();
  delete code;
}

/*******************************************************************************
 * Function: SyntacticalAnalyzer Destructor                                     *
 *                                                                              *
 * Parameters:                                                                  *
 * Return value:                                                                *
 * Description: This function will                                              *
 *******************************************************************************/
SyntacticalAnalyzer::~SyntacticalAnalyzer ()
{
  delete lex;
  p2file.close ();
}

/*******************************************************************************
 * Function: Program                                                            *
 *                                                                              *
 * Parameters:                                                                  *
 * Return value:                                                                *
 * Description: This function will                                              *
 *******************************************************************************/
int SyntacticalAnalyzer::Program ()
{
  p2file << "Entering Program function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() <<endl;
  int errors = 0;

  static set<token_type> firsts = {LPAREN_T};
  static set<token_type> follows = {};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Program");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  // Rule 1
  p2file << "Using Rule 1" << endl;
  errors += Define ();
  errors += More_Defines ();
  if (token != EOF_T)
    {
      errors++;
      lex->ReportError ("Missing end of file at end of program");
    }

  p2file << "Exiting Program function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Define ()
{
  int errors = 0;
  p2file << "Entering Define function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {LPAREN_T};
  static set<token_type> follows = {LPAREN_T, EOF_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Define");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  // Rule 2
  p2file << "Using Rule 2" << endl;
  errors += Error_Helper(LPAREN_T, "Missing left parenthesis");
  errors += Error_Helper(DEFINE_T, "Missing define");
  errors += Error_Helper(LPAREN_T, "Missing left parenthesis");

  if(lex->GetLexeme() == "main")
  {
    code->WriteCode(tabs, "int " + lex->GetLexeme() + "( ");
    isMain = true;
  }
  else
    code->WriteCode(tabs, "Object " + lex->GetLexeme() + "( ");

  errors += Error_Helper(IDENT_T, "Missing identifier");
  errors += Param_List(false);

  code -> WriteCode(0, ")\n{\n");
  
  errors += Error_Helper(RPAREN_T, "Missing right parenthesis");
  errors += Stmt(true);

  code -> WriteCode(0, "\n");

  errors += Stmt_List("\n", false, true);
  
  errors += Error_Helper(RPAREN_T, "Missing right parenthesis");

  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of Define");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting Define function; current token is: "
   << lex->GetTokenName (token) << endl;
  code->WriteCode(tabs, "\n");
  if(isMain)
    code->WriteCode(tabs, "return 0;\n");
  else
    code->WriteCode(tabs, "return __RetVal;\n");
  code->WriteCode(--tabs, "}\n\n");
  return errors;
}

int SyntacticalAnalyzer::More_Defines ()
{
  int errors = 0;
  p2file << "Entering More_Defines function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {LPAREN_T, EOF_T};
  static set<token_type> follows = {EOF_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of More_Defines");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  // Rule 3
  if(token == LPAREN_T)
    {
      p2file << "Using Rule 3" << endl;
      errors += Define();
      errors += More_Defines();
    }

  // Rule 4
  // Lambda, do nothing
  else if (token == EOF_T)
    p2file << "Using Rule 4" << endl;

  else {
    errors++;
    lex -> ReportError("Expecting first or follows of More_Defines; found: " + lex -> GetTokenName(token));
  }
  
  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of More_Defines");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting More_Defines function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Stmt_List (string symbol, bool print, bool _return)
{
  int errors = 0;
  p2file << "Entering Stmt_List function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, QUOTE_T};
  static set<token_type> follows = {RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Stmt_List");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  // Rule 5
  if(token == NUMLIT_T || token == STRLIT_T || token == QUOTE_T
     || token == IDENT_T  || token == LPAREN_T)
    {
      p2file << "Using Rule 5" << endl;
      if (print)
  code -> WriteCode(0, " " + symbol + " ");
      errors += Stmt(_return);
      errors += Stmt_List(symbol, true, false);
    }

  // Rule 6
  // Lambda, do nothing
  else if (token == RPAREN_T)
    p2file << "Using Rule 6" << endl;

  else {
    errors++;
    lex-> ReportError("Expecting first or follows of Stmt_List; found: " + lex -> GetTokenName(token));
  }
  
  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of Stmt_List");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting Stmt_List function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Stmt (bool _return)
{
  int errors = 0;
  p2file << "Entering Stmt function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, QUOTE_T};
  static set<token_type> follows = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, QUOTE_T, RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Stmt");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  // Rule 7
  if(token == NUMLIT_T || token == STRLIT_T || token == QUOTE_T)
    {
      p2file << "Using Rule 7" << endl;
      if (_return)
  code -> WriteCode(tabs, "return ");
      errors += Literal();
      if (_return)
  code -> WriteCode(0, ";");
    }

  // Rule 8
  else if(token == IDENT_T)
    {
      p2file << "Using Rule 8" << endl;
      code -> WriteCode(_return ? tabs : 0, (_return ? "return " : " ") + lex->GetLexeme() + (_return ? ";" : ""));
      token = lex->GetToken();
    }

  // Rule 9
  else if(token == LPAREN_T)
    {
      p2file << "Using Rule 9" << endl;
      token = lex->GetToken();
      errors += Action(_return);
      if (token == RPAREN_T)
  token = lex -> GetToken();
      else {
  errors++;
  lex-> ReportError("Expecting RPAREN_T but found: " + lex -> GetTokenName(token));
      }
    }

  // If we did not get a proper statement
  else {
    errors++;
    lex-> ReportError("Expecting first or follows of Stmt; found: " + lex -> GetTokenName(token));
  }

  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of Stmt");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting Stmt function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Literal ()
{
  int errors = 0;
  p2file << "Entering Literal function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {NUMLIT_T, STRLIT_T, QUOTE_T};
  static set<token_type> follows = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, QUOTE_T, RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Literal");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  // Rule 10
  if(token == NUMLIT_T)
    {
      p2file << "Using Rule 10" << endl;
      code -> WriteCode(0, " Object( " + lex -> GetLexeme() + " )");
      token = lex->GetToken();
    }

  // Rule 11
  else if(token == STRLIT_T)
    {
      p2file << "Using Rule 11" << endl;
      code -> WriteCode(0, " Object( " + lex -> GetLexeme() + " )");
      token = lex->GetToken();
    }

  // Rule 12
  else if(token == QUOTE_T)
    {
      p2file << "Using Rule 12" << endl;
      token = lex->GetToken();
      errors += Quoted_Lit();
    }

  // Error
  else
    errors += Error_Helper("Expecting <literal>");

  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of Literal");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting Literal function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Quoted_Lit ()
{
  int errors = 0;
  p2file << "Entering Quoted_Lit function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, QUOTE_T, CONS_T, IF_T,
           DISPLAY_T, NEWLINE_T, LISTOP_T, AND_T, OR_T, NOT_T, DEFINE_T,
           NUMBERP_T, SYMBOLP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T,
           PLUS_T, MINUS_T, MULT_T, DIV_T, MODULO_T, EQUALTO_T, GT_T,
           LT_T, GTE_T, LTE_T, COND_T, ELSE_T};
  static set<token_type> follows = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, QUOTE_T, RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Quoted Literal");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  // Rule 13
  p2file << "Using Rule 13" << endl;
  code -> WriteCode(0, " Object( \"");
  errors += Any_Other_Token();
  code -> WriteCode(0, "\" )");

  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of Quoted Literal");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting Quoted_Lit function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::More_Tokens ()
{
  int errors = 0;
  p2file << "Entering More_Tokens function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, QUOTE_T, CONS_T, IF_T,
           DISPLAY_T, NEWLINE_T, LISTOP_T, AND_T, OR_T, NOT_T, DEFINE_T,
           NUMBERP_T, SYMBOLP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T,
           PLUS_T, MINUS_T, MULT_T, DIV_T, MODULO_T, EQUALTO_T, GT_T,
           LT_T, GTE_T, LTE_T, COND_T, ELSE_T};
  static set<token_type> follows = {RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of More Tokens");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  static set<token_type> Any_Other_Token_firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, CONS_T, IF_T, DISPLAY_T,
           NEWLINE_T, LISTOP_T, AND_T, OR_T, NOT_T, ELSE_T, QUOTE_T,
           DEFINE_T, NUMBERP_T, SYMBOLP_T, LISTP_T, ZEROP_T, NULLP_T,
           STRINGP_T, PLUS_T, MINUS_T, MULT_T, DIV_T, MODULO_T, EQUALTO_T,
           GT_T, LT_T, GTE_T, LTE_T, COND_T};
  set<token_type>::iterator itr3 = Any_Other_Token_firsts.find(token);
  if (itr3 != Any_Other_Token_firsts.end()) {
    p2file << "Using Rule 14" << endl;
    errors += Any_Other_Token();
    errors += More_Tokens();
  }

  // Rule 15
  // Lambda, do nothing
  else if(token == RPAREN_T)
    p2file << "Using Rule 15" << endl;

  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of More Tokens");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting More_Tokens function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Param_List (bool comma)
{
  int errors = 0;

  p2file << "Entering Param_List function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {IDENT_T};
  static set<token_type> follows = {RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Param_List");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  // Rule 16
  if(token == IDENT_T)
    {
      p2file << "Using Rule 16" << endl;
      if (comma)
  code -> WriteCode(0, ", ");
      code -> WriteCode(0, " Object " + lex -> GetLexeme());
      token = lex->GetToken();
      errors += Param_List(true);
    }

  // Rule 17
  else if (token == RPAREN_T) {
    p2file << "Using Rule 17" << endl;
    comma = false;
  }

  else {
    errors++;
    lex-> ReportError("Expecting first or follows of Param_List; found: " + lex -> GetTokenName(token));
  }

  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of Param_List");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting Param_List function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Else_Part ()
{
  int errors = 0;
  p2file << "Entering Else_Part function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, QUOTE_T};
  static set<token_type> follows = {RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Else Part");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  // Rule 18
  if(token == NUMLIT_T || token == STRLIT_T || token == QUOTE_T
     || token == IDENT_T  || token == LPAREN_T)
    {
      p2file << "Using Rule 18" << endl;
      code -> WriteCode(0, "else{\n");
      errors += Stmt(true);
      code -> WriteCode(0, "\n");
      code -> WriteCode(--tabs, "}");
    }

  // Rule 19
  else if (token == RPAREN_T)
    p2file << "Using Rule 19" << endl;

  else {
    errors++;
    lex-> ReportError("Expecting first or follows of Else_Part; found: " + lex -> GetTokenName(token));
  }
  
  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of Else Part");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting Else_Part function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Stmt_Pair ()
{
  int errors = 0;
  p2file << "Entering Stmt_Pair function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {LPAREN_T, RPAREN_T};
  static set<token_type> follows = {RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Statement Pair");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  //Rule 20
  if(token == LPAREN_T)
    {
      p2file << "Using Rule 20" << endl;
      token = lex->GetToken();
      errors += Stmt_Pair_Body();
    }

  //Rule 21
  else if (token == RPAREN_T)
    p2file << "Using Rule 21" << endl;

  else {
    errors++;
    lex-> ReportError("Expecting first or follows of Stmt_Pair; found: " + lex -> GetTokenName(token));
  }

  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of Statement Pair");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting Stmt_Pair function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Stmt_Pair_Body ()
{
  int errors = 0;
  p2file << "Entering Stmt_Pair_Body function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, ELSE_T, QUOTE_T};
  static set<token_type> follows = {RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Statemet Pair Body");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  //Rule 22
  if(token == NUMLIT_T || token == STRLIT_T || token == QUOTE_T
     || token == IDENT_T  || token == LPAREN_T)
    {
      p2file << "Using Rule 22" << endl;

      errors += Stmt(false);
      errors += Stmt(true);
      if (token == RPAREN_T)
  token = lex -> GetToken();
      else{
  errors++;
  lex->ReportError("Expected firsts or follows of Stmt_Pair_Body; found: " + lex -> GetTokenName(token));
      }
      errors += Stmt_Pair();
    }

  //Rule 23
  else if(token == ELSE_T)
    {
      p2file << "Using Rule 23" << endl;
      token = lex->GetToken();
      errors += Stmt(true);
      if (token == RPAREN_T)
  token = lex -> GetToken();
      else{
  errors++;
  lex->ReportError("Expected firsts or follows of Stmt_Pair_Body; found: " + lex -> GetTokenName(token));
      }
    }

  else{
    errors++;
    lex->ReportError("Expected firsts or follows of Stmt_Pair_Body; found: " + lex -> GetTokenName(token));
  }

  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of Statement Pair Body");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting Stmt_Pair_Body function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Action (bool _return)
{
  int errors = 0;
  p2file << "Entering Action function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {IF_T, COND_T, LISTOP_T, CONS_T, AND_T, OR_T, NOT_T, NUMBERP_T,
           SYMBOLP_T, LISTP_T, ZEROP_T, NULLP_T, STRINGP_T, PLUS_T, MINUS_T,
           MULT_T, DIV_T, MODULO_T, EQUALTO_T, GT_T, LT_T, GTE_T, LTE_T,
           DISPLAY_T, NEWLINE_T, IDENT_T};
  static set<token_type> follows = {RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Action");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

  switch(token)
    {
      // Rule 24
    case IF_T:
      p2file << "Using Rule 24" << endl;
      token = lex->GetToken();
      code -> WriteCode(tabs++, "if(");
      errors += Stmt(false);
      code -> WriteCode(0, " ) {\n");
      errors += Stmt(true);
      code -> WriteCode(0, "\n");
      code -> WriteCode(--tabs, "}");
      errors += Else_Part();
      break;

      // Rule 25
    case COND_T:
      p2file << "Using Rule 25" << endl;
      token = lex->GetToken();
      errors += Error_Helper(LPAREN_T, "Expecting LPAREN_T");
      errors += Stmt_Pair_Body();
      code -> WriteCode(0, ";\n");
      break;

      // Rule 26
    case LISTOP_T:
      p2file << "Using Rule 26" << endl;
      code -> WriteCode(0, " listop( \"" + lex -> GetLexeme() + "\", ");
      token = lex->GetToken();
      errors += Stmt(false);
      code -> WriteCode(0, " )");
      break;

      // Rule 27
    case CONS_T:
      p2file << "Using Rule 27" << endl;
      token = lex->GetToken();
      code -> WriteCode(0, " cons(");
      errors += Stmt(false);
      code -> WriteCode(0, ", ");
      errors += Stmt(false);
      code -> WriteCode(0, " )");
      break;

      // Rule 28
    case AND_T:
      if (_return)
  code -> WriteCode(tabs, "return");
      p2file << "Using Rule 28" << endl;
      token = lex->GetToken();
      errors += Stmt_List(" && ", false, false);
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 29
    case OR_T:
      if (_return)
  code -> WriteCode(tabs, "return");
      p2file << "Using Rule 29" << endl;
      token = lex->GetToken();
      errors += Stmt_List(" ||", false, false);
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 30
    case NOT_T:
      if (_return)
  code -> WriteCode(tabs, "return");
      p2file << "Using Rule 30" << endl;
      token = lex->GetToken();
      code -> WriteCode(0, " !(");
      errors += Stmt(false);
      code -> WriteCode(0, " )");
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 31
    case NUMBERP_T:
      if (_return)
  code -> WriteCode(tabs, "return ");
      p2file << "Using Rule 31" << endl;
      code -> WriteCode(0, " numberp(");
      token = lex->GetToken();
      errors += Stmt(false);
      code -> WriteCode(0, " )");
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 32
    case SYMBOLP_T:
      if(_return)
  code -> WriteCode(tabs, "return ");
      p2file << "Using Rule 32" << endl;
      code -> WriteCode(0, " symbolp(");
      token = lex->GetToken();
      errors += Stmt(false);
      code -> WriteCode(0, " )");
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 33
    case LISTP_T:
      if(_return)
  code ->WriteCode(tabs, "return ");
      p2file << "Using Rule 33" << endl;
      code -> WriteCode(0, "listp( ");
      token = lex->GetToken();
      errors += Stmt(false);
      code -> WriteCode(0, " )");
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 34
    case ZEROP_T:
      if(_return)
  code ->WriteCode(tabs, "return ");
      p2file << "Using Rule 34" << endl;
      code -> WriteCode(0, "zerop( ");
      token = lex->GetToken();
      errors += Stmt(false);
      code -> WriteCode(0, " )");
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 35
    case NULLP_T:
      if(_return)
  code ->WriteCode(tabs, "return ");
      p2file << "Using Rule 35" << endl;
      code -> WriteCode(0, "nullp( ");
      token = lex->GetToken();
      errors += Stmt(false);
      code -> WriteCode(0, ")");
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 36
    case STRINGP_T:
      if(_return)
  code ->WriteCode(tabs, "return ");
      p2file << "Using Rule 36" << endl;
      code -> WriteCode(0, "stringp( ");
      token = lex->GetToken();
      errors += Stmt(false);
      code -> WriteCode(0, " )");
      break;

      // Rule 37
    case PLUS_T:
      p2file << "Using Rule 37" << endl;
      token = lex->GetToken();
      errors += Stmt_List(" +", false, false);
      break;

      // Rule 38
    case MINUS_T:
      p2file << "Using Rule 38" << endl;
      token = lex->GetToken();
      errors += Stmt(false);
      errors += Stmt_List(" -", true, false);
      break;

      // Rule 39
    case DIV_T:
      p2file << "Using Rule 39" << endl;
      token = lex->GetToken();
      errors += Stmt(false);
      errors += Stmt_List(" /", true, false);
      break;

      // Rule 40
    case MULT_T:
      p2file << "Using Rule 40" << endl;
      token = lex->GetToken();
      errors += Stmt_List(" *", false, false);
      break;

      // Rule 41
    case MODULO_T:
      p2file << "Using Rule 41" << endl;
      token = lex->GetToken();
      errors += Stmt(false);
      code -> WriteCode(0, " %");
      errors += Stmt(false);
      break;

      // Rule 42
    case EQUALTO_T:
      if(_return)
  code ->WriteCode(tabs, "return ");
      p2file << "Using Rule 42" << endl;
      token = lex->GetToken();
      errors += Stmt_List(" ==", false, false);
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 43
    case GT_T:
      if(_return)
  code ->WriteCode(tabs, "return ");
      p2file << "Using Rule 43" << endl;
      token = lex->GetToken();
      errors += Stmt_List(" >", false, false);
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 44
    case LT_T:
      if(_return)
  code ->WriteCode(tabs, "return ");
      p2file << "Using Rule 44" << endl;
      token = lex->GetToken();
      errors += Stmt_List(" <", false, false);
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 45
    case GTE_T:
      if(_return)
  code ->WriteCode(tabs, "return ");
      p2file << "Using Rule 45" << endl;
      token = lex->GetToken();
      errors += Stmt_List(" <=", false, false);
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 46
    case LTE_T:
      if(_return)
  code ->WriteCode(tabs, "return ");
      p2file << "Using Rule 46" << endl;
      token = lex->GetToken();
      errors += Stmt_List(" <=", false, false);
      if (_return)
  code -> WriteCode(0, ";");
      break;

      // Rule 47
    case IDENT_T:
      code -> WriteCode(0, " " + lex -> GetLexeme() + "(");
      p2file << "Using Rule 47" << endl;
      token = lex->GetToken();
      errors += Stmt_List(",", false, false);
      code -> WriteCode(0, " )");
      break;

      // Rule 48
    case DISPLAY_T:
      p2file << "Using Rule 48" << endl;
      token = lex->GetToken();
      code -> WriteCode(tabs, "cout <<");
      errors += Stmt(false);
      code -> WriteCode(0, ";");
      break;

      // Rule 49
    case NEWLINE_T:
      p2file << "Using Rule 49" << endl;
      code -> WriteCode(tabs, "cout << endl");
      token = lex->GetToken();
      code -> WriteCode(0, ";");
      break;

    default:
      errors += Error_Helper("Expecting <action>");

    }

  set<token_type>::iterator itr = follows.find(token);
  while (itr == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at end of Action");
    errors++;
    token = lex -> GetToken();
    itr = follows.find(token);
  }

  p2file << "Exiting Action function; current token is: "
   << lex->GetTokenName (token) << endl;
  return errors;
}

int SyntacticalAnalyzer::Any_Other_Token ()
{
  int errors = 0;
  p2file << "Entering Any_Other_Token function; current token is: "
   << lex->GetTokenName (token) << ", lexeme: " << lex->GetLexeme() << endl;

  static set<token_type> firsts = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, CONS_T, IF_T, DISPLAY_T,
           NEWLINE_T, LISTOP_T, AND_T, OR_T, NOT_T, ELSE_T, QUOTE_T,
           DEFINE_T, NUMBERP_T, SYMBOLP_T, LISTP_T, ZEROP_T, NULLP_T,
           STRINGP_T, PLUS_T, MINUS_T, MULT_T, DIV_T, MODULO_T, EQUALTO_T,
           GT_T, LT_T, GTE_T, LTE_T, COND_T};
  static set<token_type> follows = {LPAREN_T, IDENT_T, NUMLIT_T, STRLIT_T, CONS_T, IF_T, DISPLAY_T,
            NEWLINE_T, LISTOP_T, AND_T, OR_T, NOT_T, ELSE_T, QUOTE_T,
            DEFINE_T, NUMBERP_T, SYMBOLP_T, LISTP_T, ZEROP_T, NULLP_T,
            STRINGP_T, PLUS_T, MINUS_T, MULT_T, DIV_T, MODULO_T, EQUALTO_T,
            GT_T, LT_T, GTE_T, LTE_T, COND_T, RPAREN_T};
  set<token_type>::iterator itr1 = firsts.find(token);
  set<token_type>::iterator itr2 = follows.find(token);
  while (itr1 == firsts.end() && itr2 == follows.end() && token != EOF_T) {
    lex -> ReportError("Unexpected '" + lex -> GetTokenName(token) + "' found at beginning of Any Other Action");
    errors++;
    itr1 = firsts.find(token);
    itr2 = follows.find(token);
  }

    switch(token)
      {
        // Rule 50
      case LPAREN_T:
        p2file << "Using Rule 50" << endl;
        token = lex->GetToken();
        errors += More_Tokens();
        errors += Error_Helper(RPAREN_T, "Expecting RPAREN_T");
        break;

        // Rule 51
      case IDENT_T:
        p2file << "Using Rule 51" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 52
      case NUMLIT_T:
        p2file << "Using Rule 52" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 53
      case STRLIT_T:
        p2file << "Using Rule 53" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 54
      case CONS_T:
        p2file << "Using Rule 54" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 55
      case IF_T:
        p2file << "Using Rule 55" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 56
      case DISPLAY_T:
        p2file << "Using Rule 56" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 57
      case NEWLINE_T:
        p2file << "Using Rule 57" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 58
      case LISTOP_T:
        p2file << "Using Rule 58" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 59
      case AND_T:
        p2file << "Using Rule 59" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 60
      case OR_T:
        p2file << "Using Rule 60" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 61
      case NOT_T:
        p2file << "Using Rule 61" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 62
      case DEFINE_T:
        p2file << "Using Rule 62" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 63
      case NUMBERP_T:
        p2file << "Using Rule 63" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 64
      case SYMBOLP_T:
        p2file << "Using Rule 64" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 65
      case LISTP_T:
        p2file << "Using Rule 65" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 66
      case ZEROP_T:
        p2file << "Using Rule 66" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 67
      case NULLP_T:
        p2file << "Using Rule 67" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 68
      case STRINGP_T:
        p2file << "Using Rule 68" << endl;
        code->WriteCode(0, " " + lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 69
      case PLUS_T:
        p2file << "Using Rule 69" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 70
      case MINUS_T:
        p2file << "Using Rule 70" << endl;
        code->WriteCode(tabs, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 71
      case DIV_T:
        p2file << "Using Rule 71" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 72
      case MULT_T:
        p2file << "Using Rule 72" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 73
      case MODULO_T:
        p2file << "Using Rule 73" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 74
      case EQUALTO_T:
        p2file << "Using Rule 74" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 75
      case GT_T:
        p2file << "Using Rule 75" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 76
      case LT_T:
        p2file << "Using Rule 76" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 77
      case GTE_T:
        p2file << "Using Rule 77" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 78
      case LTE_T:
        p2file << "Using Rule 78" << endl;
        code->WriteCode(0,lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 79
      case QUOTE_T:
        p2file << "Using Rule 79" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        errors += Any_Other_Token();
        break;

        // Rule 80
      case COND_T:
        p2file << "Using Rule 80" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

        // Rule 81
      case ELSE_T:
        p2file << "Using Rule 81" << endl;
        code->WriteCode(0, lex->GetLexeme() + buf + " ");
        token = lex->GetToken();
        break;

      default:
        errors += Error_Helper("Expecting <any_other_token>");

      }
    buf = "";
    set<token_type>::iterator itr = follows.find(token);
    while (itr == follows.end() && token != EOF_T) {
      lex -> ReportError("Unexpected '" + lex -> GetLexeme() + "' found at end of Any Other Action");
      errors++;
      token = lex -> GetToken();
      itr = follows.find(token);
    }

    p2file << "Exiting Any_Other_Token function; current token is: "
     << lex->GetTokenName (token) << endl;
    return errors;
  }

// Helper functions to consolidate all the error print calls
int SyntacticalAnalyzer::Error_Helper(string s)
{
  lex->ReportError(s);
  token = lex->GetToken();
  return 1;
}

int SyntacticalAnalyzer::Error_Helper(token_type t, string s)
{
  if(token != t)
    {
      lex->ReportError(s);
      token = lex->GetToken();
      return 1;
    }
  token = lex->GetToken();
  return 0;
}
