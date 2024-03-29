#include <algorithm>
#include <cassert>
#include "BVH.hpp"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;

    root = recursiveBuild(primitives);

    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        switch (splitMethod) {
            case SplitMethod::NAIVE:
            {
                auto beginning = objects.begin();
                auto middling = objects.begin() + (objects.size() / 2);
                auto ending = objects.end();

                auto leftshapes = std::vector<Object*>(beginning, middling);
                auto rightshapes = std::vector<Object*>(middling, ending);

                assert(objects.size() == (leftshapes.size() + rightshapes.size()));

                node->left = recursiveBuild(leftshapes);
                node->right = recursiveBuild(rightshapes);

                node->bounds = Union(node->left->bounds, node->right->bounds);
                break;
            }
            case SplitMethod::SAH:
            {
                // 定义 10 个桶
                float min_cost = std::numeric_limits<float>::infinity();
                const int buckets = 10;
                int suitable_bucket_index = 1;
                for (int i = 1; i <= buckets; i++) {
                    auto beginning = objects.begin();
                    auto middling = objects.begin() + (objects.size() * i / buckets);
                    auto ending = objects.end();

                    auto leftshapes = std::vector<Object*>(beginning, middling);
                    auto rightshapes = std::vector<Object*>(middling, ending);

                    Bounds3 leftbounds, rightbounds;
                    for (auto object : leftshapes) {
                        leftbounds = Union(leftbounds, object->getBounds().Centroid());
                    }

                    for (auto object : rightshapes) {
                        rightbounds = Union(rightbounds, object->getBounds().Centroid());
                    }

                    float SA = leftbounds.SurfaceArea();
                    float SB = rightbounds.SurfaceArea();
                    float cost = 0.125 + (SA * leftshapes.size() + SB * rightshapes.size()) / centroidBounds.SurfaceArea();
                    if (cost < min_cost) {
                        suitable_bucket_index = i;
                        min_cost = cost;
                    }
                }

                auto beginning = objects.begin();
                auto middling = objects.begin() + (objects.size() * suitable_bucket_index / buckets);
                auto ending = objects.end();

                auto leftshapes = std::vector<Object*>(beginning, middling);
                auto rightshapes = std::vector<Object*>(middling, ending);
                assert(objects.size() == (leftshapes.size() + rightshapes.size()));

                node->left = recursiveBuild(leftshapes);
                node->right = recursiveBuild(rightshapes);

                node->bounds = Union(node->left->bounds, node->right->bounds);
                break;
            }
        }
    }

    return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // TODO Traverse the BVH to find intersection
    const std::array<int, 3> dirIsNeg = { int(ray.direction.x > 0), int(ray.direction.y > 0), int(ray.direction.z > 0) };
    if (!node->bounds.IntersectP(ray, ray.direction_inv, dirIsNeg))
    {
        return {};
    }

    if (node->object) {
        return node->object->getIntersection(ray);
    }

    /* 深度优先 */
    auto leftIntersection = getIntersection(node->left, ray);
    auto rightIntersection = getIntersection(node->right, ray);
    /*
    取最近打到的物体
    可能出现：
    1. 1 个打到 1 个没打到
    2. 两个全命中了物体，检查命中较近的物体
    */
    return leftIntersection.distance >= rightIntersection.distance ? rightIntersection : leftIntersection;
}