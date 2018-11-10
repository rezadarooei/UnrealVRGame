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
	//camera for character
	UPROPERTY()
	class UCameraComponent* CameraComp;

	UPROPERTY()
	class USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere)
	class USplineComponent* TelePath;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* DestinationMarker;

	UPROPERTY(EditAnywhere)
	float TeleportProjctileSpeed = 800;


	UPROPERTY(EditAnywhere)
	float MaxSimulationTime = 1;

	//person fade after 1 sec
	UPROPERTY(EditAnywhere)
	float TeleportFadeTime = 1.f;

	UPROPERTY()
	APlayerCameraManager * PlayerCameraManager;

	FTimerHandle FadeTimerHandle;

	UPROPERTY(EditAnywhere)
	FVector TeleportPorjectionExtetnt=FVector(100, 100, 100);

	UPROPERTY()
	class UPostProcessComponent* PostProcessComponent;

	//MaterialInterface is like normal material, but it can be used to create Dynamic material
	UPROPERTY(EditAnywhere)
	class UMaterialInterface * BlinkerMaterialBase;
	//MaterialInstanceDynamic is the final material we apply into our mesh
	UPROPERTY()
	class UMaterialInstanceDynamic* BlinkerMaterialInstance;

	UPROPERTY(EditAnywhere)
	class UCurveFloat* RadiusVsVelocity;
 
  	UPROPERTY()
  	class AHandController* LeftHandController;
  
  
  
  	UPROPERTY()
  	class AHandController* RightHandController;
  
  	

	UPROPERTY(EditAnywhere)
	float TeleportProjctileRadius = 10;

	//
	UPROPERTY()
	TArray<class USplineMeshComponent*> TeleportDynamicMeshPool;

 	UPROPERTY(EditDefaultsOnly)
 	class UStaticMesh* TelePortArchMesh;
 
 	UPROPERTY(EditDefaultsOnly)
 	class UMaterialInterface* TeleportArchMaterial;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AHandController> HandControllerClass;

private:
	void MoveForward(float value);
	void MoveRight(float value);
	void BeginTeleport();
	void EndTeleport();
	bool FindTeleportDestination(TArray<FVector> &OUTPath, FVector &OUTLocation);
	void StartFade(float FromAlpha, float ToAlpha);
	void UpdateBlinkers();
	//it give us blinker center because it changes when we want move around
	FVector2D Getblinkercentre();
	void UpdateSplines(const TArray<FVector> &Path);
	void DrawTeleportPath(const TArray<FVector> &Path);
};
