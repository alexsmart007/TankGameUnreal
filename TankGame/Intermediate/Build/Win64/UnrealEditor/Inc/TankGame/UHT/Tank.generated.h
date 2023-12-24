// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "Tank.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef TANKGAME_Tank_generated_h
#error "Tank.generated.h already included, missing '#pragma once' in Tank.h"
#endif
#define TANKGAME_Tank_generated_h

#define FID_TankGame_Source_TankGame_Public_Tank_h_13_SPARSE_DATA
#define FID_TankGame_Source_TankGame_Public_Tank_h_13_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_TankGame_Source_TankGame_Public_Tank_h_13_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS
#define FID_TankGame_Source_TankGame_Public_Tank_h_13_RPC_WRAPPERS_NO_PURE_DECLS
#define FID_TankGame_Source_TankGame_Public_Tank_h_13_ACCESSORS
#define FID_TankGame_Source_TankGame_Public_Tank_h_13_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesATank(); \
	friend struct Z_Construct_UClass_ATank_Statics; \
public: \
	DECLARE_CLASS(ATank, ACharacter, COMPILED_IN_FLAGS(0 | CLASS_Config), CASTCLASS_None, TEXT("/Script/TankGame"), NO_API) \
	DECLARE_SERIALIZER(ATank)


#define FID_TankGame_Source_TankGame_Public_Tank_h_13_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API ATank(ATank&&); \
	NO_API ATank(const ATank&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, ATank); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(ATank); \
	DEFINE_DEFAULT_CONSTRUCTOR_CALL(ATank) \
	NO_API virtual ~ATank();


#define FID_TankGame_Source_TankGame_Public_Tank_h_10_PROLOG
#define FID_TankGame_Source_TankGame_Public_Tank_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_TankGame_Source_TankGame_Public_Tank_h_13_SPARSE_DATA \
	FID_TankGame_Source_TankGame_Public_Tank_h_13_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_TankGame_Source_TankGame_Public_Tank_h_13_EDITOR_ONLY_SPARSE_DATA_PROPERTY_ACCESSORS \
	FID_TankGame_Source_TankGame_Public_Tank_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_TankGame_Source_TankGame_Public_Tank_h_13_ACCESSORS \
	FID_TankGame_Source_TankGame_Public_Tank_h_13_INCLASS_NO_PURE_DECLS \
	FID_TankGame_Source_TankGame_Public_Tank_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> TANKGAME_API UClass* StaticClass<class ATank>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_TankGame_Source_TankGame_Public_Tank_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
