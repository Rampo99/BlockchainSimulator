// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockchainCoin.h"

// Sets default values for this component's properties
ABlockchainCoin::ABlockchainCoin()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void ABlockchainCoin::BeginPlay()
{
	Super::BeginPlay();
	BlockList = {};
	
}
/*
void ABlockchainCoin::addTransaction(FString hash, Transaction t) {
	for (Block b : BlockList) {
		if (b.hash == hash) {
			b.transactions.Add(t);
		}
	}
} */
// Called every frame
void ABlockchainCoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ...
}

void ABlockchainCoin::createFirstBlock() {
	Block block("First Block",0);
	BlockList.Add(block);
	UE_LOG(LogTemp, Display, TEXT("First block created"));
}

Block ABlockchainCoin::getLastBlock() {
	return this->BlockList.Last();
}


bool ABlockchainCoin::isValid(TArray<Block> list) {
	for (int i = 1; i < list.Num(); i++) {
		Block cur = list[i];
		Block prev = list[i - 1];
		
		if (cur.hash != FMD5::HashAnsiString(*(FString::FromInt(cur.index) + cur.prevhash + FString::FromInt(cur.check)))) return false;
		if (prev.hash != cur.hash) return false;
	}

	return true;
}


Block::Block()
{
}

Block::Block(FString prevhash,int32 index) {
	this->prevhash = prevhash;
	this->index = index;
	this->cash = 0;
}



/*
Transaction::Transaction(FString from, FString to,int32 amount) {
	this->cash = amount;
	this->fromAddress = from;
	this->toAddress = to;
	this->hash = calculateHash();
}

FString Transaction::calculateHash() {
	return FMD5::HashAnsiString(*(this->fromAddress + FString::FromInt(this->cash) + this->toAddress));
}
*/