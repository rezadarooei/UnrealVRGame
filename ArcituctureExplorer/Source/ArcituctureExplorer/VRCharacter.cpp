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
#include "Curves/CurveFloat.h"
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

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

	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>("LeftController");
	LeftHandController->SetupAttachment(VRRoot);
	LeftHandController->SetTrackingSource(EControllerHand::Left);

	LeftHandControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>("LeftHandMesh");
	LeftHandControllerMesh->SetupAttachment(LeftHandController);

	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>("RightController");
	RightHandController->SetupAttachment(VRRoot);
	RightHandController->SetTrackingSource(EControllerHand::Right);


	TelePath = CreateDefaultSubobject<USplineComponent>("TelePath");
	TelePath->SetupAttachment(RightHandController);

	RighHandControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>("RighHandMesh");
	RighHandControllerMesh->SetupAttachment(RightHandController);
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
	
	UpdateBlinkers();
}

bool AVRCharacter::FindTeleportDestination(TArray<FVector> &OUTPath,FVector &OUTLocation)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector Start = RightHandController->GetComponentLocation();
		FVector Look = RightHandController->GetForwardVector();
		
		FPredictProjectilePathParams Params(TeleportProjctileRadius,Start,Look*TeleportProjctileSpeed,MaxSimulationTime,ECollisionChannel::ECC_Visibility,this);
		Params.bTraceComplex = true;
		FPredictProjectilePathResult Result;
		bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, Result);
		
		
		for (FPredictProjectilePathPointData PointData:Result.PathData) 
		{
			OUTPath.Add(PointData.Location);
		}
		
		FNavLocation NavLoc;
		//Set you NavAgentProps properties here (radius, height, etc)
		if (!bHit) { return false; }
		bool bOnNavmesh = UNavigationSystemV1::GetNavigationSystem(GetWorld())->ProjectPointToNavigation(Result.HitResult.Location, NavLoc, TeleportPorjectionExtetnt);
		if (!bOnNavmesh) { return false; }
		
		OUTLocation = NavLoc.Location;
		
	}
	return true;
}


void AVRCharacter::UpdateBlinkers()
{
	if (RadiusVsVelocity == nullptr) { return; }
	float Speed = GetVelocity().Size();
	float Radius = RadiusVsVelocity->GetFloatValue(Speed);
	BlinkerMaterialInstance->SetScalarParameterValue(TEXT("Radius"), Radius);
	FVector2D Centre = Getblinkercentre();
	BlinkerMaterialInstance->SetVectorParameterValue(TEXT("centre"), FLinearColor(Centre.X,Centre.Y,0));
}

void AVRCharacter::DrawTeleportPath(const TArray<FVector> &Path)
{
	UpdateSplines(Path);
	for (USplineMeshComponent *SplineMesh : TeleportDynamicMeshPool) {
		SplineMesh->SetVisibility(false);
	}
	int32 SegmentNum = Path.Num() - 1;
	for (int32 i = 0;i < SegmentNum;i++)
	{
		if (TeleportDynamicMeshPool.Num() <= i) {
			USplineMeshComponent *SplineMesh = NewObject<USplineMeshComponent>(this);
			SplineMesh->SetMobility(EComponentMobility::Movable);
			SplineMesh->AttachToComponent(TelePath, FAttachmentTransformRules::KeepRelativeTransform);
			SplineMesh->RegisterComponent();
			SplineMesh->SetStaticMesh(TelePortArchMesh);
			SplineMesh->SetMaterial(0, TeleportArchMaterial);
			SplineMesh->RegisterComponent();
			TeleportDynamicMeshPool.Add(SplineMesh);
		}
		USplineMeshComponent *SplineMesh = TeleportDynamicMeshPool[i];
		SplineMesh->SetVisibility(true);
		FVector StartPos, StartTangent, EndPos, EndTangent;
		TelePath->GetLocalLocationAndTangentAtSplinePoint(i, StartPos, StartTangent);
		TelePath->GetLocalLocationAndTangentAtSplinePoint(i+1, EndPos, EndTangent);
		SplineMesh->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
	}
}


void AVRCharacter::UpdateSplines(const TArray<FVector> &Path)
{
	TelePath->ClearSplinePoints(false);
	for (int32 i=0;i<Path.Num();i++)
	{
		FVector LocalPosition = TelePath->GetComponentTransform().InverseTransformPosition(Path[i]);
		FSplinePoint Point(i,LocalPosition,  ESplinePointType::Curve);
		TelePath->AddPoint(Point,false);
	}
	
}



FVector2D AVRCharacter::Getblinkercentre()
{
	FVector MovemntDrection = GetVelocity().GetSafeNormal();
	if (MovemntDrection.IsNearlyZero()) {
		return FVector2D(.5, .5);
	}
	FVector WorldStationaryLocation;
	if(FVector::DotProduct(CameraComp->GetForwardVector(), MovemntDrection) > 0)
	{
		WorldStationaryLocation = CameraComp->GetComponentLocation() + MovemntDrection * 1000; 
	}
	else
	{
		WorldStationaryLocation = CameraComp->GetComponentLocation() - MovemntDrection * 1000;
	}
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC == nullptr)
	{
		return FVector2D(.5, .5);
	}
	FVector2D ScreenStaionarylocation;
	PC->ProjectWorldLocationToScreen
	(
		WorldStationaryLocation,
		ScreenStaionarylocation);
	int32 SizeX, SizeY;
	PC->GetViewportSize(SizeX, SizeY);
	ScreenStaionarylocation.X /= SizeX;
	ScreenStaionarylocation.Y /= SizeY;
	return ScreenStaionarylocation;
}

void AVRCharacter::UpdateDestinationMarker()
{
	TArray<FVector> Path;
	FVector Location;
	bool bHasDestination = FindTeleportDestination(Path,Location);
		
		if (bHasDestination)
		{
			DestinationMarker->SetVisibility(true);
			DestinationMarker->SetWorldLocation(Location);
			DrawTeleportPath(Path);
		}
		else
		{
			DestinationMarker->SetVisibility(false);
			TArray<FVector> EmptyPath;
			DrawTeleportPath(EmptyPath);
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
	FVector Destination = DestinationMarker->GetComponentLocation();
	Destination += GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*GetActorUpVector();
	
	SetActorLocation(Destination);

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



