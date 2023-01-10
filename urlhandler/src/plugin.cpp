// Copyright (c) 2022 Manuel Schneider

#include "plugin.h"
#include <QDesktopServices>
#include <QFile>
#include <QTextStream>
#include <QUrl>
using namespace std;
using namespace albert;

Plugin::Plugin()
{
    QFile file(":tlds");
    if (!file.open(QIODevice::ReadOnly))
        throw runtime_error(QString("Unable to read tld resource").toStdString());
    valid_tlds << QTextStream(&file).readAll().split("\n");
    std::sort(valid_tlds.begin(), valid_tlds.end());
}

vector<RankItem> Plugin::rankItems(const QString &string, const bool& isValid) const
{
    vector<RankItem> results;
    auto trimmed = string.trimmed();
    auto url = QUrl::fromUserInput(trimmed);

    // Check syntax and TLD validity
    if (!url.isValid())
        return results;

    if ((url.scheme() == "http" || url.scheme() == "https")){

        auto tld = url.host().section('.', -1, -1);

        // Skip tld only queries
        if (trimmed.size() == tld.size())
            return results;

        // validate top level domain if scheme is not given (http assumed)
        if (!trimmed.startsWith("http") && !binary_search(valid_tlds.begin(), valid_tlds.end(), tld))
            return results;

        results.emplace_back(
            StandardItem::make(
                "url_hanlder",
                "Open URL in browser",
                QString("Visit %1").arg(url.authority()),
                {"xdg:www", "xdg:web-browser", "xdg:emblem-web", ":default"},
                {{
                    "open_url",
                    "Open URL",
                    [url](){
                        QDesktopServices::openUrl(url);
                    }
                }})
            , MAX_SCORE
            );
    }
    return results;
}
