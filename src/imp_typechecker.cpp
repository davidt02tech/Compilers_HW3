#include "imp_typechecker.hh"

ImpTypeChecker::ImpTypeChecker():inttype(),booltype() {
  inttype.set_basic_type("int");
  booltype.set_basic_type("bool");
}

void ImpTypeChecker::typecheck(Program* p) {
  env.clear();
  p->accept(this);
  return;
}

void ImpTypeChecker::visit(Program* p) {
  p->body->accept(this);
  return;
}

void ImpTypeChecker::visit(Body* b) {
  env.add_level();
  b->var_decs->accept(this);
  b->slist->accept(this);
  env.remove_level();  
  return;
}

void ImpTypeChecker::visit(VarDecList* decs) {
  list<VarDec*>::iterator it;
  for (it = decs->vdlist.begin(); it != decs->vdlist.end(); ++it) {
    (*it)->accept(this);
  }  
  return;
}

void ImpTypeChecker::visit(VarDec* vd) {
  ImpType type;
  type.set_basic_type(vd->type);
  if (type.ttype==ImpType::NOTYPE || type.ttype==ImpType::VOID) {
    cout << "Tipo invalido: " << vd->type << endl;
    exit(0);
  }
  list<string>::iterator it;
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it) {
    env.add_var(*it, type);
  }   
  return;
}


void ImpTypeChecker::visit(StatementList* s) {
  list<Stm*>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
  }
  return;
}

void ImpTypeChecker::visit(AssignStatement* s) {
  ImpType type = s->rhs->accept(this);
  if (!env.check(s->id)) {
    cout << "Variable " << s->id << " undefined" << endl;
    exit(0);
  }
  ImpType var_type = env.lookup(s->id);  
  if (!type.match(var_type)) {
    cout << "Tipo incorrecto en Assign a " << s->id << endl;
    exit(0);
  }
  return;
}

void ImpTypeChecker::visit(PrintStatement* s) {
  s->e->accept(this);
  return;
}

void ImpTypeChecker::visit(IfStatement* s) {
  s->cond->accept(this);
  s->tbody->accept(this);
  if (s->fbody != NULL)
    s->fbody->accept(this);
  return;
}

void ImpTypeChecker::visit(WhileStatement* s) {
  s->cond->accept(this);
  s->body->accept(this);
 return;
}


void ImpTypeChecker::visit(ForStatement* s) {
  ImpType startType = s->startExp->accept(this);
  ImpType endType = s->endExp->accept(this);

  // Verificamos que las expresiones de inicio y fin sean de tipo entero.
  if (!startType.match(inttype)) {
    cout << "Error: La expresión de inicio en el bucle for debe ser de tipo entero." << endl;
    exit(0);
  }
  if (!endType.match(inttype)) {
    cout << "Error: La expresión de fin en el bucle for debe ser de tipo entero." << endl;
    exit(0);
  }
  // Agregamos un level para el scope del for
  env.add_level();

  env.add_var(s->varName, inttype);

  // Visitar el cuerpo del bucle.
  s->body->accept(this);


  env.remove_level();
  return;
}


ImpType ImpTypeChecker::visit(BinaryExp* e) {
  ImpType t1 = e->left->accept(this);
  ImpType t2 = e->right->accept(this);

  // Verifica los tipos para operaciones aritméticas
  if (e->op == PLUS || e->op == MINUS || e->op == MULT || e->op == DIV || e->op == EXP) {
    if (!t1.match(inttype) || !t2.match(inttype)) {
      cout << "Tipos en operación aritmética deben de ser int" << endl;
      exit(0);
    }
    return inttype;
  }
  
  // Verifica los tipos para operaciones de comparación
  if (e->op == LT || e->op == LTEQ || e->op == EQ) {
    if (!t1.match(inttype) || !t2.match(inttype)) {
      cout << "Tipos en operación de comparación deben de ser int" << endl;
      exit(0);
    }
    return booltype;
  }
  
  // Verifica los tipos para operaciones lógicas
  if (e->op == AND || e->op == OR) {
    if (!t1.match(booltype) || !t2.match(booltype)) {
      cout << "Tipos en operación lógica deben de ser bool" << endl;
      exit(0);
    }
    return booltype;
  }
  
  // Si llegamos aquí, entonces hay un operador no soportado en la expresión
  cout << "Operador no soportado en BinExp" << endl;
  exit(0);
}

ImpType ImpTypeChecker::visit(NumberExp* e) {
  return inttype;
}

ImpType ImpTypeChecker::visit(IdExp* e) {
  if (env.check(e->id))
    return env.lookup(e->id);
  else {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
}

ImpType ImpTypeChecker::visit(ParenthExp* ep) {
  return ep->e->accept(this);
}

ImpType ImpTypeChecker::visit(CondExp* e) {
  ImpType booltype;
  booltype.set_basic_type("bool");
  if (!e->cond->accept(this).match(booltype)) {
    cout << "Tipo en ifexp debe de ser bool" << endl;
    exit(0);
  }
  ImpType ttype =  e->etrue->accept(this);  
  if (!ttype.match(e->efalse->accept(this))) {
    cout << "Tipos en ifexp deben de ser iguales" << endl;
    exit(0);
  }
  return ttype;
}

ImpType ImpTypeChecker::visit(BExp* exp) {
  return booltype;
}

