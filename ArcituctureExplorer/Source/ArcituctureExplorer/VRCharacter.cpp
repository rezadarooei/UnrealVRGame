// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
// Sets default values
// #define OUT
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>("RootComp");
	VRRoot->SetupAttachment(GetRootComponent());

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(VRRoot);
	
	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>("DestinationMarker");
	DestinationMarker->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewCameraOffset = CameraComp->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(NewCameraOffset);
	UpdateDestinationMarker();
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Forward", this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &AVRCharacter::MoveRight);
}

void AVRCharacter::MoveForward(float value)
{
	
	
		AddMovementInput(value*CameraComp->GetForwardVector());
	
}

void AVRCharacter::MoveRight(float value)
{
	
		AddMovementInput(value*CameraComp->GetRightVector());
	
}

void AVRCharacter::UpdateDestinationMarker()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FHitResult Hit;
		FVector Start = CameraComp->GetComponentLocation();
		FVector End = Start + CameraComp->GetForwardVector()*MaxTeleportDistance;
		bool bHit=World->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility);
		if (bHit)
		{
			DestinationMarker->SetWorldLocation(Hit.Location);
		}
	}
}

