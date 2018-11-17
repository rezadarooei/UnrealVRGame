// Fill out your copyright notice in the Description page of Project Settings.

#include "HandController.h"
#include "Components/StaticMeshComponent.h"
#include "MotionControllerComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AHandController::AHandController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>("MotionController");
	MotionController->SetupAttachment(MotionController);
	
}

// Called when the game starts or when spawned
void AHandController::BeginPlay()
{
	Super::BeginPlay();
	OnActorBeginOverlap.AddDynamic(this, &AHandController::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &AHandController::ActorEndOverlap);
}

// Called every frame
void AHandController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//in grip we change state of grip to true
		
	if (bIsClimbing)
	{
		
		FVector HandControllerDelta = GetActorLocation() - ClimbingStartLocation;
		GetAttachParentActor()->AddActorWorldOffset(-HandControllerDelta);
	}
}

void AHandController::Grip()
{

	if (!bCanClimb) return;
	//in this place bcanclimb is true which means we can climb
		//as default bisclimbing is false
	
	if(!bIsClimbing)
	{//we go into if statement
		//change state because we can climb
		bIsClimbing = true;
		//we use start location 
		ClimbingStartLocation = GetActorLocation();
		OtherController->bIsClimbing=false;
		ACharacter* Charcter = Cast<ACharacter>(GetAttachParentActor());
		if (Charcter)
		{
			Charcter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		}
	}

}

void AHandController::Release()
{
	bIsClimbing = false;
	if (bIsClimbing) 
	{
		bIsClimbing = false;
		ACharacter* Charcter = Cast<ACharacter>(GetAttachParentActor());
		if (Charcter)
		{
			Charcter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
		} 
	}
}

void AHandController::PairController(AHandController* Controller)
{
	OtherController = Controller;
	OtherController->OtherController = this;
}

void AHandController::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
//if we overlap bool become true
	bool bNewCanClimb = CanClimb();
	//bcanclimb is false and ! is true both of them is true we have ue_log
	if (!bCanClimb && bNewCanClimb)
	{
		//controller parent is VR character
		APawn* Pawn = Cast<APawn>(GetAttachParentActor());
		if (Pawn) 
		{
			APlayerController* Controller = Cast<APlayerController>(Pawn->GetController());
			if (Controller) 
			{
				Controller->PlayHapticEffect(HapticEffect, MotionController->GetTrackingSource());
			}
			
		}
		
		
	}
	//b can climb change to true
	bCanClimb = bNewCanClimb;
}

void AHandController::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//bcanclimb become false  because we end overlap
	bCanClimb = CanClimb();
}
//it shows if we overlap in the something we the Specified tag it becomes true

bool AHandController::CanClimb() const
{
	//find all overlapping array
	TArray < AActor * >  OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	for (AActor* OverlappinActor : OverlappingActors) 
	{
		if (OverlappinActor->ActorHasTag("Climbable")) 
		{
			return true;
		}
		
	}
	return false;
}

