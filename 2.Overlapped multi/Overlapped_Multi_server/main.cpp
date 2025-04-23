#include"Core.h"

int main()
{

	if (!Core::GetInst()->Init())
	{

		Core::DestroyInst();
		return 0;
	}
	Core::GetInst()->Run();
	return 0;
}