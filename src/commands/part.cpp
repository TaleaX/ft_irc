#include "Server.hpp"

void Server::part(std::vector<std::string> reqVec, Client &client)
{
	if (this->checkPart(reqVec, client))
	{
		std::vector<std::string> partChannels;
		createLst(reqVec[1], partChannels);
		for (std::vector<std::string>::iterator itPartChannels = partChannels.begin(); itPartChannels != partChannels.end(); ++itPartChannels)
		{
			std::map<std::string, Channel *>::iterator itChannel = client.getJoinedChannels().find(*itPartChannels);

			if (itChannel != client.getJoinedChannels().end())
			{
				Channel *channel = itChannel->second;

				std::string response = PART(client, *itPartChannels);
				this->sendResponse(client, response);

				std::map<std::string, Client *>::iterator itClient = channel->getClientsM().find(client.getNickname());
				if (itClient != itChannel->second->getClientsM().end())
				{
					this->sendMsgToAllInChannel(*itChannel->second, response, client.getNickname());
					channel->getClientsM().erase(itClient->second->getNickname());
				}
				size_t numClients = client.getJoinedChannels()[*itPartChannels]->getClientsM().size();

				client.getJoinedChannels().erase(*itPartChannels);

				if (!numClients)
					this->_channelsM.erase(channel->getName());
			}
		}
	}
}
