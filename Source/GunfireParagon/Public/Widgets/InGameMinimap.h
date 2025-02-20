// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMinimap.generated.h"

/**
 * 
 */

class UTextBlock;

UCLASS()
class GUNFIREPARAGON_API UInGameMinimap : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void UpdatePlayTime();
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MapNameTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeTextBlock;

	// TODO : MonsterBase Actor 변경
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TArray<TSubclassOf<AActor>> Enemy;

	// TODO : PlayerBase Actor 변경
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Minimap")
	TSubclassOf<AActor> Player;

	// TODO : Monster Texture, Potal Texture, Player Texture 고려해보기. 각각의 위젯클래스로 구현할건지 어떻게 할건지
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Textures")
	class UInGameMinimapDataAsset* CurrentTextures;

private:
	float LevelEntryTime;

	FTimerHandle TimerHandle_UpdateTime;
};
