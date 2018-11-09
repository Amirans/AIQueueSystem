// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformMisc.h"

/**
 * Queue Data Type Designed to Be more Efficient for the AI Queue System
 */

template<typename ValueType>
class TAIQueue
{

public:

	/** Default Constructor */
	TAIQueue()
	{
		Head = nullptr; Tail = nullptr;
	}

	/** Destructor */
	~TAIQueue()
	{
		Empty();	
	}
	
	/**
	 * Adds/Enqueues a Value to the Tail of the Queue
	 *
	 * @param InValue - Value to be Added 
	 * @return True if Value was Added, False Otherwise
	 */
	bool Enqueue(const ValueType& InValue)
	{
		/* Create a New One */
		TNode* NewNode = new TNode(InValue);

		/* If Empty Queue Set to Head and Tail */
		if (IsEmpty())
		{
			Head = NewNode;
			Tail = NewNode;
		}
		else
		{
			/* Add Value to the Tail*/
			TNode* OldTail = Tail;
			Tail = NewNode;
			FPlatformMisc::MemoryBarrier();
			OldTail->LastNode = Tail;
		}

		return true;
	}

	/**
	 * Removes/Dequeue the Value at the Head of the Queue
	 *
	 * @param OutValue - Value that is Dequeued
	 * @return True If a Value is Dequeued
	 */
	bool Dequeue(ValueType& OutValue)
	{
		if (IsEmpty())
		{
			return false;
		}

		/* If One Value in Queue Remove Head and Tail */
		if (Tail == Head)
		{
			Tail = nullptr;
			Head = nullptr;
		}
		else
		{
			/* Move the Head to Out Value */
			OutValue = MoveTemp(Head->Value);

			/* Replace Current Head With Head Last Node */
			TNode* OldHead = Head;
			Head = OldHead->LastNode;
			delete OldHead;
		}

		return true;
	}

	/**
	 * Remove a First Value in the Queue That Matches the Given Value
	 *
	 * @note Query Starts From Head to Tail
	 * @param InValue - Value to Remove From the Queue
	 * @return True If the Value was Removed from the Queue
	 */
	bool Remove(const ValueType& InValue)
	{

		TNode* CurrentNode = Head;
		TNode* PerviousNode = nullptr;

		/* Iterate Through While Current Queue is Valid */
		while (CurrentNode != nullptr)
		{
			/* Compare the Current Node Value */
			if (CurrentNode->Value == InValue)
			{
		
				if (PerviousNode != nullptr)
				{
					/** Take out the Node and Reconnect the Neighboring */
					PerviousNode->LastNode = CurrentNode->LastNode;
				}
				else
				{
					/** Value Found At Head */
					Head = CurrentNode->LastNode;
				}

				delete CurrentNode;

				return true;
			}

			PerviousNode = CurrentNode;
			CurrentNode = CurrentNode->LastNode;
		}

		return false;

	}

	/**
	 * Peek at the Head of the Queue without Removing it
	 *
	 * @param OutValue - The Value at the Head of the Queue
	 * @return True If Queue Has a Head Value
	 */
	bool Peek(ValueType& OutValue) const
	{
		/** Prevent Peek at Empty Queue */
		if (IsEmpty())
		{
			return false;
		}

		OutValue = Head->Value;
		return true;
	}

	/**
	 * Empty and Discards the Queue
	 *
	 * @see Pop for Removing/Discarding Single Item
	 */
	void Empty()
	{
		while (Pop());
	}

	/**
	 * Checks If Queue Is Empty
	 *
	 * @return True if Queue is Empty
	 */
	bool IsEmpty() const
	{
		return Head == nullptr;
	}

	/**
	 * Removes a Value from the Head of the Queue
	 *
	 * @return True if a Value was Removed 
	 */
	bool Pop()
	{
		if (Head != nullptr)
		{
			TNode* OldHead = Head;
			Head = OldHead->LastNode;
			delete OldHead;
			return true;
		}

		return false;
	}

	struct TNode
	{
		/** Default Constructor */
		TNode()
			:Value(NULL)
			, LastNode(nullptr)
		{};

		/** Constructor with Value InTake*/
		explicit TNode(const ValueType& NewValue)
			:Value(NewValue)
			, LastNode(nullptr)
		{};

		/** Pointer to the Last Connecting Node */
		TNode* volatile LastNode;

		/** Value of the Node */
		ValueType Value;
	};

private:

	/** Holds a Pointer to the Head of the Queue */
	TNode* volatile Head;

	/** Holds a Pointer to the Tail of the Queue */
	TNode* Tail;

	/** Delete/Hidden Copy Constructor */
	TAIQueue(const TAIQueue&) = delete;

	/** Delete/Hidden Assign Queue */
	TAIQueue& operator=(const TAIQueue&) = delete;

};
