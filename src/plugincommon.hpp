#ifndef PLUGINCOMMON_HPP
#define PLUGINCOMMON_HPP

#include "loggingcategory.hpp" // IWYU pragma: export

#include <QCoreApplication>

#include <discord-game-sdk/types.h>

#define logCrit(...) qCCritical(QTCREATOR_DRP_LOGGING, __VA_ARGS__)
#define logWarn(...) qCWarning(QTCREATOR_DRP_LOGGING, __VA_ARGS__)
#define logInfo(...) qCInfo(QTCREATOR_DRP_LOGGING, __VA_ARGS__)
#define logDebug(...) qCDebug(QTCREATOR_DRP_LOGGING, __VA_ARGS__)

#define enumStr(m_enum) QString("%1(%2)").arg(#m_enum, QString::number((int)m_enum))
#define caseEnumStr(m_enum)                                                                                                                                    \
    case m_enum: {                                                                                                                                             \
        static const auto string = enumStr(m_enum);                                                                                                            \
        return string;                                                                                                                                         \
    }

namespace DiscordRichPresence
{
struct Tr {
    Q_DECLARE_TR_FUNCTIONS(QtC::DiscordRichPresence)
};

namespace Internal
{
inline QString discordResultString(discord::Result result)
{
    switch (result) {
        caseEnumStr(discord::Result::Ok);
        caseEnumStr(discord::Result::ServiceUnavailable);
        caseEnumStr(discord::Result::InvalidVersion);
        caseEnumStr(discord::Result::LockFailed);
        caseEnumStr(discord::Result::InternalError);
        caseEnumStr(discord::Result::InvalidPayload);
        caseEnumStr(discord::Result::InvalidCommand);
        caseEnumStr(discord::Result::InvalidPermissions);
        caseEnumStr(discord::Result::NotFetched);
        caseEnumStr(discord::Result::NotFound);
        caseEnumStr(discord::Result::Conflict);
        caseEnumStr(discord::Result::InvalidSecret);
        caseEnumStr(discord::Result::InvalidJoinSecret);
        caseEnumStr(discord::Result::NoEligibleActivity);
        caseEnumStr(discord::Result::InvalidInvite);
        caseEnumStr(discord::Result::NotAuthenticated);
        caseEnumStr(discord::Result::InvalidAccessToken);
        caseEnumStr(discord::Result::ApplicationMismatch);
        caseEnumStr(discord::Result::InvalidDataUrl);
        caseEnumStr(discord::Result::InvalidBase64);
        caseEnumStr(discord::Result::NotFiltered);
        caseEnumStr(discord::Result::LobbyFull);
        caseEnumStr(discord::Result::InvalidLobbySecret);
        caseEnumStr(discord::Result::InvalidFilename);
        caseEnumStr(discord::Result::InvalidFileSize);
        caseEnumStr(discord::Result::InvalidEntitlement);
        caseEnumStr(discord::Result::NotInstalled);
        caseEnumStr(discord::Result::NotRunning);
        caseEnumStr(discord::Result::InsufficientBuffer);
        caseEnumStr(discord::Result::PurchaseCanceled);
        caseEnumStr(discord::Result::InvalidGuild);
        caseEnumStr(discord::Result::InvalidEvent);
        caseEnumStr(discord::Result::InvalidChannel);
        caseEnumStr(discord::Result::InvalidOrigin);
        caseEnumStr(discord::Result::RateLimited);
        caseEnumStr(discord::Result::OAuth2Error);
        caseEnumStr(discord::Result::SelectChannelTimeout);
        caseEnumStr(discord::Result::GetGuildTimeout);
        caseEnumStr(discord::Result::SelectVoiceForceRequired);
        caseEnumStr(discord::Result::CaptureShortcutAlreadyListening);
        caseEnumStr(discord::Result::UnauthorizedForAchievement);
        caseEnumStr(discord::Result::InvalidGiftCode);
        caseEnumStr(discord::Result::PurchaseError);
        caseEnumStr(discord::Result::TransactionAborted);
        caseEnumStr(discord::Result::DrawingInitFailed);
    }
    return {};
}

inline QString elideString(const QString &original, qsizetype length = 30)
{
    if (original.size() <= length) {
        return original;
    }
    return original.first(((length - 1) / 2) - 1).append("...").append(original.last((length - 1) / 2));
}
} // namespace Internal
} // namespace DiscordRichPresence

#endif // PLUGINCOMMON_HPP
