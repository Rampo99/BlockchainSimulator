// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockchainCoin.h"
#include "Miner.generated.h"

class MineTask : public FNonAbandonableTask {
public:
	MineTask(Block block, int32 difficulty, AMiner* miner,int32* canChange);
	~MineTask();
	FORCEINLINE TStatId GetStatId() const {
		RETURN_QUICK_DECLARE_CYCLE_STAT(MineTask, STATGROUP_ThreadPoolAsyncTasks);
	}
	int32* canChange;
	AMiner* miner;
	Block block;
	int32 difficulty;
	void mine();
	void DoWork();
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOCKCHAINSIMULATOR_API AMiner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	AMiner();

	UFUNCTION()
	void startMining(float delay);
	UFUNCTION(BlueprintCallable)
	void increasePower();
	UFUNCTION(BlueprintCallable)
	void decreasePower();
	void resetMining();
	void setCoin(ABlockchainCoin* c);
	FAsyncTask<MineTask>* mineTask;
	TArray<Block> BlockList;
	
	TArray<AMiner*> miners;
	ABlockchainCoin* coin;
	bool canPrint = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 miningPower = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString name = "none";
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void startTask();
	FTimerHandle handle;
	bool isTaskDone = false;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
		
};


