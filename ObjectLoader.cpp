/* 
 * File:   ObjectLoader.cpp
 * Author: ns
 * 
 * Created on October 9, 2018, 6:44 PM
 */

#include "ox/json.hpp"
#include "ObjectLoader.h"


using namespace std;

ObjectLoader::ObjectLoader()
{
}

ObjectLoader::ObjectLoader(const ObjectLoader& orig)
{
}

ObjectLoader::~ObjectLoader()
{
}

void _load_tileset(TILESET *p_tileset, const string &fname)
{
    //load file to buffer
    ox::file::buffer bf;
    ox::file::read(fname, bf);

    //parse
    Json::Reader reader;
    Json::Value value;
    reader.parse((char*)&bf.front(), (char*)&bf.front() + bf.size(), value, false);

    p_tileset->columns = value["columns"].asUInt();
    p_tileset->image = value["image"].asString();
    p_tileset->imageheight = value["imageheight"].asUInt();
    p_tileset->imagewidth = value["imagewidth"].asUInt();
    p_tileset->name = value["name"].asString();
    p_tileset->tilecount = value["tilecount"].asUInt();
    p_tileset->tileheight = value["tileheight"].asUInt();
    p_tileset->tilewidth = value["tilewidth"].asUInt();

}

void ObjectLoader::open(const string fname)
{
    //load file to buffer
    ox::file::buffer bf;
    ox::file::read(fname, bf);

    //parse
    Json::Reader reader;
    Json::Value value;
    reader.parse((char*)&bf.front(), (char*)&bf.front() + bf.size(), value, false);


    uint tilesets_count = value["tilesets"].size();
    uint tilesets_counter = 0;
    tilesets.resize(tilesets_count);

    /*
     * Временный костыль. Все слои всегда указывают
     * на последний tileset. Потому что я не знаю как
     * в формате tiled связаны слой и tileset.
     */
    spTILESET p_last_tileset;

    for(const auto &v : value["tilesets"])
    {
        spTILESET tileset = new TILESET;
        tilesets[tilesets_counter++] = tileset;

        string tileset_name = v["source"].asString();

        _load_tileset(tileset.get(), tileset_name);
        tileset->firstgid = v["firstgid"].asUInt();

        p_last_tileset = tileset;
    }


    short z_index = 0;
    for(const auto &v : value["layers"])
    {
        string layer_name = v["name"].asString();

        LAYER *p_layer = nullptr;

        if(layer_name.find("background") != string::npos)
        {
            LAYER l;
            backgrounds.push_back(l);
            p_layer = &backgrounds.back();
        }
        else if(layer_name.find("terrain") != string::npos)
        {
            LAYER l;
            terrains.push_back(l);
            p_layer = &terrains.back();
        }else if(layer_name == "landscape")
        {
            p_layer = &landscape;
        }
        else if(v.isMember("objects"))
        {
            for(const auto &vo : v["objects"])
            {
                objects.push_back(RectF(
                    vo["x"].asFloat(),
                    vo["y"].asFloat(),
                    vo["width"].asFloat(),
                    vo["height"].asFloat()
                ));
            }
            continue;
        }
        else
            continue;

        struct LAYER_OPTIONS &lay_opts = p_layer->options;

        p_layer->tileheight = v["tileheight"].asUInt();

        // Layer options
        lay_opts.height = v["height"].asUInt();
        lay_opts.width = v["width"].asUInt();
        lay_opts.opacity = v["opacity"].asFloat();
        lay_opts.visible = v["visible"].asBool();
        lay_opts.x = v["x"].asInt();
        lay_opts.y = v["y"].asInt();
        lay_opts.z_order = z_index++;
        p_layer->p_tileset = p_last_tileset;
        //lay_opts.name = v["name"].asString();

        vector<uint> &layer_data = p_layer->int_data;
        uint layer_data_size = v["data"].size();
        layer_data.resize(layer_data_size);

        uint n = 0;
        for(const auto &i : v["data"])
        {
            layer_data[n++] = i.asUInt();
        }
    }

}


LAYER::LAYER()
{}

LAYER::LAYER(const LAYER& o) : options(o.options),
        int_data(o.int_data), tileheight(o.tileheight),
        p_tileset(o.p_tileset)
{
}

LAYER::~LAYER(){}


Point LAYER::get_coords(const uint block_index) const
{
    Point p;

    const uint row_len = p_tileset->columns;

    // строка
    if(block_index == 0)
    {
        p.y = 0;
    }else
    {
        p.y = block_index / row_len;
        if(block_index % row_len == 0)
            p.y -= 1;
    }

    p.x = row_len - (((p.y+1) * row_len) - block_index) - 1;

    return p;
}


TILESET::TILESET() : firstgid(0), columns(0), image(""),
    name(""), imageheight(0), imagewidth(0), tilecount(0),
    tileheight(0), tilewidth(0)
{}

TILESET::TILESET(const TILESET& o) : firstgid(o.firstgid),
        columns(o.columns), image(o.image), name(o.name),
        imageheight(o.imageheight), imagewidth(o.imagewidth),
        tilecount(o.tilecount), tileheight(o.tileheight),
        tilewidth(o.tilewidth)
{}

TILESET::~TILESET()
{}

spTILESET ObjectLoader::get_tileset_by_name(string name)
{
    for(const auto &v : tilesets)
    {
        if(v->name == name)
        {
            return v;
        }
    }
    return nullptr;
}

spTILESET ObjectLoader::get_tileset_by_id(uint id)
{
    for(const auto &v : tilesets)
    {
        if(v->firstgid == id)
        {
            return v;
        }
    }
    return nullptr;
}
