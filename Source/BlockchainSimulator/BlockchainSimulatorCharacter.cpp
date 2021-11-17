// Copyright Epic Games, Inc. All Rights Reserved.

#include "BlockchainSimulatorCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ABlockchainSimulatorCharacter

ABlockchainSimulatorCharacter::ABlockchainSimulatorCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input
void ABlockchainSimulatorCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("StartCoin",IE_Released, this,&ABlockchainSimulatorCharacter::startCoin);
	PlayerInputComponent->BindAction("BuyRoom", IE_Released, this, &ABlockchainSimulatorCharacter::buyRoom);
	
	PlayerInputComponent->BindAxis("MoveForward", this, &ABlockchainSimulatorCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlockchainSimulatorCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABlockchainSimulatorCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABlockchainSimulatorCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABlockchainSimulatorCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABlockchainSimulatorCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ABlockchainSimulatorCharacter::OnResetVR);
}

void ABlockchainSimulatorCharacter::keepHacking(AMiner* temp) {
	//proof of work
	TArray<Block> templist = temp->BlockList;
	int v = miners.Num();
	for (AMiner* m : miners) {
		m->canPrint = false;
		if (m != temp) {
			TArray<Block> tmplist = m->BlockList;
			for (int j = 0; j < tmplist.Num(); j++) {
				bool check = false;
				for (int32 i = 0; i < templist.Num(); i++) {
					if (tmplist[j].hash != templist[i].hash) {
						check = true;
						break;
					}
				}

				if (check) {
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("La chain del miner " + FString::FromInt(j) + " é diversa"));
					v--;
				}
				else {
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("La chain del miner " + FString::FromInt(j) + "é uguale"));
				}
			}
		}

	}
	if (v < miners.Num() / 2) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Siccome piú della metá dei miner ha una chain diversa"));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Da quella che hai hackerato"));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("La chain che hai hackerato viene sostituita con una valida"));
		}
	}


	temp->BlockList = Coin->BlockList;
	for (AMiner* m : miners) m->canPrint = true;
}
void ABlockchainSimulatorCharacter::hackMiner(AActor* a) {
	AMiner *temp = Cast<AMiner>(a);
	
	if (temp != nullptr && temp->BlockList.Num() > 0) {
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Hackerando.."));
		FString list = "\n";
		FString list2 = "\n";
		TArray<Block> templist = temp->BlockList;

		//add 100 cash to all blocks
		for (int32 i = 0; i < templist.Num(); i++) {
			
			Block bl = templist[i];
			bl.cash = bl.cash + 100;
			bl.hash = FMD5::HashAnsiString(*(FString::FromInt(bl.index) + bl.prevhash + FString::FromInt(bl.cash) + FString::FromInt(bl.check)));;
			list += FString::FromInt(bl.index) + " " + bl.hash + "\n";
			
		}
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Sei riuscito a cambiare i valori dei blocchi e ricalcolare le hash"));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Vediamo che succede ora.."));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("I blocchi di questo miner verranno confrontati con tutti gli altri"));
		}
		UE_LOG(LogTemp, Display, TEXT("La chain di questo miner: %s "), *list);
		FTimerHandle handle;
		FTimerDelegate delegate;
		delegate.BindUFunction(this, FName("keepHacking"), temp);
		GetWorldTimerManager().SetTimer(handle,delegate, 0.1f, false, 5.0f);
		
	}
}
void ABlockchainSimulatorCharacter::startCoin() {
	if (startcheck) {
		startcheck = false;
		Coin = Cast<ABlockchainCoin>(coinactor);
		if (Coin != nullptr) {
			Coin->createFirstBlock();
		}
		else {
			UE_LOG(LogTemp, Display, TEXT("null ptr exception on creating first block"));
		}

		startMiners();

	}
	
}

void ABlockchainSimulatorCharacter::buyRoom() {
	if (roomindex < 4) {
		if (Coin->Score >= 20) {
			Coin->Score -= 20;
			AActor* tmp = dooractors[roomindex - 1];
			UDoorOpen* door = tmp->FindComponentByClass<UDoorOpen>();
			door->OpenDoor();
			startMiners();
		}
		
	}
	
}
void ABlockchainSimulatorCharacter::startMiners() {
	TArray<AActor*> tmp;
	switch (roomindex) {
	case 0:
		tmp = mineractors0;
		break;
	case 1:
		tmp = mineractors1;
		break;
	case 2:
		tmp = mineractors2;
		break;
	case 3:
		tmp = mineractors3;
		break;
	}
	TArray<AMiner*> tempminers;
	for (AActor* miner : tmp) {
		
		AMiner* temp = Cast<AMiner>(miner);
		if (temp != nullptr) {
			temp->setCoin(Coin);
			
			miners.Add(temp);
			tempminers.Add(temp);
			
		}
		else {
			UE_LOG(LogTemp, Display, TEXT("null ptr exception on miners"));
		}
		
	}

	roomindex++;
	for (AMiner* m : miners) {
		m->miners = this->miners;
		
	}
	float timer = 0.0f;
	for (AMiner* m : tempminers) {
		
		m->startMining(timer);
		timer = timer + 1.0f;
	}
}
void ABlockchainSimulatorCharacter::OnResetVR()
{
	// If BlockchainSimulator is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in BlockchainSimulator.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ABlockchainSimulatorCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void ABlockchainSimulatorCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void ABlockchainSimulatorCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABlockchainSimulatorCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ABlockchainSimulatorCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABlockchainSimulatorCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}



