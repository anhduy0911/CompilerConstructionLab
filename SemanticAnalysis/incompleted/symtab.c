/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

void freeObject(Object *obj);
void freeScope(Scope *scope);
void freeObjectList(ObjectNode *objList);
void freeReferenceList(ObjectNode *objList);

SymTab *symtab;
Type *intType;
Type *charType;

/******************* Type utilities ******************************/

Type *makeIntType(void)
{
  Type *type = (Type *)malloc(sizeof(Type));
  type->typeClass = TP_INT;
  return type;
}

Type *makeCharType(void)
{
  Type *type = (Type *)malloc(sizeof(Type));
  type->typeClass = TP_CHAR;
  return type;
}

Type *makeArrayType(int arraySize, Type *elementType)
{
  Type *type = (Type *)malloc(sizeof(Type));
  type->typeClass = TP_ARRAY;
  type->arraySize = arraySize;
  type->elementType = elementType;
  return type;
}

Type *duplicateType(Type *type)
{
  // TODO
  Type *t = (Type *)malloc(sizeof(Type));
  t->typeClass = type->typeClass;
  if (t->typeClass == TP_ARRAY)
  {
    t->arraySize = type->arraySize;
    t->elementType = duplicateType(type->elementType);
  }

  return t;
}

int compareType(Type *type1, Type *type2)
{
  // TODO
  if (type1->typeClass == type2->typeClass)
  {
    if (type1->typeClass == TP_ARRAY)
    {
      if (type1->arraySize == type2->arraySize)
        return compareType(type1->elementType, type2->elementType);
      else
        return 0;
    }
    else
      return 1;
  }
  else
    return 0;
}

void freeType(Type *type)
{
  // TODO
  if (type->typeClass == TP_ARRAY)
  {
    freeType(type->elementType);
  }
  free(type);
}

/******************* Constant utility ******************************/

ConstantValue *makeIntConstant(int i)
{
  ConstantValue *cv = (ConstantValue *)malloc(sizeof(ConstantValue));
  cv->type = TP_INT;
  cv->intValue = i;

  return cv;
}

ConstantValue *makeCharConstant(char ch)
{
  ConstantValue *cv = (ConstantValue *)malloc(sizeof(ConstantValue));
  cv->type = TP_CHAR;
  cv->charValue = ch;

  return cv;
}

ConstantValue *duplicateConstantValue(ConstantValue *v)
{
  // TODO
  ConstantValue *dup = (ConstantValue *)malloc(sizeof(ConstantValue));
  dup->type = v->type;
  if (dup->type == TP_INT)
  {
    dup->intValue = v->intValue;
  }
  else
    dup->charValue = v->charValue;

  return dup;
}

/******************* Object utilities ******************************/

Scope *createScope(Object *owner, Scope *outer)
{
  Scope *scope = (Scope *)malloc(sizeof(Scope));
  scope->objList = NULL;
  scope->owner = owner;
  scope->outer = outer;
  return scope;
}

Object *createProgramObject(char *programName)
{
  Object *program = (Object *)malloc(sizeof(Object));
  strcpy(program->name, programName);
  program->kind = OBJ_PROGRAM;
  program->progAttrs = (ProgramAttributes *)malloc(sizeof(ProgramAttributes));
  program->progAttrs->scope = createScope(program, NULL);
  symtab->program = program;

  return program;
}

Object *createConstantObject(char *name)
{
  Object *c = (Object *)malloc(sizeof(Object));
  strcpy(c->name, name);
  c->kind = OBJ_CONSTANT;
  c->constAttrs = (ConstantAttributes *)malloc(sizeof(ConstantAttributes));

  return c;
}

Object *createTypeObject(char *name)
{
  Object *t = (Object *)malloc(sizeof(Object));
  strcpy(t->name, name);
  t->kind = OBJ_TYPE;
  t->typeAttrs = (TypeAttributes *)malloc(sizeof(TypeAttributes));

  return t;
}

Object *createVariableObject(char *name)
{
  Object *v = (Object *)malloc(sizeof(Object));
  strcpy(v->name, name);
  v->kind = OBJ_VARIABLE;
  v->varAttrs = (VariableAttributes *)malloc(sizeof(VariableAttributes));

  return v;
}

Object *createFunctionObject(char *name)
{
  Object *f = (Object *)malloc(sizeof(Object));
  strcpy(f->name, name);
  f->kind = OBJ_FUNCTION;
  f->funcAttrs = (FunctionAttributes *)malloc(sizeof(FunctionAttributes));
  f->funcAttrs->scope = createScope(f, symtab->currentScope);
  f->funcAttrs->paramList = NULL;

  return f;
}

Object *createProcedureObject(char *name)
{
  Object *p = (Object *)malloc(sizeof(Object));
  strcpy(p->name, name);
  p->kind = OBJ_PROCEDURE;
  p->procAttrs = (ProcedureAttributes *)malloc(sizeof(ProcedureAttributes));
  p->procAttrs->scope = createScope(p, symtab->currentScope);
  p->procAttrs->paramList = NULL;

  return p;
}

Object *createParameterObject(char *name, enum ParamKind kind, Object *owner)
{
  Object *p = (Object *)malloc(sizeof(Object));
  strcpy(p->name, name);
  p->kind = OBJ_PARAMETER;
  p->paramAttrs = (ParameterAttributes *)malloc(sizeof(ParameterAttributes));
  p->paramAttrs->kind = kind;
  p->paramAttrs->function = owner;

  return p;
}

void freeObject(Object *obj)
{
  // TODO
  switch (obj->kind)
  {
  case OBJ_CONSTANT:
    // printf("OBJ_CONSTANT");
    free(obj->constAttrs->value);
    free(obj->constAttrs);
    break;
  case OBJ_TYPE:
    // printf("OBJ_TYPE");
    freeType(obj->typeAttrs->actualType);
    free(obj->typeAttrs);
    break;
  case OBJ_VARIABLE:
    // printf("OBJ_VARIABLE");
    freeType(obj->varAttrs->type);
    free(obj->varAttrs);
    break;
  case OBJ_FUNCTION:
    // printf("OBJ_FUNCTION");
    freeReferenceList(obj->funcAttrs->paramList);
    freeType(obj->funcAttrs->returnType);
    freeScope(obj->funcAttrs->scope);
    free(obj->funcAttrs);
    break;
  case OBJ_PROCEDURE:
    // printf("OBJ_PROCEDURE");
    freeReferenceList(obj->procAttrs->paramList);
    freeScope(obj->procAttrs->scope);
    free(obj->procAttrs);
    break;
  case OBJ_PROGRAM:
    // printf("OBJ_PROGRAM");
    freeScope(obj->progAttrs->scope);
    free(obj->progAttrs);
    break;
  case OBJ_PARAMETER:
    // printf("OBJ_PARAMETER");
    freeType(obj->paramAttrs->type);
    free(obj->paramAttrs);
  }

  // free(obj);
}

void freeScope(Scope *scope)
{
  // TODO
  freeObjectList(scope->objList);
  free(scope);
}

void freeObjectList(ObjectNode *objList)
{
  // TODO
  ObjectNode *on = objList;
  while (on != NULL)
  {
    ObjectNode *tmp = on;
    freeObject(on->object);
    on = on->next;
    free(tmp);
  }
}

void freeReferenceList(ObjectNode *objList)
{
  // TODO
  ObjectNode *on = objList;
  while (on != NULL)
  {
    ObjectNode *tmp = on;
    freeObject(on->object);
    on = on->next;
    free(tmp);
  }
}

void addObject(ObjectNode **objList, Object *obj)
{
  ObjectNode *node = (ObjectNode *)malloc(sizeof(ObjectNode));
  node->object = obj;
  node->next = NULL;
  if ((*objList) == NULL)
    *objList = node;
  else
  {
    ObjectNode *n = *objList;
    while (n->next != NULL)
      n = n->next;
    n->next = node;
  }
}

Object *findObject(ObjectNode *objList, char *name)
{
  // TODO
  while (objList != NULL)
  {
    Object *tmp = objList->object;
    if (strcmp(tmp->name, name) == 0)
    {
      return tmp;
    }
    else
      objList = objList->next;
  }

  return NULL;
}

/******************* others ******************************/

void initSymTab(void)
{
  Object *obj;
  Object *param;

  symtab = (SymTab *)malloc(sizeof(SymTab));
  symtab->globalObjectList = NULL;

  obj = createFunctionObject("READC");
  obj->funcAttrs->returnType = makeCharType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createFunctionObject("READI");
  obj->funcAttrs->returnType = makeIntType();
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEI");
  param = createParameterObject("i", PARAM_VALUE, obj);
  param->paramAttrs->type = makeIntType();
  addObject(&(obj->procAttrs->paramList), param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITEC");
  param = createParameterObject("ch", PARAM_VALUE, obj);
  param->paramAttrs->type = makeCharType();
  addObject(&(obj->procAttrs->paramList), param);
  addObject(&(symtab->globalObjectList), obj);

  obj = createProcedureObject("WRITELN");
  addObject(&(symtab->globalObjectList), obj);

  intType = makeIntType();
  charType = makeCharType();
}

void cleanSymTab(void)
{
  freeObject(symtab->program);
  freeObjectList(symtab->globalObjectList);
  free(symtab);
  freeType(intType);
  freeType(charType);
}

void enterBlock(Scope *scope)
{
  symtab->currentScope = scope;
}

void exitBlock(void)
{
  symtab->currentScope = symtab->currentScope->outer;
}

void declareObject(Object *obj)
{
  if (obj->kind == OBJ_PARAMETER)
  {
    Object *owner = symtab->currentScope->owner;
    switch (owner->kind)
    {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList), obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList), obj);
      break;
    default:
      break;
    }
  }

  addObject(&(symtab->currentScope->objList), obj);
}
