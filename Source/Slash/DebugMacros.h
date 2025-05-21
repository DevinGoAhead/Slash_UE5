#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"

inline void DRAW_DEBUG_SPHERE(AActor* ContextActor, FVector const& Center, float Radius, int32 Segments, FColor const& Color,
							bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0, float Thickness = 0.f) {
	if(ContextActor->GetWorld()) {
		DrawDebugSphere(ContextActor->GetWorld(), Center, Radius, Segments, Color, bPersistentLines, LifeTime, DepthPriority, Thickness);
	}
}

inline void DRAW_DEBUG_LINE(AActor* ContextActor, FVector const& LineStart, FVector const& LineEnd, FColor const& Color,
							bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0, float Thickness = 0.f) {
	if (ContextActor->GetWorld()) {
		DrawDebugLine(ContextActor->GetWorld(), LineStart, LineEnd, Color, bPersistentLines, LifeTime, DepthPriority);
	}
}

inline void DRAW_DEBUG_POINT(AActor* ContextActor, FVector const& Position, float Size, FColor const& PointColor,
							bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0) {
	if (ContextActor->GetWorld()) {
		DrawDebugPoint(ContextActor->GetWorld(), Position, Size, PointColor, bPersistentLines, LifeTime, DepthPriority);
	}
}

inline void ADD_SCREEN_DEBUG(int32 key, const FString& DebugMessage, float TimeToDisplay = 5.f, FColor DisplayColor = FColor::Blue) {
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(key, TimeToDisplay, DisplayColor, DebugMessage);
	}
}