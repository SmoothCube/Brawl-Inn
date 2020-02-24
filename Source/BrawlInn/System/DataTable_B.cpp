// Fill out your copyright notice in the Description page of Project Settings.

#include "DataTable_B.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

UDataTable_B::UDataTable_B()
{
}

UDataTable_B* UDataTable_B::CreateDataTable(UScriptStruct* StaticStruct, FString FileName)
{
	UDataTable_B* Table = NewObject<UDataTable_B>();
	Table->RowStruct = StaticStruct;
	Table->bIgnoreExtraFields = true;

	static FString File;
	static FString FilePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) + FString("Data/") + FileName);
	bool success = FFileHelper::LoadFileToString(File, *FilePath);
	check(success == true && "Remember to close Excel before you press play!");

	Table->CreateTableFromCSVString(File);
	return Table;
}