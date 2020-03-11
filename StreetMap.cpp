#include "provided.h"
#include <string>
#include <vector>
#include <functional>

#include <iostream> // needed for any I/O
#include <fstream>  // needed in addition to <iostream> for file I/O
#include <sstream>  // needed in addition to <iostream> for string stream I/O
using namespace std;

#include "ExpandableHashMap.h"

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap<GeoCoord, vector<StreetSegment>> map;
};

StreetMapImpl::StreetMapImpl()
{
    
}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
    /*
     For each street segment S being processed in the Open Map data file:
     1. Extract the starting GeoCoord (i.e., latitude, longitude) from S and call it B
     2. some_mapping_data_structure[B].add_item(S)
     3. Extract the ending GeoCoord and call it E
     4. R = Reverse(S) // Reverse() swaps the starting/ending GeoCoords in segment S
     5. some_mapping_data_structure[E].add_item(R)
     */
    
    ifstream infile(mapFile);    // infile is a name of our choosing
    if ( ! infile )  // check opening the file fail?
    {
        cerr << "Error: Cannot open mapdatasmall.txt!" << endl;
        return false;
    }
    
    //string line;
    for (;;)//while (getline(infile, line))
    {
        string streetName;
        if (!getline(infile, streetName))
            return false;
        
        int numSegments;
        infile >> numSegments; //get number of segements for this street
        infile.ignore(10000, '\n');
        
        for (int i=0; i<numSegments; i++)
        {
            //get starting and ending geocoordinates
            string startingLat, startingLong, endingLat, endingLong;;
            if (!getline(infile, startingLat, ' ') || !getline(infile, startingLong, ' ') || !getline(infile, endingLat, ' ') || !getline(infile, endingLong))
                return false;
        
            GeoCoord start(startingLat, startingLong); //construct starting geocoordinate
            GeoCoord end(endingLat, endingLong); //construct ending geocoordinate
            
            StreetSegment s(start, end, streetName); //create street segment
            vector<StreetSegment>* segFound = map.find(start); //get pointer to vector associated with start key
            if (segFound != nullptr) //if start geocoordinate already existing key
                (*segFound).push_back(s); //add reversed segment to street segment vector
            else
            {
                vector<StreetSegment> segs{s}; //put street segment into a vector
                map.associate(start, segs); //associate start geocoordinate to street segment vector
            }
            
            //do the same for reverse
            StreetSegment r(end, start, streetName);//construct reverse street segment
            
            vector<StreetSegment>* segFound2 = map.find(end); //get pointer to vector associated with end key
            if (segFound2 != nullptr) //if end geocoordinate already existing key
                (*segFound2).push_back(r); //add reversed segment to street segment vector
            else //if not already an existing key
            {
                vector<StreetSegment> segs{r}; //put street segment into a vector
                map.associate(end, segs); //associate end geocoordinate to reversed street segment vector
            }
        }
    }
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    //map.print();
    
    const vector<StreetSegment>* s = (map.find(gc));
    if (s->size() == 0)
        return false;
    else
        segs  = *s;
    return true;
}


//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
