// Fill out your copyright notice in the Description page of Project Settings.

#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"
#include "Components/PostProcessComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
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

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>("PostProcessingComponent");
	PostProcessComponent->SetupAttachment(GetRootComponent());
	
}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
	DestinationMarker->SetVisibility(false);
	if (BlinkerMaterialBase)
	{

		BlinkerMaterialInstance = UMaterialInstanceDynamic::Create(BlinkerMaterialBase, this);
		PostProcessComponent->AddOrUpdateBlendable(BlinkerMaterialInstance);
		BlinkerMaterialInstance->SetScalarParameterValue(TEXT("Radius"), .2);
	}
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector NewCameraOffset = CameraComp->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);
	UpdateDestinationMarker();
}

bool AVRCharacter::FindTeleportDestination(FVector &OUTLocation)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector Start = CameraComp->GetComponentLocation();
		FVector End = Start + CameraComp->GetForwardVector()*MaxTeleportDistance;
		FHitResult Hit;

		bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECollisionChannel::ECC_Visibility);
		FNavLocation NavLoc;
		//Set you NavAgentProps properties here (radius, height, etc)
		if (!bHit) { return false; }
		bool bOnNavmesh = UNavigationSystemV1::GetNavigationSystem(GetWorld())->ProjectPointToNavigation(Hit.Location, NavLoc, TeleportPorjectionExtetnt);
		if (!bOnNavmesh) { return false; }
		DrawDebugCylinder(World, Start, End, 20.f, 8, FColor::Red, true);
		OUTLocation = NavLoc.Location;
		
	}
	return true;
}
void AVRCharacter::UpdateDestinationMarker()
{
	
	FVector Location;
	bool bHasDestination = FindTeleportDestination(Location);
		
		if (bHasDestination)
		{
			DestinationMarker->SetVisibility(true);
			DestinationMarker->SetWorldLocation(Location);
		}
		else
		{
			DestinationMarker->SetVisibility(false);
		}
	
}


// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("Forward", this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAction("Teleport", IE_Released, this,&AVRCharacter::BeginTeleport);
}

void AVRCharacter::MoveForward(float value)
{
	
	
		AddMovementInput(value*CameraComp->GetForwardVector());
	
}

void AVRCharacter::MoveRight(float value)
{
	
		AddMovementInput(value*CameraComp->GetRightVector());
	
}

void AVRCharacter::BeginTeleport()
{
	
	GetWorldTimerManager().SetTimer(FadeTimerHandle, this, &AVRCharacter::EndTeleport,TeleportFadeTime);
	StartFade(0, 1);
}

void AVRCharacter::EndTeleport()
{
	
	SetActorLocation(DestinationMarker->GetComponentLocation() + GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	StartFade(1, 0);
}

void AVRCharacter::StartFade(float FromAlpha, float ToAlpha)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, TeleportFadeTime, FLinearColor::Black);


	}
}



