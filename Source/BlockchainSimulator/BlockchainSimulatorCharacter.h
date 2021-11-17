// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlockchainCoin.h"
#include "Miner.h"
#include "DoorOpen.h"
#include "BlockchainSimulatorCharacter.generated.h"



UCLASS(config=Game)
class ABlockchainSimulatorCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	ABlockchainSimulatorCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere)
	AActor *coinactor;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	ABlockchainCoin* Coin;

	UFUNCTION(BlueprintCallable)
	void hackMiner(AActor* a);
	UPROPERTY(EditAnywhere)
	TArray<AActor*> mineractors0;
	UPROPERTY(EditAnywhere)
	TArray<AActor*> mineractors1;
	UPROPERTY(EditAnywhere)
	TArray<AActor*> mineractors2;
	UPROPERTY(EditAnywhere)
	TArray<AActor*> mineractors3;

	TArray<AMiner*> miners = {};
	UPROPERTY(EditAnywhere)
	TArray<AActor*> dooractors;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool startcheck = true;
protected:
	int32 roomindex = 0;
	
	/** Resets HMD orientation in VR. */
	UFUNCTION()
	void keepHacking(AMiner* miner);
	void OnResetVR();
	UFUNCTION()
	void startCoin();
	UFUNCTION()
	void buyRoom();
	UFUNCTION()
	void startMiners();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

