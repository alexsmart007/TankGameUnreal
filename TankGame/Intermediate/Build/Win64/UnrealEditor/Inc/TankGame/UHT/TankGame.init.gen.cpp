// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeTankGame_init() {}
	static FPackageRegistrationInfo Z_Registration_Info_UPackage__Script_TankGame;
	FORCENOINLINE UPackage* Z_Construct_UPackage__Script_TankGame()
	{
		if (!Z_Registration_Info_UPackage__Script_TankGame.OuterSingleton)
		{
			static const UECodeGen_Private::FPackageParams PackageParams = {
				"/Script/TankGame",
				nullptr,
				0,
				PKG_CompiledIn | 0x00000000,
				0xC3DA300F,
				0xDD4F315A,
				METADATA_PARAMS(0, nullptr)
			};
			UECodeGen_Private::ConstructUPackage(Z_Registration_Info_UPackage__Script_TankGame.OuterSingleton, PackageParams);
		}
		return Z_Registration_Info_UPackage__Script_TankGame.OuterSingleton;
	}
	static FRegisterCompiledInInfo Z_CompiledInDeferPackage_UPackage__Script_TankGame(Z_Construct_UPackage__Script_TankGame, TEXT("/Script/TankGame"), Z_Registration_Info_UPackage__Script_TankGame, CONSTRUCT_RELOAD_VERSION_INFO(FPackageReloadVersionInfo, 0xC3DA300F, 0xDD4F315A));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
