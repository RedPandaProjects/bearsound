using BearBuildTool.Projects;
using System.IO;
using System;
public class bear_sound : Project
{
	public bear_sound(string ProjectPath)
	{
		PCHFile=Path.Combine(ProjectPath,"source", "BearSound.cpp");
		PCHIncludeFile= "BearSound.hpp";
		AddSourceFiles(Path.Combine(ProjectPath,"source"),true);
		Include.Public.Add(Path.Combine(ProjectPath,"include"));
        Projects.Private.Add("bear_core");
        Projects.Private.Add("openal");
        Projects.Private.Add("vorbis");
		Projects.Private.Add("ogg");
    }
} 