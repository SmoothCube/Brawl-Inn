#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "TimerManager.h"

namespace BI
{

	FORCEINLINE static FTimerHandle Delay(UObject* Context, const float Seconds, TFunction<void(void)> Lambda)
	{
		FTimerDelegate TimerCallback;
		TimerCallback.BindLambda(Lambda);
		FTimerHandle Handle;
		Context->GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, Seconds, false);
		return Handle;
	}

	FORCEINLINE static void Repeat(UObject* Context, const float Seconds, const int Iterations, TFunction<void(void)> Lambda)
	{
		if (Iterations <= 0)
			return;

		Delay(Context, Seconds, [=]()
			{
				Lambda();
				Repeat(Context, Seconds, Iterations - 1, Lambda);
			});
	}
}