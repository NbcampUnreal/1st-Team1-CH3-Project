// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGameMinimap.h"
#include "Components\TextBlock.h"
#include "Kismet\GameplayStatics.h"
#include "Widgets/DataAssets/InGameMinimapDataAsset.h"

void UInGameMinimap::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (!CurrentTextures)
	{
		UE_LOG(LogTemp, Warning, TEXT("Minimap Textures Is Missing"));
	}
}

void UInGameMinimap::NativeConstruct()
{
	Super::NativeConstruct();

	if (GetWorld())
	{

		LevelEntryTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
		UE_LOG(LogTemp, Display, TEXT("World Test, %f"),LevelEntryTime);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_UpdateTime, this, &UInGameMinimap::UpdatePlayTime, 1.0f, true);

	UpdatePlayTime();
}

void UInGameMinimap::UpdatePlayTime()
{
	if (TimeTextBlock)
	{
		UE_LOG(LogTemp, Display, TEXT("Time Test"));

		float CurrentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
		float ElapsedTime = CurrentTime - LevelEntryTime;

		int32 Minutes = FMath::FloorToInt32(ElapsedTime / 60.0f);
		int32 Seconds = FMath::FloorToInt32(FMath::Fmod(ElapsedTime, 60.f));

		FString TimeText = FString::Printf(TEXT("%02d분 %02d초"), Minutes, Seconds);
		TimeTextBlock->SetText(FText::FromString(TimeText));
	}
}
