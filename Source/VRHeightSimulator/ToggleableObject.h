// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
#include "ToggleableObject.generated.h"

/**
 * 
 */
UCLASS()
class VRHEIGHTSIMULATOR_API AToggleableObject : public AInteractableObject
{
	GENERATED_BODY()
public:
	void PickUp(USceneComponent* newParent) override;

	//Release only functions if the object can be picked up
	void Release() override {
		if (isPickupable) {
			Super::Release();
		}
	}

	/**
	Set the state of this object
	@param the new state
	*/
	virtual void SetState(bool newState) {
		state = newState;

		switch (state) {
		case true:
			OnTrueState(); break;
		default:
			OnFalseState(); break;
		}
	}

	UFUNCTION(BlueprintCallable)
		bool GetState() { return state; }

protected:
	UFUNCTION(BlueprintImplementableEvent)
		void OnTrueState();

	UFUNCTION(BlueprintImplementableEvent)
		void OnFalseState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isPickupable = false;

private:
	bool state = false;
};