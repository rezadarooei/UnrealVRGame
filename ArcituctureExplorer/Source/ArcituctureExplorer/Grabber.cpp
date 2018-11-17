// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "MotionControllerComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"
#include "HandController.h"
#include "MotionControllerComponent.h"
#include "VRCharacter.h"


#define  OUT
// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>("MotionController");

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPsicsHandleComponent();
	SetUpInputComponent();
// 	if (!Physcs) { return; }
// 	Physcs = Charcter->Physcs;

}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Physcs) { return; }
 	FVector TargetLocation=GetReachLineEnd();
 	DrawDebugLine(GetWorld(), GetReachLineStart(), GetReachLineEnd(), FColor::Red, true, 3.f);
 	if (Physcs->GrabbedComponent)
 	{
 		Physcs->SetTargetLocation(TargetLocation);
 		UE_LOG(LogTemp, Warning, TEXT("you move"))
 			
 	}
	
}

void UGrabber::FindPsicsHandleComponent()
{


 	Physcs = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
 	if (!Physcs)
 	{
 		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, TEXT("Physics Doesenot attached"));
 
 	}
 	UE_LOG(LogTemp, Warning, TEXT("you phyics"))
}

 void UGrabber::SetUpInputComponent()
 {
 // 	RightHandController = GetWorld()->SpawnActor<AHandController>(HandControllerClass);
 	
 	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
 	if (InputComponent)
 	{
 		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
 		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
 	}
 	else
 	{
 		GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White, TEXT("Input Doese not attached"));
 	}
 	UE_LOG(LogTemp, Warning, TEXT("you input"))
 
 }

void UGrabber::Grab()
{
	
	auto HitResult = GetFirstPhycisBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();
	if (ActorHit)
	{
		Physcs->GrabComponentAtLocationWithRotation(ComponentToGrab, NAME_None, ComponentToGrab->GetOwner()->GetActorLocation(),
			ComponentToGrab->GetOwner()->GetActorRotation());
	}
	UE_LOG(LogTemp,Warning,TEXT("you grab"))
// 
 	APawn* Pawn = Cast<APawn>(GetOwner());
 	if (Pawn)
 	{
 		APlayerController* Controller = Cast<APlayerController>(Pawn->GetController());
		
		if (!Controller)
		{
			UE_LOG(LogTemp, Warning, TEXT("you dont have controller"))
			return; }
		if (MotionController) 
		{
			UE_LOG(LogTemp, Warning, TEXT("you  have Motioncontroller"))
				MotionController->SetTrackingSource(EControllerHand::Right);

			Controller->PlayHapticEffect(HapticEffect, MotionController->GetTrackingSource());
		}
		
 	}
}
const FHitResult UGrabber::GetFirstPhycisBodyInReach()
{
	
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(""), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(Hit, GetReachLineStart(), GetReachLineEnd(), ECollisionChannel::ECC_PhysicsBody, TraceParams);
	DrawDebugLine(GetWorld(), GetReachLineStart(), GetReachLineEnd(), FColor::Red, true, 3.f);
	UE_LOG(LogTemp, Warning, TEXT("you hit"))
	return Hit;
}


void UGrabber::Release()
{
	if (!Physcs) { return; }
	Physcs->ReleaseComponent();
	UE_LOG(LogTemp, Warning, TEXT("you release"))

}

FVector UGrabber::GetReachLineStart()
{

	FVector PlrayerViewPointLocation;
	FRotator PlayerViewpointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlrayerViewPointLocation, OUT PlayerViewpointRotation);
	FVector LineTraceEnd = PlrayerViewPointLocation+PlayerViewpointRotation.Vector()*Reach;
	
	return LineTraceEnd;
}

FVector UGrabber::GetReachLineEnd()
{
	FVector PlrayerViewPointLocation;
	FRotator PlayerViewpointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUT PlrayerViewPointLocation,OUT PlayerViewpointRotation);
	FVector LineTraceStart = PlrayerViewPointLocation;
	return LineTraceStart;
}

