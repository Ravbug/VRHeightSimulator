// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "InteractableObject.generated.h"

UCLASS()
class VRHEIGHTSIMULATOR_API AInteractableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SimpleDisplay)
	UStaticMeshComponent* rootMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetComponent* InteractionLabel_A;

	//for interacting with this physics object
	virtual void PickUp(USceneComponent* newParent);
	virtual void Release();

	virtual void SetHighlightStatus(bool newStatus);

};
