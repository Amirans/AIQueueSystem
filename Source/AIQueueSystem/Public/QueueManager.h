// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QueueManager.generated.h"

class USplineComponent;
class UQueueCandidateComponent;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQueueCandidateUpdated, bool, bNewCandidate);

UCLASS()
class AIQUEUESYSTEM_API AQueueManager : public AActor
{
	GENERATED_UCLASS_BODY()

public:	

	/** Delegate to Broadcast When a Queue Candidate is Added or Removed */
	UPROPERTY(BlueprintAssignable, Category = QueueManager)
		FOnQueueCandidateUpdated OnQueueCandidateUpdated;

	/** Regenerates the Queue Slots */
	UFUNCTION(BlueprintCallable, Category = QueueManager)
		void GenerateQueueSlots();

	/** Tries to Add a Candidate to the Queue
	* @param QueueCandidateComp - The Queue Candidate Component to Add to Queue
	* @param bForceRegister - Whether Should Add even If Candidate is Already in the Queue, Will Result in re-position
	* @return True if Candidate was Added to Queue Successfully
	*/
	UFUNCTION(BlueprintCallable, Category = QueueManager)
		bool TryRegisterCandidate(UQueueCandidateComponent* QueueCandidateComp, const bool bForceRegister = false);

	/** Removes a Candidate from the Queue
	* @param QueueCandidateComp - Queue Candidate to Remove from the Queue
	*/
	UFUNCTION(BlueprintCallable, Category = QueueManager)
		void DeregisterCandidate(UQueueCandidateComponent* QueueCandidateComp);

	/** Handles and Removes Dirty Queue Candidate
	 * @note This Method Will not Notify Candidate of Removal, Use Deregister for Proper Dequeue
	 * @param QueueCandidate - Queue Candidate to Remove
	 * @return If Queue Candidate was Removed
	 */
	UFUNCTION(BlueprintCallable, Category = QueueManager)
		bool HandleDirtyCandidate(UQueueCandidateComponent* QueueCandidateComp);

	/** Gets Candidate Slot in the Queue
	 * @return Candidate Slot Index or -1 If not Part of the Queue
	 */
	UFUNCTION(BlueprintPure, Category = QueueManager)
		int32 GetCandidateSlotIndex(const UQueueCandidateComponent* QueueCandidateComp) const;

	/** Returns the Transform at a Slot Index */
	UFUNCTION(BlueprintPure, Category = QueueManager)
		FTransform GetTransformAtSlot(const int32 SlotIndex) const;

	/** Returns True If Can Accept New Candidate */
	UFUNCTION(BlueprintPure, Category = QueueManager)
		bool CanAcceptCandidate() const;

	/** Getter for Number of Slots */
	UFUNCTION(BlueprintGetter, Category = QueueManager)
		int32 GetSlotNum() const { return SlotNum; };

	/** Getter for Queue Candidates */
	UFUNCTION(BlueprintGetter, Category = QueueManager)
		TArray<UQueueCandidateComponent*> GetQueueCandidates() const { return QueueCandidates; };

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Spline Component for Queue */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = Spline)
		USplineComponent* QueueSplineComp;

	/** Length of Each Slot */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = QueueManager)
		float SlotLength;

	/** Offset Between Each Slot , Excluding the Last/End Slot */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = QueueManager)
		float SlotOffsetLength;

	/** Iterates Through and Updates Queue Candidates
	 * @note Will Only Update Dirty Candidates
	 */
	UFUNCTION(BlueprintCallable, Category = QueueManager)
		void UpdateQueueCandidatesSlot();

private:

	/** Number of Slots in the Queue */
	UPROPERTY(Category = QueueManager, BlueprintGetter = GetSlotNum)
		int32 SlotNum;

	/** Queue Candidates Currently in Queue */
	UPROPERTY(Category = QueueManager, BlueprintGetter = GetQueueCandidates)
		TArray<UQueueCandidateComponent*> QueueCandidates;

	/** Updates Individual Candidate Slot
	* @note Should not be Called Directly
	*/
	UFUNCTION(Category = QueueManager)
	void UpdateCandidateSlot(UQueueCandidateComponent* UpdatingCandidate,const int32& CandidateIndex);

};
