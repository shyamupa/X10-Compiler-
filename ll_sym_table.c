#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include"ll_sym_table.h"
/*
This is a linked list implementation of the symbol table
*/
typedef struct sym_record sym_record;
typedef struct symbol_table symbol_table;
extern int idno;
char my_buffer[100];


char* newuid()
{
	memset(my_buffer,0,100);
	sprintf(my_buffer,"V%d",idno++);
	return my_buffer;
}


symbol_table* new_sym_table(symbol_table* parent)
{
	debugger("NEW SYM TAB CREATED\n");
	symbol_table* st;
	st=(symbol_table*)malloc(sizeof(symbol_table));
	st->parent=parent;
	return st;
}
symbol_table* free_table (symbol_table* st) 
{
	symbol_table* current;
	current = st->parent;
	sym_record* p=st->Head;
	sym_record* t;
	while(p!=NULL)
	{
		t=p->next;
		free(p);
		p=t;
	}
	free(st);
	return current;
}
sym_record* insert(symbol_table* st,char* sym_name)	// inserts a record and returns a ptr to it
{
	sym_record* rv;
	rv=(sym_record*)malloc(sizeof(sym_record));
	rv->sym_name = strdup(sym_name);	// copies from yytext
	rv->uid = strdup(newuid());	
	if(st->owner_name!=NULL)
		rv->in_st_of=strdup(st->owner_name);		// copy the owner name into symrec
	sym_record* p=st->Head;
	if(p==NULL)
	{
		st->Head=rv;
	}
	else
	{
		while(p->next!=NULL) p=p->next;
		p->next=rv;
	}	
	return rv;
}
sym_record* search(symbol_table* st,char* target_name)	//searches for a record and returns a ptr to it
{
	sym_record* p;
	while(st!=NULL)
	{
		p=st->Head;	// first sym_record
		while(p!=NULL)
		{
			if(strcmp(p->sym_name,target_name)==0)
				return p;
			p=p->next;	
		}
		st=st->parent;
	}	
	return NULL;
}
void print_st(symbol_table* st)
{
	debugger("####################\n");
	if(st->owner_name!=NULL)
		debugger("PRINTING SYM_T of %s \n",st->owner_name);
	else
		debugger("PRINTING SYM_T of block \n");	
	struct sym_record* p=st->Head;
	while(p!=NULL)
	{
		debugger("%s %s type %d \n",p->sym_name,p->uid,p->type);
		p=p->next;
	}
	debugger("####################\n");
}
int search_keywords(char* sym_name)
{
	int i;
	char* keywords[]=
	{
		"abstract",
		"as",
		"assert",
		"async",
		"athome",
		"ateach",
		"atomic",
		"break",
		"catch",
		"class",
		"clocked",
		"continue",
		"default",
		"do",
		"else",
		"extends",
		"final",
		"finally",
		"finish",
		"for",
		"haszero",
		"here",
		"if",
		"implements",
		"in",
		"instanceof",
		"interface",
		"native",
		"null",
		"offer",
		"offers",
		"operator",
		"private",
		"property",
		"protected",
		"public",
		"self",
		"static",
		"struct",
		"super",
		"this",
		"throw",
		"transient",
		"true",
		"type",
		"val",
		"var",
		"void",
		"while",
		"at",
		"case",
		"def",
		"false",
		"goto",
		"import",
		"new",
		"package",
		"return",
		"switch",
		"try",
		"when",
		"println",
		"print"
	};
	for(i=0;i<61;i++)
	{
		if(strcmp(sym_name,keywords[i])==0)
		return 1;
	}
	return 0;
}
