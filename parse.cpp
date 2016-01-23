#include <parse.h>

#include <iostream>
#include <map>
#include <utility>
#include <cstdlib>

Program prog;

std::map<token, Keyword> keyword_map;

NStatementList *cur_node;
int cur_ptr=0;
bool nested=false;

void parse_error(std::string msg){
	std::cout<<"[PARSE ERROR] "<<msg<<"\n";
	exit(0);
}

void keyword_map_init(){
	#define o(id, val, num) keyword_map[val]=id;
		LEX_LIST
	#undef o
}

int to_num(token t){
	int n=0;
	for(int i=0;i<t.length();i++){
		if(t[i]<'0' || t[i]>'9') return -1;
		n*=10;
		n+=t[i]-'0';
	}
	return n;
}

token peek(){
	return tokens[cur_ptr];
}

token get_tok(){
	return tokens[cur_ptr++];
}

void parse_stmt_list();

void parse_stmt(){
	token t=get_tok();
	NStatement *stmt=new NStatement;
	std::cout<<"[DEBUG] "<<t<<"\n";
	stmt->cmd=keyword_map[t];
	if(keyword_map[t]==TOK_PROC){
		if(nested) parse_error("Procedures cannot be nested");
		NStatementList *prev_node=cur_node;
		token id=get_tok();
		if(keyword_map[id]!=TOK_INVALID) parse_error("Keywords are not permitted as procedure IDs: "
						+id+" used\n");
		nested=true;
		cur_node=&(((NProcedure*)stmt)->block);
		parse_stmt_list();
		cur_node=prev_node;
		nested=false;
		token end=get_tok();
		if(keyword_map[end]!=TOK_END)
			parse_error("Procedures must be ended with the end keyword");
	}
	else if(keyword_map[t]==TOK_PUSH){
		token v=get_tok();
		int val=to_num(v);
		if(val==-1) parse_error("Only integers can be pushed into the stack: "
					+v+" pushed instead");
		stmt->val=val;
	}
	else if(keyword_map[t]==TOK_IN){
		token io=get_tok();
		if(keyword_map[io]!=TOK_INT && keyword_map[io]!=TOK_CHAR)
			parse_error("Wrong token: "+io);
		stmt->io_type=keyword_map[io];}
	else if(keyword_map[t]==TOK_OUT){
		token io=get_tok();
		if(keyword_map[io]!=TOK_INT && keyword_map[io]!=TOK_CHAR)
			parse_error("Wrong token: "+io);
		stmt->io_type=keyword_map[io];
	}
	
	cur_node->stmts.push_back(stmt);
}

void parse_stmt_list(){
	token t=peek();
	if(keyword_map[t]==TOK_EOS) return;
	if(keyword_map[t]==TOK_END 
		|| keyword_map[t]==TOK_INT 
		|| keyword_map[t]==TOK_CHAR) 
		parse_error("Wrong token: "+t);
	else parse_stmt();
	parse_stmt_list();
}

void parse_prog(){
	token t=peek();
	if(keyword_map[t]==TOK_EOS) return;
	if(keyword_map[t]==TOK_END 
		|| keyword_map[t]==TOK_INT 
		|| keyword_map[t]==TOK_CHAR) 
		parse_error("Wrong token: "+t);
	else parse_stmt_list();
}

void parse(){
	cur_node=&prog;
	keyword_map_init();
	parse_prog();
}