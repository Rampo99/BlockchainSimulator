// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockchainCoin.generated.h"


/*
class Transaction {
public:
	Transaction(FString from, FString to, int32 amount);
	FString fromAddress;
	FString toAddress;
	int32 cash;
	FString hash;
	FString calculateHash();
};
*/

class Block {
public:
	Block();
	Block(FString prevhash,int32 index);
	FString hash;
	FString prevhash;
	int32 index;
	int32 cash;
	int32 check = 0;
};




UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOCKCHAINSIMULATOR_API ABlockchainCoin : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ABlockchainCoin();
	Block getLastBlock();
	bool isValid(TArray<Block> list);
	void createFirstBlock();
	TArray<Block> BlockList;
	int32 canChange = 0;
	//void addTransaction(FString hash, Transaction t);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	int32 Score = 0;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
		
};


