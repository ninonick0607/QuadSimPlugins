using UnrealBuildTool;
using System.IO;

public class QuadSimCore : ModuleRules
{
    public QuadSimCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage        = PCHUsageMode.UseExplicitOrSharedPCHs;
        CppStandard     = CppStandardVersion.Cpp20;
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
            // For accessing plugin directory
            // For accessing plugin directory
            "Projects",
            "ImGui",
            "Slate",
            "SlateCore",
            "UMG"
        });

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // expect ThirdParty under the plugin root: <project>/Plugins/QuadSimPlugin/ThirdParty
            // Compute plugin's base directory (two levels up from ModuleDirectory)
            string PluginRoot   = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../"));
            string ThirdParty = Path.Combine(PluginRoot, "ThirdParty");

            // C API
            string ZeroMQInc = Path.Combine(ThirdParty, "ZeroMQ", "include");
            // C++ bindings
            string CppZmqInc = Path.Combine(ThirdParty, "cppzmq", "include");
            PublicIncludePaths.Add(ZeroMQInc);
            PublicIncludePaths.Add(CppZmqInc);

            const string LibName = "libzmq-v143-mt-4_3_6";
            string LibDir = Path.Combine(ThirdParty, "ZeroMQ", "lib");

            // Link the .lib
            PublicAdditionalLibraries.Add(Path.Combine(LibDir, LibName + ".lib"));
            // Delay-load the DLL and package it
            PublicDelayLoadDLLs.Add(LibName + ".dll");
            RuntimeDependencies.Add(
                "$(BinaryOutputDir)/" + LibName + ".dll",
                Path.Combine(LibDir, LibName + ".dll")
            );
        }
    }
}
