// Fill out your copyright notice in the Description page of Project Settings.

#include "DataTable_B.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

UDataTable_B::UDataTable_B()
{
}

void UDataTable_B::LoadCSVFile(UScriptStruct* StaticStruct, FString FileName)
{

	RowStruct = StaticStruct;
	bIgnoreExtraFields = true;

	FString File;
	FString FilePath = FString(FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) + FString("Data/") + FileName);
	bool success = FFileHelper::LoadFileToString(File, *FilePath);
	check(success == true && "Remember to close Excel before you press play!");

	CreateTableFromCSVString(File);

}