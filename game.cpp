#include "game.h"
#include "state.hpp"
#include "stateIdentifiers.hpp"

#include "titlestate.hpp"
#include "menustate.hpp"
#include "gamestate.hpp"

#include "global.h"
#include "utility.hpp"   // for topLeftOrigin() 

#include <string>

Game* Game::s_pGame = new Game();
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
:mTextures()  
, mFonts()     
//, mPlayer()      // TODO：这里去掉，只在World里面加
, mStateStack(State::Context(mWindow, mTextures, mFonts))
//, mStateStack(State::Context(mWindow, mTextures, mFonts, mPlayer))
, mStatisticsText()
, mStatisticsUpdateTime()
, mStatisticsNumFrames(0)
{
	mWindow.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Kalpa", sf::Style::Default); // TODO：可以考虑支持sf::Style::Fullscreen，做一个配置文件，配置是否全屏，参考Cendric
	mWindow.setKeyRepeatEnabled(false); // you will only get a single event when the key is pressed.
	mWindow.setFramerateLimit(60);      // TODO

	//载入icon
	if (!mIcon.loadFromFile("./Media/icon.png"))
		throw std::runtime_error("ResourceHolder::load - Failed to load icon.png");
	mWindow.setIcon(mIcon.getSize().x, mIcon.getSize().y, mIcon.getPixelsPtr());

    // 载入字体和纹理  (clw note: 游戏状态里面的纹理等，是在World类里面加载)
	mFonts.load(Fonts::Main, "./Media/Traditional_Ancient_India.ttf"); 
	mTextures.load(Textures::TitleScreen, "./Media/Textures/TitleScreen.jpg"); 
	mStatisticsText.setFont(mFonts.get(Fonts::Main));  
	
	mStatisticsText.setCharacterSize(FONT_SIZE_MIDDLE);
	//topRightOrigin(mStatisticsText);  //clw note: 因为此时mStatisticsText为空，所以这一句不会起任何作用，getLocalBounds()得到的全部是0
	mStatisticsText.setPosition(MAP_FONT_BOARDER, MAP_FONT_BOARDER);

	// 注册所需的状态
	RegisterStates();

	//将第一个状态（TitleState）push到状态栈mStateStack中
	mStateStack.pushState(States::Title);

}

Game::~Game()
{

}

void Game::Run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;

	/************************************************************************/
	/* 3、最小时间步长：结合了动态时间步长和固定时间步长的方法。
	/* 通过确保传入 update(dt) 方法的时间参数不那么高（固定为TimePerFrame），而渲染可以实时进行（600FPS也ok）
	/* 也就是我们通过这个方法设置了最小的帧数，但是没有最大的。
	/* 详见https://blog.csdn.net/qq_33567644/article/details/91126517?depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-1&utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromBaidu-1
	/************************************************************************/
	while (mWindow.isOpen())
	{
		sf::Time dt = clock.restart(); //puts the time counter back to zero; returns the time elapsed
		timeSinceLastUpdate += dt;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			ProcessInput();
			Update(TimePerFrame);

			// Check inside this loop, because stack might be empty before update() call
			if (mStateStack.isEmpty())
				mWindow.close();
		}

		UpdateStatistics(dt);
		Render();
	}
}

void Game::ProcessInput()
{
	sf::Event event;  // 需要 #include <SFML/Window/Event.hpp>，在state.hpp有
	while (mWindow.pollEvent(event))  //Pop the event on top of the event queue，非阻塞，waitEvent是阻塞
		                             //(1)一次可能有多个事件; (2)如果不加这句，窗口不会处理来自用户的任何事件，处于“忙碌”状态
	{
		mStateStack.handleEvent(event);

		if (event.type == sf::Event::Closed)
			mWindow.close();
	}
}

void Game::Update(sf::Time dt)
{
	mStateStack.update(dt);
}


void Game::Render()
{
	//static int i = 0;
	mWindow.clear();

	mStateStack.draw();

	mWindow.setView(mWindow.getDefaultView()); // clw note: 只有这样，该Text才会在人物行走之后，依然一直在窗口左上角https://www.sfml-dev.org/tutorials/2.5/graphics-view.php
	mWindow.draw(mStatisticsText);

	mWindow.display();
}

void Game::UpdateStatistics(sf::Time dt)
{
	////// 计算FPS  TODO
	
	////////////////////////////////////////////////////
	mStatisticsUpdateTime += dt;
	mStatisticsNumFrames += 1;
	//method 1
	//if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	//{
	//	mStatisticsText.setString("FPS: " + std::to_string(mStatisticsNumFrames));

	//	mStatisticsUpdateTime -= sf::seconds(1.0f);
	//	mStatisticsNumFrames = 0;
	//}

	//method 2
	if (mStatisticsNumFrames == 60)
	{
		mStatisticsText.setString("FPS: " + std::to_string(1.0 / mStatisticsUpdateTime.asSeconds() * 60.0).substr(0, 4));

		//mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsUpdateTime = sf::Time::Zero;
		mStatisticsNumFrames = 0;
	}
	////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////
	//method 3
	//mStatisticsUpdateTime += dt;
	//if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	//{
	//	mStatisticsText.setString("FPS: " + std::to_string(1000000.0f / dt.asMicroseconds()).substr(0, 4));
	//	mStatisticsUpdateTime -= sf::seconds(1.0f);
	//}
	/////////////////////////////////////////////////////////
}

void Game::RegisterStates()
{
	mStateStack.registerState<TitleState>(States::Title);
	mStateStack.registerState<MenuState>(States::Menu);
	mStateStack.registerState<GameState>(States::Game);
	//mStateStack.registerState<PauseState>(States::Pause);
}