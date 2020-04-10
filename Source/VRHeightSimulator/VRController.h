// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "MotionControllerComponent.h"
#include "Components/BoxComponent.h"
#include "VRController.generated.h"

//declare event for teleportation
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeleport, const FVector&, teleportLocation);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VRHEIGHTSIMULATOR_API UVRController : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVRController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	const int TeleportMaxProjectionDistance = 10;

	AActor* hoverActor = nullptr;
	APawn* parentActor = Cast<APawn>(GetAttachmentRootActor());

	//cannot teleport to the origin
	FVector teleportTarget = FVector::ZeroVector;

	//TSubclassOf<AVRPawn> parent;

	void ConnectPointsWithLine(const TArray<FVector>& points, const FColor& lineColor, float thickness = 5.0f);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Be sure to call Init immediately after construction!
	void Init(const FName& trackingSource);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UMotionControllerComponent* Controller;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* ControllerDetect;

	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Grab();
	void Release();
	void BeginTeleport();

	enum TeleportStatus { None, Request, Confirm };
	TeleportStatus requestStatus = TeleportStatus::None;

	void RequestTeleport() {
		requestStatus = TeleportStatus::Request;
	}

	void ConfirmTeleport() {
		requestStatus = TeleportStatus::Confirm;
	}

	FVector TeleportTarget = FVector::ZeroVector;

	//declare delegate for teleportation
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTeleport Teleport;

};
