using UnrealBuildTool;
using System.IO;

public class UnrealRosFlight : ModuleRules
{
	public UnrealRosFlight(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Public dependencies are for things included in your PUBLIC headers
		PublicDependencyModuleNames.AddRange(
			new string[] { "Core", "rclUE" });

		// Private dependencies are for things used only in your PRIVATE .cpp files
		PrivateDependencyModuleNames.AddRange(
			new string[] { "CoreUObject", "Engine", "Projects", "PhysicsCore", "Chaos", "Sockets" });

		string RosRoot     = Path.Combine(ModuleDirectory, "..", "..", "ThirdParty", "ros");
		string RosInclude  = Path.Combine(RosRoot, "include");
		string RosLib      = Path.Combine(RosRoot, "lib");

		PublicIncludePaths.Add(RosInclude);

		// add every package folder inside <include>
		foreach (var dir in Directory.GetDirectories(RosInclude))
			PublicIncludePaths.Add(dir);

#if PLATFORM_LINUX
        foreach (string so in Directory.GetFiles(RosLib, "*.so"))
        {
            PublicAdditionalLibraries.Add(so);
            RuntimeDependencies.Add(so);
        }
#endif
	}
}