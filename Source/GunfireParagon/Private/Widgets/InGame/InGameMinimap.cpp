// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/InGameMinimap.h"
#include "Components\TextBlock.h"
#include "Components\CanvasPanel.h"
#include "GameFramework\Character.h"
#include "Kismet\GameplayStatics.h"
#include "Blueprint\WidgetTree.h"
#include "Widgets/DataAssets/InGameMinimapDataAsset.h"
#include "Widgets\InGame\IngameMinimapIcon.h"

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
	PlayerCharacter = GetWorld()->GetFirstPlayerController()->GetPawn<ACharacter>();

	if (GetWorld())
	{
		LevelEntryTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
		UE_LOG(LogTemp, Display, TEXT("World Test, %f"),LevelEntryTime);
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandle_UpdateTime, this, &UInGameMinimap::UpdatePlayTime, 1.0f, true);

	UpdatePlayTime();
	UpdateLevelName();
}

void UInGameMinimap::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateAllIcons();
}

void UInGameMinimap::UpdatePlayTime()
{
	if (TimeTextBlock)
	{
		float CurrentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
		float ElapsedTime = CurrentTime - LevelEntryTime;

		int32 Minutes = FMath::FloorToInt32(ElapsedTime / 60.0f);
		int32 Seconds = FMath::FloorToInt32(FMath::Fmod(ElapsedTime, 60.f));

		FString TimeText = FString::Printf(TEXT("%02d분 %02d초"), Minutes, Seconds);
		TimeTextBlock->SetText(FText::FromString(TimeText));
	}
}

void UInGameMinimap::UpdateLevelName()
{
	FString CurrentLevel = GetWorld()->GetMapName();
	CurrentLevel.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

	MapNameTextBlock->SetText(FText::FromString(CurrentLevel));
}


void UInGameMinimap::AddMinimapIcon(ACharacter* Target)
{
	if (!Target || ActiveIcons.Contains(Target) || !MinimapIconClass) return;

	UIngameMinimapIcon* NewIcon = CreateWidget<UIngameMinimapIcon>(this, MinimapIconClass);
	if (NewIcon)
	{
		RenderCanvas->AddChild(NewIcon);
		ActiveIcons.Add(Target, NewIcon);

		if (Target == PlayerCharacter)
		{
			NewIcon->SetIconTexture(PlayerIconTexture);
		}
		else
		{
			NewIcon->SetIconTexture(EnemyIconTexture);
		}
		int32 ChildCount = RenderCanvas->GetChildrenCount();

		UpdateActorIcon(Target, Target->GetActorLocation());
	}
}

void UInGameMinimap::RemoveMinimapIcon(ACharacter* Target)
{
	if (UIngameMinimapIcon** IconPtr = ActiveIcons.Find(Target))
	{
		if (UIngameMinimapIcon* Icon = *IconPtr)
		{
			Icon->RemoveFromParent();
		}
		ActiveIcons.Remove(Target);
	}
}

void UInGameMinimap::UpdateActorIcon(ACharacter* RenderTarget, FVector WorldLocation)
{
	if (!RenderTarget || !RenderCanvas) return;

	FVector2D MinimapPosition = IconRenderPosition(WorldLocation);

	if (ActiveIcons.Contains(RenderTarget))
	{
		UIngameMinimapIcon* Icon = ActiveIcons[RenderTarget];

		Icon->SetRenderTranslation(MinimapPosition);
		float Distance = FVector::Dist(PlayerCharacter->GetActorLocation(), WorldLocation);
		Icon->SetVisibilityBasedOnDistance(Distance, MaxRenderDistance);
	}
}

void UInGameMinimap::UpdateAllIcons()
{
	for (auto& Elem : ActiveIcons)
	{
		ACharacter* Target = Elem.Key;
		UIngameMinimapIcon* Icon = Elem.Value;

		if (Target && Icon)
		{
			UpdateActorIcon(Target, Target->GetActorLocation());
		}
	}
}

FVector2D UInGameMinimap::IconRenderPosition(FVector WorldLocation)
{
	//if (!PlayerCharacter || !RenderCanvas) return FVector2D::ZeroVector;

	//FVector Offset = WorldLocation - PlayerCharacter->GetActorLocation();
	//Offset *= MinimapScale;

	//FRotator PlayerRotation = PlayerCharacter->GetActorRotation();
	//float YawRad = FMath::DegreesToRadians(PlayerRotation.Yaw);

	//float CosYaw = FMath::Cos(YawRad);
	//float SinYaw = FMath::Sin(YawRad);
	//float RotatedX = Offset.X * CosYaw + Offset.Y * SinYaw;
	//float RotatedY = Offset.Y * CosYaw - Offset.X * SinYaw;

	//FVector2D MinimapCenter = FVector2D(RenderCanvas->GetCachedGeometry().GetLocalSize()) * 0.5f;
	//FVector2D MinimapPosition = MinimapCenter + FVector2D(RotatedX, -RotatedY);


	//return MinimapPosition;


	if (!PlayerCharacter || !RenderCanvas) return FVector2D::ZeroVector;

	FVector Offset = WorldLocation - PlayerCharacter->GetActorLocation();
	Offset *= MinimapScale;

	FVector Forward = PlayerCharacter->GetActorForwardVector(); 
	FVector Right = PlayerCharacter->GetActorRightVector(); 

	float RotatedX = FVector::DotProduct(Offset, Forward);
	float RotatedY = FVector::DotProduct(Offset, Right);

	FVector2D MinimapCenter = FVector2D(RenderCanvas->GetCachedGeometry().GetLocalSize()) * 0.5f;
	FVector2D MinimapPosition = MinimapCenter + FVector2D(RotatedY, -RotatedX);

	UE_LOG(LogTemp, Warning, TEXT("WorldLocation: (%.1f, %.1f) -> MinimapPosition: (%.1f, %.1f)"),
		WorldLocation.X, WorldLocation.Y, MinimapPosition.X, MinimapPosition.Y);

	return MinimapPosition;
}
