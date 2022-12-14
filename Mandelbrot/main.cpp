#include <SFML/Graphics.hpp>

#include <chrono>
#include <iostream>
#include <format>
#include <thread>

class Complex
{
public:
    float im, re;
};

sf::Color baseColor;

sf::Vector2f quadraticMap(sf::Vector2f zn, sf::Vector2f c)
{
    float re = zn.x * zn.x - zn.y * zn.y + c.x;
    float im = 2 * zn.x * zn.y + c.y;
    return { re, im };
}

float normSquared(sf::Vector2f point)
{
    return point.x * point.x + point.y + point.y;
}

sf::Vector2f mapPixelToInterval(sf::Vector2f point, sf::Vector2f boundary, sf::Vector2f topLeftInterval, sf::Vector2f bottomRightInterval)
{
    float x = topLeftInterval.x + point.x / boundary.x * (bottomRightInterval.x - topLeftInterval.x);
    float y = topLeftInterval.y + point.y / boundary.y * (bottomRightInterval.y - topLeftInterval.y);
    return { x, y };
}

sf::Color createColorGradient(sf::Color base, float value)
{
    //std::vector<sf::Color> colors = { sf::Color::Cyan, sf::Color::Red, sf::Color::Blue, sf::Color::Green, sf::Color::Yellow };
    //sf::Color result = colors[int(value*4)];
    sf::Color result = base;
    result.r = value * result.r;
    result.g = value * result.g;
    result.b = value * result.b;
    return result;
}

std::vector<float> createDistribution(sf::Vector2f interval, float granularity)
{
    float lowerBoundary = interval.x;
    float upperBoundary = interval.y;
    std::vector<float> result;
    float ratio = (upperBoundary - lowerBoundary) / granularity;
    for (float i = 0; i < granularity; i++)
    {
        result.push_back(lowerBoundary + i * ratio);
    }
    return result;
}

std::pair<sf::Vector2f, sf::Vector2f> zoomIn(sf::Vector2f position, sf::Vector2f topLeft, sf::Vector2f bottomRight, sf::Vector2f topLeftInterval, sf::Vector2f bottomRightInterval)
{
    float x = topLeftInterval.x + position.x / bottomRight.x * (bottomRightInterval.x - topLeftInterval.x);
    float xOffset = (bottomRightInterval.x - topLeftInterval.x) / 10;
    float y = topLeftInterval.y + position.y / bottomRight.y * (bottomRightInterval.y - topLeftInterval.y);
    float yOffset = (bottomRightInterval.y - topLeftInterval.y) / 10;

    std::pair<sf::Vector2f, sf::Vector2f> result;
    result.first.x = x - xOffset;
    result.first.y = y - yOffset;
    result.second.x = x + xOffset;
    result.second.y = y + yOffset;

    return result;
}

std::pair<sf::Vector2f, sf::Vector2f> zoomInRectangle(sf::Vector2f topLeftRectangle, sf::Vector2f bottomRightRectangle, sf::Vector2f topLeftBoundary, sf::Vector2f bottomRightBoundary, sf::Vector2f lastTopLeftNumeric, sf::Vector2f lastBottomRightNumeric)
{
    std::pair<sf::Vector2f, sf::Vector2f> result;
    float x = lastTopLeftNumeric.x + topLeftRectangle.x / bottomRightBoundary.x * (lastBottomRightNumeric.x - lastTopLeftNumeric.x);
    float y = lastTopLeftNumeric.y + topLeftRectangle.y / bottomRightBoundary.y * (lastBottomRightNumeric.y - lastTopLeftNumeric.y);
    result.first.x = x;
    result.first.y = y;
    x = lastTopLeftNumeric.x + bottomRightRectangle.x / bottomRightBoundary.x * (lastBottomRightNumeric.x - lastTopLeftNumeric.x);
    y = lastTopLeftNumeric.y + bottomRightRectangle.y / bottomRightBoundary.y * (lastBottomRightNumeric.y - lastTopLeftNumeric.y);
    result.second.x = x;
    result.second.y = y;
    return result;
}

void compute(sf::Image& img, sf::Vector2f topLeft, sf::Vector2f bottomRight, sf::Vector2f topLeftInterval, sf::Vector2f bottomRightInterval)
{
    int iterations = 1000;
	for (float j = topLeft.y; j < bottomRight.y; j++)
		for (float i = topLeft.x; i < bottomRight.x; i++)
        {
            //img.setPixel(i, j, createColorGradient(baseColor, 1.0f));
            //continue;

            [&] // break and continue; + self call bonus ^_^
            {
                sf::Vector2f result{ 0,0 };
                sf::Vector2f c = mapPixelToInterval({ i,j }, sf::Vector2f(img.getSize().x, img.getSize().y), topLeftInterval, bottomRightInterval);
                int k = 0;
                while (k < iterations)
                {
                    result = quadraticMap(result, c);
                    if (normSquared(result) > 4.0f)
                    {
                        sf::Color color = createColorGradient(baseColor, k / float(iterations));
                        img.setPixel(i, j, color);
                        return;
                    }
                    k++;
                }
                sf::Color color = createColorGradient(baseColor, k / float(iterations));
                img.setPixel(i, j, color);
            }(); // self called lambda
        }
}

//void computeFromVectors(sf::Image& img, std::vector<float>& xVector, std::vector<float>& yVector)
//{
//    int iterations = 100;
//    for (int j = 0; j < yVector.size(); j++)
//    {
//        for (int i = 0; i < xVector.size(); i++)
//        {
//            [&] // break and continue; + self call bonus ^_^
//            {
//                sf::Vector2f result{ 0,0 };
//                sf::Vector2f c = { xVector[i], yVector[j] };
//                int k = 0;
//                while (k < iterations)
//                {
//                    result = quadraticMap(result, c);
//                    if (normSquared(result) > 4.0f)
//                    {
//                        sf::Color color = createColorGradient(baseColor, k);
//                        img.setPixel(i, j, color);
//                        return;
//                    }
//                    k++;
//                }
//                sf::Color color = createColorGradient(baseColor, k);
//                img.setPixel(i, j, color);
//            }(); // self called lambda
//        }
//    }
//}

class Profiler
{
public:
    std::chrono::time_point<std::chrono::steady_clock> _startTime;
    std::string _label;

    Profiler(std::string label):
        _label{ label }
    {
        _startTime = std::chrono::steady_clock::now();
        std::printf(std::format("{} - started.\n", label).c_str());
    };

    ~Profiler()
    {
        std::printf(std::format("{} - time passed: {} seconds.\n", _label, std::chrono::duration<double>(std::chrono::steady_clock::now() - _startTime).count()).c_str());
    }

    double poke()
    {
        return std::chrono::duration<double>(std::chrono::steady_clock::now() - _startTime).count();
    }
};

void computeThreads(sf::Image& img, int partitionFactor, int xPixels, int yPixels, sf::Vector2f topLeftNumericValues, sf::Vector2f bottomRightNumericValues)
{
    int threadCount = partitionFactor * partitionFactor;
    std::vector<std::thread> threads;

    Profiler pClick("zooming in with threads");
    threads.clear();
    for (int i = 0; i < threadCount; i++)
    {
        sf::Vector2f topLeft;
        topLeft.x = (i % partitionFactor) * (xPixels / partitionFactor);
        topLeft.y = (i / partitionFactor) * (yPixels / partitionFactor);
        sf::Vector2f bottomRight;
        bottomRight.x = (i % partitionFactor) * (xPixels / partitionFactor) + xPixels / partitionFactor;
        bottomRight.y = (i / partitionFactor) * (yPixels / partitionFactor) + yPixels / partitionFactor;
        threads.push_back(std::thread(compute, std::ref(img), topLeft, bottomRight, topLeftNumericValues, bottomRightNumericValues));
    }

    for (int i = 0; i < threads.size(); i++)
    {
        threads[i].join();
    }
}

float norm(sf::Vector2f v)
{
    return std::sqrt(v.x*v.x + v.y*v.y);
}

float computeZoomFactor(sf::Vector2f firstPoint, sf::Vector2f secondPoint, sf::Vector2f topLeftBoundary, sf::Vector2f bottomRightBoundary)
{
    return norm(bottomRightBoundary - topLeftBoundary) / norm(secondPoint - firstPoint);
}

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
    sf::Font defaultFont;
    defaultFont.loadFromFile("Roboto-Bold.ttf");
	
    sf::Text helpText;
    helpText.setFont(defaultFont);
    helpText.setCharacterSize(12);
	helpText.setFillColor(sf::Color::White);
    helpText.setString("Hold Z and click to draw a zoom rectangle\nPress SPACE to toggle single/multi threaded computations");
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