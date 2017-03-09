#include <stdio.h>
#include <string.h>

#define NO_OF_CHARS 256
#define MAX_PATTERN_LEN 5
#define MAX_STR_LEN 100

 
int getNextState(char *pattern, int A, int state, int x) {
 
  if (state < A && x == pattern[state])
    return state+1;
 
  int nextState, i; 
 
  for (nextState = state; nextState > 0; nextState--){
    
    if(pattern[nextState-1] == x){
      
      for(i = 0; i < nextState-1; i++)
        if (pattern[i] != pattern[state-nextState+1+i])
          break;
      
      if (i == nextState-1)
        return nextState;
    
    }
  }
 
  return 0;
}
 

void computeTransitionFunction(char *pattern, int A, int TransitionFunction[][NO_OF_CHARS]){

  int state, x;
  for (state = 0; state <= A; ++state)
    for (x = 0; x < NO_OF_CHARS; ++x)
      TransitionFunction[state][x] = getNextState(pattern, A, state, x);

}
 

void search(char *pattern, char *string){

  int patternLen = strlen(pattern);
  int stringLen = strlen(string);
 
  int TransitionFunction[patternLen+1][NO_OF_CHARS];
 
  computeTransitionFunction(pattern, patternLen, TransitionFunction);
 
  int i, state=0;
  
  for (i = 0; i < stringLen; i++){
    state = TransitionFunction[state][string[i]];
    if (state == patternLen){
      printf ("Pattern found at index %d \n", i-patternLen+1);
    }
  }

}
 

int main(){

  char pattern[MAX_PATTERN_LEN];
  char string[MAX_STR_LEN];

  printf("Enter Pattern:");
  scanf("%s",pattern);

  printf("Enter String:");
  scanf("%s",string);

  search(pattern,string);

  // char *txt = "ABAACAADAABAAAAA";
  // char *pat = "AABA";
  // search(pat, txt);
  
  return 0;
}