#include "imp_interpreter.hh"


int ImpInterpreter::interpret(Program* p) {
  env.clear();
  p->accept(this);
  return 0;
}

int ImpInterpreter::visit(Program* p) {
  p->body->accept(this);
  return 0;
}

int ImpInterpreter::visit(Body* b) {
  env.add_level(); 
  b->var_decs->accept(this);
  b->slist->accept(this);
  env.remove_level();  
  return 0;
}


int ImpInterpreter::visit(VarDecList* decs) {
  list<VarDec*>::iterator it;
  for (it = decs->vdlist.begin(); it != decs->vdlist.end(); ++it) {
    (*it)->accept(this);
  }  
  return 0;
}

int ImpInterpreter::visit(VarDec* vd) {
  list<string>::iterator it;
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it) {
    env.add_var(*it);
  }   
  return 0;
}

int ImpInterpreter::visit(StatementList* s) {
  list<Stm*>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
  }
  return 0;
}

int ImpInterpreter::visit(AssignStatement* s) {
  int v = s->rhs->accept(this);
  if (!env.check(s->id)) {
    cout << "Variable " << s->id << " undefined" << endl;
    exit(0);
  }
  env.update(s->id, v);  
  //memoria_update(s->id, v);
  
  return 0;
}

int ImpInterpreter::visit(PrintStatement* s) {
  int v = s->e->accept(this);
  cout << v << endl;
  return 0;
}

int ImpInterpreter::visit(IfStatement* s) {
  if (s->cond->accept(this)) {
    s->tbody->accept(this);
  } else {
    if (s->fbody != NULL)
      s->fbody->accept(this);
  }
  return 0;
}

int ImpInterpreter::visit(WhileStatement* s) {
 while (s->cond->accept(this)) {
    s->body->accept(this);
  }
 return 0;
}

int ImpInterpreter::visit(ForStatement* s) {
  int start = s->startExp->accept(this);
  int end = s->endExp->accept(this);


  env.add_level();

  // Determinamos la dirección de la iteración y ejecutar el bucle for adecuadamente.
  if (start <= end) {
    // Caso creciente
    for (int i = start; i <= end; i++) {

      env.add_var(s->varName, i);

      s->body->accept(this);

      // Actualizar 'i' en cada iteración en caso de que se haya modificado en el cuerpo.
      env.lookup(s->varName, i);
    }
  } else {
    // Caso decreciente
    for (int i = start; i >= end; i--) {

      env.add_var(s->varName, i);


      s->body->accept(this);

      // Actualizar 'i' en cada iteración en caso de que se haya modificado en el cuerpo.
      env.lookup(s->varName, i);
    }
  }

  env.remove_level();
  return 0;
}


int ImpInterpreter::visit(BinaryExp* e) {
  int v1 = e->left->accept(this);
  int v2 = e->right->accept(this);
  int result = 0;
  switch(e->op) {
  case PLUS: result = v1+v2; break;
  case MINUS: result = v1-v2; break;
  case MULT: result = v1 * v2; break;
  case DIV: result = v1 / v2; break;
  case EXP:
    result = 1;
    while (v2 > 0) { result *= v1; v2--; }
    break;
  case LT: result = (v1 < v2) ? 1 : 0; break;
  case LTEQ: result = (v1 <= v2) ? 1: 0; break;
  case EQ: result = (v1 == v2) ? 1 : 0; break;
  case AND: result = (v1 and v2)? 1 : 0; break;
  case OR: result = (v1 or v2)? 1 : 0; break;
  }
  return result;
}

int ImpInterpreter::visit(NumberExp* e) {
  return e->value;
}

int ImpInterpreter::visit(IdExp* e) {
  if (env.check(e->id))
    return env.lookup(e->id);
  else {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
  return 0;
  /*
  if (memoria_check(e->id))
    return memoria_lookup(e->id);
  else {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
  */
}

int ImpInterpreter::visit(ParenthExp* ep) {
  return ep->e->accept(this);
}

int ImpInterpreter::visit(CondExp* e) {
  if (e->cond->accept(this) == 0)
    return e->efalse->accept(this);
  else
    return e->etrue->accept(this);
}


bool ImpInterpreter::visit(BExp* e) {
  // if (e->cond->accept(this) == 0)
  //   return e->efalse->accept(this);
  // else
  //   return e->etrue->accept(this);
  return e->value == true? 1:0;
}
