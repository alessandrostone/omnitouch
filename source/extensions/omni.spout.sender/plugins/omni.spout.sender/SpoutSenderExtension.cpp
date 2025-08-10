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
#include <carb/events/IEventStream.h>
#include <carb/events/EventSubscription.h>

#include <omni/ext/IExt.h> 
#include <omni/kit/IApp.h> 
#include <omni/kit/IStage.h>
#include <carb/eventdispatcher/IEventDispatcher.h>
#include <omni/kit/hydra_texture/IHydraTexture.h>

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

        // Acquire HydraTexture factory
        m_hydraTextureFactory = omni::kit::hydra_texture::acquireHydraTextureFactoryInterface();
        if (m_hydraTextureFactory)
        {
            m_hydraTextureFactory->startup();

            // Get USD context
            omni::kit::IApp* app = carb::getCachedInterface<omni::kit::IApp>();
            if (app)
            {
                omni::kit::IStage* stage = app->getActiveStage();
                if (stage)
                {
                    const char* usdContextName = stage->getContextName();
                    const char* engineName = "rtx"; // Assuming RTX renderer

                    // Add Hydra engine if not already attached
                    if (!stage->isHydraEngineAttached(engineName))
                    {
                        stage->addHydraEngine(engineName);
                    }

                    // Create HydraTexture
                    m_hydraTexture = m_hydraTextureFactory->createHydraTexture(
                        "OmniSpoutTexture", m_textureWidth, m_textureHeight, usdContextName, "/OmniverseKit_Persp", engineName);

                    if (m_hydraTexture)
                    {
                        // Subscribe to render events
                        m_renderEventSubscription = m_hydraTexture->getEventStream()->createSubscriptionToPushByType(
                            omni::kit::hydra_texture::EVENT_TYPE_DRAWABLE_CHANGED,
                            [this](const carb::events::IEvent& e) { onRenderEvent(e); });
                    }
                }
            }
        }
    }

    void onShutdown() override
    {
        printf("SpoutSenderExtension shutting down.\n");

        m_updateEventsSubscription.reset();

        if (m_renderEventSubscription)
        {
            m_renderEventSubscription->unsubscribe();
            m_renderEventSubscription = nullptr;
        }

        if (m_hydraTexture)
        {
            m_hydraTexture->release();
            m_hydraTexture = nullptr;
        }

        if (m_hydraTextureFactory)
        {
            m_hydraTextureFactory->shutdown();
            m_hydraTextureFactory->release();
            m_hydraTextureFactory = nullptr;
        }

        if (m_spoutSender)
        {
            m_spoutSender->ReleaseSender();
            delete m_spoutSender;
            m_spoutSender = nullptr;
        }
    }

    void onUpdate()
    {
        // This method is still here for general updates, but rendering is handled by onRenderEvent
        if (m_updateCounter % 1000 == 0)
        {
            printf("Hello from the omni.spout.sender extension! %d updates counted.\n", m_updateCounter);
        }
        m_updateCounter++;
    }

    void onRenderEvent(const carb::events::IEvent& e)
    {
        if (e.getType() != omni::kit::hydra_texture::EVENT_TYPE_DRAWABLE_CHANGED)
        {
            return;
        }

        const carb::events::Payload* payload = e.getPayload();
        if (!payload)
        {
            return;
        }

        // Get the result handle from the payload
        const void* resultHandle = nullptr;
        size_t resultHandleSize = 0;
        if (!payload->tryGet("result_handle", &resultHandle, &resultHandleSize) || resultHandleSize != sizeof(void*))
        {
            return;
        }

        // Get AOV info for LdrColor (Low Dynamic Range Color)
        omni::kit::hydra_texture::AovInfoArray ldrInfoArray = m_hydraTexture->getAovInfo(resultHandle, "LdrColor", true);

        if (ldrInfoArray.count > 0)
        {
            const omni::kit::hydra_texture::AovInfo& ldrAovInfo = ldrInfoArray.info[0];
            if (ldrAovInfo.texture.rp_resource)
            {
                // Get the texture resource
                void* textureResource = ldrAovInfo.texture.rp_resource;

                // Send the texture via Spout
                // Assuming the texture is a DirectX 11 texture (ID3D11Texture2D*)
                // Spout requires the texture pointer, width, height, and DXGI_FORMAT
                // The format needs to be determined from Omniverse's output
                // For simplicity, assuming a common format like DXGI_FORMAT_R8G8B8A8_UNORM
                // You might need to query the actual format from the textureResource
                // or Omniverse API.

                // Placeholder for format and texture type
                // Spout expects ID3D11Texture2D* for DirectX 11
                // You might need to cast or convert the rp_resource to the correct type
                // based on Omniverse's internal graphics API.

                // Example for DirectX 11 (needs actual ID3D11Texture2D*)
                // ID3D11Texture2D* d3d11Texture = static_cast<ID3D11Texture2D*>(textureResource);
                // m_spoutSender->SendTexture(d3d11Texture, DXGI_FORMAT_R8G8B8A8_UNORM);

                // For now, just print a message that we got the texture
                printf("Received LdrColor texture resource! Sending via Spout...\n");
                // You would call m_spoutSender->SendTexture here with the actual texture and format
                // m_spoutSender->SendTexture(textureResource, m_textureWidth, m_textureHeight, false, 0);
                // The last two parameters are bUseActive and hShareHandle. For DirectX 11, you might need to pass the shared handle.
                // This part needs careful implementation based on Omniverse's graphics API and Spout's requirements.
            }
        }
    }

private:
    carb::eventdispatcher::ObserverGuard m_updateEventsSubscription;
    int m_updateCounter = 0;
    Spout* m_spoutSender = nullptr;

    omni::kit::hydra_texture::IHydraTextureFactory* m_hydraTextureFactory = nullptr;
    omni::kit::hydra_texture::IHydraTexture* m_hydraTexture = nullptr;
    carb::events::EventSubscription* m_renderEventSubscription = nullptr;

    int m_textureWidth = 1280;
    int m_textureHeight = 720;
};

} // namespace sender
} // namespace spout
} // namespace omni

CARB_PLUGIN_IMPL(pluginImplDesc, omni::spout::sender::SpoutSenderExtension)

void fillInterface(omni::spout::sender::SpoutSenderExtension& iface)
{
}