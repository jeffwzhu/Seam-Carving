# Seam-Carving
Implementation of Content-Aware Resizing of Images in C++

This program uses the algorithm specified in [this paper](http://graphics.cs.cmu.edu/courses/15-463/2013_fall/hw/proj3-seamcarving/imret.pdf) to perform 'content-aware' resizing of images by always removing areas of low contrast first.

# Demo
Large reductions in sizing isn't ideal. For example, converting a 1200x600 image to a 600x300 image will result in a lot of features losing shape.
![](Seam%20Carving/Original%20Demo%20Scenes/sceneMedium1)
![](Seam%20Carving/Demo%20Resizing/medium1%20600x300.jpg)
