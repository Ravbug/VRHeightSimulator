// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Components/BoxComponent.h"
#include "VRController.h"
#include "VRPawn.generated.h"

#define SCREENPRINT(a,b) (GEngine->AddOnScreenDebugMessage(-1, 0.35f, FColor::Red, FString::Printf(TEXT(a),b)))
#define SCREENPRINT2(a) (GEngine->AddOnScreenDebugMessage(-1, 0.35f, FColor::Red, FString::Printf(TEXT(a))))


UCLASS()
class VRHEIGHTSIMULATOR_API AVRPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVRController* LeftHandController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVRController* RightHandController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* root;

	UFUNCTION()
	void VRControllerEndTeleport(const FVector& newPos);

	//VR Controller input events
	//see the Input tab of the Project Settings -> Input to see where these are defined
	// the VR template project has many pre-defined for different VR setups

	void VRControllerGrabLeft();
	void VRControllerGrabRight();
	void VRControllerReleaseLeft();
	void VRControllerReleaseRight();

	void VRControllerRequestTeleportRight();
	void VRControllerRequestTeleportLeft();
	void VRControllerConfirmTeleportRight();
	void VRControllerConfirmTeleportLeft();

private:
	
};
