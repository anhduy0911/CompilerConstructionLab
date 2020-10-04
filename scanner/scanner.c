/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank()
{
  // TODO
  while (currentChar != -1 && charCodes[currentChar] == CHAR_SPACE)
    readChar();
}

void skipComment()
{
  // TODO
  while (1)
  {
    // read next character
    readChar();

    if (currentChar == -1)
    {
      // end of file
      error(ERR_ENDOFCOMMENT, lineNo, colNo);
    }
    else if (charCodes[currentChar] == CHAR_TIMES)
    {
      // next is asterick character
      readChar();
      if (currentChar == -1)
      {
        // EOF
        error(ERR_ENDOFCOMMENT, lineNo, colNo);
      }
      else if (charCodes[currentChar] == CHAR_RPAR)
      {
        // next is right parentthesis
        readChar();
        return;
      }
    }
  }
}

Token *readIdentKeyword(void)
{
  // TODO
  int count = 0;
  Token *token = makeToken(TK_IDENT, lineNo, colNo);

  while (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT)
  {
    // add cur character to identifier
    token->string[count] = currentChar;
    count++;
    readChar();
  }

  // when end string
  token->string[count] = '\0';

  // If encounter limited length
  if (count > MAX_IDENT_LEN)
  {
    error(ERR_IDENTTOOLONG, lineNo, colNo - count);
  }
  else
  {
    // if token is keyword
    TokenType type = checkKeyword(token->string);

    // ><
    if (type != TK_NONE)
    {
      token->tokenType = type;
    }
  }
  return token;
}

Token *readNumber(void)
{
  int count = 0;
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);

  while (charCodes[currentChar] == CHAR_DIGIT)
  {
    if (count > 9)
    {
      error(ERR_NUMBERTOOLONG, token->lineNo, token->colNo);
    }

    token->string[count] = currentChar;
    count++;
    readChar();
  }

  token->string[count] = '\0';
  token->value = atoi(token->string);
  return token;
}

Token *readConstChar(void)
{
  Token *token = makeToken(TK_CHAR, lineNo, colNo);

  readChar();

  if (currentChar == -1)
  {
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
  }
  else
  {
    switch (charCodes[currentChar])
    {
    // escape character for single quote
    case CHAR_SINGLEQUOTE: // read ''
      // read next character
      readChar();
      int current = 0;
      while (charCodes[currentChar] != CHAR_SINGLEQUOTE && currentChar >= 0x20 && currentChar <= 0x7E) //printable chars
      {
        if (current >= MAX_STRING_LEN)
          error(ERR_STRINGTOOLONG, token->lineNo, token->colNo);
        token->string[current++] = currentChar;
        readChar();
      }
      if (charCodes[currentChar] == CHAR_SINGLEQUOTE) // read end '
      {
        readChar();
        if (charCodes[currentChar] == CHAR_SINGLEQUOTE) // read ''
        {
          token->string[current] = '\0';
          readChar();
          return token;
        }
        else
          error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
      }
      else
        error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
      //}
      break;
    default:
      // add the character to token string
      token->string[0] = currentChar;
      readChar();
      switch (charCodes[currentChar])
      {
      case CHAR_SINGLEQUOTE:
        /* code */
        //end token
        token->string[1] = '\0';
        readChar();
        return token;

      default:
        error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
        break;
      }
      break;
    }
  }
  return token;
}

Token *getToken(void)
{
  Token *token;

  if (currentChar == EOF)
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar])
  {
  case CHAR_SPACE:
    skipBlank();
    return getToken();
  case CHAR_LETTER:
    return readIdentKeyword();
  case CHAR_DIGIT:
    return readNumber();
  case CHAR_PLUS:
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    // Token Times
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SLASH:
    // Token Slash
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar();
    return token;
  case CHAR_EQ:
    // Token Equal
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA:
    // Token Comma
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SEMICOLON:
    // Token Semicolon
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  case CHAR_RPAR:
    // Token Right Parenthesis
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;

  case CHAR_LPAR:
    // empty token
    token = makeToken(TK_NONE, lineNo, colNo);
    readChar(); // get next one
    switch (charCodes[currentChar])
    {
    case CHAR_PERIOD:
      // LSEL
      token->tokenType = SB_LSEL;
      readChar();
      return token;
    /* code */
    case CHAR_TIMES:
      // comment, free the allocated first and skip comment
      free(token);
      skipComment();
      return getToken();
    case CHAR_SPACE:
      skipBlank();
      token->tokenType = SB_LPAR;
      return token;
    default:
      // Token Left Parenthesis
      token->tokenType = SB_LPAR;
      return token;
    }

  case CHAR_GT:
    // token greater
    token = makeToken(SB_GT, lineNo, colNo);
    readChar();
    // check next token is = or not
    if (charCodes[currentChar] == CHAR_EQ)
    {
      token->tokenType = SB_GE;
      readChar();
    }
    return token;

  case CHAR_LT:

    token = makeToken(TK_NONE, lineNo, colNo);
    readChar();
    switch (charCodes[currentChar])
    {
    case CHAR_EQ:
      token->tokenType = SB_LE;
      readChar();
      return token;
    default:
      token->tokenType = SB_LT;
    }

  case CHAR_EXCLAIMATION:
    token = makeToken(TK_NONE, lineNo, colNo);

    readChar();
    if (charCodes[currentChar] != CHAR_EQ)
    {
      error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
    }
    else
    {
      token->tokenType = SB_NEQ;
      readChar();
    }
    return token;
  case CHAR_PERIOD:
    // Token Period
    token = makeToken(SB_PERIOD, lineNo, colNo);

    // If next character is Right Parenthesis
    readChar();
    if (charCodes[currentChar] == CHAR_RPAR)
    {
      // it is token Right Parenthesis
      token->tokenType = SB_RSEL;
      readChar();
    }
    return token;
  case CHAR_COLON:
    // Token Semicolon
    token = makeToken(SB_COLON, lineNo, colNo);

    // If next character is Equal
    readChar();
    if (charCodes[currentChar] == CHAR_EQ)
    {
      // it is token Assignment
      token->tokenType = SB_ASSIGN;
      readChar();
    }
    return token;
  case CHAR_SINGLEQUOTE:
    return readConstChar();

    // ....
    // TODO
    // ....
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar();
    return token;
  }
}

/******************************************************************/

void printToken(Token *token, int toFile, char *fileName)
{
  if (toFile == 0)
  {
    printf("%d-%d:", token->lineNo, token->colNo);

    switch (token->tokenType)
    {
    case TK_NONE:
      printf("TK_NONE\n");
      break;
    case TK_IDENT:
      printf("TK_IDENT(%s)\n", token->string);
      break;
    case TK_NUMBER:
      printf("TK_NUMBER(%s)\n", token->string);
      break;
    case TK_CHAR:
      printf("TK_CHAR(\'%s\')\n", token->string);
      break;
    case TK_EOF:
      printf("TK_EOF\n");
      break;

    case KW_PROGRAM:
      printf("KW_PROGRAM\n");
      break;
    case KW_CONST:
      printf("KW_CONST\n");
      break;
    case KW_TYPE:
      printf("KW_TYPE\n");
      break;
    case KW_VAR:
      printf("KW_VAR\n");
      break;
    case KW_INTEGER:
      printf("KW_INTEGER\n");
      break;
    case KW_CHAR:
      printf("KW_CHAR\n");
      break;
    case KW_ARRAY:
      printf("KW_ARRAY\n");
      break;
    case KW_OF:
      printf("KW_OF\n");
      break;
    case KW_FUNCTION:
      printf("KW_FUNCTION\n");
      break;
    case KW_PROCEDURE:
      printf("KW_PROCEDURE\n");
      break;
    case KW_BEGIN:
      printf("KW_BEGIN\n");
      break;
    case KW_END:
      printf("KW_END\n");
      break;
    case KW_CALL:
      printf("KW_CALL\n");
      break;
    case KW_IF:
      printf("KW_IF\n");
      break;
    case KW_THEN:
      printf("KW_THEN\n");
      break;
    case KW_ELSE:
      printf("KW_ELSE\n");
      break;
    case KW_WHILE:
      printf("KW_WHILE\n");
      break;
    case KW_DO:
      printf("KW_DO\n");
      break;
    case KW_FOR:
      printf("KW_FOR\n");
      break;
    case KW_TO:
      printf("KW_TO\n");
      break;

    case SB_SEMICOLON:
      printf("SB_SEMICOLON\n");
      break;
    case SB_COLON:
      printf("SB_COLON\n");
      break;
    case SB_PERIOD:
      printf("SB_PERIOD\n");
      break;
    case SB_COMMA:
      printf("SB_COMMA\n");
      break;
    case SB_ASSIGN:
      printf("SB_ASSIGN\n");
      break;
    case SB_EQ:
      printf("SB_EQ\n");
      break;
    case SB_NEQ:
      printf("SB_NEQ\n");
      break;
    case SB_LT:
      printf("SB_LT\n");
      break;
    case SB_LE:
      printf("SB_LE\n");
      break;
    case SB_GT:
      printf("SB_GT\n");
      break;
    case SB_GE:
      printf("SB_GE\n");
      break;
    case SB_PLUS:
      printf("SB_PLUS\n");
      break;
    case SB_MINUS:
      printf("SB_MINUS\n");
      break;
    case SB_TIMES:
      printf("SB_TIMES\n");
      break;
    case SB_SLASH:
      printf("SB_SLASH\n");
      break;
    case SB_LPAR:
      printf("SB_LPAR\n");
      break;
    case SB_RPAR:
      printf("SB_RPAR\n");
      break;
    case SB_LSEL:
      printf("SB_LSEL\n");
      break;
    case SB_RSEL:
      printf("SB_RSEL\n");
      break;
    }
  }
  else
  {
    FILE *f = fopen(fileName, "a");

    fprintf(f, "%d-%d:", token->lineNo, token->colNo);

    switch (token->tokenType)
    {
    case TK_NONE:
      fprintf(f, "TK_NONE\n");
      break;
    case TK_IDENT:
      fprintf(f, "TK_IDENT(%s)\n", token->string);
      break;
    case TK_NUMBER:
      fprintf(f, "TK_NUMBER(%s)\n", token->string);
      break;
    case TK_CHAR:
      fprintf(f, "TK_CHAR(\'%s\')\n", token->string);
      break;
    case TK_EOF:
      fprintf(f, "TK_EOF\n");
      break;

    case KW_PROGRAM:
      fprintf(f, "KW_PROGRAM\n");
      break;
    case KW_CONST:
      fprintf(f, "KW_CONST\n");
      break;
    case KW_TYPE:
      fprintf(f, "KW_TYPE\n");
      break;
    case KW_VAR:
      fprintf(f, "KW_VAR\n");
      break;
    case KW_INTEGER:
      fprintf(f, "KW_INTEGER\n");
      break;
    case KW_CHAR:
      fprintf(f, "KW_CHAR\n");
      break;
    case KW_ARRAY:
      fprintf(f, "KW_ARRAY\n");
      break;
    case KW_OF:
      fprintf(f, "KW_OF\n");
      break;
    case KW_FUNCTION:
      fprintf(f, "KW_FUNCTION\n");
      break;
    case KW_PROCEDURE:
      fprintf(f, "KW_PROCEDURE\n");
      break;
    case KW_BEGIN:
      fprintf(f, "KW_BEGIN\n");
      break;
    case KW_END:
      fprintf(f, "KW_END\n");
      break;
    case KW_CALL:
      fprintf(f, "KW_CALL\n");
      break;
    case KW_IF:
      fprintf(f, "KW_IF\n");
      break;
    case KW_THEN:
      fprintf(f, "KW_THEN\n");
      break;
    case KW_ELSE:
      fprintf(f, "KW_ELSE\n");
      break;
    case KW_WHILE:
      fprintf(f, "KW_WHILE\n");
      break;
    case KW_DO:
      fprintf(f, "KW_DO\n");
      break;
    case KW_FOR:
      fprintf(f, "KW_FOR\n");
      break;
    case KW_TO:
      fprintf(f, "KW_TO\n");
      break;

    case SB_SEMICOLON:
      fprintf(f, "SB_SEMICOLON\n");
      break;
    case SB_COLON:
      fprintf(f, "SB_COLON\n");
      break;
    case SB_PERIOD:
      fprintf(f, "SB_PERIOD\n");
      break;
    case SB_COMMA:
      fprintf(f, "SB_COMMA\n");
      break;
    case SB_ASSIGN:
      fprintf(f, "SB_ASSIGN\n");
      break;
    case SB_EQ:
      fprintf(f, "SB_EQ\n");
      break;
    case SB_NEQ:
      fprintf(f, "SB_NEQ\n");
      break;
    case SB_LT:
      fprintf(f, "SB_LT\n");
      break;
    case SB_LE:
      fprintf(f, "SB_LE\n");
      break;
    case SB_GT:
      fprintf(f, "SB_GT\n");
      break;
    case SB_GE:
      fprintf(f, "SB_GE\n");
      break;
    case SB_PLUS:
      fprintf(f, "SB_PLUS\n");
      break;
    case SB_MINUS:
      fprintf(f, "SB_MINUS\n");
      break;
    case SB_TIMES:
      fprintf(f, "SB_TIMES\n");
      break;
    case SB_SLASH:
      fprintf(f, "SB_SLASH\n");
      break;
    case SB_LPAR:
      fprintf(f, "SB_LPAR\n");
      break;
    case SB_RPAR:
      fprintf(f, "SB_RPAR\n");
      break;
    case SB_LSEL:
      fprintf(f, "SB_LSEL\n");
      break;
    case SB_RSEL:
      fprintf(f, "SB_RSEL\n");
      break;
    }
    fclose(f);
  }
}

int scan(char *fileName, int toFile, char *fileOutName)
{
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF)
  {
    printToken(token, toFile, fileOutName);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

int compareFile(FILE *fPtr1, FILE *fPtr2, int *line, int *col)
{
  char ch1, ch2;

  *line = 1;
  *col = 0;

  do
  {
    // Input character from both files
    ch1 = fgetc(fPtr1);
    ch2 = fgetc(fPtr2);

    // Increment line
    if (ch1 == '\n')
    {
      *line += 1;
      *col = 0;
    }

    // If characters are not same then return -1
    if (ch1 != ch2)
      return -1;

    *col += 1;

  } while (ch1 != EOF && ch2 != EOF);

  /* If both files have reached end */
  if (ch1 == EOF && ch2 == EOF)
    return 0;
  else
    return -1;
}

/******************************************************************/

int main(int argc, char *argv[])
{
  if (argc <= 1)
  {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (argc <= 3) // to std out
  {
    if (scan(argv[1], 0, "no_file") == IO_ERROR)
    {
      printf("Can\'t read input file!\n");
      return -1;
    }

    return 0;
  }
  else
  {
    if (scan(argv[1], 1, argv[3]) == IO_ERROR) //to file
    {
      printf("Can\'t read input file!\n");
      return -1;
    }

    FILE *fPtr1 = fopen(argv[3], "r");
    FILE *fPtr2 = fopen(argv[4], "r");

    int line, col;
    int diff = compareFile(fPtr1, fPtr2, &line, &col);

    if (diff == 0)
    {
      printf("\nBoth files are equal.");
    }
    else
    {
      printf("\nFiles are not equal.\n");
      printf("Line: %d, col: %d\n", line, col);
    }

    /* Finally close files to release resources */
    fclose(fPtr1);
    fclose(fPtr2);

    return 0;
  }
}
