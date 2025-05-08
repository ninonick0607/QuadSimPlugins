using UnrealBuildTool;
using System.IO;

public class QuadSimCore : ModuleRules
{
    public QuadSimCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage         = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard      = CppStandardVersion.Cpp20;
        bEnableExceptions = true;

        PublicDependencyModuleNames.AddRange(new[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "PhysicsCore",
            "RenderCore",
            "RHI",
            "Sockets",
            "Networking",
            "Json",
            "JsonUtilities",
            "ImGui",
            "Slate",
            "SlateCore",
            "UMG"
        });

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Point at the plugin’s ThirdParty folder:
            // ModuleDirectory == Plugins/QuadSimPlugin/Source/QuadSimCore
            string ThirdPartyPath = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", "..", "ThirdParty"));

            // ZeroMQ C API headers
            string ZeroMQInc = Path.Combine(ThirdPartyPath, "ZeroMQ", "include");
            // cppzmq C++ headers
            string CppZmqInc = Path.Combine(ThirdPartyPath, "cppzmq", "include");
            PublicIncludePaths.Add(ZeroMQInc);
            PublicIncludePaths.Add(CppZmqInc);

            const string LibName = "libzmq-v143-mt-4_3_6";
            string ZeroMQLibDir = Path.Combine(ThirdPartyPath, "ZeroMQ", "lib");

            // Link the .lib
            PublicAdditionalLibraries.Add(Path.Combine(ZeroMQLibDir, LibName + ".lib"));
            // Delay-load and package the DLL
            PublicDelayLoadDLLs.Add(LibName + ".dll");
            RuntimeDependencies.Add(
                Path.Combine("$(BinaryOutputDir)", LibName + ".dll"),
                Path.Combine(ZeroMQLibDir, LibName + ".dll")
            );
        }
    }
}
