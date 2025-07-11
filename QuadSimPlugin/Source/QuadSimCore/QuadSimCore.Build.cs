using UnrealBuildTool;
using System.IO;

public class QuadSimCore : ModuleRules
{
	public QuadSimCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			"ChaosVehicles", "PhysicsCore", "RenderCore", "RHI",
			"Sockets", "Networking", "ImGui", "Slate", "SlateCore", 
			"UMG", "Json", "JsonUtilities"
		});
		PrivateDependencyModuleNames.AddRange(new string[] { "Projects", "AITestSuite" });
        
		bEnableExceptions = true;
		CppStandard = CppStandardVersion.Cpp20;
        
		// MavLinkCom configuration
		string MavLinkComPath = Path.Combine(ModuleDirectory, "..", "..", "ThirdParty", "MavLinkCom");
		string MavLinkComLibPath = Path.Combine(MavLinkComPath, "lib", "libMavLinkCom.a");
        
		if (Directory.Exists(MavLinkComPath))
		{
			// Add MavLinkCom include paths
			PublicIncludePaths.AddRange(new string[] {
				Path.Combine(MavLinkComPath, "include"),
				Path.Combine(MavLinkComPath, "mavlink"),
				Path.Combine(MavLinkComPath, "common_utils"),
				MavLinkComPath  // For common_utils includes
			});
            
			// Link the pre-built static library if it exists
			if (File.Exists(MavLinkComLibPath))
			{
				PublicAdditionalLibraries.Add(MavLinkComLibPath);
				System.Console.WriteLine("MavLinkCom integration: Using pre-built static library");
				System.Console.WriteLine("Library: " + MavLinkComLibPath);
			}
			else
			{
				System.Console.WriteLine("MavLinkCom static library not found at: " + MavLinkComLibPath);
				System.Console.WriteLine("Please run build_mavlinkcom.sh to build the library first");
			}
            
			// Define which MAVLink dialect to use
			PublicDefinitions.Add("MAVLINK_DIALECT=common");
            
			// Disable warnings that MavLinkCom might trigger
			if (Target.Platform == UnrealTargetPlatform.Linux)
			{
				PublicDefinitions.Add("__STDC_FORMAT_MACROS");
			}
			if (Target.Platform == UnrealTargetPlatform.Win64)
			{
				PrivateDefinitions.Add("_CRT_SECURE_NO_WARNINGS");
			}
            
			System.Console.WriteLine("MavLinkCom found at: " + MavLinkComPath);
		}
		else
		{
			System.Console.WriteLine("MavLinkCom not found at: " + MavLinkComPath);
			System.Console.WriteLine("Please ensure MavLinkCom is downloaded to QuadSimPlugin/ThirdParty/MavLinkCom/");
		}
	}
}