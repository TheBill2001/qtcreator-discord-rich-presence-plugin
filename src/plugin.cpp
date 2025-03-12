#include "plugin.hpp"

#include "plugincommon.hpp"
#include "pluginconstants.hpp"

#include <cstring>

#include <QAction>
#include <QDateTime>
#include <QTimerEvent>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <coreplugin/messagemanager.h>
#include <extensionsystem/iplugin.h>
#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/pluginspec.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projecttree.h>
#include <utils/filepath.h>
#include <utils/mimeconstants.h>

#include <discord_game_sdk.h>

using namespace Qt::Literals::StringLiterals;

namespace DiscordRichPresence::Internal
{
using DiscordCreateFuncPtr = EDiscordResult (*)(DiscordVersion, DiscordCreateParams *, IDiscordCore **);

struct Discord {
    IDiscordCore *core = nullptr;

    ~Discord()
    {
        if (core) {
            core->destroy(core);
        }
    }
};

DiscordRichPresencePlugin::DiscordRichPresencePlugin()
    : m_lib{this}
    , m_groupId{QUuid::createUuid()}
    , m_timestamp{QDateTime::currentSecsSinceEpoch()}
{
}

DiscordRichPresencePlugin::~DiscordRichPresencePlugin() = default;

ExtensionSystem::IPlugin::ShutdownFlag DiscordRichPresencePlugin::aboutToShutdown()
{
    return SynchronousShutdown;
}

void DiscordRichPresencePlugin::initialize()
{
    auto action = new QAction(Tr::tr("Reload Discord Rich Presence"), this);
    action->setEnabled(true);

    auto cmd = Core::ActionManager::registerAction(action, Constants::RELOAD_ACTION_ID, Core::Context(Core::Constants::C_GLOBAL));
    connect(action, &QAction::triggered, this, &DiscordRichPresencePlugin::init);

    auto menu = Core::ActionManager::createMenu(Core::Constants::M_HELP);
    menu->addAction(cmd);
    menu->addSeparator();

    connect(Core::EditorManager::instance(), &Core::EditorManager::currentEditorChanged, this, &DiscordRichPresencePlugin::updateActivity);
    connect(ProjectExplorer::ProjectTree::instance(), &ProjectExplorer::ProjectTree::currentProjectChanged, this, &DiscordRichPresencePlugin::updateActivity);

    m_timer.start(100, this);

    init();
}

void DiscordRichPresencePlugin::timerEvent(QTimerEvent *event)
{
    if (m_timer.id() == event->id()) {
        if (!m_discord.isNull() && m_discord->core) {
            m_discord->core->run_callbacks(m_discord->core);
        }
    }
}

void DiscordRichPresencePlugin::init()
{
    {
        m_lib.setFileName(u"discord_game_sdk"_s);
        if (!m_lib.load()) {
            auto spec = ExtensionSystem::PluginManager::specForPlugin(this);
            const auto pluginLibPath = spec->filePath().parentDir().resolvePath(u"discord_game_sdk"_s).toFSPathString();
            m_lib.setFileName(pluginLibPath);
            if (!m_lib.load()) {
                return;
            }
        }
    }

    {
        Discord *discord = new Discord();

        DiscordCreateParams params;
        DiscordCreateParamsSetDefault(&params);
        params.client_id = DiscordRichPresence::Constants::DISCORD_CLIENT_ID;
        params.flags = DiscordCreateFlags_NoRequireDiscord;
        params.events = nullptr;
        params.event_data = &discord;

        DiscordCreateFuncPtr discordCreateFuncPtr = (DiscordCreateFuncPtr)m_lib.resolve("DiscordCreate");
        if (discordCreateFuncPtr) {
            auto result = discordCreateFuncPtr(DISCORD_VERSION, &params, &discord->core);
            if (result != DiscordResult_Ok) {
                const auto message = Tr::tr("Cannot create Discord Game SDK instance. Error %1.").arg(discordResultString(result));
                logCrit().noquote() << message;
                Core::MessageManager::writeFlashing(message);
                return m_discord.reset();
            }
        }

        m_discord.reset(discord);
    }

    m_discord->core->set_log_hook(m_discord->core, DiscordLogLevel_Debug, nullptr, [](void *, EDiscordLogLevel level, char const *message) {
        switch (level) {
        case DiscordLogLevel_Error:
            logCrit("%s", message);
            break;
        case DiscordLogLevel_Warn:
            logWarn("%s", message);
            break;
        case DiscordLogLevel_Info:
            logInfo("%s", message);
            break;
        case DiscordLogLevel_Debug:
            logDebug("%s", message);
            break;
        }
    });

    logInfo() << Tr::tr("Discord Rich Presence connected.");

    updateActivity();
}

void DiscordRichPresencePlugin::updateActivity()
{
    if (m_discord.isNull() && !m_discord->core) {
        return;
    }

    auto editor = Core::EditorManager::currentEditor();
    auto project = ProjectExplorer::ProjectTree::currentProject();

    DiscordActivity activity;
    activity.instance = true;
    activity.timestamps.start = m_timestamp;
    activity.timestamps.end = 0;
    assignChar(activity.party.id, qPrintable(m_groupId.toString(QUuid::WithoutBraces)));
    activity.party.size.current_size = 0;
    activity.party.size.max_size = 0;
    activity.party.privacy = DiscordActivityPartyPrivacy_Public;
    assignChar(activity.secrets.join, "");
    assignChar(activity.secrets.match, "");
    assignChar(activity.secrets.spectate, "");

    auto assets = &activity.assets;

    if (editor) {
        if (project) {
            assignChar(activity.state, qPrintable(Tr::tr(Constants::STATE_WORKING_ON_PROJECT)));
            if (editor->isDesignModePreferred()) {
                assignChar(activity.details,
                           qPrintable(Tr::tr("%1's UI: %2", "'%1' is project name.").arg(project->displayName(), editor->document()->filePath().fileName())));
            } else {
                assignChar(activity.details,
                           qPrintable(Tr::tr("%1's file: %2", "'%1' is project name.").arg(project->displayName(), editor->document()->filePath().fileName())));
            }
        } else {
            assignChar(activity.details, qPrintable(editor->document()->filePath().fileName()));
            if (editor->isDesignModePreferred()) {
                assignChar(activity.state, qPrintable(Tr::tr(Constants::STATE_DESIGN_UI)));
            } else {
                assignChar(activity.state, qPrintable(Tr::tr(Constants::STATE_EDITING_FILE)));
            }
        }
    } else {
        if (project) {
            assignChar(activity.state, qPrintable(Tr::tr(Constants::STATE_WORKING_ON_PROJECT)));
            assignChar(activity.details, qPrintable(Tr::tr("Project %1").arg(project->displayName())));
        } else {
            assignChar(activity.state, qPrintable(Tr::tr(Constants::STATE_IDLE)));
            assignChar(activity.details, qPrintable(Tr::tr(Constants::DETAILS_STARTUP_SCREEN)));
        }
    }

    assignChar(assets->large_image, Constants::MAIN_LARGE_IMAGE);
    assignChar(assets->large_text, qPrintable(Tr::tr(Constants::MAIN_LARGE_TEXT)));
    assignChar(assets->small_image, "");
    assignChar(assets->small_text, "");

    auto activityManager = m_discord->core->get_activity_manager(m_discord->core);
    activityManager->update_activity(activityManager, &activity, nullptr, [](void *, EDiscordResult result) {
        if (result != DiscordResult_Ok && result != DiscordResult_TransactionAborted) {
            const auto message = Tr::tr("Cannot update Discord Activity. Error %1.").arg(discordResultString(result));
            logCrit().noquote() << message;
            Core::MessageManager::writeFlashing(message);
        }
    });
}
} // namespace DiscordRichPresence::Internal
