## Mandelbrot set

![Mandelbrot fractal in action](https://github.com/RaduHaulica/Mandelbrot-fractal/blob/0dee90d90c6d3b23cb4d887a4ed2277062180a0f/Mandelbrot/media/mandelbrot2.gif)

Visual representation of the [Mandelbrot fractal(wikipedia)](https://en.wikipedia.org/wiki/Mandelbrot_set), one of the most widely known pieces of mathematical art. It is the go-to example of an incredibly complex structure resulting from applying very simple rules.

It's the set of complex numbers that remains bounded when successively applying a specific formula.

I also implemented the ability to zoom in on a selected area of the fractal, in order to better showcase its complexity.

![Mandelbrot zoom](https://github.com/RaduHaulica/Mandelbrot-fractal/blob/1c66d38abdd0e3917d1ec903b86c1886b47105a1/Mandelbrot/media/mandelbrot%20zoom.gif)

I also tried my hand at multi threaded computation to see the impact a little optimization can have on CPU intensive jobs (spoiler: it does wonders).

![Mandelbrot multithreaded vs single threaded](https://github.com/RaduHaulica/Mandelbrot-fractal/blob/1c66d38abdd0e3917d1ec903b86c1886b47105a1/Mandelbrot/media/mandelbrot%20thread%20difference.png)

## How to use

Hold Z and click (don't drag) to create a rectangle that will be zoomed in

Press Space to toggle between single core and multi core computation
