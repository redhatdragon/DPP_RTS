#pragma once
#include <iostream>
#include <vector>
#include "BlueMath.h"

#pragma warning(push)
#pragma warning(disable:4244)
#pragma warning(disable:4018)

class SpatialHash {
    std::vector<void *> body;
public:
    SpatialHash(){
        body.reserve(64);
    }
    inline void addBody(void *_body){
		size_t bodySize = body.size();
		for (size_t i = 0; i < bodySize; i++) {
			if (body[i] == _body) {
				return;
			}
		}
		body.push_back(_body);
    }
    inline void removeBody(void *_body){
        size_t bodySize = body.size();
        for(size_t i = 0; i < bodySize; i++){
            if(body[i] == _body){
                body[i] = body[bodySize-1];
                body.pop_back();
                return;
            }
        }
    }
    inline std::vector<void *> getBodies(){
        return body;
    }
};

// A modular Spatial Hash Table, capable of working with any physics engine if used correctly.
class SpatialHashTable {
    SpatialHash *hash;
    uint32 width, height, hashWidth, hashHeight;
public:
    SpatialHashTable(uint32 _width, uint32 _height, uint32 _hashWidth, uint32 _hashHeight){
        width = _width; height = _height;
        hashWidth = _hashWidth; hashHeight = _hashHeight;
        hash = new SpatialHash[_width*_height];
    }
    inline bool insertBody(void *body, const Rect *posAndSize){
        if(posAndSize->x < 0 || posAndSize->y < 0 || posAndSize->w < 0 || posAndSize->h < 0){
            return false;
        }
        uint32 startingHashX = posAndSize->x/hashWidth;
        uint32 startingHashY = posAndSize->y/hashHeight;
        uint32 endingHashX = (posAndSize->x+posAndSize->w)/hashWidth;
        uint32 endingHashY = (posAndSize->y+posAndSize->h)/hashHeight;
        if(startingHashX > width || startingHashY > height
        || endingHashX > width || endingHashY > height){
            return false;
        }
        for(uint32 i = startingHashY; i <= endingHashY; i++){
            for(uint32 j = startingHashX; j <= endingHashX; j++){
                hash[j+i*width].addBody(body);
            }
        }
        //std::cout << startingHashX << " " << startingHashY << " ";
            //std::cout << endingHashX << " " << endingHashY << std::endl;
        return true;
    }
    inline bool removeBody(void *body, const Rect *posAndSize){
        if(posAndSize->x < 0 || posAndSize->y < 0 || posAndSize->w < 0 || posAndSize->h < 0){
            return false;
        }
        uint32 startingHashX = posAndSize->x/hashWidth;
        uint32 startingHashY = posAndSize->y/hashHeight;
        uint32 endingHashX = (posAndSize->x+posAndSize->w)/hashWidth;
        uint32 endingHashY = (posAndSize->y+posAndSize->h)/hashHeight;
        if(startingHashX > width || startingHashY > height
        || endingHashX > width || endingHashY > height){
            std::cout << posAndSize->x << posAndSize->w << std::endl;
            return false;
        }
        for(uint32 i = startingHashY; i <= endingHashY; i++){
            for(uint32 j = startingHashX; j <= endingHashX; j++){
                hash[j+i*width].removeBody(body);
            }
        }
        return true;
    }
    // Adjusts body in spatial hash table by using a velocity.
    // This will modify the posAndSize by adding the given velocity to it.
    // Useful if your body is AABB and you cast it's position and size vecter into a Rect
    // to pass in the posAndSize parameter.
    inline void moveBody(void *body, Rect *posAndSize, const Point *velocity){
        if(!removeBody(body, posAndSize)) std::cout << "remove failed" << std::endl;
        posAndSize->x += velocity->x;
        posAndSize->y += velocity->y;
        if(!insertBody(body, posAndSize)) {
            std::cout << "insert failed" << std::endl;
            //std::cout << posAndSize->x << " " << posAndSize->y << " "
                //<< posAndSize->w << " " << posAndSize->h << std::endl;
        }
    }
    inline std::vector<void *> getBodies(const Rect *posAndSize){
        if(posAndSize->x < 0 || posAndSize->y < 0 || posAndSize->w < 0 || posAndSize->h < 0){
            return std::vector<void *>();
        }
        uint32 startingHashX = posAndSize->x/hashWidth;
        uint32 startingHashY = posAndSize->y/hashHeight;
        uint32 endingHashX = (posAndSize->x+posAndSize->w)/hashWidth;
        uint32 endingHashY = (posAndSize->y+posAndSize->h)/hashHeight;
        if(startingHashX > width || startingHashY > height
        || endingHashX > width || endingHashY > height){
            return std::vector<void *>();
        }
        std::vector<void *> returnValue;
        returnValue.reserve(64);
        for(uint32 i = startingHashY; i <= endingHashY; i++){
            for(uint32 j = startingHashX; j <= endingHashX; j++){
                std::vector<void *> bodies = hash[j+i*width].getBodies();
                for(size_t k = 0; k < bodies.size(); k++){
                    bool alreadyHasBody = false;
                    for(size_t l = 0; l < returnValue.size(); l++){
                        if(returnValue[l] == bodies[k]){
                            alreadyHasBody = true;
                            break;
                        }
                    }
                    if(alreadyHasBody == false){
                        returnValue.push_back(bodies[k]);
                    }
                }
            }
        }
        return returnValue;
    }
};

#pragma warning(pop)