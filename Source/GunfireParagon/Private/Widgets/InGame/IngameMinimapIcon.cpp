// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/IngameMinimapIcon.h"
#include "Kismet\GameplayStatics.h"
#include "Components\Image.h"
#include "Components\CanvasPanel.h"
#include "Components\CanvasPanelSlot.h"

void UIngameMinimapIcon::NativeConstruct()
{
	Super::NativeConstruct();

}

void UIngameMinimapIcon::NativeDestruct()
{
	Super::NativeDestruct();

	IconImage = nullptr;
}

void UIngameMinimapIcon::SetWorldLocation(FVector2D RenderPosition)
{
	SetRenderTranslation(RenderPosition);
	UE_LOG(LogTemp, Warning, TEXT("Render Position : %.1f, %.1f"),RenderPosition.X, RenderPosition.Y);
}

void UIngameMinimapIcon::SetIconTexture(UTexture2D* Texture)
{
	if (IconImage)
	{
		IconImage->SetBrushFromTexture(Texture, true);
	}
}

void UIngameMinimapIcon::SetVisibilityBasedOnDistance(float Distance, float MaxDistance)
{
	SetVisibility(Distance > MaxDistance ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
}

