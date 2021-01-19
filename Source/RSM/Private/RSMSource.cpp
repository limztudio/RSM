// Fill out your copyright notice in the Description page of Project Settings.


#include "RSMSource.h"
#include "Components/ArrowComponent.h"
#include "RSMSourceComponent.h"


ARSMSource::ARSMSource(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<URSMSourceComponent>(TEXT("LightComponent0")))
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		FName ID_Lighting;
		FText NAME_Lighting;
		FConstructorStatics()
			: ID_Lighting(TEXT("Lighting"))
			, NAME_Lighting(NSLOCTEXT("SpriteCategory", "Lighting", "Lighting"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	RSMSourceComponent = CastChecked<URSMSourceComponent>(GetLightComponent());
	RSMSourceComponent->Mobility = EComponentMobility::Stationary;
	RSMSourceComponent->SetRelativeRotation(FRotator(-90, 0, 0));

	RootComponent = RSMSourceComponent;

#if WITH_EDITORONLY_DATA
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent0"));
	if(ArrowComponent)
	{
		ArrowComponent->ArrowColor = GetLightColor().ToFColor(true);
		ArrowComponent->bTreatAsASprite = true;
		ArrowComponent->SpriteInfo.Category = ConstructorStatics.ID_Lighting;
		ArrowComponent->SpriteInfo.DisplayName = ConstructorStatics.NAME_Lighting;
		ArrowComponent->SetupAttachment(RSMSourceComponent);
		ArrowComponent->bLightAttachment = true;
		ArrowComponent->bIsScreenSizeScaled = true;
	}
#endif // WITH_EDITORONLY_DATA
}

void ARSMSource::PostLoad()
{
	Super::PostLoad();

	if(GetLightComponent()->Mobility == EComponentMobility::Static)
	{
		GetLightComponent()->LightFunctionMaterial = NULL;
	}

#if WITH_EDITORONLY_DATA
	if(ArrowComponent)
	{
		ArrowComponent->ArrowColor = GetLightColor().ToFColor(true);
	}
#endif
}

#if WITH_EDITOR
void ARSMSource::LoadedFromAnotherClass(const FName& OldClassName)
{
	Super::LoadedFromAnotherClass(OldClassName);

	if(GetLinkerUE4Version() < VER_UE4_REMOVE_LIGHT_MOBILITY_CLASSES)
	{
		static FName SpotLightStatic_NAME(TEXT("SpotLightStatic"));
		static FName SpotLightMovable_NAME(TEXT("SpotLightMovable"));
		static FName SpotLightStationary_NAME(TEXT("SpotLightStationary"));

		check(GetLightComponent() != NULL);

		if(OldClassName == SpotLightStatic_NAME)
		{
			GetLightComponent()->Mobility = EComponentMobility::Static;
		}
		else if(OldClassName == SpotLightMovable_NAME)
		{
			GetLightComponent()->Mobility = EComponentMobility::Movable;
		}
		else if(OldClassName == SpotLightStationary_NAME)
		{
			GetLightComponent()->Mobility = EComponentMobility::Stationary;
		}
	}
}
#endif // WITH_EDITOR

void ARSMSource::SetInnerConeAngle(float NewInnerConeAngle)
{
	RSMSourceComponent->SetInnerConeAngle(NewInnerConeAngle);
}

void ARSMSource::SetOuterConeAngle(float NewOuterConeAngle)
{
	RSMSourceComponent->SetOuterConeAngle(NewOuterConeAngle);
}

// Disable for now
//void ASpotLight::SetLightShaftConeAngle(float NewLightShaftConeAngle)
//{
//	SpotLightComponent->SetLightShaftConeAngle(NewLightShaftConeAngle);
//}

#if WITH_EDITOR
void ARSMSource::EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	const FVector ModifiedScale = DeltaScale * (AActor::bUsePercentageBasedScaling ? 10000.0f : 100.0f);

	if(bCtrlDown)
	{
		FMath::ApplyScaleToFloat(RSMSourceComponent->OuterConeAngle, ModifiedScale, 0.01f);
		RSMSourceComponent->OuterConeAngle = FMath::Min(89.0f, RSMSourceComponent->OuterConeAngle);
		RSMSourceComponent->InnerConeAngle = FMath::Min(RSMSourceComponent->OuterConeAngle, RSMSourceComponent->InnerConeAngle);
	}
	else if(bAltDown)
	{
		FMath::ApplyScaleToFloat(RSMSourceComponent->InnerConeAngle, ModifiedScale, 0.01f);
		RSMSourceComponent->InnerConeAngle = FMath::Min(89.0f, RSMSourceComponent->InnerConeAngle);
		RSMSourceComponent->OuterConeAngle = FMath::Max(RSMSourceComponent->OuterConeAngle, RSMSourceComponent->InnerConeAngle);
	}
	else
	{
		FMath::ApplyScaleToFloat(RSMSourceComponent->AttenuationRadius, ModifiedScale);
	}

	PostEditChange();
}

void ARSMSource::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(ArrowComponent)
	{
		ArrowComponent->ArrowColor = GetLightColor().ToFColor(true);
	}
}
#endif

