// Headless regression check: real Galaga/Player/notifications with a fake input engine.
#include "Player.h"
#include "EngineView.h"
#include "MathUtil.h"
#include <memory>
#include <array>
#define private public
#include "Galaga.h"
#undef private
#include "GameContext.h"
#include "Keyboard.h"
#include <cassert>
#include <queue>
#include <unordered_map>
#include <iostream>
namespace CMPUT350 {
void DrawContext::DrawRect(Rect, RGBColor) {}
void DrawContext::DrawCircle(Point2D, float, RGBColor) {}
void DrawContext::DrawText(const std::string&,int,Point2D,RGBColor) {}
void DrawContext::DrawCenteredText(const std::string&,int,Point2D,RGBColor) {}
int DrawContext::GetWindowWidth() {return 768;}
int DrawContext::GetWindowHeight() {return 1024;}
}
class Engine : public CMPUT350::EngineView {
public:
    CMPUT350::NotificationManager notifications;
    bool capture=false;
    std::queue<int> presses;
    std::unordered_map<int,std::string> down,up;
    std::vector<std::shared_ptr<CMPUT350::GameObject>> objects;
    void AddGameObject(std::shared_ptr<CMPUT350::GameObject> object) override {objects.push_back(object);}
    void InstallKeyDownNotification(int key,const std::string& name) override {down[key]=name;}
    void InstallKeyUpNotification(int key,const std::string& name) override {up[key]=name;}
    void RemoveKeyDownNotification(int key) override {down.erase(key);}
    void RemoveKeyUpNotification(int key) override {up.erase(key);}
    void SetKeyCapture(bool enabled) override {capture=enabled;presses={};}
    std::optional<int> PollKeyPress() override {
        if(presses.empty())return std::nullopt;
        int key=presses.front();presses.pop();return key;
    }
    std::string GetKeyName(int key) const override {return std::to_string(key);}
    const_iterator begin() const override {return objects.begin();}
    const_iterator end() const override {return objects.end();}
    const_iterator cbegin() const override {return objects.cbegin();}
    const_iterator cend() const override {return objects.cend();}
    void Press(int key) {
        if(capture) presses.push(key);
        else if(down.contains(key)) notifications.Notify(down.at(key));
    }
    void Release(int key) {
        if(!capture && up.contains(key)) notifications.Notify(up.at(key));
    }
};
int main() {
    using namespace CMPUT350;
    Engine engine;
    auto game=std::make_shared<Galaga>(768,1024);
    GameContext context{};
    context.mEngineView=&engine;context.mNotificationManager=&engine.notifications;
    context.CurrObject=game;game->Initialize(&context);
    // Gameplay input before the player exists must be harmless.
    engine.Press(Key::A);
    engine.Press(Key::K);
    assert(engine.capture && game->state==Galaga::State::Keybinds);
    engine.Press(Key::Up);game->Update(&context);
    assert(game->selectedBinding==2);
    engine.Press(Key::Down);engine.Press(Key::Enter);game->Update(&context);
    assert(game->selectedBinding==0 && game->waitingForKey);
    for(int invalid:{static_cast<int>(Key::D),static_cast<int>(Key::P),static_cast<int>(Key::Num1),static_cast<int>(Key::Num5),-1}) {
        engine.Press(invalid);game->Update(&context);
        assert(game->waitingForKey && game->bindings[0].key==Key::A);
        assert(!game->coinInserted && game->state==Galaga::State::Keybinds);
    }
    engine.Press(Key::Left);game->Update(&context);
    assert(!game->waitingForKey && game->bindings[0].key==Key::Left);
    assert(!engine.down.contains(Key::A) && !engine.up.contains(Key::A));
    assert(engine.down.at(Key::Left)=="MoveLeftDown" && engine.up.at(Key::Left)=="MoveLeftUp");
    // Cancel without leaving the menu or changing the selected binding.
    engine.Press(Key::Enter);engine.Press(Key::Escape);game->Update(&context);
    assert(engine.capture && !game->waitingForKey && game->bindings[0].key==Key::Left);
    engine.Press(Key::Down);engine.Press(Key::Enter);engine.Press(Key::Right);game->Update(&context);
    assert(game->bindings[1].key==Key::Right && !engine.down.contains(Key::D));
    engine.Press(Key::Down);engine.Press(Key::Enter);engine.Press(Key::F);game->Update(&context);
    assert(game->bindings[2].key==Key::F && !engine.down.contains(Key::Space));
    engine.Press(Key::Escape);game->Update(&context);
    assert(!engine.capture && game->state==Galaga::State::Waiting);
    // Coin/start still work, and opening controls is ignored during play.
    engine.Press(Key::Num5);engine.Press(Key::Num1);
    assert(game->state==Galaga::State::Startup);
    engine.Press(Key::K);assert(!engine.capture);
    game->Update(&context);
    auto player=game->player.lock();assert(player);
    context.CurrObject=player;player->Initialize(&context);
    float x=player->GetLocation().x;
    engine.Press(Key::A);player->Update(&context);assert(player->GetLocation().x==x);
    engine.Press(Key::Left);player->Update(&context);assert(player->GetLocation().x==x-15);
    engine.Release(Key::Left);player->Update(&context);assert(player->GetLocation().x==x-15);
    auto count=engine.objects.size();
    engine.Press(Key::Space);player->Update(&context);assert(engine.objects.size()==count);
    engine.Press(Key::F);player->Update(&context);assert(engine.objects.size()==count+1);
    // Session bindings survive game-over reset.
    game->state=Galaga::State::GameOver;game->returnToHomeFrame=0;
    game->Update(&context);
    assert(game->bindings[0].key==Key::Left && game->bindings[2].key==Key::F);
    std::cout<<"Passed: menu navigation, raw-key capture, reserved/duplicate rejection, cancel/back, replacing both movement edges, old-key removal, live Player movement/fire, and binding retention after reset.\n";
}
