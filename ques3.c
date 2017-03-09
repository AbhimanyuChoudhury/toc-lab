#include <stdio.h>
#include <stdlib.h>

#define NO_ITEMS 7

void main(){

int prices[] = {10,25,5,7,9,13,20};

printf("Items and their prices\n");
int i;
for(i=0;i<NO_ITEMS;i++)
	printf("Item %d\tPrice %d\n",i,prices[i]);

int currency[] = {1,5,10};
printf("Acceptable currency is 1,5,10\n");

printf("Choose Item: ");
int itemChoice;
scanf("%d",&itemChoice);
printf("Price to be paid: %d\n",prices[itemChoice]);

int paid=0;

while(paid<prices[itemChoice]){
	printf("Enter Coin:");
	int coin;
	scanf("%d",&coin);

	if(coin==1){
		paid = paid+1;
	}
	if(coin==5){
		paid = paid+5;
	}
	if(coin==10){
		paid= paid+10;
	}

	if(prices[itemChoice]-paid>0)
		printf("Amount remaining = %d\n",prices[itemChoice]-paid );
	
}

if(prices[itemChoice]-paid<=0){
	printf("Congrats on buying item %d\n",itemChoice );
	printf("Your Change %d\n",paid-prices[itemChoice]);
}

}