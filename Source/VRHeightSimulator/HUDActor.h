// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "HUDActor.generated.h"


UCLASS()
class VRHEIGHTSIMULATOR_API AHUDActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHUDActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetComponent* UIWidget;

	UFUNCTION(BlueprintCallable, Category = "Actor")
	virtual void DecrementHit();

	UFUNCTION(BlueprintCallable, Category = "Actor")
	virtual void IncrementHit();

};
