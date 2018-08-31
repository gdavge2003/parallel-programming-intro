Functional Decomposition

An ecosystem is simulated over a period of 72 months. In this ecosystem, grain grows or dies depending on the temperature and precipitation (generated randomly given a specific month). Deer population depends on the amount of grain. Human hunter-gatherers in the area depends on both grain growth and deer populations.

As oppose to using multithreading to achieve better performance, this project assigns to each thread a unique identity or sphere of control in the runtime environment. Each thread carries out its own functions independently, but receives information from other threads for its own behavior as well.
