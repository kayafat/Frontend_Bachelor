using UnrealBuildTool;

public class Bachelor : ModuleRules
{
    public Bachelor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HTTP",
            "Json",
            "JsonUtilities",
            "UMG",
            "Slate", 
            "SlateCore","AudioMixer","HeadMountedDisplay"

        });

        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Optional:
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
    }
}
