/*****************************************************************//**
 * \file   object.hpp
 * \brief  ObjectData structure and Object class and subclasses
 * 
 * \author Ben
 * \date   August 2024
 *********************************************************************/
#pragma once

#include <vector>

#include "vec.hpp"

/**
 * \param int type
 * \param int material
 * \param float x
 * \param float y
 * \param float z
 * \param float l1
 * \param float dx
 * \param float dy
 * \param float dz
 * \param bool down
 * \param bool moving
 */
struct ObjectData {
    int type, material;
    vec r;
    float l1, mass;
    vec vel, angVel;
    bool down = true, moving = false, floor = false;//, firstHash = true;
};

class Object {
protected:
    ObjectData data;
    float lastT;
    VecOps vOps;

    /**
     * Returns the evaluated SDF
     * 
     * \return float
     */
    float SDF();
public:
    /**
     * Initialises the object class
     * 
     * \param ObjectData data
     */
    Object(ObjectData data);

    /**
     * Updates object data variables
     * 
     * \param float f
     */
    void update(float f);

    /**
     * Applies gravity to an object
     * 
     * \param float g
     * \param float r
     */
    void updateObject(float g, float r);

    /**
     * Determines if an object has been clicked
     * 
     * \param float xpos
     * \param float ypos
     * \param float zpos
     * \param float finalT
     * \return bool
     */
    virtual bool isClicked(std::vector<float> ro, std::vector<float> rd, float finalT);

    /**
     * Checks if two objects have collided
     * 
     * \param Object other
     * \return bool
     */
    bool checkCollision(Object& other);

    /**
     * Resolves the collision between two objects
     * 
     * \param Object other
     */
    void resolveCollision(Object& other);

    /**
     * Gets the last SDF t value
     * 
     * \return float
     */
    float getLastT();

    /**
     * Resets lastT
     * 
     */
    void reset();

    /**
     * Returns all data relating to the object
     * 
     * \return ObjectData*
     */
    ObjectData *getData();
};

class Sphere : public Object {
protected:
    /**
     * Sphere SDF
     * 
     * \param float px
     * \param float py
     * \param float pz
     * \param float cx
     * \param float cy
     * \param float cz
     * \param float r
     * \return float
     */
    float SDF(vec p, vec c, float r);
public:
    /**
     * Initialises the sphere object class
     *
     * \param ObjectData data
     */
    Sphere(ObjectData data) : Object(data) {}

    bool isClicked(std::vector<float> ro, std::vector<float> rd, float finalT) override;
};