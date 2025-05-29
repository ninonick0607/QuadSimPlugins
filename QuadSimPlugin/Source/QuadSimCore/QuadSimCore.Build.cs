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
            "UMG", "Json", "JsonUtilities",
            "rclUE" 
        });

        PrivateDependencyModuleNames.AddRange(new string[] { "Projects" });
        
	string ProjectPluginsDirectory = Path.GetFullPath(Path.Combine(ModuleDirectory, "../../../")); 

	string RclUEPath = Path.Combine(ProjectPluginsDirectory, "rclUE"); // Path to .../QuadSim/Plugins/rclUE/

	// Verify the exact subdirectory structure within rclUE for Source and ThirdParty
	// Based on common plugin structures, it might be:
	string RclUESourcePublicPath = Path.Combine(RclUEPath, "Source", "rclUE", "Public"); 
	string ROS2IncludePath = Path.Combine(RclUEPath, "ThirdParty", "ros", "include");

	PublicIncludePaths.AddRange(new string[] {
	    RclUESourcePublicPath,
	    ROS2IncludePath,
	    Path.Combine(ROS2IncludePath, "rcl"),
	    Path.Combine(ROS2IncludePath, "rcl_interfaces"),
	    Path.Combine(ROS2IncludePath, "std_msgs")
	});

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicDefinitions.Add("WITH_ROS=0");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            
            // ROS2 specific configuration for Linux
            PublicDefinitions.Add("WITH_ROS=1");
            
            // Set the ROS_VERSION definition
            PublicDefinitions.Add("ROS_VERSION=2");
            
            // If you need specific ROS2 Foxy version
            PublicDefinitions.Add("ROS_DISTRO=foxy");
        }

        bEnableExceptions = true;
        CppStandard = CppStandardVersion.Cpp20;
    } 

} 
