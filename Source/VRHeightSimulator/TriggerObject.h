// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToggleableObject.h"
#include "TriggerObject.generated.h"

/**
 * 
 */
UCLASS()
class VRHEIGHTSIMULATOR_API ATriggerObject : public AToggleableObject
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere,BlueprintReadWrite)
    TArray<AToggleableObject*> outputs;

    void SetState(bool newState) override;

    void SetHighlightStatus(bool newStatus) override;
};
