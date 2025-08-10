// Copyright (c) 2022, NVIDIA CORPORATION. All rights reserved.
//
// NVIDIA CORPORATION and its licensors retain all intellectual property
// and proprietary rights in and to this software, related documentation
// and any modifications thereto.  Any use, reproduction, disclosure or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA CORPORATION is strictly prohibited.
//

#define CARB_EXPORTS

#include <carb/PluginUtils.h>
#include <carb/graphics/GraphicsUtils.h>

#include <omni/ext/IExt.h>
#include <omni/kit/IApp.h>
#include <carb/eventdispatcher/IEventDispatcher.h>

#include "SPOUTSDK/Spout.h"

const struct carb::PluginImplDesc pluginImplDesc = { "omni.spout.sender.plugin",
                                                     "A Spout sender extension.", "NVIDIA",
                                                     carb::PluginHotReload::eEnabled, "dev" };

CARB_PLUGIN_IMPL_DEPS(carb::eventdispatcher::IEventDispatcher, carb::graphics::IGraphics)

namespace omni
{
namespace spout
{
namespace sender
{

class SpoutSenderExtension : public omni::ext::IExt
{
public:
    void onStartup(const char* extId) override
    {
        printf("SpoutSenderExtension starting up (ext_id: %s).\n", extId);

        if (carb::eventdispatcher::IEventDispatcher* eventDispatcher = 
            carb::getCachedInterface<carb::eventdispatcher::IEventDispatcher>())
        {
            m_updateEventsSubscription = eventDispatcher->observeEvent(
                carb::RStringKey("Spout Sender Extension"), carb::eventdispatcher::kDefaultOrder,
                omni::kit::kGlobalEventPostUpdate,
                [this](const carb::eventdispatcher::Event& e) { onUpdate(); });
        }

        m_spoutSender = new Spout();
        m_spoutSender->SetSenderName("Omniverse");
    }

    void onShutdown() override
    {
        printf("SpoutSenderExtension shutting down.\n");

        m_updateEventsSubscription.reset();

        if (m_spoutSender)
        {
            m_spoutSender->ReleaseSender();
            delete m_spoutSender;
            m_spoutSender = nullptr;
        }
    }

    void onUpdate()
    {
        if (m_updateCounter % 1000 == 0)
        {
            printf("Hello from the omni.spout.sender extension! %d updates counted.\n", m_updateCounter);
        }
        m_updateCounter++;
    }

private:
    carb::eventdispatcher::ObserverGuard m_updateEventsSubscription;
    int m_updateCounter = 0;
    Spout* m_spoutSender = nullptr;
};

} // namespace sender
} // namespace spout
} // namespace omni

CARB_PLUGIN_IMPL(pluginImplDesc, omni::spout::sender::SpoutSenderExtension)

void fillInterface(omni::spout::sender::SpoutSenderExtension& iface)
{
}
