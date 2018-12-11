/* 
 * File:   ObjectLoader.h
 * Author: ns
 *
 * Created on October 9, 2018, 6:44 PM
 */

#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include <string>
#include <vector>
#include <sys/types.h>
#include "oxygine-framework.h" 
#include "base.h"
#include "TiledSprite.h"

using namespace std;
using namespace oxygine;


class ObjectLoader : public Base {
public:
    ObjectLoader();
    ObjectLoader(const ObjectLoader& orig);
    virtual ~ObjectLoader();

    vector<LAYER> backgrounds;
    vector<LAYER> terrains;
    LAYER landscape;
    vector<spTILESET> tilesets;
    vector<RectF> objects;

    spTILESET get_tileset_by_name(string name);
    spTILESET get_tileset_by_id(uint id);

    void open(const string fname);

};

#endif /* OBJECTLOADER_H */
