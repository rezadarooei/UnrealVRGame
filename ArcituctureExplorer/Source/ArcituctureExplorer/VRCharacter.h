// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VRCharacter.generated.h"

UCLASS()
class ARCITUCTUREEXPLORER_API AVRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	

	void UpdateDestinationMarker();

private:
	UPROPERTY()
	class UCameraComponent* CameraComp;

	UPROPERTY()
	class USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* DestinationMarker;

	UPROPERTY(EditAnywhere)
	float MaxTeleportDistance = 1000;

	UPROPERTY(EditAnywhere)
	float TeleportFadeTime = 1.f;

	UPROPERTY()
	APlayerCameraManager * PlayerCameraManager;

	FTimerHandle FadeTimerHandle;

	UPROPERTY(EditAnywhere)
	FVector TeleportPorjectionExtetnt=FVector(100, 100, 100);

	UPROPERTY()
	class UPostProcessComponent* PostProcessComponent;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface * BlinkerMaterialBase;

	UPROPERTY()
	class UMaterialInstanceDynamic* BlinkerMaterialInstance;

	UPROPERTY(EditAnywhere)
	class UCurveFloat* RadiusVsVelocity;


private:
	void MoveForward(float value);
	void MoveRight(float value);
	void BeginTeleport();
	void EndTeleport();
	bool FindTeleportDestination(FVector &OUTLocation);
	void StartFade(float FromAlpha, float ToAlpha);
	void UpdateBlinkers();
	FVector2D Getblinkercentre();
};
