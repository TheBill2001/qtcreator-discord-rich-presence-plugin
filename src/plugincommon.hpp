#ifndef PLUGINCOMMON_HPP
#define PLUGINCOMMON_HPP

#include "loggingcategory.hpp" // IWYU pragma: export

#include <QCoreApplication>

#include <discord_game_sdk.h>

#define logCrit(...) qCCritical(QTCREATOR_DRP_LOGGING, __VA_ARGS__)
#define logWarn(...) qCWarning(QTCREATOR_DRP_LOGGING, __VA_ARGS__)
#define logInfo(...) qCInfo(QTCREATOR_DRP_LOGGING, __VA_ARGS__)
#define logDebug(...) qCDebug(QTCREATOR_DRP_LOGGING, __VA_ARGS__)

#define discordCaseEnumStr(m_prefix, m_suffix)                                                                                                                 \
    case m_prefix##_##m_suffix: {                                                                                                                              \
        static const auto string = QString("%1(%2)").arg(#m_suffix, QString::number((int)m_prefix##_##m_suffix));                                              \
        return string;                                                                                                                                         \
    }

#define assignChar(m_variable, m_value)                                                                                                                        \
    std::strncpy(m_variable, m_value, sizeof m_variable);                                                                                                      \
    m_variable[(sizeof m_variable) - 1] = '\0';

namespace DiscordRichPresence
{
struct Tr {
    Q_DECLARE_TR_FUNCTIONS(QtC::DiscordRichPresence)
};

namespace Internal
{
inline QString discordResultString(EDiscordResult result)
{
    switch (result) {
        discordCaseEnumStr(DiscordResult, Ok);
        discordCaseEnumStr(DiscordResult, ServiceUnavailable);
        discordCaseEnumStr(DiscordResult, InvalidVersion);
        discordCaseEnumStr(DiscordResult, LockFailed);
        discordCaseEnumStr(DiscordResult, InternalError);
        discordCaseEnumStr(DiscordResult, InvalidPayload);
        discordCaseEnumStr(DiscordResult, InvalidCommand);
        discordCaseEnumStr(DiscordResult, InvalidPermissions);
        discordCaseEnumStr(DiscordResult, NotFetched);
        discordCaseEnumStr(DiscordResult, NotFound);
        discordCaseEnumStr(DiscordResult, Conflict);
        discordCaseEnumStr(DiscordResult, InvalidSecret);
        discordCaseEnumStr(DiscordResult, InvalidJoinSecret);
        discordCaseEnumStr(DiscordResult, NoEligibleActivity);
        discordCaseEnumStr(DiscordResult, InvalidInvite);
        discordCaseEnumStr(DiscordResult, NotAuthenticated);
        discordCaseEnumStr(DiscordResult, InvalidAccessToken);
        discordCaseEnumStr(DiscordResult, ApplicationMismatch);
        discordCaseEnumStr(DiscordResult, InvalidDataUrl);
        discordCaseEnumStr(DiscordResult, InvalidBase64);
        discordCaseEnumStr(DiscordResult, NotFiltered);
        discordCaseEnumStr(DiscordResult, LobbyFull);
        discordCaseEnumStr(DiscordResult, InvalidLobbySecret);
        discordCaseEnumStr(DiscordResult, InvalidFilename);
        discordCaseEnumStr(DiscordResult, InvalidFileSize);
        discordCaseEnumStr(DiscordResult, InvalidEntitlement);
        discordCaseEnumStr(DiscordResult, NotInstalled);
        discordCaseEnumStr(DiscordResult, NotRunning);
        discordCaseEnumStr(DiscordResult, InsufficientBuffer);
        discordCaseEnumStr(DiscordResult, PurchaseCanceled);
        discordCaseEnumStr(DiscordResult, InvalidGuild);
        discordCaseEnumStr(DiscordResult, InvalidEvent);
        discordCaseEnumStr(DiscordResult, InvalidChannel);
        discordCaseEnumStr(DiscordResult, InvalidOrigin);
        discordCaseEnumStr(DiscordResult, RateLimited);
        discordCaseEnumStr(DiscordResult, OAuth2Error);
        discordCaseEnumStr(DiscordResult, SelectChannelTimeout);
        discordCaseEnumStr(DiscordResult, GetGuildTimeout);
        discordCaseEnumStr(DiscordResult, SelectVoiceForceRequired);
        discordCaseEnumStr(DiscordResult, CaptureShortcutAlreadyListening);
        discordCaseEnumStr(DiscordResult, UnauthorizedForAchievement);
        discordCaseEnumStr(DiscordResult, InvalidGiftCode);
        discordCaseEnumStr(DiscordResult, PurchaseError);
        discordCaseEnumStr(DiscordResult, TransactionAborted);
        discordCaseEnumStr(DiscordResult, DrawingInitFailed);
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
