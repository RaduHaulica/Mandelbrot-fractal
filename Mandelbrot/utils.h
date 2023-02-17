#pragma once

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

    Profiler(std::string label) :
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
    return std::sqrt(v.x * v.x + v.y * v.y);
}

float computeZoomFactor(sf::Vector2f firstPoint, sf::Vector2f secondPoint, sf::Vector2f topLeftBoundary, sf::Vector2f bottomRightBoundary)
{
    return norm(bottomRightBoundary - topLeftBoundary) / norm(secondPoint - firstPoint);
}