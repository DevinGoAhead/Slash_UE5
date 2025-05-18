//#pragma once
//
//// 这里尝试使用一个更聪明的方式为 配置 Treasure, 
//// 但尝试过后发现仅仅 FTreasureProperty 仍然无法解决问题, 比纯手动方便些, 但是逻辑上很别扭, 一个Treasure 要绑定许多mesh, 但却仅用一个
//// 还是需要 DataTable + Struct
//#include "CoreMinimal.h"
//#include "UObject/NoExportTypes.h"      // 如果你把这个 struct 放在非 Actor 的类中需要
//#include "Engine/StaticMesh.h"          // UStaticMesh
//#include "Sound/SoundBase.h"            // USoundBase
//#include "TreasureTypes.generated.h" // 放在最后（自动生成的头）
//
//USTRUCT(BlueprintType)
//struct FTreasureProperty
//{
//    GENERATED_BODY()
//
//    UPROPERTY(EditAnywhere)
//    UStaticMesh* Mesh;
//
//    UPROPERTY(EditAnywhere)
//    int32 CoinValue;
//
//    //UPROPERTY(EditAnywhere)
//    //FLinearColor GlowColor;
//
//    UPROPERTY(EditAnywhere)
//    USoundBase* PickupSound;
//};
//
