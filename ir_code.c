#include "ir_code.h"

extern char buffer[BUFFSIZE];
extern int tempno;
extern int labelno;
extern int generate(nodeType *n);
extern FILE* output;
extern int queue_length;	//the number of case statements minus one
extern int idno;

// global variables
char mybuf[100];
int main_found=0;
int main_was_found=0;
int seen_bool_flow=0;
int in_assign=0;
int in_func=0;
int prepost_put = 0;
nodeType* root;
nodeType* expr_queue[MAXQUEUE];
char label_queue[MAXQUEUE][16];
char switch_label[16];
char break_label[16];
char curr_class_name[64];
int loop_flag = 0;
int switch_flag = 0;


void ir_fieldlist(nodeType* n)
{
	if(n->opr.oper==EMPTY)		// base case 1
		return;
	nodeType* lc=get_operand(n,0);
	nodeType* rc=get_operand(n,1);
	nodeType* s;
	if(n->opr.oper==FIELD_ARG)		// base case when list degenerates to single arg
	{
		s=get_operand(n,0);
		printf(" .field public ");
		fprintf(output," .field public ");
		print_type(get_operand(n,1));
		printf(" %s ",s->id.symrec->sym_name); 	// get the uid and print it
		fprintf(output," %s ",s->id.symrec->sym_name); 	// get the uid and print it
		return;
	}
	else 						// else recurse on the formal arg list of lc 
	{
		ir_fieldlist(lc);
		printf("\n");
		fprintf(output,"\n");
	}	
	ir_fieldlist(rc);		// finally print the right child (actually just a formal arg)
	return;
}
// Working correctly
// generates code for class decln
void ir_class_decln(nodeType* n)
{
	nodeType* mod=get_operand(n,0);
	nodeType* class_name=get_operand(n,1);
	nodeType* field_list=get_operand(n,2);
	nodeType* body=get_operand(n,3);
	strcpy(curr_class_name,class_name->id.symrec->sym_name);
	printf("%s IS CURR CLASS\n",curr_class_name);
	printf(".class ");
	fprintf(output,".class ");
	switch(mod->con_i.value)		// print modifer info
	{
		case modPUBLIC:
			printf("public "); 
			fprintf(output,"public "); 
			break;
		case modPRIVATE:
			printf("private "); 
			fprintf(output,"private "); 
			break;
		case modPROTECTED:
			printf("protected "); 
			fprintf(output,"protected "); 
			break;				
	}
	// rest is basically the default class constructor which inherits the object class
	printf("auto ansi beforefieldinit "); 
	fprintf(output,"auto ansi beforefieldinit "); 
	printf("%s ",class_name->id.symrec->sym_name);
	fprintf(output,"%s ",class_name->id.symrec->sym_name);
	printf("extends [mscorlib]System.Object\n");
	fprintf(output,"extends [mscorlib]System.Object\n");
	printf("{\n");
	fprintf(output,"{\n");
	
	ir_fieldlist(field_list);		// .field public type fieldname \n .field public type fieldname
	printf("\n");
	fprintf(output,"\n");
	
	nodeType* p=body;
	nodeType* lc;
	while(p->opr.oper!=FUNC)
	{
		lc=get_operand(p,0);
		printf("%d is p's oper\n",p->opr.oper);
		p=lc;
	}
	printf("%d is p's oper\n",p->opr.oper);
	printf("%s is name\n",get_operand(p,0)->id.symrec->sym_name);
	
	if(strcmp(curr_class_name,get_operand(p,0)->id.symrec->sym_name)==0)
	{
		printf("YES MAN\n");
		strcpy(class_name->id.symrec->signature,get_operand(p,0)->id.symrec->signature);
		printf("CLASS SIGNATURE %s \n",class_name->id.symrec->signature);
		ir_constructor(p);
	}
	else
	{
		default_constructor();
	}
	
	generate(body);
	
	printf("}\n");
	fprintf(output,"}\n");
}
// expects a fundef node
void ir_constructor(nodeType* n)
{
	nodeType* fun_name = get_operand(n,0);	
	nodeType* formal_arguments = get_operand(n,1);
	nodeType* return_type =get_operand(n,2);
	nodeType* compound =get_operand(n,3);
	printf("GIGGITY %s\n",fun_name->id.symrec->signature);
	
	printf(".method public hidebysig specialname rtspecialname instance default void '.ctor' (");
	fprintf(output,".method public hidebysig specialname rtspecialname instance default void '.ctor' (");
	print_formal_args(formal_arguments);
	fprintf(output,")  cil managed \n");
   
    printf("{\n");
    fprintf(output,"{\n");
	
	printf("\t.maxstack %d\n",MAXSTACK_SIZE);
	fprintf(output,"\t.maxstack %d\n",MAXSTACK_SIZE);
	printf("\tldarg.0\n");
	fprintf(output,"\tldarg.0\n");
	printf("\tcall instance void object::'.ctor'()\n");
	fprintf(output,"\tcall instance void object::'.ctor'()\n");
	printf("\tldarg.0\n");
	fprintf(output,"\tldarg.0\n");
	
	generate(compound);
	printf("\tret\n"); 
	fprintf(output,"\tret\n"); 
    printf("}\n");
    fprintf(output,"}\n");
}
// prints default constructor
void default_constructor()
{
	printf(".method public hidebysig specialname rtspecialname instance default void '.ctor' ()  cil managed \n");
    fprintf(output,".method public hidebysig specialname rtspecialname instance default void '.ctor' ()  cil managed \n");
    printf("{\n");
    fprintf(output,"{\n");
	printf("\t.maxstack 8\n");
	fprintf(output,"\t.maxstack 8\n");
	printf("\tldarg.0\n");
	fprintf(output,"\tldarg.0\n");
	printf("\tcall instance void object::'.ctor'()\n");
	fprintf(output,"\tcall instance void object::'.ctor'()\n");
	printf("\tret\n"); 
	fprintf(output,"\tret\n"); 
    printf("}\n");
    fprintf(output,"}\n");
}

// Working correctly
void ir_class_decln_list(nodeType* n)
{
	nodeType* class_decln_list = get_operand(n,0);
	nodeType* class_decln = get_operand(n,1);
	generate(class_decln_list);
	generate(class_decln);
}

// Working correctly
void ir_fun_def_list(nodeType* n)
{
	nodeType* fun_def_list = get_operand(n,0);
	nodeType* fun_def = get_operand(n,1);
	generate(fun_def_list);
	generate(fun_def);
}

void ir_compound_stmt(nodeType* n)
{
	
	printf("{\n");
	fprintf(output,"{\n");
	
	if(main_found==1)		// main flag set so this is entrypoint
	{
		printf(" .entrypoint\n");
		fprintf(output," .entrypoint\n");
		main_found=0;
	}
	if(in_func==1)		// in every function need to set max stack 
	{
		printf(".maxstack %d\n",MAXSTACK_SIZE);		// GIGLAMESH use a define here
		fprintf(output,".maxstack %d\n",MAXSTACK_SIZE);
		in_func=0;
	}
	nodeType* stmtlist = get_operand(n,0);
	memset(stmtlist->opr.next,0,16);
	strcat(stmtlist->opr.next,newlabel());		// attach a new label to stmt next
	generate(stmtlist);
	
	printf("%s:\n",stmtlist->opr.next);
	fprintf(output,"%s:\n",stmtlist->opr.next);

	printf("}\n");
	fprintf(output,"}\n");
}

// generates code for assign expression
// is responsible to perform operations like stloc 
void ir_assign(nodeType* n)
{
	nodeType* unary_exp = get_operand(n,0);
	nodeType* assop = get_operand(n,1);
	nodeType* ass_exp = get_operand(n,2);
	
	printf("Unary_exp TYPE IS %d\n",unary_exp->type);
	printf("assop TYPE IS %d\n",assop->type);
	printf("ass_Exp TYPE IS %d\n",ass_exp->type);
	
	switch(assop->con_i.value)
	{
		case EQ:
			printf("OOOOOOLAALALALALA!!\n");
			if(unary_exp->opr.oper==ARRAY_INVOC)		// array invoc use arr stelem
			{	
				ir_array_lhs(unary_exp);
				generate(ass_exp);
				printf("stelem.i4\n");
				fprintf(output,"stelem.i4\n");
			}
			else
			{		
				generate(ass_exp);
				print_store_var(unary_exp);		// use store_var function to decide instruction
			}	
			break;
		case PLUS_EQ:
			if(unary_exp->opr.oper==ARRAY_INVOC)
			{	
				ir_array_lhs(unary_exp);
				ir_array_rhs(unary_exp);
				generate(ass_exp);
				printf("add\n");
				fprintf(output,"add\n");
				printf("stelem.i4\n");
				fprintf(output,"stelem.i4\n");
			}
			else
			{			
				
				print_load_var(unary_exp);		// first load to perform add
				generate(ass_exp);
				printf("add\n");
				fprintf(output,"add\n");
				print_store_var(unary_exp);		// then store value back
			}	
			break;
		case MINUS_EQ:
			
			if(unary_exp->opr.oper==ARRAY_INVOC)
			{	
				ir_array_lhs(unary_exp);
				ir_array_rhs(unary_exp);
				generate(ass_exp);
				printf("sub\n");
				fprintf(output,"sub\n");
				printf("stelem.i4\n");
				fprintf(output,"stelem.i4\n");
			}
			else
			{			
				print_load_var(unary_exp);
				generate(ass_exp);
				printf("sub\n");
				fprintf(output,"sub\n");
				print_store_var(unary_exp);
			}	
			break;
		case MULT_EQ:
			if(unary_exp->opr.oper==ARRAY_INVOC)
			{	
				ir_array_lhs(unary_exp);
				ir_array_rhs(unary_exp);
				generate(ass_exp);
				printf("mul\n");
				fprintf(output,"mul\n");
				printf("stelem.i4\n");
				fprintf(output,"stelem.i4\n");
			}
			else
			{			
				
				print_load_var(unary_exp);
				generate(ass_exp);
				printf("mul\n");
				fprintf(output,"mul\n");
				print_store_var(unary_exp);
			}	
			break;

		case DIV_EQ:
			if(unary_exp->opr.oper==ARRAY_INVOC)
			{	
				ir_array_lhs(unary_exp);
				ir_array_rhs(unary_exp);
				generate(ass_exp);
				printf("div\n");
				fprintf(output,"div\n");
				printf("stelem.i4\n");
				fprintf(output,"stelem.i4\n");
			}
			else
			{			
				
				print_load_var(unary_exp);
				generate(ass_exp);
				printf("div\n");
				fprintf(output,"div\n");
				print_store_var(unary_exp);
			}	
			break;
		default: printf("ASSOP DEFAULT\n");
	}
}

// generates code for arithmetic operators like
// plus minus etc and lsh rsh bitwise operations like xor bit and etc
// GIGLAMESH			
void ir_arithmetic(nodeType* n)
{
	nodeType* E1 = get_operand(n,0);
	nodeType* E2 = get_operand(n,1);
	generate(E1);
	generate(E2);
	switch(n->opr.oper)
	{
		case PLUS:
			printf("add\n");
			fprintf(output,"add\n");
			break;
		case MINUS:
			printf("sub\n");
			fprintf(output,"sub\n");
			break;
		case MULT:
			printf("mul\n");
			fprintf(output,"mul\n");
			break;
		case DIV:
			printf("div\n");
			fprintf(output,"div\n");
			break;
		case MODULO:
			printf("rem\n");
			fprintf(output,"rem\n");
			break;
	
		case BIT_AND:
			printf("and\n");
			fprintf(output,"and\n");
			break;
		case BIT_OR:
			printf("or\n");
			fprintf(output,"or\n");
			break;
		case XOR:
			printf("xor\n");
			fprintf(output,"xor\n");
			break;
		case LSH:
			printf("ldc.i4.s 0x1f\n");
			fprintf(output,"ldc.i4.s 0x1f\n");
			printf("and\n");
			fprintf(output,"and\n");
			printf("shl\n");
			fprintf(output,"shl\n");
			break;
		case RSH:
			printf("ldc.i4.s 0x1f\n");
			fprintf(output,"ldc.i4.s 0x1f\n");
			printf("and\n");
			fprintf(output,"and\n");
			printf("shr\n");
			fprintf(output,"shr\n");
			break;
		default:
			printf("arithmetic default\n");
			fprintf(output,"arithmetic default\n");
	}
}

// takes a node as argument and prints appropriate machine instruction to identify it
// eg will print int32 for integer node send as argument
void print_type(nodeType* n)
{
	if(n->type==typeId)
	{
		switch(n->id.symrec->type)
		{
			case MY_INT:
				printf("int32 ");
				fprintf(output,"int32 ");
				break;
			case MY_FLOAT:	
				printf("float32 ");
				fprintf(output,"float32 ");
				break;
			case MY_CHAR:
				printf("char ");
				fprintf(output,"char ");
				break;
			case MY_BOOL:
				printf("bool ");
				fprintf(output,"bool ");
				break;
			case MY_VOID:
				printf("void ");
				fprintf(output,"void ");
				break;
			default:
				printf("IN DEFAULT OF PRINT TYPE\n");		// GIGLAMESH
		}

	}
	else
	{
		switch(n->con_i.value)
		{
			case MY_INT:
				printf("int32 ");
				fprintf(output,"int32 ");
				break;
			case MY_FLOAT:	
				printf("float32 ");
				fprintf(output,"float32 ");
				break;
			case MY_CHAR:
				printf("char ");
				fprintf(output,"char ");
				break;
			case MY_BOOL:
				printf("bool ");
				fprintf(output,"bool ");
				break;
			case MY_VOID:
				printf("void ");
				fprintf(output,"void ");
				break;
			default:
				printf("IN DEFAULT OF PRINT TYPE\n");		// GIGLAMESH
		}
	}
	return;
}

// recursive function which prints the formal args given a node of formal arg list
void print_formal_args(nodeType* n)
{
	if(n->opr.oper==EMPTY)		// base case 1
		return;
	nodeType* lc=get_operand(n,0);
	nodeType* rc=get_operand(n,1);
	nodeType* s;
	if(n->opr.oper==FORMAL_ARG)		// base case when list degenerates to single arg
	{
		print_type(get_operand(n,1));
		s=get_operand(n,0);
		printf(" %s ",s->id.symrec->uid); 	// get the uid and print it
		fprintf(output," %s ",s->id.symrec->uid); 
		return;
	}
	else 						// else recurse on the formal arg list of lc 
	{
		print_formal_args(lc);
		printf(",");
		fprintf(output,",");
	}	
	print_formal_args(rc);		// finally print the right child (actually just a formal arg)
	return;
}

void create_formal_args(nodeType* n)
{
	if(n->opr.oper==EMPTY)
		return;
	nodeType* lc=get_operand(n,0);
	nodeType* rc=get_operand(n,1);
	nodeType* s;
	if(n->opr.oper==FORMAL_ARG)
	{
		s=get_operand(n,1);
		switch(s->con_i.value)
		{
				case MY_INT: 
							strcat(mybuf," int32 ");
							break;
				case MY_FLOAT: 
							strcat(mybuf," float32 "); 
							break;
				case MY_BOOL:
							strcat(mybuf," bool ");
							break;
				default:	
							strcat(mybuf," unhandled type ");
							break;					
		}
		return;
	}
	else
	{
		create_formal_args(lc);
		strcat(mybuf,",");
	}	
	create_formal_args(rc);
	return;
}
void insert_signature(nodeType* fun_name,nodeType* formalarg,nodeType* return_type)
{
	printf("DOING SIGNATURE\n");
	memset(fun_name->id.symrec->signature,0,100);
	switch(return_type->con_i.value)
	{
		case MY_INT:
					strcat(fun_name->id.symrec->signature,"int32 ");
					break;
		case MY_FLOAT:
					strcat(fun_name->id.symrec->signature,"float32 ");
					break;			
		case MY_VOID:
					strcat(fun_name->id.symrec->signature,"void ");
					break;			
	}
	strcat(fun_name->id.symrec->signature,"class ");
	strcat(fun_name->id.symrec->signature,fun_name->id.symrec->my_st->parent->owner_name);
	strcat(fun_name->id.symrec->signature,"::");
	if(strcmp(fun_name->id.symrec->my_st->parent->owner_name,fun_name->id.symrec->sym_name)==0)
	{
		strcat(fun_name->id.symrec->signature,"'.ctor'");
	}
	else
	{
		strcat(fun_name->id.symrec->signature,fun_name->id.symrec->sym_name);
	}	
	strcat(fun_name->id.symrec->signature," ( ");
	
	memset(mybuf,0,100);
	create_formal_args(formalarg);
	
	strcat(fun_name->id.symrec->signature,mybuf);
	memset(mybuf,0,100);
	strcat(fun_name->id.symrec->signature," ) ");
	printf("FINAL SIGNATURE:%s\n",fun_name->id.symrec->signature);
}

void ir_fun_def(nodeType* n)
{
	nodeType* fun_name = get_operand(n,0);	
	nodeType* formal_arguments = get_operand(n,1);
	nodeType* return_type =get_operand(n,2);
	nodeType* compound =get_operand(n,3);

	if(strcmp(curr_class_name,fun_name->id.symrec->sym_name)==0)
	{
		return;
	}

	// only static for time being
	printf(".method static ");
	fprintf(output,".method static ");
	
	print_type(return_type);
	printf("%s",fun_name->id.symrec->sym_name);
	fprintf(output,"%s",fun_name->id.symrec->sym_name);
	if(strcmp(fun_name->id.symrec->sym_name,"main")==0)
	{
		main_found=1;
		main_was_found=1;
	}	
	printf("(");
	fprintf(output,"(");
	print_formal_args(formal_arguments);
	printf(") cil managed\n");
	fprintf(output,") cil managed\n");
	in_func=1;
	generate(compound);
}

void print_vardec_code(nodeType* Idlist,nodeType* Type)
{
	if(Idlist->type==typeId)
	{
		print_type(Type);
		printf(" %s ",Idlist->id.symrec->uid); 
		fprintf(output," %s ",Idlist->id.symrec->uid); 
		return;
	}
	else
	{
		nodeType* lc=get_operand(Idlist,0);
		nodeType* rc=get_operand(Idlist,1);
		print_vardec_code(lc,Type);
		printf(",");
		fprintf(output,",");
		print_type(Type);
		printf(" %s ",rc->id.symrec->uid); 
		fprintf(output," %s ",rc->id.symrec->uid); 
	}	
	return;
}

void ir_var_dec(nodeType* n)
{
	nodeType* Idlist = get_operand(n,0);
	nodeType* Type = get_operand(n,1);
	printf(".locals init(");
	fprintf(output,".locals init(");
	print_vardec_code(Idlist,Type);
	printf(")\n");
	fprintf(output,")\n");
}

// NEED TO PUT FLAG FOR CONTROL FLOW AND EXPRESSION 
void ir_relop(nodeType* n)
{
	nodeType* E1 = get_operand(n,0);
	nodeType* E2 = get_operand(n,1);
	generate(E1);
	generate(E2);
	switch(n->opr.oper)
	{
		case LT:
			printf("clt\n");
			fprintf(output,"clt\n");
			break;
		case GT:
			printf("cgt\n");
			fprintf(output,"cgt\n");
			break;
		case LE:
			printf("cgt\n");
			fprintf(output,"cgt\n");
			printf("ldc.i4.0\n");
			fprintf(output,"ldc.i4.0\n");
			printf("ceq\n");
			fprintf(output,"ceq\n");
			break;
		case GE:
			printf("clt\n");
			fprintf(output,"clt\n");
			printf("ldc.i4.0\n");
			fprintf(output,"ldc.i4.0\n");
			printf("ceq\n");
			fprintf(output,"ceq\n");
			break;
		default:
			printf("Relational default\n");
	}
}

void ir_bool(nodeType* n)
{
	nodeType* B1 = get_operand(n,0);
	nodeType* B2 = get_operand(n,1);
	char* label1;
	char* label2;
	switch(n->opr.oper)
	{
		case NEQ:
			generate(B1);
			generate(B2);
			printf("ceq\n");
			fprintf(output,"ceq\n");
			printf("ldc.i4.0\n");
			fprintf(output,"ldc.i4.0\n");
			printf("ceq\n");
			fprintf(output,"ceq\n");
			break;
		case BOOL_EQ:
			generate(B1);
			generate(B2);
			printf("ceq\n");
			fprintf(output,"ceq\n");
			break;
		case BOOL_OR:
			generate(B1);
			label1 = strdup(newlabel());
			printf("brtrue %s\n",label1);
			fprintf(output,"brtrue %s\n",label1);
			generate(B2);
			label2 = strdup(newlabel());
			printf("br.s %s\n",label2);
			fprintf(output,"br.s %s\n",label2);
			printf("%s: ldc.i4.1 \n",label1);
			fprintf(output,"%s: ldc.i4.1 \n",label1);
			printf("%s: ",label2);
			fprintf(output,"%s: ",label2);
			break;
		case BOOL_AND:
			generate(B1);
			label1 = strdup(newlabel());
			printf("brfalse %s\n",label1);
			fprintf(output,"brfalse %s\n",label1);
			generate(B2);
			label2 = strdup(newlabel());
			printf("br.s %s\n",label2);
			fprintf(output,"br.s %s\n",label2);
			printf("%s: ldc.i4.0\n",label1);
			fprintf(output,"%s: ldc.i4.0\n",label1);
			printf("%s: ",label2);
			fprintf(output,"%s: ",label2);
			break;
		default: printf("Bool DEFAULT\n");
	}
}

void ir_explist(nodeType* n)
{
	nodeType* exp=get_operand(n,0);
	nodeType* assexp=get_operand(n,1);
	generate(exp);
	generate(assexp);
}

void ir_fun_invoc(nodeType* n)
{
	nodeType* func_name = get_operand(n,0);
	nodeType* explist = get_operand(n,1);
	if((strcmp(func_name->id.symrec->sym_name,"println")!=0) && (strcmp(func_name->id.symrec->sym_name,"print")!=0))
	{
		generate(explist);
		printf("call ");
		fprintf(output,"call ");
		printf("%s ",func_name->id.symrec->signature);
		fprintf(output,"%s \n",func_name->id.symrec->signature);
	}
	else if(strcmp(func_name->id.symrec->sym_name,"println")==0)  		// println function called
	{
		generate(explist);
		printf("call void [mscorlib]System.Console::WriteLine");
		printf("(");
		printf("int32");
		printf(")\n");

		fprintf(output,"call void [mscorlib]System.Console::WriteLine");
		fprintf(output,"(");
		fprintf(output,"int32");
		fprintf(output,")\n");
	}
	else if(strcmp(func_name->id.symrec->sym_name,"print")==0) 		// default print function called
	{
		generate(explist);
		printf("call void [mscorlib]System.Console::Write");
		printf("(");
		printf("int32");
		printf(")\n");

		fprintf(output,"call void [mscorlib]System.Console::Write");
		fprintf(output,"(");
		fprintf(output,"int32");
		fprintf(output,")\n");
	}	
}

void ir_return(nodeType* n)
{
	if(n->opr.nops != 0)
	{
		nodeType* return_exp = get_operand(n,0);
		generate(return_exp);
	}
	printf("ret \n");
	fprintf(output,"ret \n");
}
////////////////////////////////////////////////////////////////////////
				
void ir_relop_flow(nodeType* n)
{
	nodeType* B1 = get_operand(n,0);
	nodeType* B2 = get_operand(n,1);
	generate(B1);
	generate(B2);
	switch(n->opr.oper)
	{
		case LT:
			printf("MATCHED LT in ir_relop_flow\n");
			printf("blt %s\n",get_T(n));
			fprintf(output,"blt %s\n",get_T(n));
			printf("br %s\n",get_F(n));
			fprintf(output,"br %s\n",get_F(n));
			break;
		case GT:
			printf("MATCHED GT in ir_relop_flow\n");
			printf("bgt %s\n",get_T(n));
			fprintf(output,"bgt %s\n",get_T(n));
			printf("br %s\n",get_F(n));
			fprintf(output,"br %s\n",get_F(n));
			break;
		case LE:
			printf("MATCHED LE in ir_relop_flow\n");
			printf("ble %s\n",get_T(n));
			fprintf(output,"ble %s\n",get_T(n));
			printf("br %s\n",get_F(n));
			fprintf(output,"br %s\n",get_F(n));
			break;
		case GE:
			printf("MATCHED GE in ir_relop_flow\n");
			printf("bge %s\n",get_T(n));
			fprintf(output,"bge %s\n",get_T(n));
			printf("br %s\n",get_F(n));
			fprintf(output,"br %s\n",get_F(n));
			break;
		default:
			printf("Relational Flow default\n");
	}
}
void ir_if(nodeType* n)
{
	nodeType* expr = get_operand(n,0);
	nodeType* stmt = get_operand(n,1);
	set_T(expr,newlabel());
	set_F(expr,n->opr.next);
	//~ stmt->opr.next=strdup(n->opr.next);
	memset(stmt->opr.next,0,16);
	strcat(stmt->opr.next,n->opr.next);
	printf("expr true label:%s\n",get_T(expr));
	printf("expr false label:%s\n",get_F(expr));
	seen_bool_flow = 1;
	generate(expr);
	seen_bool_flow = 0;
	printf("%s:\n",get_T(expr));
	fprintf(output,"%s:\n",get_T(expr));
	generate(stmt);
	return;
}	
void ir_if_else(nodeType* n)
{
	nodeType* expr = get_operand(n,0);
	nodeType* stmt1 = get_operand(n,1);
	nodeType* stmt2 = get_operand(n,2);
	set_T(expr,newlabel());
	set_F(expr,newlabel());
	
	memset(stmt1->opr.next,0,16);
	memset(stmt2->opr.next,0,16);
	strcpy(stmt1->opr.next,n->opr.next);
	strcpy(stmt2->opr.next,n->opr.next);
	
	seen_bool_flow = 1;
	generate(expr);
	seen_bool_flow = 0;
	
	printf("%s:\n",get_T(expr));
	fprintf(output,"%s:\n",get_T(expr));
	generate(stmt1);
	printf("br.s %s\n ",n->opr.next);
	fprintf(output,"br.s %s\n",n->opr.next);
	printf("%s:\n",get_F(expr));
	fprintf(output,"%s:\n",get_F(expr));
	generate(stmt2);
	
}	
void ir_while(nodeType* n)
{
	nodeType* expr = get_operand(n,0);
	nodeType* stmt = get_operand(n,1);
	char begin[16];
	memset(begin,0,16);
	strcat(begin,newlabel());
	set_T(expr,newlabel());
	set_F(expr,n->opr.next);
	memset(stmt->opr.next,0,16);
	strcpy(stmt->opr.next,begin);
	printf("%s:\n",begin);
	fprintf(output,"%s:\n",begin);
	seen_bool_flow = 1;
	generate(expr);
	seen_bool_flow = 0;
	printf("%s:\n",get_T(expr));
	fprintf(output,"%s:\n",get_T(expr));

	//##for break statement##
	char initial_break_label[16];
	memset(initial_break_label,0,16);
	strcat(initial_break_label,break_label);
	memset(break_label,0,16);
	loop_flag = loop_flag + 1;
	strcat(break_label,n->opr.next);
	//####################
	
	generate(stmt);
	printf("br.s %s\n ",begin);
	fprintf(output,"br.s %s\n",begin);

	//for break statement
	loop_flag = loop_flag - 1;
	memset(break_label,0,16);
	strcat(break_label,initial_break_label);
}	
	
void ir_bool_flow(nodeType* n)
{
	nodeType* B1 = get_operand(n,0);
	nodeType* B2 = get_operand(n,1);
	printf("n true label:%s\n",get_T(n));
	printf("n false label:%s\n",get_F(n));
	switch(n->opr.oper)
	{
	case BOOL_OR:
		printf("MATCHED BOOL_OR in ir_bool_flow\n");
		set_T(B1,n->opr.T);
		set_F(B1,newlabel());
		set_T(B2,n->opr.T);
		set_F(B2,n->opr.F);
		generate(B1);
		printf("%s:",get_F(B1)); 
		fprintf(output,"%s:",get_F(B1)); 
		generate(B2);
		break;
	case BOOL_EQ:
		generate(B1);
		generate(B2);
		printf("MATCHED BOOL_EQ in ir_relop_flow\n");
		printf("beq %s\n",get_T(n));
		fprintf(output,"beq %s\n",get_T(n));
		printf("br %s\n",get_F(n));
		fprintf(output,"br %s\n",get_F(n));
		break;
	case NEQ:
		generate(B1);
		generate(B2);
		printf("MATCHED NEQ in ir_relop_flow\n");
		printf("bne.un %s\n",get_T(n));
		fprintf(output,"bne.un %s\n",get_T(n));
		printf("br %s\n",get_F(n));
		fprintf(output,"br %s\n",get_F(n));
		break;
	case BOOL_AND:
		set_T(B1,newlabel());
		set_F(B1, get_F(n));
		set_T(B2, get_T(n));
		set_F(B2, get_F(n));
		generate(B1);
		printf("%s:",get_T(B1)); 
		fprintf(output,"%s:",get_T(B1)); 
		generate(B2);
		break;
	default: printf("Bool DEFAULT\n");
	}
	
}

void ir_idlist(nodeType* n)
{
	nodeType* Idlist = get_operand(n,0);
	nodeType* ident = get_operand(n,1);
	generate(Idlist);
}	

void ir_stmtlist(nodeType* n)
{
	nodeType* Stmtlist = get_operand(n,0);
	nodeType* Stmt = get_operand(n,1);
	memset(Stmtlist->opr.next,0,16);
	strcat(Stmtlist->opr.next,newlabel());
	printf("Stmtlist next:%s \n",Stmtlist->opr.next);
	printf("Stmtlist type:%d \n",Stmtlist->type);
	printf("Stmtlist nops:%d \n",Stmtlist->opr.nops);
	generate(Stmtlist);
	memset(Stmt->opr.next,0,16);
	strcat(Stmt->opr.next,n->opr.next);	
	printf("%s: \n",Stmtlist->opr.next);
	fprintf(output,"%s: \n",Stmtlist->opr.next);
	generate(Stmt);
	//~ free(n);
}

void ir_asynch_list(nodeType* n)
{
	printf("Entered Async\n");
	nodeType* func = get_operand(n,0);
	nodeType* fun_name = get_operand(func,0);
	char* sign,*pch;
	sign = strdup(fun_name->id.symrec->signature);
	
	printf(".locals init (class [mscorlib]System.Threading.Thread V%d)\n",idno);
	fprintf(output,".locals init (class [mscorlib]System.Threading.Thread V%d)\n",idno);
	printf("ldnull\n");
	fprintf(output,"ldnull\n");
	printf("ldftn ");
	fprintf(output,"ldftn ");
	
	
	if(strcmp(sign,"")==0)	
		return;	
	pch = strtok (sign," ::");
	printf("%s ",pch);
	fprintf(output,"%s ",pch);
	
	printf("class ");
	fprintf(output,"class ");
	printf("%s::",fun_name->id.symrec->my_st->parent->owner_name);
	fprintf(output,"%s::",fun_name->id.symrec->my_st->parent->owner_name);
	printf("%s",fun_name->id.symrec->sym_name);
	fprintf(output,"%s",fun_name->id.symrec->sym_name);
	printf("()\n");
	fprintf(output,"()\n");
	
	printf("newobj instance void class [mscorlib]System.Threading.ThreadStart::'.ctor'(object, native int)\n");
	fprintf(output,"newobj instance void class [mscorlib]System.Threading.ThreadStart::'.ctor'(object, native int)\n");
	printf("newobj instance void class [mscorlib]System.Threading.Thread::'.ctor'(class [mscorlib]System.Threading.ThreadStart)\n");
	fprintf(output,"newobj instance void class [mscorlib]System.Threading.Thread::'.ctor'(class [mscorlib]System.Threading.ThreadStart)\n");
	printf("stloc V%d\n",idno); 
	fprintf(output,"stloc V%d\n",idno); 
	printf("ldloc V%d\n",idno); 
	fprintf(output,"ldloc V%d\n",idno); 
	printf("callvirt instance void class [mscorlib]System.Threading.Thread::Start()\n");
	fprintf(output,"callvirt instance void class [mscorlib]System.Threading.Thread::Start()\n");
	idno++;
}

//~ void ir_for(nodeType* n)
//~ {
	//~ nodeType* initialize = get_operand(n,0);
	//~ nodeType* expr = get_operand(n,1);
	//~ nodeType* increament = get_operand(n,2);
	//~ nodeType* stmt = get_operand(n,3);
	//~ char initializenext[16], begin[16];
	//~ memset(initializenext,0,16);
	//~ memset(begin,0,16);
	//~ strcat(initializenext,newlabel());
	//~ strcat(begin,newlabel());
	//~ set_T(expr,begin);
	//~ set_F(expr,n->opr.next);
	//~ generate(initialize);
	//~ 
	//~ printf("br.s %s\n",initializenext);
	//~ fprintf(output,"br.s %s\n",initializenext);
	//~ printf("%s: \n",begin);
	//~ fprintf(output,"%s: ",begin);
	//~ generate(stmt);
	//~ generate(increament);
	//~ printf("%s: ",initializenext);
	//~ fprintf(output,"%s: ",initializenext);
	//~ seen_bool_flow = 1;
	//~ generate(expr);
	//~ seen_bool_flow = 0;
//~ }	

void ir_for(nodeType* n)
{
	nodeType* initialize = get_operand(n,0);
	nodeType* expr = get_operand(n,1);
	nodeType* increament = get_operand(n,2);
	nodeType* stmt = get_operand(n,3);
	char initializenext[16], begin[16];
	memset(initializenext,0,16);
	memset(begin,0,16);
	strcat(initializenext,newlabel());
	strcat(begin,newlabel());
	set_T(expr,begin);
	set_F(expr,n->opr.next);
	generate(initialize);
	
	//##for break statement##
	char initial_break_label[16];
	memset(initial_break_label,0,16);
	strcat(initial_break_label,break_label);
	memset(break_label,0,16);
	strcat(break_label,newlabel());
	loop_flag = loop_flag + 1;
	//####################
	
	printf("br %s\n",initializenext);
	fprintf(output,"br %s\n",initializenext);
	printf("%s: \n",begin);
	fprintf(output,"%s: ",begin);
	generate(stmt);
	prepost_put = 0;
	generate(increament);
	printf("%s: ",initializenext);
	fprintf(output,"%s: ",initializenext);
	seen_bool_flow = 1;
	generate(expr);
	seen_bool_flow = 0;
	printf("%s: \n",break_label);
	fprintf(output,"%s: \n",break_label);

	//for break statement
	loop_flag = loop_flag - 1;
	memset(break_label,0,16);
	strcat(break_label,initial_break_label);

}

void ir_array_declaration(nodeType* n)
{
	nodeType* array_name = get_operand(n,0);
	nodeType* array_type = get_operand(n,1);
	nodeType* array_size = get_operand(n,2);
	
	printf(".locals init (");
	fprintf(output,".locals init (");
	print_type(array_type);
	printf("[] ");
	fprintf(output,"[] ");
	printf(" %s )\n",array_name->id.symrec->uid); 	
	fprintf(output," %s )\n",array_name->id.symrec->uid); 	
	
	generate(array_size);
	
	printf("newarr [mscorlib]System.Int32\n"); 		// create a new array
	fprintf(output,"newarr [mscorlib]System.Int32\n"); 
	printf("stloc %s\n",array_name->id.symrec->uid );
	fprintf(output,"stloc %s\n",array_name->id.symrec->uid );
}

void ir_array_lhs(nodeType* n)
{
	nodeType* array_name = get_operand(n,0);
	nodeType* array_index = get_operand(n,1);
	printf("ldloc %s\n",array_name->id.symrec->uid ); //number[i] = i
	fprintf(output,"ldloc %s\n",array_name->id.symrec->uid ); //number[i] = i
	generate(array_index);
}	

void ir_array_rhs(nodeType* n)
{
	nodeType* array_name = get_operand(n,0);
	nodeType* array_index = get_operand(n,1);
	printf("ldloc %s\n",array_name->id.symrec->uid ); 
	fprintf(output,"ldloc %s\n",array_name->id.symrec->uid ); 
	generate(array_index);
	printf("ldelem.i4\n");
	fprintf(output,"ldelem.i4\n");
}


void ir_cast(nodeType* n)
{
	printf("CASTTTTTTTTTTTTTTT\n");
	nodeType* unary_op = get_operand(n,0);
	nodeType* cast_exp = get_operand(n,1);
	if(unary_op->con_i.value == MY_MINUS)
		{
			generate(cast_exp);
			printf("neg\n");
			fprintf(output,"neg\n");
		}
	else
			generate(cast_exp);
}	


void ir_switch(nodeType* n)
{
	nodeType* expr = get_operand(n,0);
	nodeType* case_stmt_list = get_operand(n,1);
	generate(expr);//after this the value of expr will be on stack
	
	int default_present=0;
	
	char test_label[16];
	char old_break_label[16];	
	char end_label[16];	
	
	memset(test_label,0,16);
	strcat(test_label,newlabel());
	
	//at the starting jump to test the cases. 
	printf("br %s\n",test_label);
	fprintf(output,"br %s\n",test_label);
	
	switch_flag++;
	
	//label that will appear at the end of switch
	memset(end_label,0,16);
	strcat(end_label,newlabel());

	//maybe someone is already using the break label so save it so as to restore it later
	memset(old_break_label,0,16);
	strcat(old_break_label,break_label);

	//set the label to be used for break statements in switch	
	memset(break_label,0,16);
	strcat(break_label,end_label);
	
	generate(case_stmt_list);
	
	//after default jump to end of switch statement
	printf("br %s\n",end_label);				
	fprintf(output,"br %s\n",end_label);
	
	//jump here to test the conditions
	printf("%s:\n",test_label);
	fprintf(output,"%s:\n",test_label);
	
	int count = 0;
	nodeType* const_exp;
	char* label;
	queue_length = queue_length -1;			
	printf("QUEUE LENGTH:%d\n",queue_length);
	while(count <= queue_length)		//since at the end queue_length is one more than the actual queue length
	{
		const_exp = expr_queue[count];
		
		if(const_exp->type == typeOpr )
		{
			//the below if else handles default statement
			if(const_exp->opr.oper == EMPTY)
			{
				default_present = 1;
				printf("br %s\n",label_queue[count]);
				fprintf(output,"br %s\n",label_queue[count]);
			}
			else
			{
				generate(const_exp);
				printf("beq %s\n",label_queue[count]);
				fprintf(output,"beq %s\n",label_queue[count]);
			}
		}
		else
		{
			generate(const_exp);
			printf("beq %s\n",label_queue[count]);
			fprintf(output,"beq %s\n",label_queue[count]);
		}
		
		
		if(count <= queue_length-1- default_present)		//do not do it after the last statement //recheck this condition
			{
			printf("KJKJOK\n");
			generate(expr);				//basically this will load the value of x in switch(x) on the stack 
			}
		count++;
	}

	queue_length = 0;

	printf("%s:\n",end_label);
	fprintf(output,"%s:\n",end_label);

	switch_flag--;
	memset(break_label,0,16);
	strcat(break_label,old_break_label);
}	


void ir_case_stmt_list(nodeType* n)
{
	nodeType* casestmtlist = get_operand(n,0);
	nodeType* casestmt = get_operand(n,1);
	nodeType* default_stmt = get_operand(n,2);
	generate(casestmtlist);	
	generate(casestmt);	
	generate(default_stmt);	
}	
void ir_case_stmt(nodeType* n)
{
	nodeType* const_exp = get_operand(n,0);
	nodeType* stmt = get_operand(n,1);
	char mylabel[16];
	memset(mylabel,0,16);
	strcat(mylabel,newlabel());
	printf("%s: ",mylabel);
	fprintf(output,"%s: ",mylabel);
	generate(stmt);
	//~ printf("br %s\n",switch_label);
	//~ fprintf(output,"br %s \n",switch_label);
	insert_queue(const_exp,mylabel);
}	

void ir_default_stmt(nodeType* n)
{
	nodeType* stmt = get_operand(n,0);
	char mylabel[16];
	memset(mylabel,0,16);
	strcat(mylabel,newlabel());
	printf("%s: ",mylabel);
	fprintf(output,"%s: ",mylabel);
	generate(stmt);
	//~ printf("br %s\n",switch_label);
	//~ fprintf(output,"br %s \n",switch_label);
	nodeType* const_exp = empty(EMPTY);
	insert_queue(const_exp,mylabel);
}	

void ir_break(nodeType* n)
{
	if(loop_flag > 0 || switch_flag > 0)		// check if any loop or switch is active (must be active for valid break)
	{
		printf("FOR FLAG\n");
		printf("br %s\n",break_label);			
		fprintf(output,"br %s\n",break_label);
	}
	else
		printf("STRAY BREAK\n");
}

void ir_prefix(nodeType* n)
{	
	nodeType* operator = get_operand(n,0);
	nodeType* unary_exp = get_operand(n,1);
	printf("IN IR PREFIX %d, PP = %d\n",operator->con_i.value,MY_PP);
	
	generate(unary_exp);
	printf("ldc.i4 1\n");			// load 1 to add or sub
	fprintf(output,"ldc.i4 1\n");
	
	if(operator->con_i.value==MY_PP)		// if ++ then add instr
	{
		fprintf(output,"add\n");
		printf("add\n");
	}
	else if(operator->con_i.value==MY_MM)	// if -- then sub instr
	{
		fprintf(output,"sub\n");
		printf("sub\n");
	}
	print_store_var(unary_exp);				// store the val according to type
	if(prepost_put == 1)
		print_load_var(unary_exp);
}

void ir_postfix(nodeType* n)
{	
	nodeType* operator = get_operand(n,1);
	nodeType* postfix_exp = get_operand(n,0);
	printf("IN IR POSTFIX %d, PP = %d\n",operator->con_i.value,MY_PP);

	if(prepost_put == 1)
		print_load_var(postfix_exp);
	print_load_var(postfix_exp);
	printf("ldc.i4 1\n");
	fprintf(output,"ldc.i4 1\n");		
	if(operator->con_i.value==MY_PP)
	{
		printf("add\n");
		fprintf(output,"add\n");
	}
	else if(operator->con_i.value==MY_MM)
	{
		printf("sub\n");
		fprintf(output,"sub\n");
	}
	print_store_var(postfix_exp);
}

void ir_obj(nodeType* n)
{
	nodeType* ident = get_operand(n,0);	// ident	
	nodeType* class_name = get_operand(n,1);	// class name
	nodeType* initexplist = get_operand(n,2);	// class name
	
	printf(".locals init( class\n");
	fprintf(output,".locals init( class");
	
	printf(" %s ",class_name->id.symrec->sym_name);
	fprintf(output," %s ",class_name->id.symrec->sym_name);
	
	printf(" %s) \n",ident->id.symrec->uid); 
	fprintf(output," %s) \n",ident->id.symrec->uid);
	
	ir_explist(initexplist);
	
	printf("newobj instance %s\n",class_name->id.symrec->signature); 
	fprintf(output,"newobj instance %s\n",class_name->id.symrec->signature);	
	
	
	printf("stloc %s \n",ident->id.symrec->uid); 
	fprintf(output,"stloc %s \n",ident->id.symrec->uid);

}
