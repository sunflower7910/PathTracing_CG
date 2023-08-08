# PathTracing_CG
Path Tracing Renderer: Cornell Box
Path Tracing is a rendering algorithm based on the Monte Carlo method, which tracks the propagation path of light in the scene, calculates pixel color values, and generates high-quality and realistic images. In this report, we will summarize the implementation process of path tracking algorithms and some optimization strategies to improve rendering efficiency and image quality.

The following is the rendering of the project:

![Rendering](https://github.com/sunflower7910/PathTracing_CG/blob/main/pathTracing.png)

The implementation of the path tracking algorithm for this project is mainly divided into the following steps:
1. Find the intersection point between the ray and the scene
If the intersection is a light source, if the ray hits the light source for the first time, it directly returns the color of the light source. If the ray hits the light source but is not directly illuminated by the pixel, it returns 0. This problem is solved when the intersection point is an object.
3. If the intersection point is an object, generate a ray directed by the object to a randomly generated light source, intersect with the scene, and the intersection point is light2Obj. If the ray hits the light source, calculate the direct illumination value, whether the recursive ray continues to bounce, and calculate the indirect illumination. (Russian Roulette)
4. Return the obtained ray value

In order to improve the rendering efficiency and image quality of the path tracking algorithm, we have adopted the following optimization strategies:
1. Monte Carlo sampling: When sampling in the light source and reflection/refraction direction, the Monte Carlo method is used for random sampling to avoid artifacts and noise caused by uniform sampling.
2. Surrounding Box Acceleration: By constructing the Bounding Volume Hierarchy (BVH), objects in the scene are accelerated. When light intersects with scene objects, BVH is first used to determine whether it is possible to intersect, avoiding traversal of all objects.
3. Shadow ray optimization to determine whether it is parallel or intersected on the back: For shadow rays, add a small offset to avoid self intersection and prevent shadow distortion.
4. Multi threaded rendering: Using multi threaded parallel rendering to divide the image into multiple regions, with each thread responsible for rendering one region, improves rendering efficiency. The method of using the C++thread library to achieve block based parallel computation and solution for ray tracing.
5. Environmental lighting: In order to enhance the realism of the scene, global environmental lighting is introduced to simulate the indirect reflection of light in the scene.

Conclusion:
By implementing the path tracking algorithm and adopting the above optimization strategy, we successfully generated high-quality and realistic rendered images. The optimized path tracking algorithm significantly improves rendering efficiency, reduces image noise, and makes the rendering results more realistic and delicate. However, path tracking algorithms are still computationally intensive rendering methods, requiring more computational resources for complex scenes or high-resolution images.
In future work, we can further research and apply more advanced ray tracing algorithms, such as importance sampling of ray tracing and machine learning based rendering methods, to further improve rendering efficiency and image quality, and meet higher rendering requirements.
Overall, path tracing is a highly promising rendering algorithm that, through continuous optimization and improvement, will play an increasingly important role in the field of computer graphics, bringing us more realistic and excellent rendering effects.
