/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <string.h>
#include "semantics.h"
#include "error.h"

extern SymTab *symtab;
extern Token *currentToken;

static int is_searching = 0;

Object *lookupObject(char *name)
{
  Object *obj = NULL;
  static Scope *currScope = NULL;

  if (is_searching == 0)
    currScope = symtab->currentScope;

  while (currScope != NULL)
  {
    is_searching = 1;
    obj = findObject(currScope->objList, name);
    if (obj != NULL)
      return obj;
    currScope = currScope->outer;
  }

  is_searching = 0;
  return findObject(symtab->globalObjectList, name);
}

void checkFreshIdent(char *name)
{
  Object *obj = findObject(symtab->currentScope->objList, currentToken->string);
  if (obj != NULL)
    error(ERR_DUPLICATE_IDENT, currentToken->lineNo, currentToken->colNo);
}

Object *checkDeclaredIdent(char *name)
{
  is_searching = 0; // start lookup, make sure it run
  Object *obj = lookupObject(currentToken->string);

  if (obj == NULL)
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object *checkDeclaredConstant(char *name)
{
  Object *obj = NULL;

  is_searching = 0;
  do
  {
    obj = lookupObject(name);
    if (obj != NULL && obj->kind == OBJ_CONSTANT)
      break;
  } while (obj != NULL);

  if (obj == NULL)
    error(ERR_UNDECLARED_CONSTANT, currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object *checkDeclaredType(char *name)
{
  Object *obj = NULL;

  is_searching = 0;
  do
  {
    obj = lookupObject(name);
    if (obj != NULL && obj->kind == OBJ_TYPE)
      break;
  } while (obj != NULL);

  if (obj == NULL)
    error(ERR_UNDECLARED_TYPE, currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object *checkDeclaredVariable(char *name)
{
  Object *obj = NULL;

  is_searching = 0;
  do
  {
    obj = lookupObject(name);
    if (obj != NULL && obj->kind == OBJ_VARIABLE)
      break;
  } while (obj != NULL);

  if (obj == NULL)
    error(ERR_UNDECLARED_VARIABLE, currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object *checkDeclaredFunction(char *name)
{
  Object *obj = NULL;

  is_searching = 0;
  do
  {
    obj = lookupObject(name);
    if (obj != NULL && obj->kind == OBJ_FUNCTION)
      break;
  } while (obj != NULL);

  if (obj == NULL)
    error(ERR_UNDECLARED_FUNCTION, currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object *checkDeclaredProcedure(char *name)
{
  Object *obj = NULL;

  is_searching = 0;
  do
  {
    obj = lookupObject(name);
    if (obj != NULL && obj->kind == OBJ_PROCEDURE)
      break;
  } while (obj != NULL);

  if (obj == NULL)
    error(ERR_UNDECLARED_PROCEDURE, currentToken->lineNo, currentToken->colNo);

  return obj;
}

Object *checkDeclaredLValueIdent(char *name)
{
  Object *obj = NULL;

  is_searching = 0;
  do
  {
    obj = lookupObject(name);
    if (obj != NULL &&
        (obj->kind == OBJ_FUNCTION ||
         obj->kind == OBJ_PARAMETER ||
         obj->kind == OBJ_VARIABLE))
      break;
  } while (obj != NULL);

  if (obj == NULL)
    error(ERR_UNDECLARED_IDENT, currentToken->lineNo, currentToken->colNo);

  return obj;
}
