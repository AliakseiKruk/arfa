#include <stdio.h>
#include <stdlib.h>
////////////////////////////////////////////////////////////////////////////////
typedef struct expression{  // Bidirectional list for parsed expression
  int type;
  float value;
  struct expression *prev;
  struct expression *next;
} expr;
/*
types:
0 number
1 (
2 )
3 +
4 -
5 *
6 /
*/
typedef struct expression_array{
  expr *link;
  struct expression_array *next;
} expra;

//------------------------------------------------------------------------------
expr *expr_first,*expr_last;// first and last items in the expression
expra *brackets_first=0,*brackets_last=0;// first and last brackets
int brackets_depth=0;// current depth
expra *operations=0;// ordered list of operators(+-/*)
expra *pm_first=0,*pm_last=0;// additional list for + and -
expra *ud_first=0,*ud_last=0;// additional list for * and /
unsigned short int prev=0;// what was the previous item: operator or operand
  /*
  0 - operator
  1 - operand
  */
////////////////////////////////////////////////////////////////////////////////
char skip_space(char c);
char get_operator(char c);
char get_operand(char c);
void push_brackets(expr *node);
expr *push_expr(int type, float value);
void push_pm(expr *node);
void push_ud(expr *node);

unsigned long int stepen(unsigned int x,unsigned int y);// x^y: i did not want to include math.h for one function
unsigned short int char_to_number(char c);
////////////////////////////////////////////////////////////////////////////////
int main()
{
  char c;
  expr_first=(expr *)malloc(sizeof(expr));
  expr_first->prev=0;
  expr_first->next=0;
  expr_last=expr_first;
  expr_first->type=1;
  push_brackets(expr_first);
  if(!feof(stdin)){
    c=getc(stdin);
  }
  while(!feof(stdin)){
    c=skip_space(c);
    if(feof(stdin)){
      break;
    }
    if(prev){
      c=get_operator(c);
    }
    else{
      c=get_operand(c);
    }
  }
  push_expr(2,0);

  expra *bcurr;
  bcurr=brackets_first;
  operations=(expra *)malloc(sizeof(expra));
  operations->next=0;
  operations->link=brackets_first->link;
  while(1){
    pm_first=0;
    pm_last=0;
    ud_first=0;
    ud_last=0;
    expr *ecurr;
    ecurr=bcurr->link;
    brackets_depth=1;
    while(brackets_depth>0){
      ecurr=ecurr->next;
      if(ecurr->type==1){
        brackets_depth++;
        while(brackets_depth>1){
          ecurr=ecurr->next;
          if(ecurr->type==1){
            brackets_depth++;
          }
          else if(ecurr->type==2){
            brackets_depth--;
          }
        }
      }
      else if(ecurr->type==2){
        brackets_depth--;
      }
      else if(ecurr->type==3 || ecurr->type==4){
        push_pm(ecurr);
      }
      else if(ecurr->type==5 || ecurr->type==6){
        push_ud(ecurr);
      }
    }
    if(pm_first){
      pm_last->next=operations;
    }
    else{
      pm_first=operations;
    }
    if(ud_first){
      ud_last->next=pm_first;
    }
    else{
      ud_first=pm_first;
    }
    operations=ud_first;
    if(bcurr->next){
      expra *done;
      done=bcurr;
      free(done);
      bcurr=bcurr->next;
      expra *bracket;
      bracket=(expra *)malloc(sizeof(expra));
      bracket->link=bcurr->link;
      bracket->next=operations;
      operations=bracket;
    }
    else{
      free(bcurr);
      break;
    }
  }

  while(1){
    unsigned short int replace=0;
    expr *curr;
    curr=operations->link;
    expr *cprev=0;
    expr *cprevo=0;
    expr *cnext=0;
    expr *cnexto=0;
    if(curr->prev){
      cprev=curr->prev;
      if(cprev->prev){
        cprevo=cprev->prev;
      }
    }
    if(curr->next){
      cnext=curr->next;
      if(cnext->next){
        cnexto=cnext->next;
      }
    }
    if(curr->type==1){
      expr *bnexto=cnexto->next;
      if(cnext->type || cnexto->type!=2){
        fprintf(stderr,"Unknown error\n\n");
        exit(1);
      }
      else{
        curr->type=0;
        curr->value=cnext->value;
        if(curr->prev){
          curr->next=bnexto;
          bnexto->prev=curr;
        }
        else{
          curr->next=0;
        }
        free(cnext);
        free(cnexto);
      }
    }
    else if(curr->type==3){
      curr->type=0;
      curr->value=(float)cprev->value+cnext->value;
      replace=1;
    }
    else if(curr->type==4){
      curr->type=0;
      curr->value=(float)cprev->value-cnext->value;
      replace=1;
    }
    else if(curr->type==5){
      curr->type=0;
      curr->value=(float)cprev->value*cnext->value;
      replace=1;
    }
    else if(curr->type==6){
      curr->type=0;
      curr->value=(float)cprev->value/cnext->value;
      replace=1;
    }
    if(replace){
      curr->prev=cprevo;
      cprevo->next=curr;
      curr->next=cnexto;
      cnexto->prev=curr;
      free(cprev);
      free(cnext);
    }
    if(operations->next){
      expra *done;
      done=operations;
      operations=operations->next;
      free(done);
    }
    else{
      free(operations);
      break;
    }
  }

  printf("%f",expr_first->value);
  free(expr_first);
  return (0);
}
////////////////////////////////////////////////////////////////////////////////
char skip_space(char c){
  while(1){
    if(!(c==' ' || c=='\t' || c=='\n' || c=='\r')){
      break;
    }
    if(!feof(stdin)){
      c=getc(stdin);
    }
    else{
      break;
    }
  }
  return c;
}
//------------------------------------------------------------------------------
char get_operator(char c){
  short int type=0;
  if(c=='+'){
    type=3;
  }
  else if(c=='-'){
    type=4;
  }
  else if(c=='*'){
    type=5;
  }
  else if(c=='/'){
    type=6;
  }
  else if(c==')'){
    brackets_depth--;
    if(brackets_depth<0){
      fprintf(stderr,"Right bracker for not existing left bracket was fount\n\n");
      exit(1);
    }
    push_expr(2,0);
    type=2;
    prev=1;
  }
  if(type>2){
    push_expr(type,0);
    prev=0;
  }
  if(!type){
    fprintf(stderr,"symbol'%c' was found where operator expected\n\n",c);
    exit(1);
  }
  if(!feof(stdin)){
    c=getc(stdin);
  }
  else{
    c=0;
  }
  return c;
}
//------------------------------------------------------------------------------
char get_operand(char c){
  if(c=='('){
    push_brackets(push_expr(1,0));
    brackets_depth++;
    prev=0;
    if(!feof(stdin)){
      c=getc(stdin);
    }
    else{
      c=0;
    }
    return c;
  }
  else if(c>='0' && c<='9'){
    unsigned short int number[20];
    unsigned short int length=0;
    while(c>='0' && c<='9'){
      number[length++]=char_to_number(c);
      if(!feof(stdin)){
        c=getc(stdin);
      }
      else{
        c=0;
      }
    }
    float value=0;
    unsigned short int i;
    for(i=0;i<length;i++){
      value+=(float)number[length-1-i]*stepen(10,i);
    }
    push_expr(0,value);
    prev=1;
    return c;
  }
  else{
    fprintf(stderr,"symbol'%c' was found where operand expected\n\n",c);
    exit(1);
  }
}
//------------------------------------------------------------------------------
void push_brackets(expr *node){
  if(brackets_first==0){
    brackets_first=(expra *)malloc(sizeof(expra));
    brackets_first->link=node;
    brackets_first->next=0;
    brackets_last=brackets_first;
  }
  else{
    expra *tmp;
    tmp=(expra *)malloc(sizeof(expra));
    tmp->link=node;
    tmp->next=0;
    brackets_last->next=tmp;
    brackets_last=tmp;
  }
}
//------------------------------------------------------------------------------
expr *push_expr(int type, float value){
  expr *expr_tmp;
  expr_tmp=(expr *)malloc(sizeof(expr));
  expr_tmp->prev=expr_last;
  expr_tmp->next=0;
  expr_tmp->type=type;
  expr_tmp->value=value;
  expr_last->next=expr_tmp;
  expr_last=expr_tmp;
  return expr_last;
}
//------------------------------------------------------------------------------
void push_pm(expr *node){
  if(pm_first==0){
    pm_first=(expra *)malloc(sizeof(expra));
    pm_first->link=node;
    pm_first->next=0;
    pm_last=pm_first;
  }
  else{
    expra *tmp;
    tmp=(expra *)malloc(sizeof(expra));
    tmp->link=node;
    tmp->next=0;
    pm_last->next=tmp;
    pm_last=tmp;
  }
}
//------------------------------------------------------------------------------
void push_ud(expr *node){
  if(ud_first==0){
    ud_first=(expra *)malloc(sizeof(expra));
    ud_first->link=node;
    ud_first->next=0;
    ud_last=ud_first;
  }
  else{
    expra *tmp;
    tmp=(expra *)malloc(sizeof(expra));
    tmp->link=node;
    tmp->next=0;
    ud_last->next=tmp;
    ud_last=tmp;
  }
}
///////////////////////////////////////////////////////////////////////////////
unsigned long int stepen(unsigned int x,unsigned int y){//x^y
  unsigned long int value=1;
  unsigned short int i;
  for(i=0;i<y;i++){
    value*=x;
  }
  return value;
}
//------------------------------------------------------------------------------
unsigned short int char_to_number(char c){
  switch(c){// to exclude ASCII dependency
    case '1':return 1;break;
    case '2':return 2;break;
    case '3':return 3;break;
    case '4':return 4;break;
    case '5':return 5;break;
    case '6':return 6;break;
    case '7':return 7;break;
    case '8':return 8;break;
    case '9':return 9;break;
    default: return 0; break;
  };
}
//------------------------------------------------------------------------------
