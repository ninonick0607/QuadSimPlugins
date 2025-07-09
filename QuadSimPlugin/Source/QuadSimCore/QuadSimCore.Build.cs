using UnrealBuildTool;
using System.IO;

public class QuadSimCore : ModuleRules
{ // Opening brace for the CLASS
    public QuadSimCore(ReadOnlyTargetRules Target) : base(Target)
    { // Opening brace for the CONSTRUCTOR
        // ... all your existing code from inside the constructor ...
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
            "ChaosVehicles", "PhysicsCore", "RenderCore", "RHI",
            "Sockets", "Networking", "ImGui", "Slate", "SlateCore", 
            "UMG", "Json", "JsonUtilities"
        });
        PrivateDependencyModuleNames.AddRange(new string[] { "Projects", "AITestSuite" });
        
        // Platform-specific configurations can go here if needed

        bEnableExceptions = true;
        CppStandard = CppStandardVersion.Cpp20;
    } 

} 
