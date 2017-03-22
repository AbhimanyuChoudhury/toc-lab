#include <stdio.h>

#define MAX 100

typedef struct stack
{
	char data[MAX];
	int top;
}stack;

stack s;

void push(char data){
	s.data[++s.top] = data;
}

char pop(){
	return s.data[s.top--];
}


int precedence(char c){
	if(c == '(')
		return 0;
	if(c == '+' )
		return 1;
	if(c == '.' )
		return 2;
	else
		return -1;
}

int typeof1(char c){

	if(c == '(')
		return 1;
	if(c == ')')
		return 2;
	if(c == '+' || c == '.')
		return 3;
	if (c == ' ')
		return 4;
	else
		return 5;
}


int main() {

char array[20];	
char p;
char *c;

s.top=-1;

printf("Enter the infix expression");
scanf("%s",array);
c = array;

while(*c != '\0'){

	int x = typeof1(*c);
	
	if(x==5)
		printf("%c",*c );

	else if(x == 1)
		push(*c);
	
	else if(x == 2){
		while(( p = pop())!='(')
       	 	printf("%c",p);
    }
 
    else {

     	while(precedence(s.data[s.top]) >= precedence(*c) )
      		printf("%c",pop());
      	push(*c);
    }

	c++;
}

while(s.top != -1){
        printf("%c",pop());
    }
 

printf("\n");
return 0;

}
