// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HandController.h"

#include "Grabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ARCITUCTUREEXPLORER_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	
	class UInputComponent* InputComponent = nullptr;
	void Grab();
	void Release();
	class UPhysicsHandleComponent* Physcs = nullptr;
	void FindPsicsHandleComponent();
	void SetUpInputComponent();
	
	float Reach = 1000;
	const FHitResult GetFirstPhycisBodyInReach();
	FVector GetReachLineStart();
	FVector GetReachLineEnd();
	UPROPERTY(EditDefaultsOnly)
	class UHapticFeedbackEffect_Base* HapticEffect;
	UPROPERTY(EditDefaultsOnly)
	class UMotionControllerComponent* MotionController;

	/*class AVRCharacter* Charcter;*/
// 	UPROPERTY(EditDefaultsOnly)
// 		TSubclassOf<AHandController> HandControllerClass;
// 
// 	UPROPERTY()
// 		AHandController* RightHandController;

// 	UPROPERTY()
// 		class ACharacter* Charcter;
};
