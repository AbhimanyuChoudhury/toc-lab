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
//typedef struct State State;
typedef struct State
{
    int c;
    struct State *out;
    struct State *out1;
    int lastlist;
}State;
State matchstate = { Match };    /* matching state */
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

/*
 * A partially built NFA without the matching state filled in.
 * Frag.start points at the start state.
 * Frag.out is a list of places that need to be set to the
 * next state for this fragment.
 */
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

/* point_to_start the list of states at out to point to start. */
void
point_to_start(Ptrlist *l, State *s)
{
    Ptrlist *next;
    
    for(; l; l=next){
        next = l->next;
        l->s = s;
    }
}

/* Join the two lists l1 and l2, returning the combination. */
Ptrlist*
join_lists(Ptrlist *l1, Ptrlist *l2)
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
postfix_to_nfa(char *postfix)
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
        case '.':    /* catenate */
            e2 = pop();
            e1 = pop();
            point_to_start(e1.out, e2.start);
            push(frag(e1.start, e2.out));
            break;
        case '+':    /* alternate */
            e2 = pop();
            e1 = pop();
            s = state(Split, e1.start, e2.start);
            push(frag(s, join_lists(e1.out, e2.out)));
            break;
        case '?':    /* zero or one */
            e = pop();
            s = state(Split, e.start, NULL);
            push(frag(s, join_lists(e.out, list1(&s->out1))));
            break;
        case '*':    /* zero or more */
            e = pop();
            s = state(Split, e.start, NULL);
            point_to_start(e.out, s);
            push(frag(s, list1(&s->out1)));
            break;
        // case '+':    /* one or more */
        //     e = pop();
        //     s = state(Split, e.start, NULL);
        //     point_to_start(e.out, s);
        //     push(frag(e.start, list1(&s->out1)));
        //     break;
        }
    }

    e = pop();
    if(stackp != stack)
    {
        printf("\nlauda" );
        return NULL;
    }

    point_to_start(e.out, &matchstate);
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

void add_new_state(List*, State*);
void step_operation(List*, int, List*);

/* Compute initial state list */
List*
begin_list(State *start, List *l)
{
    l->n = 0;
    listid++;
    add_new_state(l, start);
    return l;
}

/* Check whether state list contains a match. */
int
ismatch(List *l)
{
    int i;

    for(i=0; i<l->n; i++)
        if(l->s[i] == &matchstate)
            return 1;
    return 0;
}

/* Add s to l, following unlabeled arrows. */
void
add_new_state(List *l, State *s)
{
    if(s == NULL || s->lastlist == listid)
        return;
    s->lastlist = listid;
    if(s->c == Split){
        /* follow unlabeled arrows */
        add_new_state(l, s->out);
        add_new_state(l, s->out1);
        return;
    }
    l->s[l->n++] = s;
}

/*
 * step_operation the NFA from the states in clist
 * past the character c,
 * to create next NFA state set nlist.
 */
void
step_operation(List *clist, int c, List *nlist)
{
    int i;
    State *s;

    listid++;
    nlist->n = 0;
    for(i=0; i<clist->n; i++){
        s = clist->s[i];
        if(s->c == c)
            add_new_state(nlist, s->out);
    }
}

/* Run NFA to determine whether it matches s. */
int
match(State *start, char *s)
{
    int i, c;
    List *clist, *nlist, *t;

    clist = begin_list(start, &l1);
    nlist = &l2;
    for(; *s; s++){
        c = *s & 0xFF;
        step_operation(clist, c, nlist);
        t = clist; clist = nlist; nlist = t;    /* swap clist, nlist */
    }
    return ismatch(clist);
}

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

printf("Enter the infix expression        ");
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
                   regex[j++] = p;
               }    
    }
 
    else {

         while(precedence(s.data[s.top]) >= precedence(*c) )
         {
          char len = pop();
        printf("%c",len);
        regex[j++] = len;

         }
          push(*c);
    }

    c++;
}

while(s.top != -1){
        char len = pop();
        printf("%c",len);
        regex[j++] = len;

    }

printf("\n");
printf("j(regex length)= %d\n",j);
char *help=(char *)malloc(j*sizeof(char));
int ii;
for(ii=0;ii<j;ii++){
    help[ii]=regex[ii];
}
 

 

    /*    if(regex == NULL){
        fprintf(stderr, "bad regexp %s\n", argv[1]);
        return 1;}*/
    
    char test[20];
    printf("Enter the string to be checked     ");
    scanf("%s",test);
    char* lol = regex;
    start = postfix_to_nfa(help);
    printf("%s\n",regex);

    if(start == NULL){
        fprintf(stderr, "error in postfix_to_nfa %s\n", regex);
        return 1;
    }

    
    l1.s = malloc(nstate*sizeof l1.s[0]);
    l2.s = malloc(nstate*sizeof l2.s[0]);
        if(match(start, test))
            printf("%s\n", test);
    return 0;
    
}
