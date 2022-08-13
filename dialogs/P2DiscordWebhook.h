#pragma once
#include "libs/JSON/json.hpp"

#include <QDebug>
#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <steam_api.h>

using json = nlohmann::json;

namespace ui
{
	class CP2DiscordWebhook : public QObject
	{
	private:
		QNetworkAccessManager *manager;
		QNetworkRequest request;
		json m_URLResult;
		bool m_isReady = false;
		bool isValid = true;
		char *errorMessage;

	public:
		explicit CP2DiscordWebhook( const std::string &link );
		~CP2DiscordWebhook() override;
		void RunWebhook( SubmitItemUpdateResult_t *item );
		[[nodiscard]] bool Validate() const;
		char *ErrorMessage();
	};
} // namespace ui