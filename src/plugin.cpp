#include "plugin.hpp"

#include "plugincommon.hpp"
#include "pluginconstants.hpp"

#include <QAction>
#include <QDateTime>
#include <QTimerEvent>

#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/editormanager/ieditor.h>
#include <extensionsystem/iplugin.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projecttree.h>
#include <utils/mimeconstants.h>

namespace DiscordRichPresence::Internal
{
DiscordRichPresencePlugin::DiscordRichPresencePlugin()
    : m_groupId{QUuid::createUuid()}
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
        if (!m_core.isNull()) {
            m_core->RunCallbacks();
        }
    }
}

void DiscordRichPresencePlugin::init()
{
    {
        discord::Core *newCore{};
        auto result = discord::Core::Create(Constants::DISCORD_CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &newCore);
        if (!newCore) {
            if (result != discord::Result::InternalError) {
                logCrit() << Tr::tr("Cannot create Discord Game SDK instance. Error %1.").arg(discordResultString(result));
            }
            m_core.reset();
            return;
        }

        m_core.reset(newCore);
    }

    static const std::function<void(discord::LogLevel, const char *)> logHock = [](discord::LogLevel level, const char *message) {
        switch (level) {
        case discord::LogLevel::Error:
            logCrit("%s", message);
            break;
        case discord::LogLevel::Warn:
            logWarn("%s", message);
            break;
        case discord::LogLevel::Info:
            logInfo("%s", message);
            break;
        case discord::LogLevel::Debug:
            logDebug("%s", message);
            break;
        }
    };
    m_core->SetLogHook(discord::LogLevel::Debug, logHock);

    logInfo() << Tr::tr("Discord Rich Presence connected.");

    updateActivity();
}

void DiscordRichPresencePlugin::updateActivity()
{
    if (m_core.isNull()) {
        return;
    }

    auto editor = Core::EditorManager::currentEditor();
    auto project = ProjectExplorer::ProjectTree::currentProject();

    discord::Activity activity;
    activity.SetInstance(true);
    activity.GetTimestamps().SetStart(m_timestamp);
    activity.GetTimestamps().SetEnd(0);
    activity.GetParty().SetId("");
    activity.GetParty().GetSize().SetCurrentSize(0);
    activity.GetParty().GetSize().SetMaxSize(0);
    activity.GetParty().SetPrivacy(discord::ActivityPartyPrivacy::Public);
    activity.GetSecrets().SetJoin("");
    activity.GetSecrets().SetMatch("");
    activity.GetSecrets().SetSpectate("");

    auto asset = &activity.GetAssets();

    if (editor) {
        if (project) {
            activity.SetState(qPrintable(Tr::tr(Constants::STATE_WORKING_ON_PROJECT)));
            if (editor->isDesignModePreferred()) {
                activity.SetDetails(
                    qPrintable(Tr::tr("%1's UI: %2", "'%1' is project name.").arg(project->displayName(), editor->document()->filePath().fileName())));
            } else {
                activity.SetDetails(
                    qPrintable(Tr::tr("%1's file: %2", "'%1' is project name.").arg(project->displayName(), editor->document()->filePath().fileName())));
            }
        } else {
            activity.SetDetails(qPrintable(editor->document()->filePath().fileName()));
            if (editor->isDesignModePreferred()) {
                activity.SetState(qPrintable(Tr::tr(Constants::STATE_DESIGN_UI)));
            } else {
                activity.SetState(qPrintable(Tr::tr(Constants::STATE_EDITING_FILE)));
            }
        }
    } else {
        if (project) {
            activity.SetState(qPrintable(Tr::tr(Constants::STATE_WORKING_ON_PROJECT)));
            activity.SetDetails(qPrintable(Tr::tr("Project %1").arg(project->displayName())));
        } else {
            activity.SetState(qPrintable(Tr::tr(Constants::STATE_IDLE)));
            activity.SetDetails(qPrintable(Tr::tr(Constants::DETAILS_STARTUP_SCREEN)));
        }
    }

    asset->SetLargeImage(Constants::MAIN_LARGE_IMAGE);
    asset->SetLargeText(qPrintable(Tr::tr(Constants::MAIN_LARGE_TEXT)));
    asset->SetSmallImage("");
    asset->SetSmallText("");

    m_core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
        if (result != discord::Result::Ok && result != discord::Result::TransactionAborted) {
            logCrit().noquote() << Tr::tr("Cannot update Discord Activity. Error %1.").arg(discordResultString(result));
        }
    });
}
} // namespace DiscordRichPresence::Internal
