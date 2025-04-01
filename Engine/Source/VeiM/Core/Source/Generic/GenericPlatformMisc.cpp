#include "GenericPlatformMisc.h"
#include "HAL/PlatformService.h"
#include "Misc/Paths.h"
#include "Misc/Singleton.h"

namespace VeiM
{


	struct GenericPlatformMisc::StaticData
	{
		fs::path RootDir;
		fs::path EngineDir;
		fs::path LaunchDir;
		fs::path ProjectDir;
	};

	fs::path GenericPlatformMisc::LaunchDir()
	{
		return Singleton<StaticData>::Get().LaunchDir;
	}

	fs::path GenericPlatformMisc::EngineDir()
	{
		fs::path& engineDir = Singleton<StaticData>::Get().EngineDir;
		if (engineDir.empty())
		{
			MakeEngineDir(engineDir);
		}
		return engineDir;
	}

	fs::path GenericPlatformMisc::RootDir()
	{
		fs::path& rootDir = Singleton<StaticData>::Get().RootDir;
		if (rootDir.empty())
		{
			fs::path tempPath = Paths::EngineDir();
			bool bTrimmed = Paths::TrimPathAt(tempPath, TEXT("/Engine"));
			if (!bTrimmed)
			{
				tempPath = PlatformService::BaseDir();
				bTrimmed = Paths::TrimPathAt(tempPath, TEXT("/Engine/Binaries"));
				if (!bTrimmed)
				{
					bTrimmed = Paths::TrimPathAt(tempPath, TEXT("/../Binaries"));
					if (bTrimmed)
					{
						tempPath = tempPath / "../../";
					}
					else
					{
						if (tempPath.has_extension())
						{
							tempPath = tempPath.parent_path();
						}
					}
				}
			}
			rootDir = fs::absolute(tempPath);
		}
		return rootDir;
	}

	fs::path GenericPlatformMisc::ProjectDir()
	{
		fs::path& projectDir = Singleton<StaticData>::Get().ProjectDir;

		static bool bWasProjectFilePathSetBefore = false;
		if (!bWasProjectFilePathSetBefore && Paths::IsProjectFilePathSet())
		{
			projectDir.clear();
			bWasProjectFilePathSetBefore = true;
		}
		if (projectDir.empty())
		{
			if (Paths::IsProjectFilePathSet()) // TODO: Look where to set ProjectFilePath
			{
				projectDir = Paths::GetProjectFilePath();
			}
			else
			{
				// For editor build Find project file path by game name and set it to Paths and projectDir
			}
		}

		return projectDir;
	}

	void GenericPlatformMisc::MakeEngineDir(fs::path& outEngineDir)
	{
		fs::path defaultEngineDir = "../../../../Engine";
		PlatformService::SetCurrentWorkingDirectoryToBaseDir();

		fs::path possibleDir = PlatformService::BaseDir() / defaultEngineDir / "Binaries";
		if (fs::exists(possibleDir))
		{
			outEngineDir = fs::absolute(defaultEngineDir);
			return;
		}
		VM_CORE_WARN("Failed to determine engine directory: Default directory is '{0}'", outEngineDir.string());
		outEngineDir = fs::absolute(defaultEngineDir);
	}

}
