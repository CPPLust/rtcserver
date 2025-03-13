

#include "base/network.h"



int main(int argc, char* argv[])
{
	xrtc::NetworkManager manager;

	manager.create_networks();

	const std::vector<xrtc::Network*>& l = manager.get_networks();

	for (auto net : l)
	{
		printf("+++++++++++++++++++net %s\n", net->to_string().c_str());
	}

	return 0;
}