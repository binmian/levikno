#include <levikno/levikno.h>

// INFO: this program plays sound from an external sound file,
//       note that we do not need to search for a physical device
//       or initialize rendering since we are not using anything
//       related to rendering in this program

int main(int argc, char** argv)
{
	// [Create Context]
	// create the context to load the library

	LvnContextCreateInfo lvnCreateInfo{};
	lvnCreateInfo.enableLogging = true;
	lvnCreateInfo.enableVulkanValidationLayers = true;
	lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
	lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

	lvn::createContext(&lvnCreateInfo);

	// [Sound source]
	// sound source info struct
	LvnSoundCreateInfo soundCreateInfo{};
	soundCreateInfo.filepath = "res/audio/birdSoundEffect.mp3";  // filepath to the sound file (.mp3)
	soundCreateInfo.pos = LvnVec3(0.0f, 0.0f, 0.0f);             // position of the sound
	soundCreateInfo.volume = 1.0f;                               // volume of the sound (default if 1.0, mute is 0.0)
	soundCreateInfo.pan = 0.0f;                                  // pan of the sound between left and right ear (middle is 0.0, left is -1.0, right is 1.0)
	soundCreateInfo.pitch = 1.0f;                                // pitch of sound (default is 1.0, lower or height decreases and increases pitch)
	soundCreateInfo.looping = false;                             // set the sound to loop when it ends

	// create sound source
	LvnSound* sound;
	lvn::createSoundFromFile(&sound, &soundCreateInfo);

	// play sound
	lvn::soundPlayStart(sound);

	LVN_INFO("sound is playing");

	while (lvn::soundIsPlaying(sound))
	{
		// wait for sound to finish
	}

	LVN_INFO("finished playing sound");

	// destroy sound
	lvn::destroySound(sound);

	// terminate context
	lvn::terminateContext();

	return 0;
}
