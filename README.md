# Seam-Carving
Implementation of Content-Aware Resizing of Images in C++

This program uses the algorithm specified in [this paper](http://graphics.cs.cmu.edu/courses/15-463/2013_fall/hw/proj3-seamcarving/imret.pdf) to perform 'content-aware' resizing of images by always removing areas of low contrast first.

# Demo

This image was resized from 1600x775 to 800x387 (half-size)

![](Seam%20Carving/Original%20Demo%20Scenes/sceneMedium2.jpg)
![](Seam%20Carving/Demo%20Resizing/medium2%20800x387.jpg)


This image was resized from 525x350 to 262x175 (half-size)

![](Seam%20Carving/Original%20Demo%20Scenes/sceneSmall2.jpg)
![](Seam%20Carving/Demo%20Resizing/small2%20262x175.jpg)


Sometimes, large reductions in sizing isn't ideal. For example, images with high-contrast areas (e.g. mountains) surrounded by low-contrast areas (e.g. sky) will sometimes completely lose shape--though a smaller resizing would work as intended.

![](Seam%20Carving/Original%20Demo%20Scenes/sceneMedium1.jpg)
![](Seam%20Carving/Demo%20Resizing/medium1%20600x300.jpg)
