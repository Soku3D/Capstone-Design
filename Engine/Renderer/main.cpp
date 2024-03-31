#include "RenderApp.h"

int main()
{
	soku::RenderApp app(1920, 1080);
	if (app.Initialize())
	{
		app.Run();
	}
	return 0;
}