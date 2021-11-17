// Fill out your copyright notice in the Description page of Project Settings.


#include "Miner.h"

// Sets default values for this component's properties
AMiner::AMiner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;
	// ...
}

// Called when the game starts
void AMiner::BeginPlay()
{	
	
	Super::BeginPlay();
	BlockList = {};
	miners = {};
	// ...
	
}


// Called every frame
void AMiner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isTaskDone && mineTask->IsDone()) {
		isTaskDone = false;
		delete mineTask;
		startMining(miningPower);
	}
	// ...
}

void AMiner::increasePower() {
	if (miningPower > 1) {
		miningPower -= 1;
	}
	resetMining();
	
	
}
void AMiner::decreasePower() {
	miningPower += 1;
	resetMining();
	
}
void AMiner::resetMining() {
	isTaskDone = true;
	mineTask->Cancel();
	
}

void AMiner::startMining(float delay) {
	
	Block b = coin->getLastBlock();
	Block myBlock(b.hash, b.index + 1);
	mineTask = new FAsyncTask<MineTask>(myBlock, miningPower, this, &coin->canChange);
	GetWorldTimerManager().SetTimer(handle, this, &AMiner::startTask,0.1f, false,delay);
	
}

void AMiner::startTask() {
	mineTask->StartBackgroundTask();
}
void AMiner::setCoin(ABlockchainCoin* c) {
	coin = c;
}

MineTask::MineTask(Block b, int32 difficulty, AMiner* miner,int32 *canChange) {
	this->block = b;
	this->difficulty = difficulty;
	this->miner = miner;
	this->canChange = canChange;
}
MineTask::~MineTask() {

}

void MineTask::mine() {
	FString minehash;
	for (int32 i = 0; i < difficulty; i++) {
		minehash.AppendChar('0');
	}

	UE_LOG(LogTemp, Display, TEXT("Starting hash mining"));
	FString tmp = block.hash.Left(difficulty);
	while (tmp != minehash) {
		block.check = block.check + 1;
		block.hash = FMD5::HashAnsiString(*(FString::FromInt(block.index) + block.prevhash + FString::FromInt(block.cash) + FString::FromInt(block.check)));

		tmp = block.hash.Left(difficulty);
		UE_LOG(LogTemp, Display, TEXT("Hash: %s"), *block.hash);
		UE_LOG(LogTemp, Display, TEXT("tmp: %s"), *tmp);
		UE_LOG(LogTemp, Display, TEXT("Minehash: %s"), *minehash);
	}
	UE_LOG(LogTemp, Display, TEXT("count: %d"), *canChange)
	if (*canChange < 1) {
		*canChange = *canChange + 1;
		UE_LOG(LogTemp, Display, TEXT("count: %d"), *canChange)

		
		if (*canChange <= 1) {
			miner->BlockList.Add(block);
			if (miner->miners.Num() > 0) {
				for (AMiner* m : miner->miners) {
					if (m != miner) {
						m->resetMining();
						m->BlockList = miner->BlockList;

					}

				}
			}
			if (miner->coin->BlockList.Last().hash == block.prevhash) {
				int32 index = miner->coin->BlockList.Add(block);
				miner->coin->Score = miner->coin->Score + 1;
				FString bstring;
				bstring = FString::FromInt(block.index) + " " + block.hash;
				UE_LOG(LogTemp, Display, TEXT("Blocco aggiunto: %s"), *bstring);
				UE_LOG(LogTemp, Display, TEXT("Score: %d"), miner->coin->Score);
				if(miner->canPrint) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Blocco "+bstring+" minato da : " + miner->name));
				


			}
		}
		
		*canChange = *canChange -1;
	}
	
}
void MineTask::DoWork() {
	mine();
	miner->resetMining();
}
