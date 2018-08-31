Numeric Integration with OpenMP

In this project, I wrote a program that sets up two Bezier surfaces. Given a number of subdivisions, the xy-coordinate area of the surfaces is broken up into square tiles based on the subdivision value. Each square tile’s volume is calculated as height between the two surfaces multiplied by the tile area. The volume from all tiles are aggregated together, and the final volume between the two surfaces is calculated. This project explores the use of parallel programming to divide the work of calculating all the tiles’ heights.
