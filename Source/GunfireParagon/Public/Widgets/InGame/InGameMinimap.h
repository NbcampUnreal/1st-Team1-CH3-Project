// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMinimap.generated.h"

/**
 * 
 */

class UTextBlock;
class UIngameMinimapIcon;
class UCanvasPanel;

UCLASS()
class GUNFIREPARAGON_API UInGameMinimap : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void UpdatePlayTime();
	UFUNCTION()
	void UpdateLevelName();

	UFUNCTION()
	void AddMinimapIcon(ACharacter* Target);
	UFUNCTION()
	void RemoveMinimapIcon(ACharacter* Target);
	UFUNCTION()
	void UpdateActorIcon(ACharacter* RenderTarget, FVector WorldLocation);

	TMap<ACharacter*, UIngameMinimapIcon*> ActiveIcons;
	void UpdateAllIcons();
protected:
	
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* RenderCanvas;
	
	UPROPERTY()
	ACharacter* PlayerCharacter;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MapNameTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TimeTextBlock;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Textures")
	class UInGameMinimapDataAsset* CurrentTextures;

	// Icons
	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	UTexture2D* PlayerIconTexture;

	UPROPERTY(EditDefaultsOnly, Category = "Minimap")
	UTexture2D* EnemyIconTexture;
	
	UPROPERTY(EditAnywhere, Category = "Minimap")
	TSubclassOf<UIngameMinimapIcon> MinimapIconClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float MinimapScale = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	float MaxRenderDistance = 1600.f;

private:
	FVector2D IconRenderPosition(FVector WorldLocation);

	float LevelEntryTime;

	FTimerHandle TimerHandle_UpdateTime;

	
};
