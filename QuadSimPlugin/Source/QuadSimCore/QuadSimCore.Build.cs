using UnrealBuildTool;
using System.IO;
using System;

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
        
        // Third party paths
        string ThirdPartyPath = Path.Combine(ModuleDirectory, "..", "..", "ThirdParty");
        string MAVSDKPath = Path.Combine(ThirdPartyPath, "MAVSDK");
        
        // MAVSDK configuration
        if (Directory.Exists(MAVSDKPath))
        {
            Console.WriteLine("MAVSDK found at: " + MAVSDKPath);
            
            // Add include paths
            PublicIncludePaths.Add(Path.Combine(MAVSDKPath, "include"));
            
            // IMPORTANT: Add MAVLink headers from MAVSDK - correct path is under mavsdk/mavlink
            string MAVLinkBasePath = Path.Combine(MAVSDKPath, "include", "mavsdk", "mavlink");
            string MAVLinkCommonPath = Path.Combine(MAVLinkBasePath, "common");
            
            if (Directory.Exists(MAVLinkBasePath))
            {
                // Add both base mavlink path and common subdirectory
                PublicIncludePaths.Add(MAVLinkBasePath);
                PublicIncludePaths.Add(MAVLinkCommonPath);
                
                // Also add the mavsdk directory itself for alternative include syntax
                PublicIncludePaths.Add(Path.Combine(MAVSDKPath, "include", "mavsdk"));
                
                Console.WriteLine("MAVLink headers found at: " + MAVLinkBasePath);
                Console.WriteLine("MAVLink common headers found at: " + MAVLinkCommonPath);
            }
            else
            {
                // Try legacy location as fallback
                string MAVLinkLegacyPath = Path.Combine(MAVSDKPath, "include", "mavlink");
                if (Directory.Exists(MAVLinkLegacyPath))
                {
                    PublicIncludePaths.Add(MAVLinkLegacyPath);
                    Console.WriteLine("MAVLink legacy headers found at: " + MAVLinkLegacyPath);
                }
            }
            
            // Platform-specific configuration
            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                string LibPath = Path.Combine(MAVSDKPath, "lib");
                string BinPath = Path.Combine(MAVSDKPath, "bin");
                
                // Add MAVSDK library
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, "mavsdk.lib"));
                
                // Add all required dependencies
                string[] RequiredLibs = new string[] {
                    "absl_base.lib",
                    "absl_strings.lib",
                    "absl_synchronization.lib",
                    "absl_time.lib",
                    "absl_time_zone.lib",
                    "absl_raw_logging_internal.lib",
                    "absl_throw_delegate.lib"
                };
                
                foreach (string lib in RequiredLibs)
                {
                    string libFile = Path.Combine(LibPath, lib);
                    if (File.Exists(libFile))
                    {
                        PublicAdditionalLibraries.Add(libFile);
                    }
                }
                
                // Copy DLLs to output
                if (Directory.Exists(BinPath))
                {
                    string[] DllFiles = Directory.GetFiles(BinPath, "*.dll");
                    foreach (string DllFile in DllFiles)
                    {
                        string DllName = Path.GetFileName(DllFile);
                        RuntimeDependencies.Add(Path.Combine("$(BinaryOutputDir)", DllName), DllFile);
                    }
                }
                
                // Add Windows-specific definitions
                PublicDefinitions.Add("_CRT_SECURE_NO_WARNINGS");
                PublicDefinitions.Add("NOMINMAX");
            }
        }
        else
        {
            Console.WriteLine("ERROR: MAVSDK not found at: " + MAVSDKPath);
            Console.WriteLine("Please download pre-built MAVSDK binaries from:");
            Console.WriteLine("https://github.com/mavlink/MAVSDK/releases");
            Console.WriteLine("Extract to: " + MAVSDKPath);
        }
    }
}