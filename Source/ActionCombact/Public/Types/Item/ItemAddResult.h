#pragma once

#include "CoreMinimal.h"
#include "ItemAddResult.generated.h"

UENUM(BlueprintType)
enum class EItemAddResult :uint8
{
	IAR_NoItemAdded UMETA(DisplayName = "No item added"),
	IAR_PartialAmountItemAdded UMETA(DisplayName = "Partial amount of item added"),
	IAR_AllItemAdded UMETA(DisplayName = "All of item added"),
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_BODY()

	FItemAddResult() :
		ActualAmountAdded(0),
		OperationResult(EItemAddResult::IAR_NoItemAdded),
		ResultMessage(FText::GetEmpty())
	{
	}

	FItemAddResult(int32 Amount, EItemAddResult Result, const FText& Text) :
		ActualAmountAdded(Amount),
		OperationResult(Result),
		ResultMessage(Text)
	{
	}

	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	int32 ActualAmountAdded;

	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	EItemAddResult OperationResult;

	UPROPERTY(BlueprintReadOnly, Category = "Item Add Result")
	FText ResultMessage;

	static FItemAddResult AddedNone(const FText& ErrorText)
	{
		return FItemAddResult{ 0, EItemAddResult::IAR_NoItemAdded, ErrorText };
	}
	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorText)
	{
		return FItemAddResult{ PartialAmountAdded, EItemAddResult::IAR_PartialAmountItemAdded, ErrorText };
	}
	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message)
	{
		return FItemAddResult{ AmountAdded, EItemAddResult::IAR_AllItemAdded, Message };
	}
};