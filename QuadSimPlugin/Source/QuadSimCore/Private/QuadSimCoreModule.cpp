// Ensure plugin's ZMQ DLL is found at runtime by adding the plugin Binaries folder to the DLL search path
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

class FQuadSimCoreModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        // Locate the project Plugins directory and plugin binaries subfolder
        FString PluginsDir = FPaths::ProjectPluginsDir();
        FString BinarySubDir = FPlatformProcess::GetBinariesSubdirectory();
        FString BinariesPath = FPaths::Combine(PluginsDir, TEXT("QuadSimPlugin"), TEXT("Binaries"), BinarySubDir);
        // Add to DLL search path so that the delay-loaded ZMQ DLL can be located
        FPlatformProcess::AddDllDirectory(*BinariesPath);
    }

    virtual void ShutdownModule() override
    {
        // No cleanup required
    }
};

IMPLEMENT_MODULE(FQuadSimCoreModule, QuadSimCore)