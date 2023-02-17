#include <SFML/Graphics.hpp>

#include <chrono>
#include <iostream>
#include <format>
#include <thread>

#include "utils.h"

int main()
{
    const float MLB = -2.0f;
    const float MUB = 2.0f;
    const float XB = 1000.0f;
    const float YB = 800.0f;

    baseColor = sf::Color::Red;

    sf::RenderWindow window(sf::VideoMode(XB, YB), "SFML works!");

    sf::Image img;
    img.create(XB, YB, sf::Color::Blue);

    sf::Vector2f mandelbrotSetTopLeft = { -2,-2 };
    sf::Vector2f mandelbrotSetBottomRight = { 2 , 2 };

    //{
    //    Profiler p1("compute whole image");
    //    compute(img, { 0,0 }, { XB, YB }, mandelbrotSetTopLeft, mandelbrotSetBottomRight);
    //}

    int partitionFactor{ 2 };
    int threadCount{ partitionFactor * partitionFactor };
    std::vector<std::thread> threads;

    //{
    //    Profiler p2("4 THREADS BABY!");
    //    for (int i = 0; i < threadCount; i++)
    //    {
    //        sf::Vector2f topLeft;
    //        topLeft.x = (i % partitionFactor) * (XB / partitionFactor);
    //        topLeft.y = (i / partitionFactor) * (YB / partitionFactor);
    //        sf::Vector2f bottomRight;
    //        bottomRight.x = (i % partitionFactor) * (XB / partitionFactor) + XB / partitionFactor;
    //        bottomRight.y = (i / partitionFactor) * (YB / partitionFactor) + YB / partitionFactor;
    //        threads.push_back(std::thread(compute, std::ref(img), topLeft, bottomRight, mandelbrotSetTopLeft, mandelbrotSetBottomRight));
    //    }

    //    for (int i = 0; i < threads.size(); i++)
    //    {
    //        threads[i].join();
    //    }
    //}

    int partitionFactor2{ 3 };
    int threadCount2{ partitionFactor2 * partitionFactor2 };
    std::vector<std::thread> threads2;

    {
        Profiler p3("9 more threads");
        for (int i = 0; i < threadCount2; i++)
        {
            sf::Vector2f topLeft;
            topLeft.x = (i % partitionFactor2) * (XB / partitionFactor2);
            topLeft.y = (i / partitionFactor2) * (YB / partitionFactor2);
            sf::Vector2f bottomRight;
            bottomRight.x = (i % partitionFactor2) * (XB / partitionFactor2) + XB / partitionFactor2;
            bottomRight.y = (i / partitionFactor2) * (YB / partitionFactor2) + YB / partitionFactor2;
            threads2.push_back(std::thread(compute, std::ref(img), topLeft, bottomRight, mandelbrotSetTopLeft, mandelbrotSetBottomRight));
        }

        for (int i = 0; i < threads2.size(); i++)
        {
            threads2[i].join();
        }
    }

    sf::Sprite imgSprite;
    sf::Texture imgTexture;
    imgTexture.loadFromImage(img);
    imgSprite.setTexture(imgTexture);

    // ====================================
    //  setting up stuff
    // ====================================
    sf::Font defaultFont;
    defaultFont.loadFromFile("Roboto-Bold.ttf");
	
    sf::Text helpText;
    helpText.setFont(defaultFont);
    helpText.setCharacterSize(12);
	helpText.setFillColor(sf::Color::White);
    helpText.setString("Hold Z and click (don't drag) to draw a zoom rectangle\nPress SPACE to toggle single/multi threaded computations");
    helpText.setPosition(50.0f, 750.0f);

    sf::Text zoomFactorText;
    zoomFactorText.setFont(defaultFont);
    zoomFactorText.setCharacterSize(12);
    zoomFactorText.setFillColor(sf::Color::Yellow);
    zoomFactorText.setPosition({ 20, 20 });

    std::string threadString{ "Single threaded" };
    sf::Text threadText;
    threadText.setFont(defaultFont);
    threadText.setCharacterSize(12);
    threadText.setFillColor(sf::Color::Yellow);
    threadText.setPosition({ 20, 40 });

    sf::Text durationText;
    durationText.setFont(defaultFont);
    durationText.setCharacterSize(12);
    durationText.setFillColor(sf::Color::Yellow);
    durationText.setPosition({ 20, 60 });
    double computeDuration{ 0 };

    float zoomFactor{ 1 };
    float inputCooldown{ 0.2f }, inputCooldownAccumulator{ 0.0f };

    sf::Clock frameClock;
    float dt;

    sf::Vector2f lastTopLeft{ mandelbrotSetTopLeft };
    sf::Vector2f lastBottomRight{ mandelbrotSetBottomRight };

    bool useThreads{ false };

    sf::RectangleShape selector;
    selector.setFillColor(sf::Color::Transparent);
    selector.setOutlineColor(sf::Color::White);
    selector.setOutlineThickness(1);
    bool firstPointSelected{ false };
    sf::Vector2f firstPoint;
    sf::Vector2f secondPoint;


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        sf::Vector2i mPos = sf::Mouse::getPosition(window);

        dt = frameClock.restart().asSeconds();

        if (inputCooldownAccumulator > 0.0f)
        {
            inputCooldownAccumulator = std::fmax(0.0f, inputCooldownAccumulator - dt);
        }
        else
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                inputCooldownAccumulator = inputCooldown;
                useThreads = !useThreads;
                threadString = useThreads ? "Multi threaded" : "Single threaded";
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            {
                inputCooldownAccumulator = inputCooldown;
                std::cout << "Mouse pos: " << mPos.x << ", " << mPos.y << '\n';
                //std::pair<sf::Vector2f, sf::Vector2f> boundaries = zoomIn(sf::Vector2f(mPos), { 0,0 }, { XB, YB }, lastTopLeft, lastBottomRight);
                //lastTopLeft = boundaries.first;
                //lastBottomRight = boundaries.second;

                if (!firstPointSelected)
                {
                    firstPoint = sf::Vector2f(mPos);
                    firstPointSelected = true;
                }
                else
                {
                    secondPoint = sf::Vector2f(mPos);

                    //check second > first
                    if (secondPoint.x < firstPoint.x)
                    {
                        std::swap(secondPoint.x, firstPoint.x);
                    }
                    if (secondPoint.y < firstPoint.y)
                    {
                        std::swap(secondPoint.y, firstPoint.y);
                    }

                    std::pair<sf::Vector2f, sf::Vector2f> boundaries = zoomInRectangle(firstPoint, secondPoint, { 0 ,0 }, { XB, YB }, lastTopLeft, lastBottomRight);
                    lastTopLeft = boundaries.first;
                    lastBottomRight = boundaries.second;

                    zoomFactor *= computeZoomFactor(firstPoint, secondPoint, { 0, 0 }, { XB, YB });

					if (useThreads)
					{
                        Profiler p("zoom multi thread");
						computeThreads(img, 4, XB, YB, lastTopLeft, lastBottomRight);
                        computeDuration = p.poke();
					}
					else
					{
						Profiler p("zoom single thread");
						compute(img, { 0,0 }, { XB, YB }, lastTopLeft, lastBottomRight);
                        computeDuration = p.poke();
					}

					imgTexture.loadFromImage(img);
					imgSprite.setTexture(imgTexture);
                    firstPointSelected = false;
                }
            }
        }

        if (firstPointSelected)
        {
            selector.setPosition(firstPoint);
            selector.setSize(sf::Vector2f(mPos) - firstPoint);
        }
		zoomFactorText.setString(std::format("Zoom factor: {}", zoomFactor));
        threadText.setString(threadString);
        if (computeDuration != 0)
            durationText.setString(std::format("Compute duration: {}", computeDuration));

        window.clear();
        window.draw(imgSprite);
        window.draw(helpText);
        if (firstPointSelected)
            window.draw(selector);
        window.draw(zoomFactorText);
        window.draw(threadText);
        if (computeDuration != 0)
            window.draw(durationText);
        window.display();
    }

    return 0;
}