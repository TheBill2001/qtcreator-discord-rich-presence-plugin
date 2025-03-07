#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include <QBasicTimer>
#include <QUuid>

#include <coreplugin/editormanager/ieditor.h>
#include <extensionsystem/iplugin.h>

#include <discord-game-sdk/core.h>

namespace DiscordRichPresence
{
namespace Internal
{
class DiscordRichPresencePlugin final : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DiscordRichPresencePlugin.json")
public:
    DiscordRichPresencePlugin();
    ~DiscordRichPresencePlugin() override final;

    ShutdownFlag aboutToShutdown() override final;

Q_SIGNALS:
    void coreCleared();

protected:
    void initialize() override final;
    void timerEvent(QTimerEvent *event) override;

private:
    QBasicTimer m_timer;
    QScopedPointer<discord::Core> m_core;

    const QUuid m_groupId;
    const qint64 m_timestamp;

    void init();
    void updateActivity();
};
} // namespace Internal
} // namespace DiscordRichPresence

#endif // PLUGIN_HPP
