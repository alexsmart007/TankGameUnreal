// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "TankGame/Public/MyInputConfigData.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeMyInputConfigData() {}
// Cross Module References
	ENGINE_API UClass* Z_Construct_UClass_UDataAsset();
	ENHANCEDINPUT_API UClass* Z_Construct_UClass_UInputAction_NoRegister();
	TANKGAME_API UClass* Z_Construct_UClass_UMyInputConfigData();
	TANKGAME_API UClass* Z_Construct_UClass_UMyInputConfigData_NoRegister();
	UPackage* Z_Construct_UPackage__Script_TankGame();
// End Cross Module References
	void UMyInputConfigData::StaticRegisterNativesUMyInputConfigData()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UMyInputConfigData);
	UClass* Z_Construct_UClass_UMyInputConfigData_NoRegister()
	{
		return UMyInputConfigData::StaticClass();
	}
	struct Z_Construct_UClass_UMyInputConfigData_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_InputMove_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InputMove;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_InputShoot_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InputShoot;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_InputRotate_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InputRotate;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UMyInputConfigData_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UDataAsset,
		(UObject* (*)())Z_Construct_UPackage__Script_TankGame,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMyInputConfigData_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UMyInputConfigData_Statics::Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Store pointer to Input Action for native binding\n*/" },
#endif
		{ "IncludePath", "MyInputConfigData.h" },
		{ "ModuleRelativePath", "Public/MyInputConfigData.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Store pointer to Input Action for native binding" },
#endif
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputMove_MetaData[] = {
		{ "Category", "MyInputConfigData" },
		{ "ModuleRelativePath", "Public/MyInputConfigData.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputMove = { "InputMove", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMyInputConfigData, InputMove), Z_Construct_UClass_UInputAction_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputMove_MetaData), Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputMove_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputShoot_MetaData[] = {
		{ "Category", "MyInputConfigData" },
		{ "ModuleRelativePath", "Public/MyInputConfigData.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputShoot = { "InputShoot", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMyInputConfigData, InputShoot), Z_Construct_UClass_UInputAction_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputShoot_MetaData), Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputShoot_MetaData) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputRotate_MetaData[] = {
		{ "Category", "MyInputConfigData" },
		{ "ModuleRelativePath", "Public/MyInputConfigData.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputRotate = { "InputRotate", nullptr, (EPropertyFlags)0x0010000000010015, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UMyInputConfigData, InputRotate), Z_Construct_UClass_UInputAction_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputRotate_MetaData), Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputRotate_MetaData) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UMyInputConfigData_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputMove,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputShoot,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UMyInputConfigData_Statics::NewProp_InputRotate,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UMyInputConfigData_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UMyInputConfigData>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UMyInputConfigData_Statics::ClassParams = {
		&UMyInputConfigData::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UMyInputConfigData_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UMyInputConfigData_Statics::PropPointers),
		0,
		0x001000A0u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UMyInputConfigData_Statics::Class_MetaDataParams), Z_Construct_UClass_UMyInputConfigData_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UMyInputConfigData_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_UMyInputConfigData()
	{
		if (!Z_Registration_Info_UClass_UMyInputConfigData.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UMyInputConfigData.OuterSingleton, Z_Construct_UClass_UMyInputConfigData_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UMyInputConfigData.OuterSingleton;
	}
	template<> TANKGAME_API UClass* StaticClass<UMyInputConfigData>()
	{
		return UMyInputConfigData::StaticClass();
	}
	UMyInputConfigData::UMyInputConfigData(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UMyInputConfigData);
	UMyInputConfigData::~UMyInputConfigData() {}
	struct Z_CompiledInDeferFile_FID_TankGame_Source_TankGame_Public_MyInputConfigData_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_TankGame_Source_TankGame_Public_MyInputConfigData_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UMyInputConfigData, UMyInputConfigData::StaticClass, TEXT("UMyInputConfigData"), &Z_Registration_Info_UClass_UMyInputConfigData, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UMyInputConfigData), 1491116678U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_TankGame_Source_TankGame_Public_MyInputConfigData_h_1561239301(TEXT("/Script/TankGame"),
		Z_CompiledInDeferFile_FID_TankGame_Source_TankGame_Public_MyInputConfigData_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_TankGame_Source_TankGame_Public_MyInputConfigData_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
