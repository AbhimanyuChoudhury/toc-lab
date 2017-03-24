 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



#define MAX 100

#define DEBUG 0

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

enum
{
	Match = 256,
	Split = 257
};
typedef struct State State;
struct State
{
	int c;
	State *out;
	State *out1;
	int lastlist;
};
State matchstate = { Match };	/* matching state */
int nstate;

/* Allocate and initialize State */
State*
state(int c, State *out, State *out1)
{
	State *s;
	
	nstate++;
	s = malloc(sizeof *s);
	s->lastlist = 0;
	s->c = c;
	s->out = out;
	s->out1 = out1;
	return s;
}

typedef struct Frag Frag;
typedef union Ptrlist Ptrlist;
struct Frag
{
	State *start;
	Ptrlist *out;
};

/* Initialize Frag struct. */
Frag
frag(State *start, Ptrlist *out)
{
	Frag n = { start, out };
	return n;
}

/*
 * Since the out pointers in the list are always 
 * uninitialized, we use the pointers themselves
 * as storage for the Ptrlists.
 */
union Ptrlist
{
	Ptrlist *next;
	State *s;
};

/* Create singleton list containing just outp. */
Ptrlist*
list1(State **outp)
{
	Ptrlist *l;
	
	l = (Ptrlist*)outp;
	l->next = NULL;
	return l;
}

/* Patch the list of states at out to point to start. */
void
patch(Ptrlist *l, State *s)
{
	Ptrlist *next;
	
	for(; l; l=next){
		next = l->next;
		l->s = s;
	}
}

/* Join the two lists l1 and l2, returning the combination. */
Ptrlist*
append(Ptrlist *l1, Ptrlist *l2)
{
	Ptrlist *oldl1;
	
	oldl1 = l1;
	while(l1->next)
		l1 = l1->next;
	l1->next = l2;
	return oldl1;
}

/*
 * Convert postfix regular expression to NFA.
 * Return start state.
 */
State*
post2nfa(char *postfix)
{
	char *p;
	Frag stack[1000], *stackp, e1, e2, e;
	State *s;
	
	// fprintf(stderr, "postfix: %s\n", postfix);

	if(postfix == NULL)
		return NULL;

	#define push(s) *stackp++ = s
	#define pop() *--stackp

	stackp = stack;
	for(p=postfix; *p; p++){
		switch(*p){
		default:
			s = state(*p, NULL, NULL);
			push(frag(s, list1(&s->out)));
			if(DEBUG)
				printf("%d\n",stackp );
			break;
		case '.':	/* catenate */
			e2 = pop();
			e1 = pop();
			patch(e1.out, e2.start);
			push(frag(e1.start, e2.out));
			break;
		case '+':	/* alternate */
			e2 = pop();
			e1 = pop();
			s = state(Split, e1.start, e2.start);
			push(frag(s, append(e1.out, e2.out)));
			break;
		case '?':	/* zero or one */
			e = pop();
			s = state(Split, e.start, NULL);
			push(frag(s, append(e.out, list1(&s->out1))));
			break;
		case '*':	/* zero or more */
			e = pop();
			s = state(Split, e.start, NULL);
			patch(e.out, s);
			push(frag(s, list1(&s->out1)));
			break;
		// case '+':	/* one or more */
		// 	e = pop();
		// 	s = state(Split, e.start, NULL);
		// 	patch(e.out, s);
		// 	push(frag(e.start, list1(&s->out1)));
		// 	break;
		}
	}

	e = pop();
	if(stackp != stack)
	{
		printf("\nlauda" );
		return NULL;
	}

	patch(e.out, &matchstate);
	return e.start;
#undef pop
#undef push
}

typedef struct List List;
struct List
{
	State **s;
	int n;
};
List l1, l2;
static int listid;

void addstate(List*, State*);
void step(List*, int, List*);



int
main()
{
	int i;
	char *post;
	State *start;
int j = 0;	
char array[20];	
char regex[20];
char p;
char *c;

s.top=-1;

printf("Enter the infix expression");
scanf("%s",array);
c = array;

while(*c != '\0'){

	int x = typeof1(*c);
	
	if(x==5)
		{
			printf("%c",*c );
			regex[j++] = *c;
		}	
	else if(x == 1)
		push(*c);
	
	else if(x == 2){
		while(( p = pop())!='(')
       		{
       			printf("%c",p);
       			regex[j++] = *c;
       		}	
    }
 
    else {

     	while(precedence(s.data[s.top]) >= precedence(*c) )
     	{
      		printf("%c",pop());
      		regex[j++] = *c;

     	}
      	push(*c);
    }

	c++;
}

while(s.top != -1){
		char len = pop();
        printf("%c",len);
          printf("\n");
        regex[j++] = len;

    }

 

	/*	if(regex == NULL){
		fprintf(stderr, "bad regexp %s\n", argv[1]);
		return 1;}*/
	
    char test[20];
    printf("Enter the string to be checked");
    scanf("%s",test);
    char* lol = regex;
  	//printf("%s\n",regex);
	start = post2nfa(regex);

	if(start == NULL){
		fprintf(stderr, "error in post2nfa %s\n", regex);
		return 1;
	}

	
	l1.s = malloc(nstate*sizeof l1.s[0]);
	l2.s = malloc(nstate*sizeof l2.s[0]);
		if(match(start, test))
			printf("%s\n", test);
	return 0;
	
}
	
