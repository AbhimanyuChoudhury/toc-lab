#include <stdio.h>
#include <stdlib.h>

#define NO_ITEMS 7

int prices[] = {10,25,5,7,9,13,20};
int currency[] = {1,5,10};


void startState();
void incompleteState(int itemChoice,int paid);
void invalidState(int itemChoice, int paid);
void finishingState();
void finishedState();


void main(){

printf("Items and their prices\n");
int i;
for(i=0;i<NO_ITEMS;i++)
	printf("Item %d\tPrice %d\n",i,prices[i]);

printf("Acceptable currency is 1,5,10\n");

startState();
}


void startState(){

printf("Choose Item: ");
int itemChoice;
scanf("%d",&itemChoice);
printf("Price to be paid: %d\n",prices[itemChoice]);

int paid=0;
incompleteState(itemChoice,paid);
}



void incompleteState(int itemChoice, int paid){

//int paid=0;

while(paid<prices[itemChoice]){
	printf("Enter Coin:");
	int coin;
	scanf("%d",&coin);

	if(coin==1){
		paid = paid+1;
	}
	else if(coin==5){
		paid = paid+5;
	}
	else if(coin==10){
		paid= paid+10;
	}

	else{
		invalidState(itemChoice,paid);
		break;
	}

	if(prices[itemChoice]-paid>0)
		printf("Paid = %d,  Amount remaining = %d\n",paid,prices[itemChoice]-paid );

}

if(prices[itemChoice]-paid<=0){
	if(prices[itemChoice]-paid<0)
		printf("Your Change is: %d\n", paid-prices[itemChoice]);
	finishingState();
}

}

void invalidState(int itemChoice, int paid){
	printf("Please Enter Valid Coin\n");
	incompleteState(itemChoice,paid);
}



void finishingState(){
	printf("Thank You For Buying Our Item\n");
	printf("Do You Want Another Item- Y(Yes) or N(No):");
	char choice;
	scanf(" %c",&choice);
	if(choice=='Y')
		startState();
	if(choice=='N')
		finishedState();
}

void finishedState(){
	printf("Thank You! Have a nice day\n");
}
