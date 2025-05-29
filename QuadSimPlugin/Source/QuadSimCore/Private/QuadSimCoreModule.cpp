// Copyright (c) Quadcopter Simulation Plugin
// Minimal module implementation for QuadSimCore
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FQuadSimCoreModule : public IModuleInterface
{
public:
    /** IModuleInterface implementation */
    virtual void StartupModule() override {}
    virtual void ShutdownModule() override {}
};

IMPLEMENT_MODULE(FQuadSimCoreModule, QuadSimCore)