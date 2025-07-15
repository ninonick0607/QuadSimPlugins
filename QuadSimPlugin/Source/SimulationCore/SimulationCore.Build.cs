using UnrealBuildTool;

public class SimulationCore : ModuleRules
{
    public SimulationCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "ImGui"  // For time control UI
        });
        
        // We'll add QuadSimCore dependency later if needed
    }
}