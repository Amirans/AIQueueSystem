// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "QueueCandidateComponent.generated.h"

class AQueueManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCandidateQueueChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCandidateQueueSlotChanged,FTransform,NewSlotTransform);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AIQUEUESYSTEM_API UQueueCandidateComponent : public USceneComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UQueueCandidateComponent();

	/** Delegate to Notify Queue Changed */
	UPROPERTY(BlueprintAssignable, Category = QueueCandidate)
		FOnCandidateQueueChanged OnCandidateQueueChanged;

	/** Delegate to Notify Candidate Slot on Queue Has Changed */
	UPROPERTY(BlueprintAssignable, Category = QueueCandidate)
		FOnCandidateQueueSlotChanged OnCandidateQueueSlotChanged;

	/** Tried to Add this Candidate to a Queue
	 * @param QueueManager  - Queue to Add this Candidate to
	 * @return True if Candidate is Added to the Queue
	 */
	UFUNCTION(BlueprintCallable, Category = QueueCandidate)
		bool TryRegiserQueue(AQueueManager* NewQueueManager);

	/** invoked By Queue Manage When this Candidate is Added to a Queue
	 * @param QueueManager - Queue Manager that This Candidate is a Part Of
	 */
	UFUNCTION(Category = QueueCandidate)
		void OnAddedToQueue(AQueueManager* NewQueueManager);

	/** Removes the Candidate From the Current Queue
	 * @note Removing Will Not Provide New Position
	 */
	UFUNCTION(BlueprintCallable, Category = QueueCandidate)
		void RemoveFromQueue();

	/** Invoked By Queue Manager When This Candidate is Removed from the Queue
	 * @note Removing Will not Provide New Position
	 */
	UFUNCTION(Category = QueueCandidate)
		void OnRemovedFromQueue();

	/** Invoked By Queue Manager to Update New Slot
	 * @param NewTransform - Updated Transform of the Candidate
	 */
	UFUNCTION(Category = QueueCandidate)
		void UpdateQueueSlot(FTransform NewSlotTransform, const AQueueManager* InstigatorQueue);

	/** Returns True if the Candidate has a Queue */
	UFUNCTION(BlueprintPure, Category = QueueCandidate)
		bool IsInQueue() const { return QueueManager != nullptr; };

	/** Returns the Current Queue Manager, Nullptr If not Part of a Queue */
	UFUNCTION(BlueprintGetter, Category = QueueCandidate)
		AQueueManager* GetQueueManager() const { return QueueManager; };
	
	/** Get Queue Slot Transform
	 * @note If Not Part of the Queue Will return Component Owner Transform
	 * @return Slot Transform
	 */
	UFUNCTION(BlueprintCallable, Category = QueueCandidate)
		FTransform GetSlotTransform() const;

	/** Get Queue Slot Index for the Candidate
	 * @return Queue Slot Index or -1 If not Part of a Queue
	 */
	UFUNCTION(BlueprintCallable, Category = QueueCandidate)
		int32 GetQueueSlotIndex() const;

protected:

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called When the Component End Play
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	/** Current Queue Manager that the Candidate is Part of */
	UPROPERTY(Category = QueueCandidate, BlueprintGetter = GetQueueManager)
		AQueueManager* QueueManager;

	UPROPERTY(Category = QueueCandiadte, BlueprintGetter = GetSlotTransform)
		FTransform SlotTransform;
};
