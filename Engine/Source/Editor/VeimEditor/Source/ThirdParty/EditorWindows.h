#pragma once


struct ImGuiWindows
{
	bool Fps{ false };
	bool Demo{ false };
	bool Metrics{ false };
	bool DebugLog{ false };
	bool IdStackTool{ false };
	bool About{ false };
	bool StyleEditor{ false };
	bool UserGuide{ false };

	static void ShowFpsWindow(bool* p_open);
};

