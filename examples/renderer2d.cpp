#include <levikno/lvn_renderer.h>


int main(int argc, char** argv)
{
    LvnContextCreateInfo lvnCreateInfo{};
    lvnCreateInfo.logging.enableLogging = true;
    lvnCreateInfo.logging.enableGraphicsApiDebugLogs = true;
    lvnCreateInfo.windowapi = Lvn_WindowApi_glfw;
    lvnCreateInfo.graphicsapi = Lvn_GraphicsApi_vulkan;

    lvn::createContext(&lvnCreateInfo);

    lvn::renderInit("renderer2d", 800, 600);

    LvnWindow* window = lvn::getRendererWindow();
    lvn::windowSetVSync(window, true);

    LvnImageData whiteImage = std::move(lvn::loadImageData("res/images/debug.png", 4, true));
    LvnImageData imageData = std::move(lvn::loadImageData("res/images/woodBox.jpg", 4, true));

    LvnTextureCreateInfo textureCreateInfo{};
    textureCreateInfo.imageData = imageData;
    textureCreateInfo.format = Lvn_TextureFormat_Unorm;
    textureCreateInfo.wrapS = Lvn_TextureMode_Repeat;
    textureCreateInfo.wrapT = Lvn_TextureMode_Repeat;
    textureCreateInfo.minFilter = Lvn_TextureFilter_Linear;
    textureCreateInfo.magFilter = Lvn_TextureFilter_Linear;

    LvnTexture* texture;
    lvn::createTexture(&texture, &textureCreateInfo);

    textureCreateInfo.imageData = whiteImage;
    LvnTexture* whiteTexture;
    lvn::createTexture(&whiteTexture, &textureCreateInfo);

    lvn::rendererUploadTexture(texture);
    lvn::rendererUploadTexture(whiteTexture);

    LvnSprite sprite = lvn::configSpriteInit({200.0f,200.0f}, {0.0f,0.0f,1.0f,1.0f}, texture);
    LvnSprite sprite2 = lvn::configSpriteInit({400.0f,400.0f}, {0.0f,0.0f,1.0f,1.0f}, whiteTexture);


    while (lvn::renderWindowOpen())
    {
        lvn::windowPollEvents();

        lvn::drawBegin();
        lvn::drawClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        lvn::drawRect({0.0f, 0.0f}, {50.0f, 50.0f}, {255,255,255,255});
        lvn::drawRect({0.0f, 100.0f}, {128.0f, 128.0f}, {255,0,127,255});
        lvn::drawRect({cos(lvn::getContextTime()) * 200.0f, sin(lvn::getContextTime()) * 200.0f}, {20.0f, 80.0f}, {127,127,255,255});

        lvn::drawTriangle({-400.0f, 0.0f}, {-50.0f, 0.0f}, {0.0f, 50.0f}, {50.0f, 0.0f}, {255,255,0,255});
        lvn::drawTriangle({-400.0f, -100.0f}, {-50.0f, 0.0f}, {0.0f, 50.0f}, {50.0f, sin(lvn::getContextTime() * 2) * 25.0f + 25.0f}, {255,0,255,255});

        lvn::drawCircle({-400.0f, 400.0f}, 200.0f, {0, 255, 255, 255});
        lvn::drawCircleSector({-400.0f, 400.0f}, 150.0f, 90.0f, 315.0f, {20, 20, 128, 255});
        lvn::drawPolyNgon({-400.0f, 400.0f}, 100.0f, 6, {100, 0, 20, 255});
        lvn::drawPolyNgonSector({-400.0f, 400.0f}, 50.0f, 0.0f, 145.0f, 3, {0, 0, 0, 255});
        lvn::drawCircleSector({-100.0f, 400.0f}, 50.0f, 0.0f, lvn::clampAngleDeg(lvn::getContextTime() * 45.0f), {20, 20, 128, 255});
        lvn::drawPolyNgonSector({-100.0f, 600.0f}, 50.0f, 0.0f, 90.0f, abs(8 * cos(lvn::getContextTime() * 0.5f)), {128, 128, 128, 255});

        lvn::drawSprite(sprite, {300.0f, 100.0f}, {255,255,255,255});

        lvn::drawText("hello world", {200.0f, -300.0f}, {225,255,255,255}, 1.0f);
        lvn::drawText("giga\nchad", {200.0f + 50 * sin(lvn::getContextTime()), -400.0f}, {225,255,255,255}, 2.0f);
        lvn::drawTextBox("The quick brown fox jumps over the lazy dog", {0.0f, 450.0f}, {225,0,255,255}, 1.0f, 2.0f, 200.0f + 100 * cos(lvn::getContextTime()));

        lvn::drawEnd();
    }

    lvn::destroyTexture(texture);
    lvn::destroyTexture(whiteTexture);

    lvn::renderTerminate();
    lvn::terminateContext();

    return 0;
}
