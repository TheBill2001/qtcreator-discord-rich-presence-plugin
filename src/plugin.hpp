#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include <QBasicTimer>
#include <QLibrary>
#include <QUuid>

#include <coreplugin/editormanager/ieditor.h>
#include <extensionsystem/iplugin.h>

namespace DiscordRichPresence
{
namespace Internal
{
struct Discord;
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
    QLibrary m_lib;
    QScopedPointer<Discord> m_discord;
    QBasicTimer m_timer;

    const QUuid m_groupId;
    const qint64 m_timestamp;

    void init();
    void updateActivity();
};
} // namespace Internal
} // namespace DiscordRichPresence

#endif // PLUGIN_HPP
