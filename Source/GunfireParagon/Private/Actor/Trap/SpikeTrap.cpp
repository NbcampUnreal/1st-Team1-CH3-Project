#include "Actor/Trap/SpikeTrap.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ASpikeTrap::ASpikeTrap()
{
    PrimaryActorTick.bCanEverTick = false;

    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));

     GridSize = 50.0f;  

    for (int i = 0; i < 9; i++)
    {
        FString SpikeName = FString::Printf(TEXT("SpikeMesh_%d"), i);
        UStaticMeshComponent* NewSpike = CreateDefaultSubobject<UStaticMeshComponent>(*SpikeName);
        
        NewSpike->SetupAttachment(RootComponent);

      
        float XOffset = (i % 3) * GridSize - GridSize;  
        float YOffset = (i / 3) * GridSize - GridSize;  

        NewSpike->SetRelativeLocation(FVector(XOffset, YOffset, 0.0f)); 
        NewSpike->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
        NewSpike->OnComponentHit.AddDynamic(this, &ASpikeTrap::OnSpikeHit);  

        Spikes.Add(NewSpike);
    }
    Startheight = GetActorLocation();
}

void ASpikeTrap::BeginPlay()
{
    Super::BeginPlay();

   
    GetWorldTimerManager().SetTimer(SpikeTimerHandle, this, &ASpikeTrap::StartTrapSequence, InitialDelay, false);
}

void ASpikeTrap::StartTrapSequence()
{
    
    ShakeCount = 0;
    GetWorldTimerManager().SetTimer(ShakeTimerHandle, this, &ASpikeTrap::ShakeSpikes, 0.1f, true);
}


void ASpikeTrap::ShakeSpikes()
{
    if (ShakeCount >= (ShakeDuration / 0.1f)) 
    {
        GetWorldTimerManager().ClearTimer(ShakeTimerHandle);
        RaiseSpikes(); 
        return;
    }

    for (UStaticMeshComponent* Spike : Spikes)
    {
        if (Spike)
        {
            FVector CurrentLocation = Spike->GetRelativeLocation();
            float Offset = FMath::RandRange(-ShakeIntensity, ShakeIntensity);  
            CurrentLocation.Z += Offset;
            Spike->SetRelativeLocation(CurrentLocation);
        }
    }

    ShakeCount++;
}

void ASpikeTrap::RaiseSpikes()
{
    bHasDealtDamage = false; 
    for (UStaticMeshComponent* Spike : Spikes)
    {
        if (Spike)
        {
            FVector NewLocation = Spike->GetRelativeLocation();
            NewLocation.Z += SpikeHeight;  
            Spike->SetRelativeLocation(NewLocation);
            Spike->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);  
        }
    }

    bIsSpikesUp = true;

    GetWorldTimerManager().SetTimer(SpikeTimerHandle, this, &ASpikeTrap::LowerSpikes, RetractDelay, false);
}

void ASpikeTrap::LowerSpikes()
{

    for (UStaticMeshComponent* Spike : Spikes)
    {
        if (Spike)
        {
            FVector NewLocation = Spike->GetRelativeLocation();
            NewLocation.Z = Startheight.Z-50.0f;  
            Spike->SetRelativeLocation(NewLocation);
            Spike->SetCollisionEnabled(ECollisionEnabled::NoCollision);  
        }
    }

    bIsSpikesUp = false;

    GetWorldTimerManager().SetTimer(SpikeTimerHandle, this, &ASpikeTrap::StartTrapSequence, ActivationDelay, false);
}
void ASpikeTrap::OnSpikeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor->ActorHasTag("Player") && bIsSpikesUp && !bHasDealtDamage)  
    {
        bHasDealtDamage = true; 
        UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, nullptr, this, nullptr);
    }
}