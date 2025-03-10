// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InGame/CardWidget.h"
#include "Components\Image.h"
#include "Components\TextBlock.h"
#include "Components\Button.h"
#include "Engine\DataTable.h"
#include "Player\PlayerCharacter.h"
#include "Kismet\GameplayStatics.h"

void UCardWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	FindCardByEnum(ECardEffectType::AttackPowerIncrease, ECardRarity::Common);
	if (SelectButton && !SelectButton->OnClicked.IsBound())
	{
		SelectButton->OnClicked.AddDynamic(this, &UCardWidget::ActivateObject);
	}

	GradeValue = 0;
	ApplyCardType = ECardEffectType::AttackPowerIncrease;
	ApplyValue = 0.f;
}

void UCardWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCardWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UCardWidget::FindCardByEnum(ECardEffectType CardType, ECardRarity CardGrade)
{
	if (!UICardInfoDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("UICardDataTable is Null"));
		return;
	}

	FString ContextString;
	TArray<FName> RowNames = UICardInfoDataTable->GetRowNames();
	GradeValue = (int32)CardGrade + 1;

	for (const FName& RowName : RowNames)
	{
		FUIObjectBaseData* RowData = UICardInfoDataTable->FindRow<FUIObjectBaseData>(RowName, ContextString);

		if (RowData && RowData->CardType == CardType)
		{
			UpdateCardUI(*RowData);
			ApplyCardType = CardType;
			ApplyValue = RowData->IncreaseAmount * GradeValue;
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("FindCardByEnum :: No Matching CardType Found"));
}

void UCardWidget::UpdateCardUI(const FUIObjectBaseData& Data)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(Data.CardName));
	}

	if (DescriptionText)
	{
		float FinalValue = Data.IncreaseAmount * GradeValue;
		FString UpdatedDescription = Data.Description.Replace(TEXT("{Value}"), *FString::Printf(TEXT("%.1f"), FinalValue));

		DescriptionText->SetText(FText::FromString(UpdatedDescription));
	}

	if (ObjectImage && Data.Texture)
	{
		ObjectImage->SetBrushFromTexture(Data.Texture);
	}
}

void UCardWidget::ActivateObject()
{
	UE_LOG(LogTemp, Warning, TEXT("Button Test"));
	// PlayerInstance에게 넘겨주기

	if(APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0)))
	{
		FCardEffect ReturnCardEffect(ApplyCardType, EEffectValueType::FlatValue, ApplyValue);
		PlayerCharacter->ApplyCardEffect(ReturnCardEffect);
	}

	SelectWidget.Broadcast();
}