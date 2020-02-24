// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DataTable_B.generated.h"



UCLASS()
class BRAWLINN_API UDataTable_B : public UDataTable
{
	GENERATED_BODY()
public:
	UDataTable_B();

	static UDataTable_B* CreateDataTable(UScriptStruct* StaticStruct, FString FileName);

	template <class T>
	T* GetRow(FName RowName);
	
};

template<class T>
inline T* UDataTable_B::GetRow(FName RowName)
{
		FString Context;
		return FindRow<T>(RowName, Context, true);
}
